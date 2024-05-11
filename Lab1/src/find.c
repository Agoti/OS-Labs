#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

// 返回一个路径中的文件名
char*
fmtname(char *path)
{
  static char buf[DIRSIZ+1];
  char *p;

  // Find first character after last slash.
  for(p=path+strlen(path); p >= path && *p != '/'; p--)
    ;
  p++;

  if(strlen(p) >= DIRSIZ)
    return p;
  memmove(buf, p, strlen(p));
  //memset(buf+strlen(p), ' ', DIRSIZ-strlen(p));
  buf[strlen(p)] = 0;
  return buf;
}

void 
find(char* path, char* target){

  char buf[512], *p; // 存储路径的缓冲区和操作指针
  int fd; // 打开的文件的描述符
  struct dirent de; // 目录项信息
  struct stat st; // 文件信息
  
  //sleep(5);

  if((fd = open(path, 0)) < 0){
    // 打开文件错误
    fprintf(2, "find: cannot open %s\n", path);
    return;
  }

  if(fstat(fd, &st) < 0){
    // 读取文件状态错误
    fprintf(2, "find: cannot stat %s\n", path);
    close(fd);
    return;
  }
  
  //printf("file: %d, dir: %d\n", T_FILE, T_DIR);
  //printf("find in %s, type %d\n", path, st.type);
  
  switch(st.type){ // 根据文件类型
  case T_DEVICE:
  case T_FILE:
    //设备和文件：
    if(strcmp(target,fmtname(path))==0){
      // 文件名相符，输出路径  
      printf("%s\n", path);
    }
    break;

  case T_DIR:
    //文件夹：
    if(strlen(path) + 1 + DIRSIZ + 1 > sizeof(buf)){
      // 路径太长
      printf("find: path too long\n");
      break;
    }
    strcpy(buf, path);
    p = buf+strlen(buf);
    *p++ = '/'; //处理一下路径字符串，拷贝到buf并加斜杠
    while(read(fd, &de, sizeof(de)) == sizeof(de)){ //读取每一个目录项
      if(de.inum == 0)
        continue;
      // 处理路径
      memmove(p, de.name, DIRSIZ);
      p[DIRSIZ] = 0;
      if(strcmp(fmtname(buf), ".") != 0 && strcmp(fmtname(buf), "..") != 0){ 
        //如果不是.和..
        find(buf, target); //递归find
      }
    }
    break;
  }
  close(fd);
}

int
main(int argc, char *argv[])
{
  int i;
  char path[DIRSIZ];

  if(argc < 3){
    printf("Usage: find [path] [name]\n");
    exit(0);
  }
  //在给定的目录下，依次查找给定的文件名
  memmove(path, argv[1], DIRSIZ);
  for(i=2; i<argc; i++)
    find(path, argv[i]);
  exit(0);
}
