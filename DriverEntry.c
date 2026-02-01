#include "PeTools.h"

void DriverUnload(PDRIVER_OBJECT pDriver) {
	DbgPrint("DriverUnload!!!");
}

NTSTATUS DriverEntry(PDRIVER_OBJECT pDriver, PUNICODE_STRING pRegPath) {
	PeLoadFile();
	GetFuncAddress();
	pDriver->DriverUnload = DriverUnload;
	return STATUS_SUCCESS;
}