#include "PeTools.h"

void DriverUnload(PDRIVER_OBJECT pDriver) {
	DbgPrint("DriverUnload!!!");
}

NTSTATUS DriverEntry(PDRIVER_OBJECT pDriver, PUNICODE_STRING pRegPath) {
	ULONG code = 0;
	HANDLE handle = 0;
	char *fileBuffer = PeLoadFile(&handle);
	ULONG funcAddr = GetFuncAddress(fileBuffer, "NtClose");
	if (NULL != funcAddr) {
		GetServiceCode(funcAddr);
	}
	DbgPrint("code: %d", code);
	if (fileBuffer) ExFreePool(fileBuffer);
	if (handle) ZwClose(handle);
	pDriver->DriverUnload = DriverUnload;
	return STATUS_SUCCESS;
}