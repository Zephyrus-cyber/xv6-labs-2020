#include "kernel/types.h"
#include "user/user.h"

#define RD 0
#define WR 1

const uint INT_LEN = sizeof(int);

void
find_prime(int lpipe[2])    
{   
    close(lpipe[WR]);   // 当前进程不需要写lpipe
    int first;
    if (read(lpipe[RD], &first, sizeof(int)) == sizeof(int)) {    // 该进程有素数
        fprintf(1, "prime %d\n", first);
        int rpipe[2];   // 创建右邻居的管道
        pipe(rpipe);
        //close(rpipe[RD]);   // 当前进程不需要读rpipe,但是不能在这里就关闭！
	//因为此时rpipe[RD]的引用计数还是1，close之后就真关闭了，子进程就无法使用
        int num;
        // read会阻塞，且while一直循环，直到写端被关闭read返回0
        while (read(lpipe[RD], &num, sizeof(int) == sizeof(int))) {    
            if (num % first != 0) {
                write(rpipe[WR], &num, sizeof(int));
            } 
        }
        close(lpipe[RD]);   // 注意这里用完了及时关闭这俩fd
        close(rpipe[WR]);

        // 感觉并不是真正上的并发，虽然多个进程同时存在，但只是等到下一个进程工作的完成
        // 并没有说多个进程同时在进行计算处理的工作
        if (fork() == 0) {
            find_prime(rpipe);  // 递归，当前进程的“右”管道即右邻居进程的“左”管道
        } else {
	    close(rpipe[RD]);
            wait(0);
        }
    } else {
        // 否则没有素数，关闭fd，直接结束
        close(lpipe[RD]);
    }
    exit(0);
}

int 
main(int argc, char *argv[])
{
    int p[2];
    pipe(p);
    for (int i = 2; i <= 35; i++) {
        write(p[WR], &i, INT_LEN);
    }
    if (fork() == 0) {
        find_prime(p);
    } else {
        close(p[RD]);
        close(p[WR]);
        wait(0);
    }
    exit(0);
}
