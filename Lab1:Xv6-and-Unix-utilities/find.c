#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fs.h"
#include "user/user.h"

void find(char* path, char* target_file);

int main(int argc, char* argv[]) {
    if (argc < 3) {
        fprintf(2, "Usage: find path filename...\n");
        exit(1);
    }

    find(argv[1], argv[2]);

    exit(0);
}

void find(char* path, char* target_file) {
    int fd;
    struct stat st;
    struct dirent de;
    char buf[512], *p;

    if ((fd = open(path, 0)) < 0) {
        fprintf(2, "find: cannot open %s\n", path);
        return;
    }

    if (fstat(fd, &st) < 0) {
        fprintf(2, "find: cannot fstat %s\n", path);
        close(fd);
        return;
    }

    switch (st.type) {
        case T_FILE:
            break;  // path参数不接受文件名
        case T_DIR:
            // path最后添加'/'
            strcpy(buf, path);
            p = buf + strlen(buf);
            *p = '/'; ++p;
            // 遍历path下的dirent
            while (read(fd, &de, sizeof(de)) == sizeof(de)) {
                if (de.inum == 0) continue;
                memmove(p, de.name, DIRSIZ);
                p[DIRSIZ] = 0;
                
                if (stat(buf, &st) < 0) {
                    printf("find: cannot stat %s\n", path);
                    continue;
                }
                if (st.type == T_DIR) {
                    if (!strcmp(de.name, ".") || !strcmp(de.name, "..")) {
                        continue;
                    }
                    find(buf, target_file);
                } else if (st.type == T_FILE) {
                    if (strcmp(target_file, de.name) == 0) {
                        printf("%s\n", buf);
                    }    
                }
                
            }
            break;
    }
}

