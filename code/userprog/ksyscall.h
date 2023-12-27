/**************************************************************
 *
 * userprog/ksyscall.h
 *
 * Kernel interface for systemcalls
 *
 * by Marcus Voelp  (c) Universitaet Karlsruhe
 *
 **************************************************************/

#ifndef __USERPROG_KSYSCALL_H__
#define __USERPROG_KSYSCALL_H__

#include "kernel.h"
#include "synchconsole.h"

#define MAXLENGTH 255

//------------------
void SysHalt()
{
  kernel->interrupt->Halt();
}

int SysAdd(int op1, int op2)
{
  return op1 + op2;
}

// Tăng giá trị program counter cho mỗi lần gọi system call
void IncreasePC()
/* Modify return point */
{
  /* set previous programm counter (debugging only)*/
  kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));

  /* set programm counter to next instruction (all Instructions are 4 byte wide)*/
  kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4);

  /* set next programm counter for brach execution */
  kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg) + 4);
}

// Đọc kí tự input được nhập từ bàn phím
char SysReadChar() { return kernel->synchConsoleIn->GetChar(); }

// Đọc chuỗi input từ bàn phím và trả về mảng chứa dữ liệu đã nhập
char *SysReadString()
{
  // Tạo buffer đẻ lưu dữ liệu nhập, giả sử chỉ cho phép tối đa 255 kí tự
  char *newBuffer = new char[255];
  int realSize = 0;
  for (int i = 0; i < 255; i++)
  {
    char c = SysReadChar();
    // Nếu người nhập nhập kí tự Enter thì dừng
    if (c == '\n')
    {
      break;
    }
    else
    {
      newBuffer[i] = c;
      realSize++;
    }
  }
  // Tạo một buffer mới có kích thước vừa đủ chữa dữ liệu đã input và xóa buffer được cấp phát dư thừa
  char *buffer = new char[realSize + 1];
  for (int i = 0; i < realSize; i++)
  {
    buffer[i] = newBuffer[i];
  }
  buffer[realSize] = '\0';
  delete newBuffer;
  newBuffer = NULL;
  return buffer;
}

// Xuát kí tự chỉ định ra màn hình console
void SysPrintChar(char character)
{
  kernel->synchConsoleOut->PutChar(character);
}
// In chuỗi kí tự hoặc mảng chuỗi ra màn hình console
void SysPrintString(char *buffer, int length)
{
  // Xuất length kí tự ra màn hình console bằng cách xuất từng kí tự
  int i = 0;
  while (i < length)
  {
    SysPrintChar(buffer[i++]);
  }
}

// Tạo một file chỉ định, trả về 1 nếu tạo file thành công, -1 nếu tạo thất bại
int SysCreate(char *fileName)
{
  int result = 1;
  int fileNameLength = strlen(fileName);

  if (fileNameLength == 0)
  {
    DEBUG(dbgSys, "\nTen file khong duoc de trong");
    result = -1;
  }
  else if (fileName == NULL)
  {
    DEBUG(dbgSys, "\nKhong du bo nho de cap phat");
    result = -1;
  }
  else
  {
    if (!kernel->fileSystem->Create(fileName))
    {
      DEBUG(dbgSys, "\nTao file that bai");
      result = -1;
    }
    else
    {
      DEBUG(dbgSys, "\nTao file thanh cong");
    }
  }
  return result;
}

// Trả về 1 nếu xóa file thành công, -1 nếu thất bại
int SysRemove(char *filename)
{
  // Kiểm tra filename có hợp lệ
  if (filename == NULL || strlen(filename) < 1)
  {
    return -1;
  }

  // Kiểm tra xem file có đang mở không
  OpenFile *fileOpening = kernel->fileSystem->GetFileDescriptorByName(filename);
  if (fileOpening != NULL)
  {
    DEBUG(dbgSys, "File is opening !! Close file to remove !!");
    return -1;
  }
  // Xóa file
  if (kernel->fileSystem->Remove(filename))
  {
    return 1;
  }
  return -1;
}

