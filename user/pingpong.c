#include "kernel/types.h"
#include "user/user.h"

int
main()
{
    int p1[2], p2[2];	// p1:父进程发子进程，p2:子进程发父进程
    char buf = 'p';	// 注意要初始化
    pipe(p1);    // 创建管道，p1[0]是读fd，p1[1]是写fd
    pipe(p2);

    int child = fork();
    if (child > 0) {
	// 注意fork之后子进程有一份独立的表管理fd（包括管道的fd)
	// 及时关闭避免在该进程内被误实用
	close(p1[0]);	// 子进程关闭该fd不影响父进程使用该fd
	close(p2[1]);

        write(p1[1], &buf, sizeof(char));
        close(p1[1]);
        read(p2[0], &buf, sizeof(char));
        close(p2[0]);
        printf("%d: received pong\n", getpid());
        exit(0);
    } else if (child == 0) {
	close(p1[1]);
	close(p2[0]);

        read(p1[0], &buf, sizeof(char));    // 阻塞，直到父进程往p1写数据
        close(p1[0]);
        printf("%d: received ping\n", child);
        write(p2[1], &buf, sizeof(char));
        close(p2[1]);
        exit(0);
    } else {
	// 注意这里也要关闭
	close(p1[0]);
	close(p1[1]);
	close(p2[0]);
	close(p2[1]);
        fprintf(2, "fork error\n");
	exit(1);
    }
    exit(0);
}
