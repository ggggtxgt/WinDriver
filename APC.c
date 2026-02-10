#include "APC.h"

void DriverUnload(PDRIVER_OBJECT pDriver) {
	DbgPrint("DriverUnload!!!");
}

VOID MyKernelRouting(
	IN struct _KAPC* Apc,						// 指向当前APC对象的指针
	IN OUT PKNORMAL_ROUTINE* NormalRoutine,		// 指向普通例程指针的指针（可修改）
	IN OUT PVOID* NormalContext,				// 指向普通上下文指针的指针（可修改）
	IN OUT PVOID* SystemArgument1,				// 指向系统参数1指针的指针（可修改）
	IN OUT PVOID* SystemArgument2				// 指向系统参数2指针的指针（可修改）
	) {
	DbgPrint("MyKernelRouting已执行!!!");
	ExFreePool(Apc);
}

VOID MyNormalRouting(IN PVOID NormalContext, IN PVOID SystemArgument1, IN PVOID SystemArgument2) {
	DbgPrint("MyNormalRouting已执行!!!");
}

void MyFunc() {
	// 注意：此处只能使用非分页内存
	ULONG size = sizeof(KAPC);
	PKAPC papc = ExAllocatePool(NonPagedPool, size);
	if (!papc) {
		DbgPrint("内存申请失败!!!");
		return;
	}
	memset(papc, 0, size);
	PETHREAD peThread = NULL;
	NTSTATUS status = PsLookupThreadByThreadId(3076, &peThread);
	if (!NT_SUCCESS(status)) {
		DbgPrint("PsLookupThreadByThreadId错误!!!");
		ExFreePool(papc);
		return;
	}
	KeInitializeApc(papc, peThread, OriginalApcEnvironment, MyKernelRouting, NULL, 0x450c10, KernelMode, NULL);
	BOOLEAN result = KeInsertQueueApc(papc, NULL, NULL, 0);
	if (FALSE == result) {
		DbgPrint("APC插入失败!!!");
		ExFreePool(papc);
		return;
	}

}

NTSTATUS DriverEntry(PDRIVER_OBJECT pDriver, PUNICODE_STRING pRegPath) {
	pDriver->DriverUnload = DriverUnload;
	MyFunc();
	return STATUS_SUCCESS;
}