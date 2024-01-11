#include "syscall.h"
int main()
{
    char buffer[200];
    int len;
    int result;
    int resultWrite;
    char filenameSource[200];
    char filenameDestinate[200];
    int openFileIdSrc;
    int openFileIdDest;

    PrintString("Nhap ten file nguon: ");
    ReadString(filenameSource);

    PrintString("Nhap ten file dich: ");
    ReadString(filenameDestinate);

    openFileIdSrc = Open(filenameSource, 1);
    if (openFileIdSrc != -1)
    {
        len = Seek(-1, openFileIdSrc);
        if (Seek(0, openFileIdSrc) == -1)
        {
            PrintString("Seek that bai\n");
            
        }
        result = Read(buffer, len, openFileIdSrc);
        if (result == -1)
        {
            PrintString("Doc bi loi!!!\n");
            
        }
        if (Close(openFileIdSrc) == -1)
        {
            PrintString("Dong file nguon that bai!!!\n");
            
        }
        else
        {
            PrintString("Dong file nguon thanh cong\n");
        }

        openFileIdDest = Open(filenameDestinate, 0);
        if (openFileIdDest == -1)
        {
            if (Create(filenameDestinate) == -1)
            {
                PrintString("\nFile khong ton tai. Loi tao file!!\n");
                
            }
            else
            {
                PrintString("\nFile khong ton tai. Tao file thanh cong !!\n");
                openFileIdDest = Open(filenameDestinate, 0);
                if (openFileIdDest == -1)
                {
                    PrintString("Mo file dich that bai\n");
                    
                }
            }
        }
        resultWrite = Write(buffer, len, openFileIdDest);
        if (resultWrite == -1)
        {
            PrintString("Loi Write khi copy file!!!\n");
            
        }
        PrintString("\nCopy du lieu thanh cong !!\n");
        if (Close(openFileIdDest) == -1)
        {
            PrintString("Dong file dich that bai!!!\n");
            
        }
        else
        {
            PrintString("Dong file thanh cong\n");
        }

    }
    else
    {
        PrintString("\nFile nguon khong ton tai !!\n");
    }

}