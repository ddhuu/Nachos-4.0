#include "syscall.h"
#define MAXLENGTH 255
char filename[256];
int main()
{
    int result;
    PrintString("Nhap ten file can delete: ");
    ReadString(filename);

    result = Remove(filename);

    if (result == -1)
    {
        PrintString("\nXoa file khong thanh cong.\n");
    }
    if (result == 0)
    {
        PrintString("\nXoa file thanh cong.\n");
    }
    Halt();
}