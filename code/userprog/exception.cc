

// exception.cc
//	Entry point into the Nachos kernel from user programs.
//	There are two kinds of things that can cause control to
//	transfer back to here from user code:
//
//	syscall -- The user code explicitly requests to call a procedure
//	in the Nachos kernel.  Right now, the only function we support is
//	"Halt".
//
//	exceptions -- The user code does something that the CPU can't handle.
//	For instance, accessing memory that doesn't exist, arithmetic errors,
//	etc.
//
//	Interrupts (which can also cause control to transfer from user
//	code into the Nachos kernel) are handled elsewhere.
//
// For now, this only handles the Halt() system call.
// Everything else core dumps.
//
// Copyright (c) 1992-1996 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "ksyscall.h"
#include "main.h"
#include "syscall.h"

//----------------------------------------------------------------------
// ExceptionHandler
// 	Entry point into the Nachos kernel.  Called when a user program
//	is executing, and either does a syscall, or generates an addressing
//	or arithmetic exception.
//
// 	For system calls, the following is the calling convention:
//
// 	system call code -- r2
//		arg1 -- r4
//		arg2 -- r5
//		arg3 -- r6
//		arg4 -- r7
//
//	The result of the system call, if any, must be put back into r2.
//
// If you are handling a system call, don't forget to increment the pc
// before returning. (Or else you'll loop making the same system call forever!)
//
//	"which" is the kind of exception.  The list of possible exceptions
//	is in machine.h.
//----------------------------------------------------------------------

// Input: - User space address (int)
// - Limit of buffer (int)
// Output:- Buffer (char*)
// Purpose: Copy buffer from User memory space to System memory space
char *User2System(int virtAddr, int limit)
{
	int i; // index
	int oneChar;
	char *kernelBuf = NULL;
	kernelBuf = new char[limit + 1]; // need for terminal string
	if (kernelBuf == NULL)
		return kernelBuf;
	memset(kernelBuf, 0, limit + 1);
	// printf("\n Filename u2s:");
	for (i = 0; i < limit; i++)
	{
		kernel->machine->ReadMem(virtAddr + i, 1, &oneChar);
		kernelBuf[i] = (char)oneChar;
		// printf("%c",kernelBuf[i]);
		if (oneChar == 0)
			break;
	}
	return kernelBuf;
}

// Input: - User space address (int)
// - Limit of buffer (int)
// - Buffer (char[])
// Output:- Number of bytes copied (int)
// Purpose: Copy buffer from System memory space to User memory space
int System2User(int virtAddr, int len, char *buffer)
{
	if (len < 0)
		return -1;
	if (len == 0)
		return len;
	int i = 0;
	int oneChar = 0;
	do
	{
		oneChar = (int)buffer[i];
		kernel->machine->WriteMem(virtAddr + i, 1, oneChar);
		i++;
	} while (i < len && oneChar != 0);
	return i;
}

