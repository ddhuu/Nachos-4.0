// filesys.h
//	Data structures to represent the Nachos file system.
//
//	A file system is a set of files stored on disk, organized
//	into directories.  Operations on the file system have to
//	do with "naming" -- creating, opening, and deleting files,
//	given a textual file name.  Operations on an individual
//	"open" file (read, write, close) are to be found in the OpenFile
//	class (openfile.h).
//
//	We define two separate implementations of the file system.
//	The "STUB" version just re-defines the Nachos file system
//	operations as operations on the native UNIX file system on the machine
//	running the Nachos simulation.
//
//	The other version is a "real" file system, built on top of
//	a disk simulator.  The disk is simulated using the native UNIX
//	file system (in a file named "DISK").
//
//	In the "real" implementation, there are two key data structures used
//	in the file system.  There is a single "root" directory, listing
//	all of the files in the file system; unlike UNIX, the baseline
//	system does not provide a hierarchical directory structure.
//	In addition, there is a bitmap for allocating
//	disk sectors.  Both the root directory and the bitmap are themselves
//	stored as files in the Nachos file system -- this causes an interesting
//	bootstrap problem when the simulated disk is initialized.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation
// of liability and disclaimer of warranty provisions.

#ifndef FS_H
#define FS_H

#include "copyright.h"
#include "openfile.h"
#include "sysdep.h"

#define MAX_FILE 20		 // 20 file descriptors
#define plusSocketId 100 // Tăng Id của file Socket để tránh bị trùng với openFileId

#ifdef FILESYS_STUB // Temporarily implement file system calls as
// calls to UNIX, until the real file system
// implementation is available
class FileSystem
{
public:
	FileSystem()
	{
		// Tạo 20 file descriptor với 2 file đầu dành cho Console IO
		fileDescriptorTable = new OpenFile *[MAX_FILE];

		for (int i = 0; i < MAX_FILE; i++)
		{
			fileDescriptorTable[i] = NULL;
		}
		this->Create("ConsoleInput");							 // phần tử 0 -> ConsoleInput
		this->Create("ConsoleOutput");							 // phần tử 1 -> ConsoleOutput
		fileDescriptorTable[0] = this->Open("ConsoleInput", 0);	 // 0 cho write -read
		fileDescriptorTable[1] = this->Open("ConsoleOutput", 1); // Mode 1 cho read - only
	}

	~FileSystem()
	{
		for (int i = 0; i < MAX_FILE; i++)
		{
			if (fileDescriptorTable[i] != NULL)
			{
				delete fileDescriptorTable[i];
				fileDescriptorTable[i] = NULL;
			}
		}
		delete[] fileDescriptorTable;
		Remove("ConsoleInput");
		Remove("ConsoleOutput");
	}

	//  --------------------------------  Quản lý phần File ------------------------------------------
	// Tạo mới một file và trả về true nếu tạo file thành công
	bool Create(char *name)
	{
		int fileDescriptor = OpenForWrite(name);

		if (fileDescriptor == -1)
			return FALSE;
		Close(fileDescriptor);
		return TRUE;
	}

	// Xóa file chỉ định và trả về true nếu xóa file thành công
	bool Remove(char *name) { return Unlink(name) == 0; }

	OpenFile *Open(char *name)
	{
		int fileDescriptor = OpenForReadWrite(name, FALSE);

		if (fileDescriptor == -1)
			return NULL;
		return new OpenFile(fileDescriptor);
	}

	// Mở một file với tên và kiểu read-write hoặc read-only và trả về đỗi tượng đó, trả về NULL nếu thất bại
	OpenFile *Open(char *name, int mode)
	{
		int fileDescriptor = 0;
		if (mode == 0)
		{
			fileDescriptor = OpenForReadWrite(name, FALSE);
		}
		else if (mode == 1)
		{
			fileDescriptor = OpenForRead(name, FALSE);
		}

		if (fileDescriptor == -1)
			return NULL;
		return new OpenFile(fileDescriptor, name, mode);
	}

	// Đòng file thành công trả về 0, trả về -1 nếu thất bại
	int Close(int fileId) // fileId có thể là openFileId hoặc là Socket id
	{
		if (fileId < 2 || fileId >= MAX_FILE)
		{
			return -1;
		}

		if (fileDescriptorTable[fileId] != NULL)
		{
			delete fileDescriptorTable[fileId];
			fileDescriptorTable[fileId] = NULL;
			return 0;
		}
		return -1;
	}

