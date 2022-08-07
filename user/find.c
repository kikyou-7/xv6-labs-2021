#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

void
find(char *path, char *file_name)
{
  char buf[512], *p;
  int fd;
  struct dirent de;
  struct stat st;

  if((fd = open(path, 0)) < 0){
    fprintf(2, "ls: cannot open %s\n", path);
    return;
  }

  if(fstat(fd, &st) < 0){
    fprintf(2, "ls: cannot stat %s\n", path);
    close(fd);
    return;
  }

  switch(st.type){
  case T_FILE:
    if (strcmp(path + strlen(path) - strlen(file_name), file_name) == 0) 
        printf("%s\n", path);
    break;

  case T_DIR:
    if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf){
      printf("find: path too long\n");
      break;
    }
    strcpy(buf, path);
    p = buf+strlen(buf);
    *p++ = '/';
    // 读取fd目录下的内容 放在de中
    while(read(fd, &de, sizeof(de)) == sizeof(de)){
      if(de.inum == 0)
        continue;
      memmove(p, de.name, DIRSIZ);
      p[DIRSIZ] = 0;
      if(stat(buf, &st) < 0){
        printf("find: cannot stat %s\n", buf);
        continue;
      }
      if (strcmp(buf + strlen(buf) - 3, "/..") != 0
      && strcmp(buf + strlen(buf) - 2, "/.") != 0) {
        find(buf, file_name);
      }
    }
    break;
  }
  close(fd);
}
// args[1] 文件路径 args[2] 目标文件名  
int main(int argn, char *args[]) 
{
    if (argn < 3) {
        fprintf(2, "input file");
        exit(0);
    }
    char file_name[512];
    strcpy(file_name, args[2]);
    find(args[1], file_name);
    exit(0);
}