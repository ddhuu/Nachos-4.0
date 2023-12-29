#include "syscall.h"

int main() {
    int id = 0;
    int ec = 0;
    
    PrintString("join is executing Createfile.c\n");
    id = Exec("createfile");
    PrintString("join is joining into CreateFile\n");
    ec = Join(id);
    PrintString("join has returned\n");
    PrintString("join receive exit code from CreateFile: ");
    PrintNum(ec);
    PrintString("\n");
}