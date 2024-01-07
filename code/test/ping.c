// ping.c
#include "syscall.h"

int main() {
    int i;
    for (i = 0; i < 1000; i++) {
        Wait("Ping"); 
        PrintString("Ping");
        Signal("Pong"); 
    }
}