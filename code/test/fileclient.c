#include "syscall.h"

int main()
{
    int fileSize;
    int socketId;
    char fileName[32];
    char messageSendToServer[100];
    char messageRecieveFromServer[100];
    char testCloseServer[100];
    int openFileId;
    int len;
    int resultSend;
    int resultGet;
    char c;
    int i;
    PrintString("Nhap ten file can doc: ");
    ReadString(fileName);

    openFileId = Open(fileName, 1);

    if (openFileId != -1)
    {
        // Lấy kích thước file băng cách Seek -1 (đưa con trỏ tới cuỗi file = kích thước file)
        fileSize = Seek(-1, openFileId);
        if (fileSize == -1)
        {
            PrintString("Seek that bai!!!\n");
            Close(openFileId);
            Halt();
        }
        if (Seek(0, openFileId) == -1)
        {
            PrintString("Seek that bai!!!\n");
            Close(openFileId);
            Halt();
        }

        // Đọc file và lưu vào mảng để gửi đến server
        for (i = 0; i < fileSize; i++)
        {
            if (Read(&c, 1, openFileId) == -1)
            {
                PrintString("Read bi loi!!!");
                Close(openFileId);
                Halt();
            }
            messageSendToServer[i] = c;
        }
        messageSendToServer[fileSize] = '\0';
        if (Close(openFileId) == -1)
        {
            PrintString("Dong file that bai");
            Halt();
        }
        if (fileSize == 0)
        {
            PrintString("Khong chap nhan file rong.\n");
            Halt();
        }

        // Tạo một socket
        socketId = SocketTCP();
        if (socketId == -1)
        {
            PrintString("Tao socket that bai!!!");
            Halt();
        }
        // Kết nối socket tới Server
        if (Connect(socketId, "127.0.0.1", 8080) == -1)
        {
            PrintString("Ket noi den server that bai!!! \n");
            Close(socketId);
            Halt();
        }
        // PrintString("Test close server, input a char: ");
        // ReadString(testCloseServer);
        // Gửi message từ client đến server
        resultSend = Send(socketId, messageSendToServer, fileSize);
        if (resultSend == 0)
        {
            PrintString("Ket noi bi dong.\n");
            Close(socketId);
            Halt();
        }
        else if (resultSend == -1)
        {
            PrintString("Loi gui message\n");
            Close(socketId);
            Halt();
        }
        // Nhận message tử server và trả về số byte nhận được
        resultGet = Receive(socketId, messageRecieveFromServer, resultSend);
        if (resultGet > 0)
        {
            PrintString("Nhan message thanh cong tu server. Noi dung nhan: \n");
            PrintString(messageRecieveFromServer);
        }
        else if (resultGet == 0)
        {
            PrintString("Ket noi bi dong\n");
            Close(socketId);
            Halt();
        }
        else if (resultGet == -1)
        {
            PrintString("Loi nhan message\n");
            Close(socketId);
            Halt();
        }
        // Đóng socket
        if (Close(socketId) == -1)
        {
            PrintString("Close socket that bai");
            Halt();
        }
    }
    else
    {
        PrintString("Mo file that bai!!\n\n");
        Halt();
    }

    // Write into file
    PrintString("\n * * * Nhap ten file de ghi message tu server: ");
    ReadString(fileName);
    openFileId = Open(fileName, 0);

    if (openFileId == -1)
    {
        if (Create(fileName) == -1)
        {
            PrintString("Tao file that bai\n");
            Halt();
        }
        else
        {
            PrintString("File nhan khong ton tai. Tao file thanh cong de ghi\n");
            openFileId = Open(fileName, 0);
        }
    }
    // Ghi số byte nhận được vào file có id là openFileId
    if (Write(messageRecieveFromServer, resultGet, openFileId) == -1)
    {
        PrintString("Ghi file bi loi !!!\n");
        Halt();
    }
    PrintString("Da ghi messsage vao file: ");
    PrintString(fileName);
    PrintString("\n");
    if (Close(openFileId) == 0)
    {
        PrintString("Dong file thanh cong\n");
    }
    else
    {
        PrintString("Loi dong file!!\n");
    }

    Halt();
}