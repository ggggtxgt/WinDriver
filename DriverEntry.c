#include <ntifs.h>

void DriverUnload(PDRIVER_OBJECT pDriver) {
	DbgPrint("DriverUnload!!!");
}

// 获取全局句柄表地址
ULONG GetPspCidTable() {
	ULONG funcAddr = (ULONG)PsLookupProcessByProcessId;
	return *(ULONG*)(funcAddr + 0x20);
}

// 通过全局句柄表遍历进程
void EnumProcessByPspCidTable() {
	ULONG psCidTable = GetPspCidTable();
	if (!MmIsAddressValid(psCidTable)) {
		return;
	}
	ULONG psCidTableAddr = *(ULONG*)psCidTable;
	// 获取 TableCode
	ULONG tableCode = *(ULONG*)psCidTableAddr;
	// 判断目录层级
	UCHAR tableLv = tableCode & 0xF;
	// 获取表地址
	ULONG tableAddr = tableCode & 0xFFFFF0;
	switch (tableLv) {
	case 0:
		break;
	case 1: {
		for (SIZE_T i = 0; i < 1024; i++) {
			ULONG subTable = *(ULONG*)(tableAddr + i * 4);
			if (0 == subTable) break;
			for (SIZE_T j = 0; j < 512; j++) {
				ULONG subTable2 = *(ULONG*)(subTable + j * 8);
				ULONG objAddr = subTable2 & 0xFFFFFF8;
				if (!MmIsAddressValid(objAddr)) {
					continue;
				}
				ULONG objHeader = objAddr - 0x18;
				UCHAR typeIndex = *(CHAR*)(objHeader + 0xc);
				if (7 == typeIndex) {
					DbgPrint("其为一个进程!!!");
					if (MmIsAddressValid(objAddr + 0x16c)) {
						DbgPrint("%s", objAddr + 0x16c);
					}
				}
			}
		}
		break;
	}
	case 2:
		break;
	default:
		break;
	}
}

NTSTATUS DriverEntry(PDRIVER_OBJECT pDriver, PUNICODE_STRING pRegPath) {
	pDriver->DriverUnload = DriverUnload;
	return STATUS_SUCCESS;
}