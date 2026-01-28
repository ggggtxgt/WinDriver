#include <ntifs.h>

void DriverUnload(PDRIVER_OBJECT pDriver) {
	DbgPrint("DriverUnload!!!");
}

// 创建文件函数详解：
/*
NTSYSAPI NTSTATUS ZwCreateFile(
    _Out_ PHANDLE            FileHandle,           // 输出：返回的文件句柄
    _In_  ACCESS_MASK        DesiredAccess,        // 输入：请求的访问权限
    _In_  POBJECT_ATTRIBUTES ObjectAttributes,     // 输入：对象属性结构
    _Out_ PIO_STATUS_BLOCK   IoStatusBlock,        // 输出：I/O状态块
    _In_opt_ PLARGE_INTEGER  AllocationSize,       // 输入（可选）：初始分配大小
    _In_  ULONG              FileAttributes,       // 输入：文件属性
    _In_  ULONG              ShareAccess,          // 输入：共享访问模式
    _In_  ULONG              CreateDisposition,    // 输入：创建处理方式
    _In_  ULONG              CreateOptions,        // 输入：创建选项
    _In_opt_ PVOID           EaBuffer,             // 输入（可选）：扩展属性缓冲区
    _In_  ULONG              EaLength              // 输入：扩展属性缓冲区长度
);
*/

// 读取文件函数详解：
/*
NTSYSAPI NTSTATUS ZwReadFile(
    _In_        HANDLE           FileHandle,           // 输入：文件句柄
    _In_opt_    HANDLE           Event,                // 输入（可选）：事件对象句柄
    _In_opt_    PIO_APC_ROUTINE  ApcRoutine,           // 输入（可选）：APC例程
    _In_opt_    PVOID            ApcContext,           // 输入（可选）：APC上下文
    _Out_       PIO_STATUS_BLOCK IoStatusBlock,        // 输出：I/O状态块
    _Out_       PVOID            Buffer,               // 输出：接收数据的缓冲区
    _In_        ULONG            Length,               // 输入：要读取的字节数
    _In_opt_    PLARGE_INTEGER   ByteOffset,           // 输入（可选）：读取起始偏移
    _In_opt_    PULONG           Key                   // 输入（可选）：键值（用于字节范围锁）
);
*/
void FileOperate() {
    // -----------------------------------------------------------------------------------------------------------
    // 创建文件：
    // 初始化谁的句柄
    HANDLE hFile = NULL; 
    // 初始化对象属性结构
    OBJECT_ATTRIBUTES attr = { sizeof(OBJECT_ATTRIBUTES) };
    attr.RootDirectory = NULL;
    attr.Attributes = OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE;
    attr.SecurityDescriptor = NULL;
    attr.SecurityQualityOfService = NULL;
    // 指定需要操作的文件路径
    UNICODE_STRING path = RTL_CONSTANT_STRING(L"\\??\\c:\\test.txt");
    attr.ObjectName = &path;
    // IO 状态块，用于接收操作结果
    IO_STATUS_BLOCK ioState = { 0 };
    // 创建文件
    NTSTATUS status = ZwCreateFile(&hFile, GENERIC_ALL, &attr, &ioState, 0, FILE_ATTRIBUTE_NORMAL, 0,
                                   FILE_OPEN_IF, FILE_NON_DIRECTORY_FILE | FILE_SYNCHRONOUS_IO_NONALERT, NULL, 0);
    // -----------------------------------------------------------------------------------------------------------
    // 读取文件内容：
    // 读取操作的 IO 状态块
    IO_STATUS_BLOCK ioReadBlock = { 0 };
    // 读取偏移，从文件头开始
    LARGE_INTEGER offset = { 0 };
    // 分配缓冲区存储读取的数据
    PVOID fileBuffer = ExAllocatePool(NonPagedPool, 0x100);
    if (NT_SUCCESS(status) && NULL != fileBuffer) {
        DbgPrint("文件打开成功!!!");
        RtlZeroMemory(fileBuffer, 0x100);
        // 读取文件内容
        NTSTATUS readStatus = ZwReadFile(hFile, NULL, NULL, NULL, &ioReadBlock, fileBuffer, 0x100, &offset, NULL);
        if (NT_SUCCESS(readStatus)) {
            DbgPrint("读取文件成功!!!");
            // 根据读取的长度输出读取的内容
            ULONG bytesRead = (ULONG)ioReadBlock.Information;
            if (bytesRead > 0) {
                DbgPrint("读取的内容为: %.*s\n", bytesRead, (char*)fileBuffer);
            }
        } else {
            DbgPrint("读取文件失败!!!");
        }
        ZwClose(hFile);
        ExFreePool(fileBuffer);
    } else {
        DbgPrint("打开文件失败!!!");
    }
    // -----------------------------------------------------------------------------------------------------------
}

NTSTATUS DriverEntry(PDRIVER_OBJECT pDriver, PUNICODE_STRING pRegPath) {
    FileOperate();
	pDriver->DriverUnload = DriverUnload;
	return STATUS_SUCCESS;
}