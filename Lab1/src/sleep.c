#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  //如果参数不够则输出提示信息
  if(argc<2){
      fprintf(2, "Usage: sleep [time]\n");
      exit(1);
  }
  //将字符串转换为数字
  int time = atoi(argv[1]);
  //调用系统调用sleep
  sleep(time);
  exit(0);
}
