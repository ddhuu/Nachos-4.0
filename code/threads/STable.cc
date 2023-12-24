#include"STable.h"

STable::STable()
{
    this->bm = new Bitmap(MAX_SEMAPHORE);

    for(int i = 0;  i< MAX_SEMAPHORE ; i++)
    {
        this->table[i] = NULL;
    }
}

STable::~STable()
{
    if(this->bm)
    {
        delete this->bm;
        this->bm = NULL;
    }
    for(int i = 0 ; i < MAX_SEMAPHORE ; i++)
    {
        if(this->table[i])
        {
            delete this->table[i];
            this->table[i] = NULL;
        }
    }
}
int STable::Create(char* name, int value)
{
    DEBUG(dbgSynch,"STable::Create Semaphore" <<name<<":"<<value);
    //check whhether semaphore is exist
    for(int i= 0; i < MAX_SEMAPHORE ; i++)
    {
        if(bm->Test(i))
        {
            if(strcmp(name,table[i]->getName())==0)
            {
                DEBUG(dbgSynch,"STable: Find existed Semaphore");
                return -1;
            }
        }
    }
    // FInd FreeSlot in table
    int id = this->FindFreeSlot();
    // If table is full then ruturn -1
    if(id<0)
    {
        DEBUG(dbgSynch,"STable: Error table is full");
        return -1;
    }

    // If find emtpy slot then load semaphore to table[id]

    this->table[id] = new Sem(name,value);
    DEBUG(dbgSynch,"STable: Create in table["<<id<<"]");
    return 0;
}

int STable:: Wait(char* name)
{
    DEBUG(dbgSynch,"STable::Wait("<<name<<")");
    for(int i =0 ; i< MAX_SEMAPHORE ; i++)
    {
        //Check does slot[i] load semaphore
        if(bm->Test(i))
        {
            if(strcmp(name,table[i]->getName())==0)
            {
                table[i]->Wait();
                return 0;
            }
        }
    }
    DEBUG(dbgSynch,"Not exists semaphore in wait ");
    return -1;
}

int STable::Signal(char* name)
{
    DEBUG(dbgSynch,"STable::Signal("<<name<<")");
    for(int i =0 ; i< MAX_SEMAPHORE; i++)
    {
        if(bm->Test(i))
        {   
            // If exists then make semaphore up
            if(strcmp(name,table[i]->getName())==0)
            {
                table[i]->Signal();
                return 0;
            }
        }
    }
    DEBUG(dbgSynch,"Not Exists semaphore in signal");
    return -1;

}
int STable::FindFreeSlot()
{
    return this->bm->FindAndSet();
}