#include <ntifs.h>

typedef struct _LDR_DATA_TABLE_ENTRY {
	LIST_ENTRY InLoadOrderLinks;
	LIST_ENTRY InMemoryOrderLinks;
	LIST_ENTRY InlnitializationOrderLinks;
	PVOID DIIBase;
	PVOID EntryPoint;
	ULONG SizeOflmage;
	UNICODE_STRING FullDIlName;
	UNICODE_STRING BaseDllName;
	ULONG Flags;
	USHORT LoadCount;
	USHORT TlsIndex;
	union {
		LIST_ENTRY HashLinks;
		struct {
			PVOID SectionPointer;
			ULONG CheckSum;
		};
	};
	union {
		struct {
			ULONG TimeDateStamp;
		};
		struct {
			PVOID Loadedlmports;
		};
	};
	struct _ACTIVATION_CONTEXT* EntryPointActivationContext; 
	PVOID Patchlnformation;
} LDR_DATA_TABLE_ENTRY, *PLDR_DATA_TABLE_ENTRY;

void DriverUnload(PDRIVER_OBJECT pDriver) {
	DbgPrint("DriverUnload!!!");
}

void Func(PDRIVER_OBJECT pDriver) {
	PLDR_DATA_TABLE_ENTRY pLdr = (PLDR_DATA_TABLE_ENTRY)pDriver->DriverSection;
	LIST_ENTRY list = pLdr->InLoadOrderLinks;
	PLIST_ENTRY pFlink = list.Flink;
	PLIST_ENTRY pBlink = pFlink->Blink;
	UNICODE_STRING driverName = { 0 };
	RtlInitUnicodeString(&driverName, L"Dbgv.sys");
	while (pFlink != pBlink) {
		DbgPrint("%Z", (ULONG)pFlink + 0x2c);
		if (0 == RtlCompareUnicodeString(&driverName, (ULONG)pFlink + 0x2c, TRUE)) {
			RemoveEntryList(pFlink);
			break;
		}
		pFlink = pFlink->Flink;
	}
}

NTSTATUS DriverEntry(PDRIVER_OBJECT pDriver, PUNICODE_STRING pRegPath) {
	Func(pDriver);
	pDriver->DriverUnload = DriverUnload;
	return STATUS_SUCCESS;
}