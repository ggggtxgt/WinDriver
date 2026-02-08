#include <ntifs.h>

void DriverUnload(PDRIVER_OBJECT pDriver) {
	DbgPrint("DriverUnload!!!");
}

// 线程回调函数
void ThreadProc(PVOID pEprocess) {
	PVOID pTargetProcess = NULL;
	LARGE_INTEGER time = RtlConvertLongToLargeInteger(-10000 * 3000);
	for (SIZE_T i = 0; i < 50; i++) {
		NTSTATUS status = PsLookupProcessByProcessId(i * 4, &pTargetProcess);
		if (NT_SUCCESS(status)) {
			// 遍历私有句柄表，之后匹配，降权
			SetHandleAccess(pEprocess, pTargetProcess);
		}
		KeDelayExecutionThread(KernelMode, FALSE, &time);
	}
}

// 降权
void SetHandleAccess(PVOID srcProcess, PVOID targetProcess) {
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
			// 降权
		}
	}
}

NTSTATUS DriverEntry(PDRIVER_OBJECT pDriver, PUNICODE_STRING pRegPath) {
	// 引用计数
	PVOID pEprocess = NULL;
	HANDLE hThread = 0;
	// 查找保护的进程对象
	NTSTATUS status = PsLookupProcessByProcessId(123, &pEprocess);
	if (NT_SUCCESS(status)) {
		PsCreateSystemThread(&hThread, NULL, NULL, NULL, NULL, ThreadProc, pEprocess);
		ObReferenceObject(pEprocess);
	}
	if (hThread) ZwClose(hThread);
	pDriver->DriverUnload = DriverUnload;
	return STATUS_SUCCESS;
}