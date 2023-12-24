#include "sem.h"

Sem:: Sem(char* na, int i)
{
    strcpy(this->name,na);
    sem = new Semaphore(this->name,i);
}

Sem::~Sem()
{
    if(sem)
    {
        delete sem;
    }
}

void Sem::Wait()
{
    sem->P();
}
void Sem::Signal()
{
    sem->V();
}
char* Sem:: getName()
{
    return this->name;
}