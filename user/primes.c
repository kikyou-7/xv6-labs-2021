#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h" 
// 第i个子进程筛掉第i个素数的倍数
void sevie(int p[]) 
{
    int pright[2];
    pipe(pright);
    int buf;
    int prime;
    read(p[0], &prime, sizeof prime); 
    if (prime == -1) {
        exit(0);
    }
    printf("prime %d\n", prime);
    // 主进程 筛 
    if (fork() != 0) {
        close(pright[0]); 
        while (read(p[0], &buf, sizeof buf) && buf != -1) {
            if (buf % prime != 0) {
                write(pright[1], &buf, sizeof buf);
            }
        }
        buf = -1;
        write(pright[1], &buf, sizeof buf);
        wait(0);
        close(p[0]);
        close(pright[1]);
    }
    else {
        close(p[0]);
        close(pright[1]); 
        sevie(pright);
    }
}
int main(int argn, char *args[]) 
{
    int p[2];
    pipe(p);
    int i;
    if (fork() != 0) {
        for (i = 2; i <= 35 ;i++) {
            write(p[1], &i, sizeof i);
        }
        i = -1;
        write(p[1], &i, sizeof i);
        wait(0);
    }
    else {
        close(p[1]);
        sevie(p);
        close(p[0]);
    }
    exit(0);
}