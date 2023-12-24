#ifndef PTABLE_H
#define PTABLE_H

#include"pcb.h"
#include"bitmap.h"
#include"synch.h"
#include"thread.h"

#define MAX_PROCESSES 10

class PCB;
class Semaphore;
class PTable{
    private:
        PCB* pcbs[MAX_PROCESSES];
        int  numProcesses;
        Bitmap* slotManager; // Make the locations  that have been used in pcb
        Semaphore* sem; // Use to prevent the case of loading 2 processes at the same time 
    public:
        PTable(int size = MAX_PROCESSES);
        ~PTable();

        //Allocates the start up process
        void InitFirstProcess(const char* fileName,Thread* thread);

        // Sets up PCB and shchedues the program stored in 'fileName'
        // Returns the process id on success , else return -1.
        int  ExecUpdate(char* fileName);// Process the SC_Exec

        //Update the process table when the current thread exit wit ExitCode
        int  ExitUpdate(int exitCode); // Process the SC_Exit

        //Update the process table when the current thread join into the thread
        int  JoinUpdate(int id); // Process the SC_Join

        //Return an index of a free slot in the table
        //If the table is full then return -1
        int  GetFreeSlot();

        //Check a process exist or not
        bool IsExist(int pid);
        
        //Remove a PCB 
        void Remove(int pid);

        //Print the current process table
        void Print();


        int GetCurrentThreadId();

};

#endif