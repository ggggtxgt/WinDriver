#include <ntifs.h>

UCHAR* PsGetProcessImageFileName(__in PEPROCESS Process);

void DriverUnload(PDRIVER_OBJECT pDriver) {
	DbgPrint("DriverUnload!!!");
}

// 枚举进程
void EnumProcess() {
	PEPROCESS pEprocess = NULL;
	// 传入进程ID，将会返回对应的进程对象
	for (SIZE_T i = 4; i < 20000; i += 4) {
		NTSTATUS status = PsLookupProcessByProcessId((HANDLE)i, & pEprocess);
		if (NT_SUCCESS(status)) {
			DbgPrint("进程名称:%s", PsGetProcessImageFileName(pEprocess));
		}
	}
}

// 枚举线程
void EnumThread() {
	PETHREAD pEthread = NULL;
	PEPROCESS pEprocess = NULL;
	for (SIZE_T i = 4; i < 200000; i += 4) {
		NTSTATUS status = PsLookupThreadByThreadId((HANDLE)i, &pEthread);
		if (NT_SUCCESS(status)) {
			pEprocess = IoThreadToProcess(pEthread);
			DbgPrint("进程名称:%s, 线程id:%d", PsGetProcessImageFileName(pEprocess), i);
		}
	}
}

NTSTATUS DriverEntry(PDRIVER_OBJECT pDriver, PUNICODE_STRING pRegPath) {
	// EnumProcess();
	EnumThread();
	pDriver->DriverUnload = DriverUnload;
	return STATUS_SUCCESS;
}