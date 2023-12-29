#include "PTable.h"
#include "bitmap.h"
#include "synch.h"
#include <stdio.h>

PTable::PTable(int size)
{
    numProcesses = size;
    slotManager = new Bitmap(numProcesses);
    sem = new Semaphore("ptable_sem",1);
    for (int i = 0 ; i < MAX_PROCESSES ; i++)
    {
        pcbs[i] = NULL;
    }
}

PTable::~PTable()
{
    delete slotManager;
    delete sem;
    for (int i =0 ; i< numProcesses; i++)
    {
        delete pcbs[i];
    }
}

void PTable::InitFirstProcess(const char* fileName, Thread* thread)
{
    pcbs[0] = new PCB(fileName,thread);
    numProcesses++;
    slotManager->Mark(0);
}

int PTable::ExecUpdate(char* fileName)
{
    sem->P();
    DEBUG(dbgThread,"PTable::ExecUpdate(\""<<fileName<<"\")");

    //prevent self-exec
    int currentThreadId = GetCurrentThreadId();
    if(strcmp(pcbs[currentThreadId]->GetFileName(),fileName) == 0)
    {
        fprintf(stderr,"PTable:%s cannot execute itself.\n",fileName);
        sem->V();
        return -1;
    }
    
    //Allocate a new PCB 

    DEBUG(dbgThread,"PTable: Looking fot free slot...");
    int slot = GetFreeSlot();
    if (slot == -1)
    {
        fprintf(stderr,"PTable: Maximum number of process reached :<.\n");
        sem->V();
        return -1;
    }

    DEBUG(dbgThread,"PTable: Obtained slot" <<slot);
    pcbs[slot] = new PCB();
    pcbs[slot]->parentID = currentThreadId;

    DEBUG(dbgThread,"PTable: Scheduling program for execution.....\n");
    int result = pcbs[slot]->Exec(fileName,slot);
    numProcesses++;
    sem->V();
    return result;
}

int PTable::ExitUpdate(int exitcode)
{
    int currentThreadId = GetCurrentThreadId();
    if(currentThreadId == 0)
    {
        kernel->interrupt->Halt();
    }
    else
    {
        pcbs[currentThreadId]-> SetExitCode(exitcode);
        pcbs[currentThreadId]->JoinRelease();
        pcbs[currentThreadId]->ExitWait();
        Remove(currentThreadId);
    }
}

int PTable::JoinUpdate(int id)
{
    int currentThreadId = GetCurrentThreadId();
    if(!IsExist(id)){
        printf("Ptable:There is no process with id: %d\n",id);
         return -1;
    }
    if (id == currentThreadId)
    {
        printf("PTable: Process with id %d cannot join to itself\n",currentThreadId);
        return -2;
    }
    else if(pcbs[id]->parentID != currentThreadId)
    {
        printf("PTable: Process with id %d is not parent of process with id %d",currentThreadId,id);
        return -3;
    }
   
   pcbs[currentThreadId]->IncNumWait();
   pcbs[id]->JoinWait();

   pcbs[currentThreadId]->DecNumWait();
   pcbs[id]->ExitRelease();

   return pcbs[id]->GetExitCode();

}

int PTable::GetFreeSlot()
{
    return slotManager->FindAndSet();
}

bool PTable::IsExist(int pid)
{
    if(pid<0 || pid>= MAX_PROCESSES){
        return FALSE;
    }
    return (pcbs[pid] != NULL);
}

void PTable::Remove(int pid)
{
    if(IsExist(pid))
    {
        numProcesses--;
        slotManager->Clear(pid);
        delete pcbs[pid];
        pcbs[pid]= NULL;
    }
}

int PTable::GetCurrentThreadId()
{
    for(int i =0 ; i < MAX_PROCESSES ; i++)
    {
        if (pcbs[i] != 0)
        {
            if(pcbs[i]->GetThread()  == kernel->currentThread ){
                return pcbs[i]->GetID();
            }
        }
    }
    return -1;
}

void PTable::Print()
{
    printf("\n\nTime: %d\n",kernel->stats->totalTicks);
    printf("Current process table: \n");
    printf("ID\tParent\tExecutable File\n");
    int currentThreadId = GetCurrentThreadId();
    for(int i = 0; i < MAX_PROCESSES ; i++)
    {
        if(pcbs[i]){
            printf(
                "%d\t%d\t%s%s\n",
                pcbs[i]->GetID(),
                pcbs[i]->parentID,
                pcbs[i]->GetFileName(),
                i == currentThreadId ? "[current thread]":"");
        }
    }
}