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
	// 提权 CR3 先找到游戏进程CR3，并申请内存将游戏进程CR3内容复制到新内存
	// 先找到游戏进程CR3
	ULONG gameCr3 = *(ULONG*)((ULONG)pGameProcess + 0x18);
	// 将物理地址映射为线性地址
	PHYSICAL_ADDRESS oldCr3PhyAddr = { 0 };
	oldCr3PhyAddr.LowPart = gameCr3;
	PVOID oldCr3Addr = MmMapIoSpace(oldCr3PhyAddr, 0x1000, MmNonCached);
	// 申请内存空间并复制
	PVOID newCr3Addr = ExAllocatePool(NonPagedPool, 0x1000);
	memcpy(newCr3Addr, oldCr3Addr, 0x1000);
	// 修改CR3为新的内存物理地址
	PHYSICAL_ADDRESS newCr3PhyAddr = MmGetPhysicalAddress(newCr3Addr);
	*(ULONG*)((ULONG)newGameProcess + 0x18 + 0x18) = newCr3PhyAddr.LowPart;
	SetHandleAccess(pGameProcess, pCeProcess, newGameProcess);
}

NTSTATUS DriverEntry(PDRIVER_OBJECT pDriver, PUNICODE_STRING pRegPath) {
	MyFunc();
	pDriver->DriverUnload = DriverUnload;
	return STATUS_SUCCESS;
}