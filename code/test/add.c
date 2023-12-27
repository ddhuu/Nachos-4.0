/* add.c
 *	Simple program to test whether the systemcall interface works.
 *
 *	Just do a add syscall that adds two values and returns the result.
 *
 */

#include "syscall.h"

int main() {
    PrintString("printstring is starting\n");
    PrintString("hello\n");
    PrintString("printstring is exiting with exit code: 0\n");
    Exit(0);
}
