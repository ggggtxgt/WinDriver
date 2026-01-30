#include <iostream>
#include <Windows.h>

#define MSG_CODE_WRITE CTL_CODE(FILE_DEVICE_UNKNOWN, 0x801, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define MSG_CODE_READ CTL_CODE(FILE_DEVICE_UNKNOWN, 0x802, METHOD_BUFFERED, FILE_ANY_ACCESS)

// DeviceIoControl 函数详解：
/*
BOOL DeviceIoControl(
    HANDLE       hDevice,             // 通过CreateFile打开设备后获得的句柄
    DWORD        dwIoControlCode,     // 控制码，用于指定操作类型，驱动根据此码执行相应操作
    LPVOID       lpInBuffer,          // 输入缓冲区指针，包含传递给驱动的数据
    DWORD        nInBufferSize,       // 输入缓冲区的大小（字节数）
    LPVOID       lpOutBuffer,         // 输出缓冲区指针，用于接收驱动返回的数据
    DWORD        nOutBufferSize,      // 输出缓冲区的大小（字节数）
    LPDWORD      lpBytesReturned,     // 实际接收到的数据大小（字节数），即驱动写入输出缓冲区的数据量
    LPOVERLAPPED lpOverlapped         // 用于异步操作的OVERLAPPED结构指针，同步操作时通常为NULL
);
*/

// 生成设备I/O控制码（IOCTL）的宏
/*
#define CTL_CODE(
    DeviceType,  // [in] 设备类型标识（16位），标识设备类别，如FILE_DEVICE_UNKNOWN
    Function,    // [in] 功能代码（12位），驱动内定义的具体操作，0x800-0xFFF为用户自定义范围
    Method,      // [in] 数据传输方法（2位），定义数据如何在内核与用户态间传递
    Access       // [in] 访问权限要求（2位），指定调用此IOCTL所需的权限
    )
*/

int main() {
    // 打开设备
    HANDLE handle = CreateFileW(L"\\\\.\\Device02", GENERIC_ALL, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_SYSTEM, 0);
    if (handle) {
        printf("打开成功!!!\n");
        // --------------------------------------------------------------------------------------------------------
        // R3向R0写入数据
        const char* str = "Hello Driver";
        DWORD realWrite = 0;
        WriteFile(handle, str, strlen(str) + 1, &realWrite, NULL);
        printf("写入数据成功!!!\n");
        // --------------------------------------------------------------------------------------------------------
        // R3读取R0写入的数据
        const char* readBuff[0x100] = { 0 };
        DWORD realRead = 0;
        ReadFile(handle, readBuff, 0x100, &realRead, NULL);
        printf("读取数据成功!!!\n");
        printf("R0写入的数据为: %s\n", readBuff);
        // --------------------------------------------------------------------------------------------------------
        // 以 DeviceIoControl 向 R0 写入数据
        LPVOID inBuff = malloc(0x100);
        const char* srcStr = "Hello Driver -- SendMessage";
        DWORD returnBytes = 0;
        memset(inBuff, 0, 0x100);
        memcpy(inBuff, srcStr, 0x100);
        DeviceIoControl(handle, MSG_CODE_WRITE, inBuff, 0x100, NULL, 0, &returnBytes, NULL);
        // --------------------------------------------------------------------------------------------------------
        // 以 DeviceIoControl 读取 R3 写入的数据
        LPVOID outBuff = malloc(0x100);
        memset(outBuff, 0, 0x100);
        DeviceIoControl(handle, MSG_CODE_READ, NULL, 0, outBuff, 0, &returnBytes, NULL);
        printf("DeviceControl Read: %s\n", outBuff);
        // --------------------------------------------------------------------------------------------------------
        CloseHandle(handle);
    }
    else {
        printf("打开失败!!!");
    }
    system("pause");
    return 0;
}