#include "syscall.h"

int main() {
	int pingPID, pongPID;
	int pingExitCode, pongExitCode;
	PrintString("Ping-Pong test starting...\n\n");
	pingPID = Exec("ping");
	pongPID = Exec("pong");

	pingExitCode = Join(pingPID);
	pongExitCode = Join(pongPID);


	PrintString("\nFinish Ping-Pong test\n");
}

