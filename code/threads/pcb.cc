#include "pcb.h"
#include "thread.h"
#include "addrspace.h"
#include "main.h"
#include "synch.h"
#include <stdio.h>


// Entry point for executing the process
static void StartProcess(void *args)
{
    char* fileName = (char*) args;
    DEBUG(dbgThread, "PCB: Starting process " << fileName);
    AddrSpace *addrspace = new AddrSpace;
    if (addrspace->Load(fileName)) {
        addrspace->Execute();
    }
    ASSERTNOTREACHED();
}
PCB::PCB()
{
    pid = -1;
    parentID = -1;
    numwait = 0;
    exitcode = 0;
    file = NULL;
    thread = NULL;
    joinsem = new Semaphore("joinsem",0);
    exitsem = new Semaphore("exitsem",0);
    mutex = new Semaphore("mutex",1);
}
PCB::PCB(const char* fileName, Thread* thread):PCB()
{
    pid = 0;
    file = new char[strlen(fileName) + 1];
    strcpy(file,fileName);
    this->thread = thread;
}
PCB::~PCB()
{
    delete file;
    delete joinsem;
    delete exitsem;
    delete mutex;
    if (thread)
    {
        thread->Finish();
        delete thread;
    }
}
int PCB::Exec(char* fileName, int pid)
{
    mutex->P();
    this->pid = pid;
    
    file = new char[strlen(fileName)];
    strcpy(file,fileName);

    DEBUG(dbgThread,"PCB: Forking"<<file<<".........");

    thread = new Thread(file);
    thread->Fork(StartProcess,file);

    mutex->V();
    return pid;
}

int PCB::GetID()
{
    return pid;
}

int PCB::GetNumWait()
{
    return numwait;
}

void PCB::JoinWait()
{
    joinsem->P();
}

void PCB::JoinRelease()
{
    joinsem->V();
}
void PCB::ExitWait()
{
    exitsem->P();
}
void PCB::ExitRelease()
{
    exitsem->V();
}
void PCB::IncNumWait()
{
    mutex->P();
    ++numwait;
    mutex->V();
}
void PCB::DecNumWait()
{
    mutex->P();
    --numwait;
    mutex->V();
}
void PCB::SetExitCode(int exitCode)
{
    exitcode = exitCode;
}
int PCB::GetExitCode()
{
    return exitcode;
}
const char* PCB::GetFileName()
{
    return file;
}
const Thread* PCB::GetThread()
{
    return thread;
}