	// Trả về số byte đã đọc được, -1 nếu thát bại
	int Read(char *buffer, int charCount, int openFileId)
	{
		if (openFileId < 0 || openFileId >= 20)
			return -1;
		if (fileDescriptorTable[openFileId] == NULL)
			return -1;
		int result = fileDescriptorTable[openFileId]->Read(buffer, charCount);
		// Neu doc k du byte tu ham Read cua OpenFile, return -1
		if (result <= 0)
			return -1;
		return result;
	}

	// Viêt số kí tự (charCount) kí tự vào đối tượng có openFileId chỉ định, trả về số kí tự viết được hoặc -1 nếu thất bại
	int Write(char *buffer, int charCount, int openFileId)
	{
		if (openFileId >= 20)
			return -1;
		if (fileDescriptorTable[openFileId] == NULL || openFileId == 1)
			return -1;
		int result = fileDescriptorTable[openFileId]->Write(buffer, charCount);
		if (result <= 0)
		{
			return -1;
		}
		return result;
	}

	// Xóa một đối tượng OpenFile tại vị trí fileId của fileDescriptorTable, trả về true nếu thành công
	// Nếu fileId là một số lón thì đó là socket id
	bool RemoveFileDescriptor(int fileId)
	{
		if (fileId > MAX_FILE || fileId < 0)
		{
			// Kiểm tra xem fileId có phải là socket id
			for (int i = 2; i < MAX_FILE; i++)
			{
				if (fileDescriptorTable[i] != NULL)
				{
					if (fileDescriptorTable[i]->isSocket())
					{
						if (fileDescriptorTable[i]->socketId == fileId)
						{
							CloseSocket(fileId - plusSocketId);
							delete fileDescriptorTable[i];
							fileDescriptorTable[i] = NULL;
							char socketName[50] = "SocketFileId";
							sprintf(socketName + strlen(socketName), "%d", fileId);
							Remove(socketName);
							return true;
						}
					}
				}
			}
			return false;
		}
		if (fileDescriptorTable[fileId] != NULL)
		{
			delete fileDescriptorTable[fileId];
			fileDescriptorTable[fileId] = NULL;
			return true;
		}
		return false;
	}

	// Thêm đối tượng OpenFile có fileName vào index chỉ định trong fileDescriptorTable
	OpenFile *AddToFileDescriptorTable(int index, char *filename, int mode)
	{
		if (fileDescriptorTable[index] == NULL)
		{
			fileDescriptorTable[index] = Open(filename, mode);
			return fileDescriptorTable[index];
		}
		return NULL;
	}

	// Kiểm tra xem fileDescriptorTable còn slot trống không
	int GetFileDescriptorEmptySlot()
	{
		for (int i = 2; i < MAX_FILE; i++)
		{
			if (fileDescriptorTable[i] == NULL)
				return i;
		}
		return -1;
	}

	// Lấy đối tượng OpenFile thông qua openFileId, trả về đỗi tượng nếu file đang mở(tồn tại trong fileDescriptorTable) , trả về NULL nếu thât bại
	OpenFile *GetFileDescriptorByID(int openFileID)
	{
		if (openFileID < 0 || openFileID >= MAX_FILE)
		{
			return NULL;
		}
		return fileDescriptorTable[openFileID];
	}

	// Lấy đối tượng OpenFile thông qua tên file, trả về đỗi tượng nếu file đang mở(tồn tại trong fileDescriptorTable) , trả về NULL nếu thât bại
	OpenFile *GetFileDescriptorByName(char *filename)
	{
		for (int i = 0; i < MAX_FILE; i++)
		{
			if (fileDescriptorTable[i] != NULL)
			{
				char *openfileName = fileDescriptorTable[i]->GetFileName();
				if (openfileName != NULL && strcmp(openfileName, filename) == 0)
				{
					return fileDescriptorTable[i];
				}
			}
		}
		return NULL;
	}

