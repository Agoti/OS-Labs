#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

static int numbers[34]; //储存2～35

int
main(int argc, char *argv[])
{
  int leftp[2], rightp[2]; //每个进程的左管道，右管道
  int buffer[1]; //读入整数的缓冲区
  int res; //read的返回值
  int i;
  int pivot[1]; //每个素数筛的“主元”
  
  for(i=0; i<34; ++i){
    numbers[i] = i+2; //2～35
  }  
  pipe(rightp); //主进程右管道

  if(fork() == 0){ // 开辟第一个素数筛
    while(1){
      // 每一个素数筛：
      leftp[0] = rightp[0];
      leftp[1] = rightp[1]; //父进程的右管道变为自己的左管道
      close(leftp[1]); //关闭左管道的写端，只从左管道读
      res = read(leftp[0], pivot, sizeof(int)); //读取主元
      if(res < 0){
       //读取错误
       fprintf(2,"read error\n");
       close(leftp[0]);
       exit(1);
      }
      else if(res == 0) {
        //发现父进程不能给自己传递主元，说明筛选结束，直接结束进程
        close(leftp[0]);
        break;
      }
      else {
        //打印主元
        printf("prime %d\n", *pivot);
      }
      pipe(rightp);//创建右管道
      // printf("%d %d %d %d\n",leftp[0],leftp[1],rightp[0],rightp[1]);
      if(fork() == 0) {
        // 新的素数筛（子进程）：
        close(leftp[0]); //关闭父进程左管道的读端，父进程的左管道和自己没有关系。
        continue; //跳回循环开始处
      }
      close(rightp[0]); //作为父进程：关闭右管道读端。
      while(1){
        res = read(leftp[0], buffer, sizeof(int)); //不断从左管道读数，进行筛选
        if(res < 0){
          //读数错误
          fprintf(2,"read error\n");
          close(leftp[0]);
          exit(1);
        }
        else if(res == 0) {
          //左管道没有数传给自己了，进程可以结束了
          close(rightp[1]);
          close(leftp[0]);
          break;
        }
        else{
          // 如果传来的数不是主元的倍数，就传给下一个素数筛
          // printf("prime %d: received %d\n", *pivot, *buffer);
          if(*buffer % *pivot != 0){
            write(rightp[1], buffer, 4);
            // printf("prime %d: send %d\n", *pivot, *buffer);
          }            
        }
      }
      wait(0);
      break;
    }  
  }
  else{
    //主进程：
    close(rightp[0]); // 关闭主进程的右管道的读端。
    for(i=0; i<34; ++i){
      write(rightp[1], numbers+i, sizeof(int)); // 将2～35传递给第一个素数筛
      // printf("main: send %d\n", numbers[i]);
    }
    close(rightp[1]); // 关闭右管道写端。
    wait(0);
  }    

  exit(0);
}

