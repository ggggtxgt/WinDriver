#pragma once
#include <ntifs.h>
#include <ntimage.h>

// 加载 PE 文件
char* PeLoadFile(PHANDLE handle);
// 获取函数地址
ULONG GetFuncAddress(char* fileBuffer, const char *funcName);
// 将 rva 转换为 foa
ULONG RvaToFoa(char *fileBuffer, ULONG rva);
// 获取服务号
ULONG GetServiceCode(char* funcAddr);