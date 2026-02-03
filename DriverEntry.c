#include <ntifs.h>

void DriverUnload(PDRIVER_OBJECT pDriver) {
	DbgPrint("DriverUnload!!!");
}

void TraverseProcess() {
	ULONG processName = 0;
	// 获取当前进程对象
	ULONG currentProcess = PsGetCurrentProcess();
	PLIST_ENTRY processList = (PLIST_ENTRY)(currentProcess + 0xb8);
	PLIST_ENTRY nextList = processList->Flink;
	// 遍历链表
	while (processList != nextList) {
		ULONG eprocess = ((ULONG)nextList - 0xb8);
		processName = eprocess + 0x16c;
		if (0 == strcmp(processName, "123.exexe")) {
			DbgPrint("已经找到!!!");
			*(ULONG*)(eprocess + 0x26c) |= 0x800;
			break;
		}
		nextList = nextList->Flink;
	}
}

NTSTATUS DriverEntry(PDRIVER_OBJECT pDriver, PUNICODE_STRING pRegPath) {
	TraverseProcess();
	pDriver->DriverUnload = DriverUnload;
	return STATUS_SUCCESS;
}