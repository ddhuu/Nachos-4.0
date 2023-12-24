#ifndef STABLE_H
#define STABLE_H
#include"sem.h"
#include"bitmap.h"

#define MAX_SEMAPHORE 10
class Sem;

class STable{
    private:
        Bitmap* bm; //Manage empty slot
        Sem* table[MAX_SEMAPHORE]; // Lock Manager
    public:
        //Constructor
        //Create 10 null semaphore for semTab
        //Init bm
        STable();

        ~STable();



        int Create(char* name, int value);
        int Wait(char* name);
        int Signal(char* name);
        int FindFreeSlot();

};
#endif