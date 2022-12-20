#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"


void
find(char *path, char *target)
{   // buf存放当前文件完整的路径名，用于传递给stat打开该文件获取文件状态
    // p始终指向buf中最后一个"/"的后一位，*p表示文件名
    // 而且这样做还有一个好处就是while循环递归时候因为p始终指向"/"后一位，直接覆盖之前的内容即可
    char buf[512], *p;  
    int fd; // 根据path打开目录得到其fd
    struct dirent de; // 目录项结构体
    struct stat st; // 文件状态结构体

    if((fd = open(path, 0)) < 0){ // open也可以打开目录，并返回fd（fd很大的一点作用就是抽象了文件（包括目录）、管道和设备之间的区别，都视为字节流）
        fprintf(1, "find: cannot open %s\n", path);
        return;
    }

    if(fstat(fd, &st) < 0){ // fstat需要fd，所以需要在前面先open
        fprintf(1, "find: cannot stat %s\n", path);
        close(fd);
        return;
    }    

    switch (st.type) {
    case T_DEVICE:
        fprintf(1, "the first arg should be a directory name, not a device\n");
        break;
    case T_FILE:
        fprintf(1, "the first arg should be a directory name, not a file\n");
        break;
    case T_DIR:     
        // 当前路径的长度+最大目录项允许的长度不应超过buf的大小   
        // 且字符串数组以“/0”结尾还各需要+1 
        if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf){
            fprintf(1, "find: path too long\n");
            break;
        }
        strcpy(buf, path);  // 将path的内容拷贝到buf
        p = buf+strlen(buf);
        *p++ = '/';
        while(read(fd, &de, sizeof(de)) == sizeof(de)){
            if(de.inum == 0)  
                continue;
            memmove(p, de.name, DIRSIZ);
            p[DIRSIZ] = 0;  // 字符串结束标志
            if(stat(buf, &st) < 0){
                fprintf(1, "ls: cannot stat %s\n", buf);
                continue;
            }
            if (st.type == T_DIR && strcmp(p, ".") != 0 && strcmp(p, "..") != 0) {
                find(buf, target);
            } else if (strcmp(target, p) == 0) {
                fprintf(1, "%s\n", buf);
            }   
        }
        break;       
    }
    close(fd);
}

int
main(int argc, char *argv[])
{
    if (argc != 3) {
        fprintf(1, "usage:find <directory> <file>\n");
        exit(1);
    }
    find(argv[1], argv[2]);
    exit(0);
}
