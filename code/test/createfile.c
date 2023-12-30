#include "syscall.h"

int main()
{
    char fileName[50];
    int result = 0;
    PrintString("* * * Nhap vao ten file can tao: ");
    ReadString(fileName);

    result = Create(fileName);
    if (result == -1)
    {
        PrintString("Tao file that bai\n");
    }
    else if (result == 0)
    {
        PrintString("Tao file thanh cong\n");
    }
    Exit(0);
}