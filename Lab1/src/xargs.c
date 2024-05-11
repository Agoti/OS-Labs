#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/param.h"

// void print_strings(char* s[], char* name){
//   int i=0;
//   printf("%s: ", name);
//   for(; s[i] != 0; ++i){
//     printf("%s;", s[i]);
//   }
//   printf("\n");
// }

int 
main(int argc, char *argv[]) {
  char *args[MAXARG+1]; //后方命令的参数列表
  char line[512]; //储存read的数据
  char buffer[512]; //缓冲区
  char* tail = buffer; //buffer中的指针，记录对应参数字符串的首地址
  int i, n, j, k = 0;

  if (argc < 2) {
    // 参数太少
    fprintf(2, "Usage: xargs command [args...]\n");
    exit(1);
  }

  for (i = 1; i < argc && i <= MAXARG - 1; i++) {
    args[i-1] = argv[i]; //准备xargs后方的命令现有的参数列表
  }
  
  while ((n = read(0, line, sizeof(line))) > 0) { //不断从标准输入从读
    for(j=0; j<n; ++j){    	
      if (line[j] == '\n'){ //如果是回车
        buffer[k++] = '\0'; // 最后一个参数末尾\0  
        if(i >= MAXARG){ // 参数太多
          fprintf(2, "xargs: too many args");
          exit(1);
        }
        args[i-1] = tail; //准备最后一个参数字符串
        args[i] = 0;
        k = 0; // 重置缓冲区和参数个数
        tail = buffer;
        i = argc;
        if (fork() == 0) { // fork一个子进程执行命令           
          // print_strings(argv, "argv");
          // print_strings(args, "args");          
          exec(args[0], args);
          // exec不会在此返回，否则说明出错了
          fprintf(2, "xargs: failed to execute command %s\n", args[0]);
          exit(1);
        }
        wait(0);
      }
      else if(line[j] == ' '){ //如果是空格
        buffer[k++] = '\0'; //一个参数的结尾
        if(i >= MAXARG){ //参数太多
          fprintf(2, "xargs: too many args");
          exit(1);
        }
        args[i-1] = tail; //设置该参数字符串的首地址
        ++i; //参数列表增加1
        tail = buffer + k; //下一个参数字符串的首地址
      }
      else{ //不是回车和空格
        buffer[k++] = line[j]; //拷进buffer
      }
    }
  }

  exit(0);
}