	// Trả về OpenFileId thông qua filename, trả về -1 nếu không tìm thấy
	int GetFileDescriptorID(char *filename)
	{
		for (int i = 0; i < MAX_FILE; i++)
		{
			if (fileDescriptorTable[i] != NULL)
			{
				char *openfileName = fileDescriptorTable[i]->GetFileName();
				if (openfileName != NULL && strcmp(openfileName, filename) == 0)
				{
					return i;
				}
			}
		}
		return -1;
	}

	// Seek con trỏ tới vị trí pos chỉ định đối với đối tượng OpenFile có id la openFileId
	int Seek(int pos, int openFileId)
	{
		if (openFileId <= 1 || openFileId >= 20)
			return -1;
		if (fileDescriptorTable[openFileId] == NULL)
			return -1;
		// use seek(-1) to move to the end of file
		if (pos == -1)
			pos = fileDescriptorTable[openFileId]->Length();
		if (pos < -1 || pos > fileDescriptorTable[openFileId]->Length())
			return -1;
		return fileDescriptorTable[openFileId]->Seek(pos);
	}
	//  ---------------------------- Kết thúc phần Quản lý File ------------------------------------------

	//  --------------------------------  Quản lý phần Socket ------------------------------------------
	// Mở một socket và trả về vị trí socket nếu list Socket còn slot, nếu hết slot trả về -1
	int OpenNewSocket()
	{
		int emptySlot = GetFileDescriptorEmptySlot();
		if (emptySlot == -1)
		{
			return -1;
		}
		// Tăng socketId để tránh bị trùng với openFileId
		int socketDescriptor = OpenSocketTCP() + plusSocketId;
		char socketName[50] = "SocketFileId";
		sprintf(socketName + strlen(socketName), "%d", socketDescriptor);
		Create(socketName);
		fileDescriptorTable[emptySlot] = Open(socketName, 1);
		fileDescriptorTable[emptySlot]->socketId = socketDescriptor;
		return socketDescriptor;
	}

	// Connect socket tới server trả về 0 nếu thành công hoặc -1 nếu lỗi
	int Connect(int socketDescriptor, char *ip, int port)
	{
		return ConnectSocket(socketDescriptor - plusSocketId, ip, port);
	}

	// Gửi message tới server thông qua id của nó(file descriptor id), trả về số byte đã gửi hoặc -1 nếu lỗi gửi thất bại hoặc không tồn tại id trong list socket
	int SendSocketMessage(int socketDescriptor, char *buffer, int length)
	{
		for (int i = 0; i < 20; i++)
		{
			if (fileDescriptorTable[i]->isSocket())
			{
				if (fileDescriptorTable[i]->socketId == socketDescriptor)
				{
					int result = SendMessageToSocket(socketDescriptor - plusSocketId, buffer);
					if (result <= length)
					{
						return result;
					}
				}
			}
		}
		return -1;
	}

	// Trả về số byte nhận được nếu nhận thành công, trả về -1 nếu lỗi hoặc socket id không tồn tại trong list socket
	int ReceiveSocketMessage(int socketDescriptor, char *buffer, int length)
	{
		for (int i = 0; i < 20; i++)
		{
			if (fileDescriptorTable[i]->isSocket())
			{
				if (fileDescriptorTable[i]->socketId == socketDescriptor)
				{
					int result = RecieveMessageFromSocket(socketDescriptor - plusSocketId, buffer);
					if (result <= length)
					{
						return result;
					}
				}
			}
		}
		return -1;
	}
	//  ----------------------------  Kết thúc phần Quản lý Socket --------------------------------------

private:
	OpenFile **fileDescriptorTable; // mảng file descriptor table
};
#else // FILESYS
class FileSystem
{
public:
	FileSystem(bool format); // Initialize the file system.
							 // Must be called *after* "synchDisk"
							 // has been initialized.
							 // If "format", there is nothing on
							 // the disk, so initialize the directory
							 // and the bitmap of free blocks.

	bool Create(char *name, int initialSize);
	// Create a file (UNIX creat)

	OpenFile *Open(char *name); // Open a file (UNIX open)

	bool Remove(char *name); // Delete a file (UNIX unlink)

	void List(); // List all the files in the file system

	void Print(); // List all the files and their contents

private:
	OpenFile *freeMapFile;	 // Bit map of free disk blocks,
							 // represented as a file
	OpenFile *directoryFile; // "Root" directory -- list of
							 // file names, represented as a file
};

#endif // FILESYS

#endif // FS_H
