#include "syscall.h"

int main() {
    int id = 0;
    int ec = 0;

    
    PrintString("Test Exec\n");
    id = Exec("createfile");

}