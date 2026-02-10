#include <ntifs.h>

void DriverUnload(PDRIVER_OBJECT pDriver) {
	DbgPrint("DriverUnload!!!");
}

// 回调函数
void KdeferredRoutine(KDPC* Dpc, PVOID DeferredContext, PVOID SystemArgument1, PVOID SystemArguemnt2) {
	DbgPrint("KdeferredRoutine已执行!!!");
	DbgPrint("DPC当前运行的CPU:%d", KeGetCurrentProcessorIndex());
}

void MyFunc() {
	// 初始化 DPC 对象
	PKDPC pDpc = NULL;
	pDpc = ExAllocatePool(NonPagedPool, sizeof(KDPC));
	if (NULL == pDpc) {
		DbgPrint("内存申请失败!!!");
		return;
	}
	// 初始化 DPC 对象
	KeInitializeDpc(pDpc, KdeferredRoutine, NULL);
	// 指定 DPC 运行的CPU核心号
	KeSetTargetProcessorDpc(pDpc, 1);
	// 将DPC对象插入到DPC队列
	KeInsertQueueDpc(pDpc, NULL, NULL);
}

NTSTATUS DriverEntry(PDRIVER_OBJECT pDriver, PUNICODE_STRING pRegPath) {
	pDriver->DriverUnload = DriverUnload;
	MyFunc();
	return StandardDesign;
}