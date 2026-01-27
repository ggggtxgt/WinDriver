#include <ntifs.h>

void DriverUnload(PDRIVER_OBJECT pDriver) {
    DbgPrint("DriverUnload!!!");
}

// UNICODE_STRING 结构详解：
/*
typedef struct _UNICODE_STRING {
    USHORT Length;          // 字符串长度（字节数），不包含结束符
    USHORT MaximumLength;   // 缓冲区最大容量（字节数）
    PWSTR  Buffer;          // 指向Unicode字符串的指针
} UNICODE_STRING, * PUNICODE_STRING;
*/

// UNICODE_STRING 常见用法：
void UseUnicodeString() {
    // -----------------------------------------------------------------------------------------------------------
    // 定义 WCHAR 类型的数组
    WCHAR wstr[100] = L"Using WCHAR Array!!!";
    // 创建并初始化 UNICODE_STRING 类型变量
    UNICODE_STRING ustr01 = { 0 };
    // 使用 WCHAR 数组为 UNICODE_STRING Buffer 赋值
    ustr01.Buffer = wstr;
    // 设置 UNICODE_STRING Length 长度
    ustr01.Length = wcslen(wstr) * sizeof(WCHAR);
    ustr01.MaximumLength = 100;
    DbgPrint("%ws", ustr01.Buffer);
    // -----------------------------------------------------------------------------------------------------------
    DECLARE_CONST_UNICODE_STRING(ustr02, L"Using DECLARE_CONST_UNICODE_STRING!!!");
    DbgPrint("%ws", ustr02.Buffer);
    // -----------------------------------------------------------------------------------------------------------
    UNICODE_STRING ustr03 = RTL_CONSTANT_STRING(L"Using RTL_CONSTANT_STRING!!!");
    DbgPrint("%ws", ustr03.Buffer);
    // -----------------------------------------------------------------------------------------------------------
    UNICODE_STRING ustr04 = { 0 };
    RtlInitUnicodeString(&ustr04, L"Using RtlInitUnicodeString!!!");
    DbgPrint("%ws", ustr04.Buffer);
    // -----------------------------------------------------------------------------------------------------------
}

// ANSI_STRING 结构详解：
/*
typedef struct _STRING {
    USHORT Length;          // 字节数（不包括终止符）
    USHORT MaximumLength;   // 缓冲区字节容量
    PSTR   Buffer;          // 指向CHAR数组的指针
} STRING, * PSTRING, ANSI_STRING, * PANSI_STRING;
*/

// ANSI_STRING 常见用法：
// ANSI_STRING 常见用法：
void UseAnsiString() {
    // -----------------------------------------------------------------------------------------------------------
    // 方法1：使用 CHAR 数组进行初始化；
    CHAR cstr[100] = "Using CHAR Array!!!";
    ANSI_STRING astr01 = { 0 };
    astr01.Buffer = cstr;
    astr01.Length = (USHORT)strlen(astr01.Buffer);
    astr01.MaximumLength = 100;
    DbgPrint("%s", astr01.Buffer);
    // -----------------------------------------------------------------------------------------------------------
    // 方法2：使用 RtlInitAnsiString 初始化；
    ANSI_STRING astr02 = { 0 };
    RtlInitAnsiString(&astr02, "Using RtlInitAnsiString!!!");
    DbgPrint("%s", astr02.Buffer);
    // -----------------------------------------------------------------------------------------------------------
     // 方法3：使用 RTL_CONSTANT_STRING；
    ANSI_STRING astr03 = RTL_CONSTANT_STRING("Using RTL_CONSTANT_STRING!!!");
    DbgPrint("%s", astr03.Buffer);
    // -----------------------------------------------------------------------------------------------------------
}

NTSTATUS DriverEntry(PDRIVER_OBJECT pDriver, PUNICODE_STRING pRegPath) {
    UseUnicodeString();
    DbgPrint("----------分割线----------");
    UseAnsiString();
    pDriver->DriverUnload = DriverUnload;
    return STATUS_SUCCESS;
}