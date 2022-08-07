#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argn, char *args[]) 
{
    if (argn < 2) {
        fprintf(2, "input a number");
        exit(1);
    }
    sleep(atoi(args[1]));
    exit(0);
}