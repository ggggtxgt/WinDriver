#include <ntifs.h>

void DriverUnload(PDRIVER_OBJECT pDriver) {
	DbgPrint("DriverUnload!!!");
}

// ZwOpenKey 函数详解：
/*
NTSYSAPI NTSTATUS ZwOpenKey(
	_Out_ PHANDLE            KeyHandle,          // 输出：注册表键句柄
	_In_  ACCESS_MASK        DesiredAccess,      // 输入：请求的访问权限
	_In_  POBJECT_ATTRIBUTES ObjectAttributes    // 输入：对象属性结构
);
*/

void SetRegistryValue() {
	HANDLE hKey = NULL;
	OBJECT_ATTRIBUTES attr = { 0 };
	UNICODE_STRING path = RTL_CONSTANT_STRING(L"\\Registry\\Machine\\SOFTWARE\\MyKey");
	UNICODE_STRING value = RTL_CONSTANT_STRING(L"test01");
	InitializeObjectAttributes(&attr, &path, OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE, NULL, NULL);
	NTSTATUS status = ZwOpenKey(&hKey, GENERIC_ALL, &attr);
	if (NT_SUCCESS(status)) {
		DbgPrint("打开成功!!!");
		NTSTATUS setStatus = ZwSetValueKey(hKey, &value, 0, REG_SZ, L"DriverStudy", (wcslen(L"DriverStudy") + 1) * sizeof(WCHAR));
		if (NT_SUCCESS(setStatus)) {
			DbgPrint("ZwSetValueKey调用成功!!！");
		}
	} else {
		DbgPrint("打开失败!!!");
	}
	if (hKey) ZwClose(hKey);
}

// ZwQueryValueKey 函数详解：
/*
NTSYSAPI NTSTATUS ZwOpenKey(
	_Out_ PHANDLE            KeyHandle,          // 输出：注册表键句柄
	_In_  ACCESS_MASK        DesiredAccess,      // 输入：请求的访问权限
	_In_  POBJECT_ATTRIBUTES ObjectAttributes    // 输入：对象属性结构
);
*/

void QueryRegistryValue() {
	HANDLE hKey = NULL;
	OBJECT_ATTRIBUTES attr = { 0 };
	UNICODE_STRING path = RTL_CONSTANT_STRING(L"\\Registry\\Machine\\SOFTWARE\\MyKey");
	UNICODE_STRING value = RTL_CONSTANT_STRING(L"test01");
	InitializeObjectAttributes(&attr, &path, OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE, NULL, NULL);
	NTSTATUS openStatus = ZwOpenKey(&hKey, GENERIC_ALL, &attr);
	PKEY_VALUE_PARTIAL_INFORMATION dataBuff = ExAllocatePool(NonPagedPool, 0x100);
	ULONG resultLen = 0;
	if (NT_SUCCESS(openStatus) && NULL != dataBuff) {
		DbgPrint("注册表打开成功!!!");
		RtlZeroMemory(dataBuff, 0x100);
		ZwQueryValueKey(hKey, &value, KeyValuePartialInformation, dataBuff, 0x100, &resultLen);
		if (REG_SZ == dataBuff->Type) {
			DbgPrint("读取的信息: %ls", dataBuff->Data);
		}
	}
	if (hKey) ZwClose(hKey);
	if (dataBuff) ExFreePool(dataBuff);
}

// 
/*
NTSYSAPI NTSTATUS ZwDeleteValueKey(
	_In_ HANDLE           KeyHandle,     // 输入：要删除值的注册表键句柄
	_In_ PUNICODE_STRING  ValueName      // 输入：要删除的值名称
);
NTSYSAPI NTSTATUS ZwDeleteKey(
	_In_ HANDLE KeyHandle  // 输入：要删除的注册表键句柄
);
*/
void DeleteRegistry() {
	// @todo
}

NTSTATUS DriverEntry(PDRIVER_OBJECT pDriver, PUNICODE_STRING pRegPath) {
	SetRegistryValue();
	QueryRegistryValue();
	pDriver->DriverUnload = DriverUnload;
	return STATUS_SUCCESS;
}