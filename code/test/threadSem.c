#include "syscall.h"

int main() {
    int a = Signal("Testing");
    if (a ==0) PrintString("Signal thanh cong\n");
    Exit(0);    
}