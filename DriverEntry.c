#include <ntifs.h>

void DriverUnload(PDRIVER_OBJECT pDriver) {
	DbgPrint("DriverUnload!!!");
}

void MemoryAlloc() {
	// -----------------------------------------------------------------------------------------------------------
	// 方法1：使用 ExAllocatePool 进行分配；
	PCHAR pwstr01 = ExAllocatePool(NonPagedPool, 0x100);
	if (pwstr01) {
		RtlZeroMemory(pwstr01, 0x100);						// 初始化内存
		RtlCopyMemory(pwstr01, L"ExAllocatePool!!!", 34);	// 为该内存赋值
		UNICODE_STRING ustr01 = { 0 };
		RtlInitUnicodeString(&ustr01, pwstr01);				// 以申请的内存为 UNICODE_STRING 赋值
		DbgPrint("%wZ", &ustr01);							// 直接输出 UNICODE_STRING 之中 Buffer
		ExFreePool(pwstr01);								// 释放申请的内存
	}
	// -----------------------------------------------------------------------------------------------------------
	// 方法2：使用 ExAllocatePoolWithTag 进行分配；
	PWCHAR pwstr02 = ExAllocatePoolWithTag(NonPagedPool, 0x100, "ExAllocatePoolWithTag");
	if (pwstr02) {
		RtlZeroMemory(pwstr02, 0x100);
		RtlCopyMemory(pwstr02, L"ExAllocatePoolWithTag!!!", 48);
		UNICODE_STRING ustr02 = { 0 };
		RtlInitUnicodeString(&ustr02, pwstr02);
		DbgPrint("%wZ", &ustr02);
		ExFreePoolWithTag(pwstr02, "ExFreePoolWithTag");
	}
	// -----------------------------------------------------------------------------------------------------------
	// 方法3：使用旁视列表进行内存分配；
	PNPAGED_LOOKASIDE_LIST pNpList = ExAllocatePool(NonPagedPool, sizeof(NPAGED_LOOKASIDE_LIST));
	if (pNpList) {
		ExInitializeNPagedLookasideList(pNpList, NULL, NULL, 0, 0x100, '100', 0);
		PWCHAR pwstr03 = ExAllocateFromNPagedLookasideList(pNpList);
		if (pwstr03) {
			RtlZeroMemory(pwstr03, 0x100);
			RtlCopyMemory(pwstr03, L"ExAllocateFromNPagedLookasideList!!!", 74);
			UNICODE_STRING ustr03 = { 0 };
			RtlInitUnicodeString(&ustr03, pwstr03);
			DbgPrint("%wZ", &ustr03);
			ExFreeToNPagedLookasideList(pNpList, pwstr03);	// 释放旁视列表申请的内存空间
			ExDeleteNPagedLookasideList(pNpList);			// 释放旁视列表对象
		}
	}
	// -----------------------------------------------------------------------------------------------------------
}

NTSTATUS DriverEntry(PDRIVER_OBJECT pDriver, PUNICODE_STRING pRegPath) {
	MemoryAlloc();
	pDriver->DriverUnload = DriverUnload;
	return STATUS_SUCCESS;
}