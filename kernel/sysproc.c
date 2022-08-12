#include "types.h"
#include "riscv.h"
#include "param.h"
#include "defs.h"
#include "date.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"

uint64
sys_exit(void)
{
  int n;
  if(argint(0, &n) < 0)
    return -1;
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
  if(argaddr(0, &p) < 0)
    return -1;
  return wait(p);
}

uint64
sys_sbrk(void)
{
  int addr;
  int n;

  if(argint(0, &n) < 0)
    return -1;
  
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


  if(argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(myproc()->killed){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}


#ifdef LAB_PGTBL
int
sys_pgaccess(void)
{
  // 需要检查的起始虚拟地址 结果存在buffer_ad中
  // 遍历n个page n<=64, 检查每个page对应的PTE的PTE_A
  uint64 start_ad, buffer_ad, a;
  int n;
  if(argaddr(0, &start_ad) < 0)
    return -1;
  if (argint(1, &n) < 0) 
    return -1;
  if (argaddr(2, &buffer_ad) < 0)
    return -1;
  uint64 temp_buf = 0;
  if (n > 64) 
    panic("sys_pgaccess: 64 pages limited");
  pte_t *pte;
  for(a = start_ad; a < start_ad + n*PGSIZE; a += PGSIZE){
    if((pte = walk(myproc()->pagetable, a, 0)) == 0)
      panic("sys_pgaccess: walk");
    if((*pte & PTE_V) == 0)
      panic("sys_pgaccess: not mapped");
    if(PTE_FLAGS(*pte) == PTE_V)
      panic("sys_pgaccess: not a leaf");
    if (*pte & PTE_A) {
      temp_buf |= 1 << ((a - start_ad) / PGSIZE);
      // 处理完后把这页设为未被访问状态
      *pte ^= PTE_A;
    }
  }
  if(copyout(myproc()->pagetable, buffer_ad, (char *) &temp_buf, sizeof temp_buf)) 
    return -1;
  return 0;
}
#endif

uint64
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
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

