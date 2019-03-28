// ONLY MODIFY CODE BELOW THE NEXT OCCURENCE OF THE FOLLOWING LINE !
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#include "types.h"
#include "defs.h"
#include <stdio.h>
#include "proc.h"

#define NCPU 1

struct cpu cpus[NCPU];
int ncpu;

void printstate(enum procstate pstate){ // DO NOT MODIFY
  switch(pstate) {
    case UNUSED   : printf("UNUSED");   break;
    case EMBRYO   : printf("EMBRYO");   break;
    case SLEEPING : printf("SLEEPING"); break;
    case RUNNABLE : printf("RUNNABLE"); break;
    case RUNNING  : printf("RUNNING");  break;
    case ZOMBIE   : printf("ZOMBIE");   break;
    default       : printf("????????");
  }
}

// Per-CPU process scheduler.
// Each CPU calls scheduler() after setting itself up.
// Scheduler never returns.  It loops, doing:
//  - choose a process to run
//  - swtch to start running that process
//  - eventually that process transfers control
//      via swtch back to the scheduler.

// local to scheduler in xv6, but here we need them to persist outside,
// because while xv6 runs scheduler as a "coroutine" via swtch,
// here swtch is just a regular procedure call.
struct proc *p;
struct cpu *c = cpus;

// +++++++ ONLY MODIFY BELOW THIS LINE ++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void
scheduler(void)
{ int runnableFound; // DO NOT MODIFY/DELETE

  c->proc = 0;

  runnableFound = 1 ; // force one pass over ptable
  int runCountTable [NPROC]; // table to count number of runs
  for(int i = 0; i < NPROC; i++){
    runCountTable[i] = 0;
  }
  struct proc* priorityQueue [NPROC];
  for(int i = 0; i < NPROC; i++){
    priorityQueue[i] = &ptable.proc[i];
  }

  while(runnableFound){ // DO NOT MODIFY
    // Enable interrupts on this processor.
    // sti();
    runnableFound = 0; // DO NOT MODIFY
  
    for(int i = 0; i < NPROC && !runnableFound; i++){
      p = priorityQueue[i];
      if(p->state != RUNNABLE)
        continue;
      runCountTable[i]++;
      runnableFound = 1; // DO NOT MODIFY/DELETE/BYPASS 

      // Switch to chosen process.  It is the process's job
      // to release ptable.lock and then reacquire it
      // before jumping back to us.
      c->proc = p;
      //switchuvm(p);
      p->state = RUNNING;


      swtch(p);
      // p->state should not be running on return here.
      //switchkvm();
      // Process is done running for now.
      // It should have changed its p->state before coming back.
      c->proc = 0;
    }
    // release(&ptable.lock);
    
    //sort queue (selection sort)
    // for(int i = NPROC - 1; i > 0; i--){
    //   int maxIndex = i;
    //   for(int j = i - 1; j >= 0; j--){
    //     if(runCountTable[j] > runCountTable[maxIndex]){
    //       maxIndex = j;
    //     }
    //   }

    //   int tmpNum = runCountTable[i];
    //   runCountTable[i] = runCountTable[maxIndex];
    //   runCountTable[maxIndex] = tmpNum;

    //   struct proc *tmpProc = priorityQueue[i];
    //   priorityQueue[i] = priorityQueue[maxIndex];
    //   priorityQueue[maxIndex] = tmpProc;

    // }

    for(int i = 0; i < NPROC; i++){
      int minReached = 0;
      for(int j = i; j >= 1 && minReached != 1; j--){
        if(runCountTable[j] < runCountTable[j - 1]){
          int tmpNum = runCountTable[j - 1];
          runCountTable[j-1] = runCountTable[j];
          runCountTable[j] = tmpNum;

          struct proc *tmpProc = priorityQueue[j - 1];
          priorityQueue[j - 1] = priorityQueue[j];
          priorityQueue[j] = tmpProc;
        }else{
          minReached = 1;
        }
      }
    }

  }
  printf("No RUNNABLE process!\n");
}

