#pragma once
#include <ntifs.h>
#include <ntimage.h>

char* fileBuffer;

// 加载 PE 文件
NTSTATUS PeLoadFile();
// 获取函数地址
ULONG GetFuncAddress();
// 将 rva 转换为 foa
ULONG RvaToFoa(ULONG rva);
// 清理内存
void PeDeleteFileBuffer();