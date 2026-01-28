#include <ntifs.h>

void DriverUnload(PDRIVER_OBJECT pDriver) {
	DbgPrint("DriverUnload!!!");
}

// ZwWriteFile 写入函数详解：
/*
NTSYSAPI NTSTATUS ZwWriteFile(
	_In_        HANDLE           FileHandle,           // 输入：文件句柄
	_In_opt_    HANDLE           Event,                // 输入（可选）：事件对象句柄
	_In_opt_    PIO_APC_ROUTINE  ApcRoutine,           // 输入（可选）：APC例程
	_In_opt_    PVOID            ApcContext,           // 输入（可选）：APC上下文
	_Out_       PIO_STATUS_BLOCK IoStatusBlock,        // 输出：I/O状态块
	_In_        PVOID            Buffer,               // 输入：要写入的数据缓冲区
	_In_        ULONG            Length,               // 输入：要写入的字节数
	_In_opt_    PLARGE_INTEGER   ByteOffset,           // 输入（可选）：写入起始偏移
	_In_opt_    PULONG           Key                   // 输入（可选）：键值（用于字节范围锁）
);
*/
void FileOperate() {
	// -----------------------------------------------------------------------------------------------------------
	// 创建文件：
	HANDLE hwrite = NULL;
	OBJECT_ATTRIBUTES attr = { 0 };
	UNICODE_STRING uwrite = RTL_CONSTANT_STRING(L"\\??\\c:\\test.txt");
	InitializeObjectAttributes(&attr, &uwrite, OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE, NULL, NULL);
	IO_STATUS_BLOCK ioStatus = { 0 };
	NTSTATUS status = ZwCreateFile(&hwrite, GENERIC_WRITE, &attr, &ioStatus, NULL, FILE_ATTRIBUTE_NORMAL, 0,
								   FILE_OPEN, FILE_SYNCHRONOUS_IO_NONALERT | FILE_NON_DIRECTORY_FILE, NULL, 0);
	LARGE_INTEGER offset = { 0 };
	if (NT_SUCCESS(status)) {
		DbgPrint("NT_SUCCESS!!!");
		FILE_STANDARD_INFORMATION fileInfo;
		status = ZwQueryInformationFile(hwrite, &ioStatus, &fileInfo, sizeof(FILE_STANDARD_INFORMATION), FileStandardInformation);
		if (NT_SUCCESS(status)) {
			DbgPrint("文件大小为: %lld", fileInfo.EndOfFile.QuadPart);
		}
	// -----------------------------------------------------------------------------------------------------------
		// 向文件写入内容：
		ZwWriteFile(hwrite, NULL, NULL, NULL, &ioStatus, "hello", sizeof("hello"), &offset, NULL);
		status = ZwClose(hwrite);
		if (NT_SUCCESS(status)) {
			DbgPrint("文件已正常关闭!!!");
		}
	}
	DbgPrint("FileOperate Over!!!");
}

NTSTATUS DriverEntry(PDRIVER_OBJECT pDriver, PUNICODE_STRING pRegPath) {
    FileOperate();
	pDriver->DriverUnload = DriverUnload;
	return STATUS_SUCCESS;
}