#include <ntifs.h>

void DriverUnload(PDRIVER_OBJECT pDriver) {
	DbgPrint("DriverUnload!!!");
}

// 创建自定义结构体
typedef struct _TEST {
    ULONG a;
    ULONG b;
} TEST, * PTEST;

// 自定义比较函数：
RTL_GENERIC_COMPARE_RESULTS NTAPI RtlCmp(__in struct _RTL_GENERIC_TABLE* Table, __in PVOID FirstStruct, __in PVOID SecondStruct) {
    DbgPrint("RtlCmp");
    PTEST first = FirstStruct;
    PTEST second = SecondStruct;
    if (first->a == second->a) {
        return GenericEqual;
    } else if (first->a > second->a) {
        return GenericGreaterThan;
    }
    return GenericLessThan;
}

// 自定义内存申请函数：
PVOID RtlAlloc(__in struct _RTL_GENERIC_TABLE* Table, __in CLONG ByteSize) {
    DbgPrint("RtlAlloc");
    ExAllocatePool(NonPagedPool, ByteSize);
}

// 自定义内存释放函数：
VOID RtlFree(__in struct _RTL_GENERIC_TABLE* Table, __in PVOID Buffer) {
    DbgPrint("RtlFree");
    ExFreePool(Buffer);
}

void UseGenericTable() {
    RTL_GENERIC_TABLE root = { 0 };
    TEST test01 = { 1, 2 };
    TEST test02 = { 2, 3 };
    TEST test03 = { 3, 4 };
    TEST test04 = { 4, 5 };
    BOOLEAN insertResult = FALSE;
    // 初始化二叉查找树结构，需要指定比较、内存申请、内存释放函数
    RtlInitializeGenericTable(&root, RtlCmp, RtlAlloc, RtlFree, NULL);
    DbgPrint("RtlInitializeGenericTable");
    // -----------------------------------------------------------------------------------------------------------
    // 操作一：插入结点；
    RtlInsertElementGenericTable(&root, &test01, sizeof(TEST), &insertResult);
    DbgPrint("插入结果: %d", insertResult);
    RtlInsertElementGenericTable(&root, &test02, sizeof(TEST), &insertResult);
    DbgPrint("插入结果: %d", insertResult);    
    RtlInsertElementGenericTable(&root, &test03, sizeof(TEST), &insertResult);
    DbgPrint("插入结果: %d", insertResult);    
    RtlInsertElementGenericTable(&root, &test04, sizeof(TEST), &insertResult);
    DbgPrint("插入结果: %d", insertResult);
    DbgPrint("------------------------------");
    // -----------------------------------------------------------------------------------------------------------
    // 操作二：查询结点个数；
    ULONG num = RtlNumberGenericTableElements(&root);
    DbgPrint("二叉搜索树节点个数: %d", num);
    DbgPrint("------------------------------");
    // -----------------------------------------------------------------------------------------------------------
    // 操作三：遍历；
    // 第一种遍历方法：
    for (ULONG i = 0; i < num; i++) {
        PTEST ptest = RtlGetElementGenericTable(&root, i);
        DbgPrint("方法一遍历结果为: %d", ptest->a);
    }
    DbgPrint("------------------------------");
    // 第二种遍历方法：
    ULONG RestarKey = 0;
    PTEST ptr = NULL;
    for (ptr = RtlEnumerateGenericTableWithoutSplaying(&root, &RestarKey);
        ptr != NULL;
        ptr = RtlEnumerateGenericTableWithoutSplaying(&root, &RestarKey)) {
        DbgPrint("方法二遍历结果为: %d", ptr->a);
    }
    // -----------------------------------------------------------------------------------------------------------
    // 操作四：查找指定结点；
    PTEST pfind = RtlLookupElementGenericTable(&root, &test03);
    if (NULL != pfind) {
        DbgPrint("已找到指定节点!!!");
    }
    // -----------------------------------------------------------------------------------------------------------
    // 其他操作：删除指定节点、判断是否为空；
    // RtlDeleteElementGenericTable();
    // RtlIsGenericTableEmpty();
    // -----------------------------------------------------------------------------------------------------------
}

NTSTATUS DriverEntry(PDRIVER_OBJECT pDriver, PUNICODE_STRING pRegPath) {
	UseGenericTable();
	pDriver->DriverUnload = DriverUnload;
	return STATUS_SUCCESS;
}