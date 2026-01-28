#include <ntifs.h>

void DriverUnload(PDRIVER_OBJECT pDriver) {
	DbgPrint("DriverUnload!!!");
}

// ZwCreateKey 函数详解：
/*
NTSYSAPI NTSTATUS ZwCreateKey(
    _Out_       PHANDLE            KeyHandle,          // 输出：注册表键句柄
    _In_        ACCESS_MASK        DesiredAccess,      // 输入：请求的访问权限
    _In_        POBJECT_ATTRIBUTES ObjectAttributes,   // 输入：对象属性结构
    _Reserved_  ULONG              TitleIndex,         // 保留：标题索引（必须为0）
    _In_opt_    PUNICODE_STRING    Class,              // 输入（可选）：键类名
    _In_        ULONG              CreateOptions,      // 输入：创建选项
    _Out_opt_   PULONG             Disposition         // 输出（可选）：处置结果
);
*/

void OperateRegistry() {
    HANDLE keyHandle = NULL;
    OBJECT_ATTRIBUTES attr = { 0 };
    ULONG Disposition = 0;
    UNICODE_STRING path = RTL_CONSTANT_STRING(L"\\Registry\\Machine\\SOFTWARE\\MyKey");
    InitializeObjectAttributes(&attr, &path, OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE, NULL, NULL);
    NTSTATUS status = ZwCreateKey(&keyHandle, KEY_WRITE, &attr, 0, NULL, REG_OPTION_NON_VOLATILE, &Disposition);
    if (NT_SUCCESS(status)) {
        if (REG_CREATED_NEW_KEY == Disposition) {
            DbgPrint("新key已经创建!!!");
        } else {
            DbgPrint("该key已经存在!!!");
        }
    } else {
        DbgPrint("ZwCreateKey执行失败!!!");
    }
    // 关闭句柄、释放资源
    if (keyHandle) ZwClose(keyHandle);
}

NTSTATUS DriverEntry(PDRIVER_OBJECT pDriver, PUNICODE_STRING pRegPath) {
    OperateRegistry();
	pDriver->DriverUnload = DriverUnload;
	return STATUS_SUCCESS;
}