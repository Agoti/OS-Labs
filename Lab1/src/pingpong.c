#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  int p1[2],p2[2]; //储存管道的读端和写端
  char buffer[1] = {0}; //子进程接受信息的缓冲区
  char info[1] = {0x30}; //传输的信息
    
  pipe(p1);
  pipe(p2); //创建两个管道
  if(fork() == 0){ //创建子进程
    //子进程
    close(p1[1]);
    close(p2[0]); //关闭不用的管道端口。子进程从p1读，向p2写。
    if(read(p1[0], buffer, 1) != 1){
      //子进程接收错误
      fprintf(2,"child read error\n");
      exit(1);
    }
    //接收到信息，输出文字
    printf("%d: received ping\n", getpid());
    if(write(p2[1], buffer, 1) != 1){
      //子进程发送错误
      fprintf(2,"child write error\n");
      exit(1);
    }
  }
  else{
    //父进程
    close(p1[0]);
    close(p2[1]); //关闭不用的管道端口。父进程向p1写，从p2读。
    if(write(p1[1], info, 1) != 1){
      //父进程发送错误
      fprintf(2,"parent write error\n");
      exit(1);
    }
    if(read(p2[0], buffer, 1) != 1){
      //父进程接收错误
      fprintf(2,"parent read error\n");
      exit(1);
    }
    //接收到消息，输出文字
    printf("%d: received pong\n", getpid());    
  }
  exit(0);
}
