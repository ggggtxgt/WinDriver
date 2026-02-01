#include <ntifs.h>

void DriverUnload(PDRIVER_OBJECT pDriver) {
    // 删除符号连接、设备对象
    UNICODE_STRING deviceSymbolName = RTL_CONSTANT_STRING(L"\\??\\Device02");
    IoDeleteSymbolicLink(&deviceSymbolName);
    IoDeleteDevice(pDriver->DeviceObject);
	DbgPrint("DriverUnload!!!");
}

// 派遣函数 -- 类似于回调函数
NTSTATUS DispatchCreate(struct _DEVICE_OBJECT* DeviceObject, struct _IRP* Irp) {
    DbgPrint("IRP_MJ_CREATE 已经触发!!!");
    Irp->IoStatus.Status = STATUS_SUCCESS;
    Irp->IoStatus.Information = 0;
    IoCompleteRequest(Irp, IO_NO_INCREMENT);
    return STATUS_SUCCESS;
}

// 派遣函数 -- 类似于回调函数
NTSTATUS DispatchRead(struct _DEVICE_OBJECT* DeviceObject, struct _IRP* Irp) {
    DbgPrint("IRP_MJ_READ 已经触发!!!");
    // 使用直接读写方式：R3写入数据，而R0需要从中取出数据
    // 获取当前IRP栈
    PIO_STACK_LOCATION irpStack = IoGetCurrentIrpStackLocation(Irp);
    // 获取缓冲区大小
    ULONG bufferLen = MmGetMdlByteCount(Irp->MdlAddress);
    DbgPrint("缓冲区大小为: %d", bufferLen);
    // 获取R3缓冲区地址
    ULONG r3Addr = MmGetMdlVirtualAddress(Irp->MdlAddress);
    DbgPrint("缓冲区地址为: 0X%08x", r3Addr);
    PVOID r0Buff = MmGetSystemAddressForMdlSafe(Irp->MdlAddress, NormalPagePriority);
    RtlCopyMemory(r0Buff, "直接读写方式", strlen("直接读写方式") + 1);
    Irp->IoStatus.Information = strlen("直接读写方式");
    Irp->IoStatus.Status = STATUS_SUCCESS;
    IoCompleteRequest(Irp, IO_NO_INCREMENT);
    return STATUS_SUCCESS;
}

void Connection(PDRIVER_OBJECT pDriver) {
    PDEVICE_OBJECT pDevice = NULL;
    // 设备名称
    UNICODE_STRING deviceName = RTL_CONSTANT_STRING(L"\\Device\\Device01");
    UNICODE_STRING deviceSymbolName = RTL_CONSTANT_STRING(L"\\??\\Device02");
    // 创建设备对象
    IoCreateDevice(pDriver, 0, &deviceName, FILE_DEVICE_UNKNOWN, FILE_DEVICE_SECURE_OPEN, FALSE, &pDevice);
    // 创建符号连接
    NTSTATUS status = IoCreateSymbolicLink(&deviceSymbolName, &deviceName);
    if (!NT_SUCCESS(status)) {
        DbgPrint("创建符号连接失败!!!");
        IoDeleteDevice(pDevice);
    }
    // 指定数据交互方式
    // DO_BUFFERED_IO: 缓冲区读写，将R3缓存区数据复制到R0
    // DO_DIRECT_IO: 直接读写，R3与R0不同线性地址映射到同一个物理页
    pDevice->Flags |= DO_DIRECT_IO;
    // 设置派遣函数 -- 定义不同类型的派遣函数，根据类型进行调用
    pDriver->MajorFunction[IRP_MJ_CREATE] = DispatchCreate;
    pDriver->MajorFunction[IRP_MJ_READ] = DispatchRead;

}

NTSTATUS DriverEntry(PDRIVER_OBJECT pDriver, PUNICODE_STRING pRegPath) {
    Connection(pDriver);
	pDriver->DriverUnload = DriverUnload;
	return STATUS_SUCCESS;
}