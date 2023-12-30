#include "syscall.h"

int main() {
    int a = Signal("Huu");
    if (a ==0) PrintString("Signal thanh cong\n");
    Exit(0);    
}