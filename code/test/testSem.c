#include "syscall.h"

int main() {
    int b;
    int a;
    a = CreateSemaphore("Testing",0);
    if (a==0) {
        PrintString("CreateSemaphore thanh cong \n");
    }
    b = Exec("threadSem");
    a = Wait("Huu");
    Join(b);
    if(a==0) {
        PrintString("Wait thanh cong\n");
    }

    Exit(0);
}