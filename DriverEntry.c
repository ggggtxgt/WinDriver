#include <ntifs.h>

/******************************************************************************
 * @func      DriverUnload
 * @brief     驱动卸载回调函数：当驱动被卸载时，系统自动启用该函数；
 * @param     驱动对象的结构体指针；
*******************************************************************************/
void DriverUnload(PDRIVER_OBJECT pDriver) {
    DbgPrint("DriverUnload!!!");
}

/******************************************************************************
 * @func      DriverEntry
 * @brief     驱动入口函数：相当于驱动程序的 main 函数；
 * @param     指向驱动对象的结构体指针；
 * @param     指向驱动在注册表中路径的UNICODE字符串指针；
 * @return    NTSTATUS类型的状态码，表示驱动是否加载成功；
*******************************************************************************/
NTSTATUS DriverEntry(PDRIVER_OBJECT pDriver, PUNICODE_STRING pRegPath) {
    DbgBreakPoint();
    // 调试输出
    DbgPrint("Hello World!");
    // 设置驱动的卸载回调函数
    pDriver->DriverUnload = DriverUnload;
    // 返回成功状态码
    return STATUS_SUCCESS;
}