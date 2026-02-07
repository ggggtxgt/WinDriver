#include <ntifs.h>

KEVENT kEvent;
PETHREAD pThread;

void MyThread(PVOID arg) {
	pThread = PsGetCurrentThread();
	for (SIZE_T i = 0; i < 10000; i++) {
		LARGE_INTEGER timeout = RtlConvertLongToLargeInteger(-10 * 1000 * 2000);
		NTSTATUS status = KeWaitForSingleObject(&kEvent, Executive, KernelMode, FALSE, &timeout);
		if (STATUS_TIMEOUT == status) {
			DbgPrint("等待超时，继续执行!!!");
			continue;
		} else {
			DbgPrint("已接收信号，线程执行终止操作!!!");
			PsTerminateSystemThread(STATUS_SUCCESS);
		}
	}
}

void DriverUnload(PDRIVER_OBJECT pDriver) {
	KeSetEvent(&kEvent, 0, FALSE);
	NTSTATUS status = KeWaitForSingleObject(pThread, Executive, KernelMode, FALSE, NULL);
	if (NT_SUCCESS(status)) {
		DbgPrint("线程已经终止，执行DriverUnload!!!");
	}
	DbgPrint("DriverUnload!!!");
}

NTSTATUS DriverEntry(PDRIVER_OBJECT pDriver, PUNICODE_STRING pRegPath) {
	KeInitializeEvent(&kEvent, SynchronizationEvent, FALSE);
	HANDLE hThread = 0;
	NTSTATUS status = PsCreateSystemThread(&hThread, GENERIC_ALL, NULL, NULL, NULL, MyThread, NULL);
	if (NT_SUCCESS(status)) {
		DbgPrint("线程创建成功!!!");
	}
	NtClose(hThread);
	pDriver->DriverUnload = DriverUnload;
	return STATUS_SUCCESS;
}