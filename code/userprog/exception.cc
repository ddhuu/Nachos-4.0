

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
#include "main.h"
#include "syscall.h"
#include "ksyscall.h"
#include "synchconsole.h"
#include "stdint.h"
#include "time.h"
#include "STable.h"
#include "PTable.h"

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





//-------------------PROJECT 02 --------------------//

char **User2System2(int virtAddr, int argc, int limit)
{
	int i;
	int oneChar;
	char **kernelBuf = new char*[argc+1];

	for (i = 0; i < argc; i++)
	{
		
		int argi;
		kernel->machine->ReadMem(virtAddr + i * 4, 4, &argi);
		kernelBuf[i] = new char[limit];

		kernelBuf[i] = User2System(argi,MAXLENGTH);
	}

	return kernelBuf;
}



void Handle_Exec()
{

	int addr = kernel->machine->ReadRegister(4);
	char *fileName;
	fileName = User2System(addr, MAXLENGTH);

	int result = SysExec(fileName);
	
	kernel->machine->WriteRegister(2, result);
	delete fileName;
}


void Handle_Join()
{
	int id = kernel->machine->ReadRegister(4);
	int result = kernel->pTab->JoinUpdate(id);
	kernel->machine->WriteRegister(2, result);
}


void Handle_Exit()
{
	int exitCode = kernel->machine->ReadRegister(4);
	int result = kernel->pTab->ExitUpdate(exitCode);
}


void Handle_CreateSemaphore()
{

	int virtAddr = kernel->machine->ReadRegister(4); 
	int semVal = kernel->machine->ReadRegister(5);	 
	char *name = User2System(virtAddr, MAXLENGTH); 
	
	if(name == NULL)
	{
		printf("\nNot enough memory in System");
		kernel->machine->WriteRegister(2, -1);
		delete[] name;
		return;
	}
	
	int res = kernel->semTab->Create(name, semVal);

	if(res == -1)
	{
		printf("\nCan not create semaphore");
	}
	
	delete[] name;	
	kernel->machine->WriteRegister(2, res);
	return;
}


void Handle_Wait()
{
	int virtAddr = kernel->machine->ReadRegister(4);
	char *name = User2System(virtAddr, MAXLENGTH);

	if(name == NULL)
	{
		printf("\nNot enough memory in System");
		kernel->machine->WriteRegister(2, -1);
		delete[] name;
		return;
	}

	int res = kernel->semTab->Wait(name);
	
	if(res == -1)
	{
		printf("\nNot exists semaphore");
	}

	delete[] name;
	kernel->machine->WriteRegister(2, res);
	return;
}


void Handle_Signal()
{
	int virtAddr = kernel->machine->ReadRegister(4);
	char *name = User2System(virtAddr, MAXLENGTH);

	if(name == NULL)
	{
		printf("\n Not enough memory in System");
		kernel->machine->WriteRegister(2, -1);
		delete[] name;
		return;
	}
	
	int res = kernel->semTab->Signal(name);

	if(res == -1)
	{
		printf("\nNot exists semaphore");
	}
	
	delete[] name;
	kernel->machine->WriteRegister(2, res);
	return;
}

void Handle_PrintNum()
{
	
	int number = kernel->machine->ReadRegister(4);

	/*int: [-2147483648 , 2147483647] --> max buffer = 11*/
	const int MAX_BUFFER = 11;
	char *num_buffer = new char[MAX_BUFFER];

	// make a temp array full with 0
	int temp[MAX_BUFFER] = {0};

	// index counter
	int i, j;
	i = j = 0;

	bool isPositive = true;

	// negative number
	if (number < 0)
	{
		number = -number;
		num_buffer[i] = '-';
		i++;
		isPositive = false;
	}

	// save each num in number from end to start into temp array
	do
	{
		temp[j] = number % 10;
		number /= 10;
		j++;
	} while (number);

	int length = isPositive ? j : j + 1; // real buffer size for number

	while (j)
	{
		j--;
		num_buffer[i] = '0' + (char)temp[j];
		i++;
	}

	// print the result to console
	for (int i = 0; i < length; i++)
		kernel->synchConsoleOut->PutChar(num_buffer[i]);
}

void Handle_ExecV() {
	int argc1 = kernel->machine->ReadRegister(4);
	int argv1 = kernel->machine->ReadRegister(5); 
	
	int numberArg = argc1;
	
	char ** argv= User2System2(argv1,numberArg,MAXLENGTH);
	char*program=argv[0];
	cout<<"\n"<<argv[0]<<endl;
	int result = SysExecV(program, argv);

	for (int i = 0; i < numberArg; i++)
	{
		delete[] argv[i];
	}

	kernel->machine->WriteRegister(2, result);
}

void ExceptionHandler(ExceptionType which)
{
	int type = kernel->machine->ReadRegister(2);

	// DEBUG(dbgSys, "Received Exception " << which << " type: " << type << "\n");

	switch (which)
	{
	case NoException:
		return;
    case PageFaultException:
		DEBUG(dbgSys, " PageFaultException");
		kernel->interrupt->Halt();
		break;

	case ReadOnlyException:
		DEBUG(dbgSys, "\n ReadOnlyException");
		kernel->interrupt->Halt();
		break;

	case BusErrorException:
		DEBUG(dbgSys, "\n BusErrorException");
		kernel->interrupt->Halt();
		break;

	case AddressErrorException:
		DEBUG(dbgSys, "\n AddressErrorException");
		kernel->interrupt->Halt();
		break;

	case OverflowException:
		DEBUG(dbgSys, "\n OverflowException");
		kernel->interrupt->Halt();
		break;

	case IllegalInstrException:
		DEBUG(dbgSys, "\n IllegalInstrException");
		kernel->interrupt->Halt();
		break;

	case NumExceptionTypes:
		DEBUG(dbgSys, "\n NumExceptionTypes");
		kernel->interrupt->Halt();
		break;
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

		case SC_Exec:
		{
			Handle_Exec();
			IncreasePC();
			break;
		}

		case SC_Join:
		{
			Handle_Join();
			IncreasePC();
			break;
		}

		case SC_Exit:
		{
			Handle_Exit();
			IncreasePC();
			break;
		}

		case SC_CreateSemaphore:
		{
			Handle_CreateSemaphore();
			IncreasePC();
			break;
		}

		case SC_Wait:
		{
			Handle_Wait();
			IncreasePC();
			break;
		}

		case SC_Signal:
		{
			Handle_Signal();
			IncreasePC();
			break;
		}
		case SC_PrintNum:
		{
			Handle_PrintNum();
			IncreasePC();
			break;
		}
		case SC_ExecV:
		{
			Handle_ExecV();
			IncreasePC();
			break;
		}

		default:
			break;
		}

		break;
	}

	default:
		cerr << "" << (int)which << "\n";
		break;
	}
	// ASSERTNOTREACHED();
}