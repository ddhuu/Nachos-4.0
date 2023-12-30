#include "syscall.h"
#define MAX_LENGTH 50

int main()
{
    int openFileId;
    int fileSize;
    char *c; // Ky tu de in ra
    char fileName[50];
    int i; // Index for loop
    PrintString("* * * Nhap vao ten file can doc: ");
    ReadString(fileName);
    //    Goi ham ReadString de doc vao ten file
    //    Co the su dung Open(stdin), nhung de tiet kiem thoi gian test ta dung ReadString
    //    ReadString(fileName, MAX_LENGTH);

    openFileId = Open(fileName, 1); // Goi ham Open de mo file
    // openFileId = Open("honghung.txt", 1); // Goi ham Open de mo file

    if (openFileId != -1) // Kiem tra Open co loi khong
    {
        // Seek den cuoi file de lay duoc do dai noi dung (fileSize)
        fileSize = Seek(-1, openFileId);

        if (fileSize == -1)
        {
            PrintString("Seek that bai!!!");
            
        }

        // Seek den dau tap tin de tien hanh Read
        if (Seek(0, openFileId) == -1)
        {
            PrintString("Seek that bai!!!");
            
        }

        for (i = 0; i < fileSize; i++) // Cho vong lap chay tu 0 - fileSize
        {
            if (Read(c, 1, openFileId) == -1)
            { // Goi ham Read de doc tung ki tu noi dung file
                PrintString("Read bi loi!!!!");
                
            }
            PrintString(c); // Goi ham PrintString de in tung ki tu ra man hinh
        }
        PrintString("\n");
        if (Close(openFileId) == -1) // Goi ham Close de dong file
        {
            PrintString("Dong file that bai!!!\n");
            
        }
        PrintString("Dong file thanh cong\n");
        
    }
    else
    {
        PrintString("* * * Mo file That bai * * * \n");
        
    }
    
}