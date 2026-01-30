#include <ntifs.h>

#define MSG_CODE_WRITE CTL_CODE(FILE_DEVICE_UNKNOWN, 0x801, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define MSG_CODE_READ CTL_CODE(FILE_DEVICE_UNKNOWN, 0x802, METHOD_BUFFERED, FILE_ANY_ACCESS)

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
NTSTATUS DispatchClose(struct _DEVICE_OBJECT* DeviceObject, struct _IRP* Irp) {
    DbgPrint("IRP_MJ_CLOSE 已经触发!!!");
    Irp->IoStatus.Status = STATUS_SUCCESS;
    Irp->IoStatus.Information = 0;
    IoCompleteRequest(Irp, IO_NO_INCREMENT);
    return STATUS_SUCCESS;
}

// 派遣函数 -- 类似于回调函数
NTSTATUS DispatchWrite(struct _DEVICE_OBJECT* DeviceObject, struct _IRP* Irp) {
    DbgPrint("IRP_MJ_WRITE 已经触发!!!");
    // R3写入数据，而R0需要从中取出数据
    // 获取当前IRP栈
    PIO_STACK_LOCATION irpStack = IoGetCurrentIrpStackLocation(Irp);
    // 获取缓冲区大小
    ULONG bufferLen = irpStack->Parameters.Write.Length;
    // 获取缓冲区地址
    PVOID buff = Irp->AssociatedIrp.SystemBuffer;
    if (buff) {
        DbgPrint("%s", buff);
    }
    Irp->IoStatus.Status = STATUS_SUCCESS;
    Irp->IoStatus.Information = 0;
    IoCompleteRequest(Irp, IO_NO_INCREMENT);
    return STATUS_SUCCESS;
}

// 派遣函数 -- 类似于回调函数
NTSTATUS DispatchRead(struct _DEVICE_OBJECT* DeviceObject, struct _IRP* Irp) {
    DbgPrint("IRP_MJ_READ 已经触发!!!");
    // R3写入数据，而R0需要从中取出数据
    // 获取当前IRP栈
    PIO_STACK_LOCATION irpStack = IoGetCurrentIrpStackLocation(Irp);
    // 获取缓冲区地址
    PVOID buff = Irp->AssociatedIrp.SystemBuffer;
    if (buff) {
        memcpy(buff, "Hello Driver", strlen("Hello Driver") + 1);
    }
    Irp->IoStatus.Status = STATUS_SUCCESS;
    Irp->IoStatus.Information = strlen("Hello Driver") + 1;
    IoCompleteRequest(Irp, IO_NO_INCREMENT);
    return STATUS_SUCCESS;
}

// 派遣函数 -- 类似于回调函数
NTSTATUS DispatchDeviceControl(struct _DEVICE_OBJECT* DeviceObject, struct _IRP* Irp) {
    DbgPrint("IRP_MJ_READ 已经触发!!!");
    // R3写入数据，而R0需要从中取出数据
    // 获取当前IRP栈
    PIO_STACK_LOCATION irpStack = IoGetCurrentIrpStackLocation(Irp);
    // 获取缓冲区地址
    PVOID buff = Irp->AssociatedIrp.SystemBuffer;
    if (buff) {
        memcpy(buff, "Hello Driver", strlen("Hello Driver") + 1);
    }
    Irp->IoStatus.Status = STATUS_SUCCESS;
    Irp->IoStatus.Information = strlen("Hello Driver") + 1;
    IoCompleteRequest(Irp, IO_NO_INCREMENT);
    return STATUS_SUCCESS;
}

// 默认派遣函数
NTSTATUS DispatchAll(struct _DEVICE_OBJECT* DeviceObject, struct _IRP* Irp) {
    // 获取当前IRP栈
    PIO_STACK_LOCATION pIostack = IoGetCurrentIrpStackLocation(Irp);
    // 获取缓冲区大小
    ULONG inLen = pIostack->Parameters.DeviceIoControl.InputBufferLength;
    ULONG outLen = pIostack->Parameters.DeviceIoControl.OutputBufferLength;
    // 根据类型进行对应操作
    if (IRP_MJ_DEVICE_CONTROL == pIostack->MajorFunction) {
        // 根据操作码选择分支操作
        switch (pIostack->Parameters.DeviceIoControl.IoControlCode) {
            case MSG_CODE_WRITE:
                DbgPrint("DeviceControl: %s", Irp->AssociatedIrp.SystemBuffer);
                Irp->IoStatus.Information = 0;
                break;
            case MSG_CODE_READ:
                memcpy(Irp->AssociatedIrp.SystemBuffer, "this is DeviceControl Read!!!", strlen("this is DeviceControl Read!!!"));
                Irp->IoStatus.Information = strlen("this is DeviceControl Read!!!");
        }
    } else {
        // 不进行任何操作
        Irp->IoStatus.Information = 0;
    }
    Irp->IoStatus.Status = STATUS_SUCCESS;
    IoCompleteRequest(Irp, IO_NO_INCREMENT);
    return STATUS_SUCCESS;
}

// 创建设备对象函数详解：
/*
NTSTATUS IoCreateDevice(
    _In_     PDRIVER_OBJECT  DriverObject,          // 调用此函数的驱动对象指针，设备将关联到此驱动
    _In_     ULONG           DeviceExtensionSize,   // 设备扩展结构的大小（字节数），用于存储设备特定的上下文数据
    _In_opt_ PUNICODE_STRING DeviceName,            // 可选，设备对象名称,通常设为NULL
    _In_     ULONG           DeviceType,            // 设备类型，定义于wdm.h
    _In_     ULONG           DeviceCharacteristics, // 设备特性标志，控制设备行为
    _In_     BOOLEAN         Exclusive,             // 是否独占设备
    _Out_    PDEVICE_OBJECT  *DeviceObject          // 输出参数，接收新创建设备对象的指针
);
*/

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
    pDevice->Flags |= DO_BUFFERED_IO;
    // 设置派遣函数 -- 定义不同类型的派遣函数，根据类型进行调用
    /*
    pDriver->MajorFunction[IRP_MJ_CREATE] = DispatchCreate;
    pDriver->MajorFunction[IRP_MJ_CLOSE] = DispatchClose;
    pDriver->MajorFunction[IRP_MJ_WRITE] = DispatchWrite;
    pDriver->MajorFunction[IRP_MJ_READ] = DispatchRead;
    pDriver->MajorFunction[IRP_MJ_DEVICE_CONTROL] = DispatchDeviceControl;
    */
    // 将 MajorFunction 填充为一种派遣函数，在派遣函数判断不同类型
    ULONG i;
    for (i = 0; i < IRP_MJ_MAXIMUM_FUNCTION; i++) {
        pDriver->MajorFunction[i] = DispatchAll;
    }
}

NTSTATUS DriverEntry(PDRIVER_OBJECT pDriver, PUNICODE_STRING pRegPath) {
    Connection(pDriver);
	pDriver->DriverUnload = DriverUnload;
	return STATUS_SUCCESS;
}