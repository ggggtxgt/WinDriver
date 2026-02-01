#include <ntifs.h>

// 全局变量：用于保存 SSDT 函数表的映射地址
PULONG g_funcTableAddr;

typedef struct _SSDT {
	PULONG funcAddrTable;	// 系统调用函数地址表
	PULONG countTable;		// 每个系统调用的参数数量表(可选)
	ULONG funcNumber;		// 系统调用总数
	PULONG argTable;		// 参数表(可选)
} SSDT, *PSSDT;
// NtClose 函数类型定义
typedef NTSTATUS(NTAPI *_NtClose)(HANDLE handle);
// 保存原始的 NtClose 函数指针
_NtClose OldNtClose = NULL;
// 声明外部导出的 SSDT 表(仅在x86 Windows 之中有效）
EXTERN_C PSSDT KeServiceDescriptorTable;
// ；安装/卸载 Hook 函数声明
NTSTATUS InstallHook();
NTSTATUS UninstallHook();
// Hook 之后的 NtClose 函数
NTSTATUS NTAPI HookNtClose(HANDLE handle) {
	DbgPrint("NtClose 已被 Hook!!!");
	return OldNtClose(handle);
}

void DriverUnload(PDRIVER_OBJECT pDriver) {
	UninstallHook();
	DbgPrint("DriverUnload!!!");
}

// 安装 HOOK
NTSTATUS InstallHook() {
	// 获取 SSDT 函数表的物理地址
	PHYSICAL_ADDRESS pyhAddr = MmGetPhysicalAddress(KeServiceDescriptorTable->funcAddrTable);
	// 将物理地址映射到内核虚拟地址空间
	g_funcTableAddr = MmMapIoSpace(pyhAddr, PAGE_SIZE, MmNonCached);
	// 保存原始 NtClose 函数地址(索引0x32对应 NtClose）
	OldNtClose = g_funcTableAddr[0x32];
	// 将 SSDT 中的 NtClose 地址替换为 Hool 函数地址
	g_funcTableAddr[0x32] = (ULONG)HookNtClose;
	return STATUS_SUCCESS;
}

// 卸载 HOOK
NTSTATUS UninstallHook() {
	// 若保存原始函数指针，则恢复
	if (OldNtClose) {
		g_funcTableAddr[0x32] = OldNtClose;
	}
	// 解除物理地址映射
	MmUnmapIoSpace(g_funcTableAddr, PAGE_SIZE);
	return STATUS_SUCCESS;
}

NTSTATUS DriverEntry(PDRIVER_OBJECT pDriver, PUNICODE_STRING pRegPath) {
	InstallHook();
	pDriver->DriverUnload = DriverUnload;
	return STATUS_SUCCESS;
}