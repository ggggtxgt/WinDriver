#include <ntifs.h>

void DriverUnload(PDRIVER_OBJECT pDriver) {
	DbgPrint("DriverUnload!!!");
}

// 遍历私有句柄表，并且提权
void SetHandleAccess(PVOID srcProcess, PVOID targetProcess, PVOID newGameProcess) {
	ULONG handleTable = *(ULONG*)((ULONG)targetProcess + 0xf4);
	if (!MmIsAddressValid(handleTable)) {
		return;
	}
	ULONG tableCode = *(ULONG*)handleTable;
	tableCode = tableCode & 0xfffffff0;
	if (!MmIsAddressValid(tableCode)) {
		return;
	}
	for (SIZE_T i = 0; i < 512; i++) {
		ULONG objAddr = *(ULONG*)(tableCode + 8 * i);
		objAddr = objAddr & 0xfffffff8;
		if (!MmIsAddressValid(objAddr)) {
			continue;
		}
		if (objAddr + 0x18 == srcProcess) {
			// 提权
			*(ULONG*)(tableCode + i * 8) = (ULONG)newGameProcess | 3;
			ULONG mask = *(ULONG*)(tableCode + i * 8 + 4) | 0x0010 | 0x0020;
			*(ULONG*)(tableCode + i * 8 + 4) = mask;
		}
	}
}

void MyFunc() {
	PVOID pGameProcess = NULL;
	PVOID pCeProcess = NULL;
	PVOID newGameProcess = NULL;
	// 找到游戏进程对象
	NTSTATUS status = PsLookupProcessByProcessId(2812, &pGameProcess);
	if (NT_SUCCESS(status)) return;
	// 找到CE OD进行对象
	status = PsLookupProcessByProcessId(3256, &pCeProcess);
	if (NT_SUCCESS(status)) return;
	// 申请内存
	newGameProcess = ExAllocatePool(NonPagedPool, 0x300);
	if (!newGameProcess) return;
	// 复制游戏进程对象到新申请的内存
	memset(newGameProcess, 0, 0x300);
	memcpy(newGameProcess, (PVOID)((ULONG)pGameProcess - 0x18), 0x300);
	// 遍历CE/OD进程私有句柄表
	// 先将句柄值进行修改，让其指向新申请的内存
	// 提权
	SetHandleAccess(pGameProcess, pCeProcess, newGameProcess);
}

NTSTATUS DriverEntry(PDRIVER_OBJECT pDriver, PUNICODE_STRING pRegPath) {
	MyFunc();
	pDriver->DriverUnload = DriverUnload;
	return STATUS_SUCCESS;
}