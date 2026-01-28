#include <ntifs.h>

void DriverUnload(PDRIVER_OBJECT pDriver) {
	DbgPrint("DriverUnload!!!");
}

// 创建文件：
NTSTATUS MyCreateFile(PHANDLE phFile, PUNICODE_STRING path, PIO_STATUS_BLOCK ioStatus) {
	NTSTATUS status;
	OBJECT_ATTRIBUTES attr = { 0 };
	InitializeObjectAttributes(&attr, path, OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE, NULL, NULL);
	status = ZwCreateFile(phFile, GENERIC_READ | GENERIC_WRITE, &attr, &ioStatus, NULL, FILE_ATTRIBUTE_NORMAL,
						  0, FILE_OPEN_IF, FILE_NON_DIRECTORY_FILE | FILE_SYNCHRONOUS_IO_ALERT, NULL, 0);
	return status;
}

// 分为多次复制文件：
NTSTATUS MyCopyFile(PUNICODE_STRING destpath, PUNICODE_STRING srcpath) {
	NTSTATUS status;
	HANDLE readHandle = NULL;
	HANDLE writeHandle = NULL;
	PVOID buffer = NULL;
	ULONG bufferLen = 0x1000;
	IO_STATUS_BLOCK ioStatus = { 0 };
	LARGE_INTEGER offset = { 0 };
	buffer = ExAllocatePool(NonPagedPool, bufferLen);
	if (NULL == buffer) {
		return -1;
	}
	RtlZeroMemory(buffer, bufferLen);
	status = MyCreateFile(&readHandle, srcpath, &ioStatus);
	if (!NT_SUCCESS(status)) {
		return status;
	}
	status = MyCreateFile(&writeHandle, destpath, &ioStatus);
	if (!NT_SUCCESS(status)) {
		return status;
	}
	while (TRUE) {
		status = ZwReadFile(readHandle, NULL, NULL, NULL, &ioStatus, buffer, bufferLen, &offset, NULL);
		if (!NT_SUCCESS(status)) {
			// 读到文件末尾出现错误，属于正常情况
			if (status == STATUS_END_OF_FILE) {
				status = STATUS_SUCCESS;
			}
			break;
		}
		// 此时不能使用 bufferLen，否则后续数据不足 bufferLen 时，将会出现错误
		status = ZwWriteFile(writeHandle, NULL, NULL, NULL, &ioStatus, buffer, ioStatus.Information, &offset, NULL);
		if (!NT_SUCCESS(status)) {
			break;
		}
		// 更新 offset
		offset.QuadPart += ioStatus.Information;
	}
	// 关闭句柄、释放资源
	if (readHandle) ZwClose(readHandle);
	if (writeHandle) ZwClose(writeHandle);
	if (buffer) ExFreePool(buffer);
	return status;
}

NTSTATUS DriverEntry(PDRIVER_OBJECT pDriver, PUNICODE_STRING pRegPath) {
	// 将文件内容由 1.txt 复制到 2.txt 之中
	UNICODE_STRING src = RTL_CONSTANT_STRING(L"\\??\\c:\\1.txt");
	UNICODE_STRING dest = RTL_CONSTANT_STRING(L"\\??\\c:\\2.txt");
	MyCopyFile(&dest, &src);
	pDriver->DriverUnload = DriverUnload;
	return STATUS_SUCCESS;
}