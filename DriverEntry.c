#include <ntifs.h>

void DriverUnload(PDRIVER_OBJECT pDriver) {
	DbgPrint("DriverUnload!!!");
}

// LIST_ENTRY 结构体详解：
/*
typedef struct _LIST_ENTRY {
	struct _LIST_ENTRY* Flink;  // 后向指针
	struct _LIST_ENTRY* Blink;  // 前向指针
} LIST_ENTRY, * PLIST_ENTRY;
*/

// 创建自定义结构体
typedef struct _LIST {
	ULONG m_dataA;			// 数据 A
	ULONG m_dataB;			// 数据 B
	LIST_ENTRY m_ListEntry;	// LIST_ENTRY 变量
	ULONG m_dataC;			// 数据 C（注意偏移）
} LIST, *PLIST;

void UseListEntry() {
	PLIST_ENTRY header = ExAllocatePool(NonPagedPool, sizeof(PLIST_ENTRY));
	LIST node01 = { 0 };
	LIST node02 = { 0 };
	LIST node03 = { 0 };
	LIST node04 = { 0 };
	node01.m_dataA = 0x123;
	node02.m_dataA = 0x234;
	node03.m_dataA = 0x345;
	node04.m_dataA = 0x456;
	// -----------------------------------------------------------------------------------------------------------
	// 遍历链表；
	if (header) {
		RtlZeroMemory(header, sizeof(LIST_ENTRY));
		InitializeListHead(header);						// 初始化 LIST_ENTRY，使其头尾结点相同
		InsertHeadList(header, &node01.m_ListEntry);	// 从头部插入结点
		InsertHeadList(header, &node02.m_ListEntry);	// 从头部插入结点
		InsertHeadList(header, &node03.m_ListEntry);	// 从头部插入结点
		InsertHeadList(header, &node04.m_ListEntry);	// 从头部插入结点
		PLIST_ENTRY curNode = NULL;
		curNode = header->Flink;
		while (curNode != header) {
			PLIST addr = CONTAINING_RECORD(curNode, LIST, m_ListEntry);	// 获取当前结点首地址
			DbgPrint("%x\n", addr->m_dataA);
			curNode = curNode->Flink;
		}
		ExFreePool(header);
	}
	// -----------------------------------------------------------------------------------------------------------
	// 移除链表元素；
	/*
	RemoveHeadList();		// 移除头结点，成功之后，将会返回移除结点地址，用于释放内存
	RemoveTailList();		// 移除尾结点，成功之后，将会返回移除结点地址，用于释放内存
	RemoveEntryList();		// 移除指定结点，成功之后返回移除结点地址，用于释放内存
	*/
	// -----------------------------------------------------------------------------------------------------------
}

NTSTATUS DriverEntry(PDRIVER_OBJECT pDriver, PUNICODE_STRING pRegPath) {
	UseListEntry();
	pDriver->DriverUnload = DriverUnload;
	return STATUS_SUCCESS;
}