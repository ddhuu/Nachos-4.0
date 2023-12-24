#ifndef SEM_H
#define SEM_H
#include "synch.h"
class Semaphore;


// A class for managing Semaphore
class Sem{
    private: 
        char name[50]; //Name of Semaphore
        Semaphore* sem; // Create semaphore for management
    public:
        Sem(char* na, int i);

        ~Sem();
        
        void Wait();

        void Signal();

        char* getName();

};
#endif; 