void ExceptionHandler(ExceptionType which)
{
	int type = kernel->machine->ReadRegister(2);

	// DEBUG(dbgSys, "Received Exception " << which << " type: " << type << "\n");

	switch (which)
	{
	case NoException:
		return;

	case SyscallException:
	{
		switch (type)
		{
		case SC_Halt:
		{
			DEBUG(dbgSys, "Shutdown, initiated by user program.\n");
			SysHalt();
			ASSERTNOTREACHED();
			break;
		}

		case SC_Create:
		{
			// Input: file name
			// Output: trả về 0 nếu tạo file thành công, -1 nếu thât bại
			char *filename;
			int filenameAddr = kernel->machine->ReadRegister(4);
			filename = User2System(filenameAddr, MAXLENGTH);
			int result = SysCreate(filename);
			if (result == -1)
			{
				kernel->machine->WriteRegister(2, -1);
			}
			else
			{
				kernel->machine->WriteRegister(2, 0);
			}
			delete filename;
			IncreasePC();
			break;
		}

		case SC_Remove:
		{
			int virtAddr;
			char *filename;
			virtAddr = kernel->machine->ReadRegister(4);
			filename = User2System(virtAddr, MAXLENGTH);
			int result = SysRemove(filename);
			if (result == -1)
			{
				kernel->machine->WriteRegister(2, -1);
			}
			else
			{
				kernel->machine->WriteRegister(2, 0);
			}
			delete filename;
			IncreasePC();
			break;
		}

		case SC_Open:
		{
			// Input: FileName, Type (mode)
			// Output: Trả ve openFileId nếu mở file thành cêng, -1 nếu lỗi
			int virtAddr = kernel->machine->ReadRegister(4);
			int type = kernel->machine->ReadRegister(5);
			char *filename;
			if (type != 0 && type != 1)
			{
				kernel->machine->WriteRegister(2, -1);
				DEBUG(dbgSys, "\n Type is not accepted.");
				IncreasePC();
				ASSERTNOTREACHED();
			}
			// Copy chuoi tu vung nho User Space sang System Space voi bo dem name
			filename = User2System(virtAddr, 32);
			int openFileID = SysOpen(filename, type);
			if (openFileID == -1)
			{
				kernel->machine->WriteRegister(2, -1);
				DEBUG(dbgSys, "\n Mo file that bai, co the do loi hoac khong con slot");
			}
			else
			{
				kernel->machine->WriteRegister(2, openFileID); // tra ve OpenFileID
				DEBUG(dbgSys, "\n Mo file thanh cong!!! ID File: " << openFileID);
			}
			delete[] filename;
			IncreasePC();
			break;
		}

		case SC_Close:
		{
			// Input : openFileId hoặc SocketId
			//  Output: Đóng file trả về 0 nếu thành công, -1 thất bại
			int fileID = kernel->machine->ReadRegister(4);
			int result = SysClose(fileID);
			if (result == -1)
			{
				kernel->machine->WriteRegister(2, -1);
			}
			else
			{
				kernel->machine->WriteRegister(2, 0);
			}
			IncreasePC();
			break;
		}

		case SC_Read:
		{
			// Input: buffer(char*), so ky tu(size), openFileId
			// Output: -1 nếu lỗi, trả về đã đọc nếu thành công
			int virtAddr = kernel->machine->ReadRegister(4);
			int size = kernel->machine->ReadRegister(5);
			int openFileID = kernel->machine->ReadRegister(6);
			// *buf này đẻ lưu input từ người dùng
			char *buffer = User2System(virtAddr, size);

			int result = SysRead(buffer, size, openFileID);
			if (result > 0)
			{
				kernel->machine->WriteRegister(2, result);
				// Trả về dữ liệu input từ buf(của hệ thống) về cho người dùng -> truyền vào virtAddr
				System2User(virtAddr, size + 1, buffer);
			}
			else
			{
				kernel->machine->WriteRegister(2, result);
			}
			delete buffer;
			IncreasePC();
			break;
		}

		case SC_Write:
		{
			// Input: buffer(char*), so ky tu(size), openFileId
			// Output: -1 nếu lỗi, trả về đã viết được nếu thành công
			int virtAddr = kernel->machine->ReadRegister(4);
			int size = kernel->machine->ReadRegister(5);
			int openFileId = kernel->machine->ReadRegister(6);

			char *buffer = User2System(virtAddr, size);
			int result = SysWrite(buffer, size, openFileId);
			if (result == -1)
			{
				kernel->machine->WriteRegister(2, -1);
			}
			else
			{
				kernel->machine->WriteRegister(2, result);
			}
			IncreasePC();
			delete buffer;
			break;
		}

		case SC_Seek:
		{
			// Input: Vị trí(position), openFileId
			// Output: -1 nếu lỗi, trả về vị tri(seek) con trỏ trong file nếu thành công
			int pos = kernel->machine->ReadRegister(4);
			int openFileID = kernel->machine->ReadRegister(5);

			int result = SysSeek(pos, openFileID);
			if (result == -1)
			{
				kernel->machine->WriteRegister(2, -1);
			}
			else
			{
				kernel->machine->WriteRegister(2, result);
			}
			IncreasePC();
			break;
		}

		case SC_SocketTCP:
		{
			// Input: không
			// Output: 0 nếu thành công , -1 nếu thất bại
			// Tạo socket và trả về file descriptor id hoặc -1 nếu lỗi.
			int result = kernel->fileSystem->OpenNewSocket();
			if (result != -1)
			{
				DEBUG(dbgSys, "Ket qua socket tcp: " << result);
				kernel->machine->WriteRegister(2, result);
			}
			else
			{
				kernel->machine->WriteRegister(2, -1);
				DEBUG(dbgSys, "Loi tao socket do tao that bao hoac het slot");
			}
			IncreasePC();
			break;
		}

		case SC_Connect:
		{
			// Input: socketId, ipAddess, port
			// Output: 0 nếu kết nỗi đến server thành công, -1 nếu thất bại
			int socketId = kernel->machine->ReadRegister(4);
			int virtAddr = kernel->machine->ReadRegister(5);
			int port = kernel->machine->ReadRegister(6);
			char *ipAddress = User2System(virtAddr, MAXLENGTH);
			int result = kernel->fileSystem->Connect(socketId, ipAddress, port);
			if (result == -1)
			{
				kernel->machine->WriteRegister(2, -1);
				DEBUG(dbgSys, "Loi ket noi den Server.");
			}
			else
			{
				kernel->machine->WriteRegister(2, 0);
			}
			delete ipAddress;
			IncreasePC();
			break;
		}

		case SC_Send:
		{
			// Input: socketId, message cần gửi
			// Output: Trả về số byte gửi thành công đến server hoặc -1 nếu lỗi
			int socketId = kernel->machine->ReadRegister(4);
			int virtAddr = kernel->machine->ReadRegister(5);
			int length = kernel->machine->ReadRegister(6);
			char *buffer = User2System(virtAddr, MAXLENGTH);
			int result = kernel->fileSystem->SendSocketMessage(socketId, buffer, length);
			DEBUG(dbgSys, "Sent bytes: " << result);
			kernel->machine->WriteRegister(2, result);
			delete buffer;
			IncreasePC();
			break;
		}

		case SC_Receive:
		{
			// Input: socketId , message từ server
			// Output: Trả về số byte nhận được từ server hoặc trả về -1 nếu thất bại
			int socketId = kernel->machine->ReadRegister(4);
			int virtAddr = kernel->machine->ReadRegister(5);
			int length = kernel->machine->ReadRegister(6);
			char *buffer = User2System(virtAddr, MAXLENGTH);

			int result = kernel->fileSystem->ReceiveSocketMessage(socketId, buffer, length);
			if (result == -1)
			{
				kernel->machine->WriteRegister(2, -1);
				DEBUG(dbgSys, "Nhan that bai: ");
			}
			else
			{
				kernel->machine->WriteRegister(2, result);
				System2User(virtAddr, strlen(buffer) + 1, buffer);
			}
			delete buffer;
			IncreasePC();
			break;
		}

		case SC_ReadString:
		{
			// Đọc chuỗi từ input người dùng và lưu vào user space (virtAddr)
			int virtAddr = kernel->machine->ReadRegister(4);
			char *buffer = SysReadString();
			int length = strlen(buffer);
			System2User(virtAddr, length + 1, buffer);
			kernel->machine->WriteRegister(2, length);
			delete[] buffer;
			buffer = NULL;
			IncreasePC();
			break;
		}

		case SC_PrintString:
		{
			// Xuất chuỗi ra màn hình console
			int memPtr = kernel->machine->ReadRegister(4);
			char *buffer = User2System(memPtr, MAXLENGTH);
			SysPrintString(buffer, strlen(buffer));
			delete[] buffer;
			IncreasePC();
			break;
		}

		case SC_Exit:
		{
			int id = kernel->machine->ReadRegister(4);
			IncreasePC();
			break;
		}

		default:
			break;
		}

		break;
	}

	default:
		cerr << "Unexpected user mode exception" << (int)which << "\n";
		break;
	}
	// ASSERTNOTREACHED();
}