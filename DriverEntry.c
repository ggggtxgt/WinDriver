#include <ntifs.h>

void DriverUnload(PDRIVER_OBJECT pDriver) {
	DbgPrint("DriverUnload!!!");
}

void Func() {
	PEPROCESS pEprocess = NULL;
	KAPC_STATE apcStatus = { 0 };
	NTSTATUS status = PsLookupProcessByProcessId((HANDLE)2064, &pEprocess);
	if (NT_SUCCESS(status)) {
		PULONG pvalue = (PULONG)0x1b60000;
		KeStackAttachProcess(pEprocess, &apcStatus);
		DbgPrint("value: %d", pvalue);
	}
	KeUnstackDetachProcess(&apcStatus);
}

NTSTATUS DriverEntry(PDRIVER_OBJECT pDriver, PUNICODE_STRING pRegPath) {
	Func();
	pDriver->DriverUnload = DriverUnload;
	return STATUS_SUCCESS;
}