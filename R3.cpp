#include <iostream>
#include <Windows.h>

#define MSG_CODE_WRITE CTL_CODE(FILE_DEVICE_UNKNOWN, 0x801, METHOD_IN_DIRECT, FILE_ANY_ACCESS)
#define MSG_CODE_READ CTL_CODE(FILE_DEVICE_UNKNOWN, 0x802, METHOD_OUT_DIRECT, FILE_ANY_ACCESS)

int main() {
    // 打开设备
    HANDLE handle = CreateFileW(L"\\\\.\\Device02", GENERIC_ALL, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_SYSTEM, 0);
    if (handle) {
        printf("打开成功!!!\n");
        DWORD returnBytes = 0;
        // 申请输入缓冲区
        LPVOID inBuff = malloc(0x100);
        LPVOID outBuff = malloc(0x100);
        // 初始化缓冲区
        memset(inBuff, 0, 0x100);
        memset(outBuff, 0, 0x100);
        memcpy(inBuff, "输入缓冲区数据", strlen("输入缓冲区数据") + 1);
        memcpy(outBuff, "输出缓冲区数据", strlen("输入缓冲区数据") + 1);
        // 直接写入
        DeviceIoControl(handle, MSG_CODE_WRITE, inBuff, 0x100, NULL, 0, &returnBytes, NULL);
        // 直接读取
        DeviceIoControl(handle, MSG_CODE_READ, NULL, 0, outBuff, 0x100, &returnBytes, NULL);
        printf("读取的数据为:%s\n", outBuff);
        CloseHandle(handle);
    }
    else {
        printf("打开失败!!!");
    }
    system("pause");
    return 0;
}