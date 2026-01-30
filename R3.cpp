#include <iostream>
#include <Windows.h>

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
        CloseHandle(handle);
    }
    else {
        printf("打开失败!!!");
    }
    system("pause");
    return 0;
}