// Trả về fileID nếu thành công, -1 nếu thất bại
int SysOpen(char *filename, int type)
{
  if (filename == NULL || strlen(filename) < 1)
  {
    DEBUG(dbgSys, "Filename khong hop le");
    return -1;
  }

  // Kiểm tra xem file đã được mở trước đó chưa
  int openFileId = kernel->fileSystem->GetFileDescriptorID(filename);
  if (openFileId != -1)
  {
    DEBUG(dbgSys, "File " << filename << " da mo truoc do");
    return openFileId;
  }

  // Kiem tra xem con slot de open file khong
  int emptySlot = kernel->fileSystem->GetFileDescriptorEmptySlot();
  if (emptySlot == -1)
  {
    DEBUG(dbgSys, "Khong du slot de mo file");
    return -1;
  }

  // Mở file thành công - trả về emptySlot cũng chính là trả về fileID của file được mở
  if (kernel->fileSystem->AddToFileDescriptorTable(emptySlot, filename, type) != NULL)
  {
    return emptySlot;
  }
  else
  {
    // Mở file không thành công
    return -1;
  }
}

// Trả về 0 nếu đóng file hoặc đóng Socket thành công , -1 nếu thất bại
int SysClose(int openFileID)
{
  if (kernel->fileSystem->RemoveFileDescriptor(openFileID) == true)
  {
    return 0;
  }
  return -1;
}

// Trả về số char đọc được, trả về -1 nếu lỗi
int SysRead(char *buffer, int size, int openFileID)
{
  if (openFileID < 0 || openFileID >= 20)
  {
    DEBUG(dbgSys, "\nKhong the read vi openFileId khong hop le.");
    return -1;
  }
  // Kiểm tra file có đang mở không
  if (kernel->fileSystem->GetFileDescriptorByID(openFileID) == NULL)
  {
    DEBUG(dbgSys, "\nKhong the read vi file nay chua open.");
    return -1;
  }

  // Xét trường hợp đọ file ConsoleOutput thi trả về -1
  if (openFileID == 1)
  {
    DEBUG(dbgSys, "\nKhong the read file ConsoleOuput.");
    return -1;
  }

  // Xét trường hợp open ConsoleInput, dùng lớp SynchConsoleInput để đọc
  if (openFileID == 0)
  {
    return kernel->synchConsoleIn->GetString(buffer, size);
  }
  else
  {
    // Dùng đối tượng OpenFile đọc nội dung file vào buffer và trả về số byte đọc được
    return kernel->fileSystem->Read(buffer, size, openFileID);
  }
}

// Seek con trỏ tới vị trí chỉ định, trả về 0 nếu thành công. -1 nếu thât bại
int SysSeek(int pos, int openFileId)
{
  // Kiểm tra xem openFileId có phải là ConsoleInput hay ConsoleOutput không
  if (openFileId == 0 || openFileId == 1)
  {
    DEBUG(dbgSys, "\nKhong the seek file Console IO.");
    return -1;
  }
  if (openFileId < 0 || openFileId >= MAX_FILE)
  {
    DEBUG(dbgSys, "\nKhong the seek vi openFileID khong hop le.");
    return -1;
  }
  // Kiểm tra xem file có đang open không
  if (kernel->fileSystem->GetFileDescriptorByID(openFileId) == NULL)
  {
    DEBUG(dbgSys, "\nKhong the seek file vi file chua open.");
    return -1;
  }
  // Trả về kểt quả seek - vị trí con trỏ trong file
  return kernel->fileSystem->Seek(pos, openFileId);
}

int SysWrite(char *buffer, int size, int openFileId)
{
  if (openFileId < 0 || openFileId >= 20)
  {
    DEBUG(dbgSys, "\nKhong the read vi openFileId khong hop le.");
    return -1;
  }
  // Kiểm tra file có đang mở không
  if (kernel->fileSystem->GetFileDescriptorByID(openFileId) == NULL)
  {
    DEBUG(dbgSys, "\nKhong the read vi file nay chua open.");
    return -1;
  }

  // xét những trường hợp openFileId đặc biệt Console IO
  if (openFileId == 0)
  {
    DEBUG(dbgFile, "ConsoleInput can't be written !!\n");
    return -1;
  }
  // consoleOUT
  else if (openFileId == 1)
  {
    return kernel->synchConsoleOut->PutString(buffer, size);
  }

  OpenFile *file2write = kernel->fileSystem->GetFileDescriptorByID(openFileId);
  if (file2write->GetFileType() == 1)
  {
    DEBUG(dbgFile, "Read-only files can't be written !!\n");
    return -1;
  }
  return kernel->fileSystem->Write(buffer, size, openFileId);
}

#endif /* ! __USERPROG_KSYSCALL_H__ */
