// pong.c
#include "syscall.h"

void main() {
    int i;
    for (i = 0; i < 10; i++) {
        Wait("Pong"); 
        PrintString("Pong");
        Signal("Ping"); 
    }
}   