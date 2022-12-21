#include "kernel/types.h"
#include "user/user.h"
#include "kernel/param.h"

int
main(int argc, char *argv[])
{
    // argv[0]=xargs, argv[1]=是需要执行的命令, 剩下的是执行命令需要的参数
    if (argc < 2) {
        fprintf(1, "usage:xargs command <arguments...>\n");
        exit(1);
    }
    if (!(argc-1 <= MAXARG)) {    // argc里面包括xargs，不用算进exec允许的最大参数数量
        fprintf(1, "too many arguments for exec\n");
        exit(1);
    }
    char buf[512];
    char *params[MAXARG];
    int p=0;  // 指向args的第一个空位置
    // 首先把xargs本身带的参数放进args
    for (int i=1; i < argc; i++) {
        params[p++] = argv[i];
    }
    // 外层的循环表示每次获取一行的参数并调用exec处理
    for (;;) {
        int i = 0;  // 指向buf中第一个空位置
        // 内层的循环表示获取一行的参数直到遇到换行符
        for (;;) {
            int n = read(0, &buf[i], 1);  // 每次只读取一个字符
            if (n == 0 || buf[i] == '\n') {   // n=0表示读取到字符串末尾，或者读到换行符break
                break;
            }
	    i++;  // 不能放在buf[i++]里面否则当读到末尾时i就不是0了
        }
        if (i == 0) {   // 说明是从读取到字符串末尾的条件break出来的，break外层大循环
            break;  
        }
        // 否则处理当前这一行的参数
        buf[i] = '\0';	// 把换行符替换成结束符
        params[argc-1] = buf;	// 注意这里要固定替换params中最后一个参数，不再变化
        // fork出子进程调用exec
        if (fork() == 0) {
            exec(argv[1], params);
        } else {
            wait(0);
        }
    }
    exit(0);
}
