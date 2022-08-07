#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h" 

int main(int argn, char *args[]) 
{
    int p0[2], p1[2];
    pipe(p0);
    pipe(p1); 
    char buf;
    if (fork() != 0) {
        close(p0[1]);
        close(p1[0]);
        write(p1[1], "!", 1);
        wait(0); 
        read(p0[0], &buf, 1);
        printf("%d: received pong\n", getpid());
        close(p0[0]);
        close(p1[1]);
    }
    else {
        close(p0[0]);
        close(p1[1]);
        read(p1[0], &buf, 1);
        write(p0[1], "!", 1);
        printf("%d: received ping\n", getpid());
        close(p0[1]); 
        close(p1[0]);
    }
    exit(0);
}