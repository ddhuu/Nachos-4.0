#include "syscall.h"

int main()
{
    int resultSend;
    int resultReceive;
    int i = 0;
    char messageSend[255];
    char messageRecieved[255];
    char testCloseServer[255];
    int listSocketId[3];
    int length;
    PrintString("Nhap message can gui toi server: ");
    length = ReadString(messageSend);
    if (length == 0)
    {
        PrintString("Message khong hop le");
        Halt();
    }
    // Tạo 4 socket theo yêu cầu , gừi message tới server và trả về messsage đã được in hoa
    for (i = 0; i < 4; i++)
    {
        // Tạo một socket và trả về file descriptor Id, trả về -1 nếu lỗi
        listSocketId[i] = SocketTCP();
        if (listSocketId[i] == -1)
        {
            PrintString("Tao socket that bai !!!\n");
        }

        // Kiểm tra socket kết nối tới server thành công không , trả về 0 nếu thành công, -1 nếu thất bại
        if (Connect(listSocketId[i], "127.0.0.1", 8080) == -1)
        {
            PrintString("Ket noi toi server that bai !!! \n");
            continue;
        }
        // PrintString("Test close server. Hay nhap 1 ki tu bat ki\n");
        // ReadString(testCloseServer);
        // Gửi message và trả về số byte đã gửi, trả về 0 nếu Kết nỗi bị đóng, -1 nếu thất bại
        resultSend = Send(listSocketId[i], messageSend, length);
        if (resultSend == 0)
        {
            PrintString("Ket noi bi dong\n");
            continue;
        }
        else if (resultSend == -1)
        {
            PrintString("Gui message that bai, kiem tra lai\n");
            continue;
        }
        // Nhận dữ liệu từ socket và lưu và messageReceived
        resultReceive = Receive(listSocketId[i], messageRecieved, resultSend);
        if (resultReceive == -1)
        {
            PrintString("Nhan du lieu that bai!!!");
            continue;
        }
        else if (resultReceive == 0)
        {
            PrintString("Ket noi bi dong\n");
            continue;
        }
        PrintString(messageRecieved);
        PrintString("\n");
    }
    // Đóng socket
    for (i = 0; i < 4; i++)
    {
        if (Close(listSocketId[i]) == 0)
        {
            PrintString("Dong Socket thanh cong\n");
        }
        else
        {
            PrintString("Dong Socket that bai\n");
            Halt();
        }
    }
    Halt();
}