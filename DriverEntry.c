#include <ntifs.h>

void DriverUnload(PDRIVER_OBJECT pDriver) {
	DbgPrint("DriverUnload!!!");
}

ULONG FindEprocessByName(const char* processName) {
	ULONG result = 0;
	// 获取当前进程对象
	ULONG currentProcess = PsGetCurrentProcess();
	PLIST_ENTRY processList = (PLIST_ENTRY)(currentProcess + 0xb8);
	PLIST_ENTRY nextList = processList->Flink;
	// 遍历
	while (processList != nextList) {
		ULONG eprocess = (ULONG)nextList - 0xb8;
		PUCHAR currentName = eprocess + 0x16c;
		if (0 == strcmp(currentName, processName)) {
			return eprocess;
		}
		nextList = nextList->Flink;
	}
	return result;
}

NTSTATUS DriverEntry(PDRIVER_OBJECT pDriver, PUNICODE_STRING pRegPath) {
	ULONG eprocess = FindEprocessByName("pack.exe");
	if (eprocess) {
		PLIST_ENTRY processList = (PLIST_ENTRY)(eprocess + 0xb8);
		processList->Blink->Flink = processList->Flink;
		processList->Flink->Blink = processList->Blink;
	}
	pDriver->DriverUnload = DriverUnload;
	return STATUS_SUCCESS;
}