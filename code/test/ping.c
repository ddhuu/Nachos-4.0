// ping.c
#include "syscall.h"

int main() {
    int i;
    for (i = 0; i < 10; i++) {
        Wait("Ping"); 
        PrintString("Ping");
        Signal("Pong"); 
    }
}