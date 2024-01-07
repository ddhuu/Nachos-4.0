#include "syscall.h"

int main() {
    int pingPID, pongPID;
    int pingExitCode, pongExitCode;

    // Create semaphores
    int a = CreateSemaphore("Ping", 1); 
    int b = CreateSemaphore("Pong", 0); 

    PrintString("Ping-Pong test starting...\n\n");

    pingPID = Exec("ping");
    pongPID = Exec("pong");

    pingExitCode = Join(pingPID);
    pongExitCode = Join(pongPID);

    PrintString("\nFinish Ping-Pong test\n");
}