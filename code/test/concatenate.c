#include "syscall.h"

// Kết quả nối của 2 file sẽ được thể hiện ở file resultCon.txt
int main()
{
    char filename1[100];
    char filename2[100];

    char s1[255];
    char s2[256];
    int fileLength1;
    int fileLength2;
    int result;
    int id;
    int len;
    PrintString("Nhap ten file nguon 1: ");
    ReadString(filename1);

    PrintString("Nhap ten file nguon 2: ");
    ReadString(filename2);

    id = Open(filename1, 1);
    if (id == -1)
    {
        PrintString("\nMo file nguon 1 that bai !!\n");
        Halt();
    }
    len = Seek(-1, id);
    if (Seek(0, id) == -1)
    {
        PrintString("Seek that bai\n");
        Halt();
    }
    fileLength1 = Read(s1, len, id);
    if (fileLength1 == -1)
    {
        PrintString("Read nguon 1 bi loi!!!\n");
        Halt();
    }
    if (Close(id) == -1)
    {
        PrintString("Dong file nguon 1 that bai\n");
        Halt();
    }

    id = Open(filename2, 1);
    if (id == -1)
    {
        PrintString("\nMo file nguon 2 that bai !!\n");
        Halt();
    }
    len = Seek(-1, id);
    if (Seek(0, id) == -1)
    {
        PrintString("Seek that bai\n");
        Halt();
    }
    fileLength2 = Read(s2, len, id);
    if (fileLength2 == -1)
    {
        PrintString("Read file nguon 2 bi loi!!!\n");
        Halt();
    }
    if (Close(id) == -1)
    {
        PrintString("Dong file nguon 2 that bai\n");
        Halt();
    }

    // Tạo file lưu kết quả
    id = Open("resultConcatenate.txt", 0);
    if (id != -1)
    {
        Remove("resultConcatenate.txt");
        id = -1;
    }
    if (id == -1)
    {
        if (Create("resultConcatenate.txt") == -1)
        {
            PrintString("File khong ton tai. Tao file moi that bai!!!\n");
            Halt();
        }
        else
        {
            PrintString("File khong ton tai. Tao file thanh cong\n");
            id = Open("resultConcatenate.txt", 0);
            if (id == -1)
            {
                PrintString("Mo file that bai!!!\n");
                Halt();
            }
        }
    }

    // Xác định kích thước
    if (Write(s1, fileLength1, id) == -1)
    {
        PrintString("Qua tring Write bi loi!!!");
        Halt();
    }
    if (Seek(fileLength1, id) == -1)
    {
        PrintString("Seek toi cuoi file that bai!!\n");
        Halt();
    }

    if (Write(s2, fileLength2, id) == -1)
    {
        PrintString("Qua trinh write bi loi!!!\n");
        Halt();
    }

    if (Close(id) == 0)
    {
        PrintString("\nNoi file thanh cong\n");
    }
    else
    {
        PrintString("\n Noi file that bai !!\n");
    }

    Halt();
}