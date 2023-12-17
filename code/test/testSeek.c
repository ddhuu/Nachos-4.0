#include "syscall.h"

#define MODE_READWRITE 0
#define MODE_READ 1

#define stdin 0
#define stdout 1

int main()
{
    char buffer[100];
    char *filename;
    int read;
    int fileid;

    PrintString("Nhap ten file: ");
    ReadString(filename);

    fileid = Open(filename, MODE_READ);
    if (fileid == -1)
    {
        PrintString("Mo file that bai\n");
    }
    if (Seek(15, fileid) != -1)
    {
        read = Read(buffer, 50, fileid);
        if (read != -1)
        {
            PrintString("\nKet qua sau khi Seek: \n");
            PrintString(buffer);
            PrintString("\n");
            Close(fileid);
        }
    }
    else
    {
        PrintString("Seek that bai\n");
    }

    Halt();
}