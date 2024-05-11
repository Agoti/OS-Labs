#include "types.h"
#include "riscv.h"
#include "param.h"
#include "defs.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"

uint64
sys_exit(void)
{
  int n;
  argint(0, &n);
  exit(n);
  return 0;  // not reached
}

uint64
sys_getpid(void)
{
  return myproc()->pid;
}

uint64
sys_fork(void)
{
  return fork();
}

uint64
sys_wait(void)
{
  uint64 p;
  argaddr(0, &p);
  return wait(p);
}

uint64
sys_sbrk(void)
{
  uint64 addr;
  int n;

  argint(0, &n);
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

uint64
sys_sleep(void)
{
  int n;
  uint ticks0;


  argint(0, &n);
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(killed(myproc())){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}


#ifdef LAB_PGTBL
//新增：系统调用pgaccess
int
sys_pgaccess(void)
{
  uint64 va, result;
  int sz;
  argaddr(0, &va);  
  argint(1, &sz);
  argaddr(2, &result); // 从用户空间获得参数

  int buffer = 0; // 缓冲区
  struct proc* p = myproc(); // 陷入前的进程

  if(sz > 32){ // 最多获取32个页面
    return -1;
  }
  for(int i = 0; i < sz; ++i){
    // 获得各个虚拟地址对应的PTE
    pte_t* pte = walk(p->pagetable, va + i * PGSIZE, 0);
    if(pte && (*pte & PTE_A)){ // 如果PTE存在并且PTE_A位为1
      buffer |= 1 << i; // 设置缓冲位图
      *pte &= ~PTE_A; // 之后PTE的PTE_A位置0
    }
  }

  // 将结果拷贝到用户空间
  copyout(p->pagetable, result, (char*)&buffer, sizeof(buffer));
  return 0;
}
#endif

uint64
sys_kill(void)
{
  int pid;

  argint(0, &pid);
  return kill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}
