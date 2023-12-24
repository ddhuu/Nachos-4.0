#ifndef PCB_H
#define PCB_H

#include"synch.h"
#include"thread.h"
class Semaphore;
/*
The Process Control Block (PCB) is a class that contains information related to a specific process, including:

- Process ID: A unique identifier for the process.
- Parent ID: The identifier of the parent process that created this process.
- Executable file: The file that contains the program to be executed by this process.
- Thread: The thread used to execute the program.
- Joining operation information: Data needed to manage the process of one thread waiting for another to finish execution.

Instances of the PCB class are entries in the system-wide process table, managed by the PTable class.
*/

class PCB{
    private:

        Semaphore* joinsem; // semaphore for join process
        Semaphore* exitsem; // Semaphore for managing the exit process
        Semaphore* mutex;// Semaphore for ensuring mutual exclusion
        int exitcode;// Exit code of the process
        int numwait; // the number  of joined process
        char* file; // Name of executable file
        Thread* thread; // The thread to be run
        int pid; // Process ID

    public:
        int parentID;

        PCB();
        ~PCB();

        PCB(const char* filename , Thread *thread); // Create thread and schedule for program store in "filename"
        
        // Load the program has the name is "filename" and the process id  is pid
        // Return the process ID on success, -1 on failure
        int  Exec(char* filename, int pid);

        int  GetID();// Return the pid of the pid of the current process
 
        int  GetNumWait();// Return  the number of waiting process

        void JoinWait(); // The parent process wait for the child process finishes

        void JoinRelease();// The child process notice the parent process

        void ExitWait(); // The child process finish

        void ExitRelease();// The parent process accept to exit the child process

        void IncNumWait();// Increase the number of the waiting process

        void DecNumWait();// Decrease the number of the waitting process

        void SetExitCode(int exitcode);// Set the exitcode for the process

        int  GetExitCode();// Return the exitcode

        const char* GetFileName();// Return the process name

        const Thread* GetThread();// Returns the thread object of this process

};
#endif