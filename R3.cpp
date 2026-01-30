#include <iostream>
#include <Windows.h>

int main() {
    // 打开设备
    HANDLE handle = CreateFileW(L"\\\\.\\Device02", GENERIC_ALL, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_SYSTEM, 0);
    if (handle) {
        printf("打开成功!!!");
        CloseHandle(handle);
    }
    else {
        printf("打开失败!!!");
    }
    system("pause");
    return 0;
}