#include <iostream>
#include <Windows.h>

// APC 注入函数：将dll注入到目标进程的指定线程之中
void APCInject() {
	DWORD piid = 0;		// 目标进程ID
	DWORD threadId = 0;	// 目标线程ID
	printf("请输入进程ID\n");
	scanf_s("%d", &piid);
	// 打开目标进程，获取进程句柄
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, piid);
	if (!hProcess) {
		printf("打开进程失败!!!\n");
		return;
	}
	printf("请输入线程ID\n");
	scanf_s("%d", &threadId);
	// 打开目标线程，并获取线程句柄
	HANDLE hThread = OpenThread(PROCESS_ALL_ACCESS, FALSE, threadId);
	if (!hThread) {
		printf("打开线程失败!!!\n");
		CloseHandle(hProcess);
		return;
	}
	// 在目标进程的虚拟地址空间分配内存
	PVOID pbuff = VirtualAllocEx(hProcess, NULL, strlen("test.dll") + 1, MEM_COMMIT, PAGE_READWRITE);
	if (NULL == pbuff) {
		printf("内存申请失败!!!\n");
		CloseHandle(hThread);
		CloseHandle(hProcess);
		return;
	}
	ULONG realWrite = 0;
	// 将 dll路径字符串写入目标进程分配的内存
	BOOL result = WriteProcessMemory(hProcess, pbuff, "test.dll", strlen("test.dll") + 1, &realWrite);
	if (FALSE == result) {
		printf("写入内存失败!!!\n");
		VirtualFreeEx(hProcess, pbuff, 0, MEM_RELEASE);
		CloseHandle(hThread);
		CloseHandle(hProcess);
		return;
	}
	// 获取 LoadLibraryA 函数地址
	PVOID funcAddr = GetProcAddress(GetModuleHandleA("kernel32.dll"), "LoadLibraryA");
	if (!funcAddr) {
		printf("获取函数地址失败!!!\n");
		VirtualFreeEx(hProcess, pbuff, 0, MEM_RELEASE);
		CloseHandle(hThread);
		CloseHandle(hProcess);
		return;
	}
	QueueUserAPC((PAPCFUNC)funcAddr, hThread, (ULONG)pbuff);
}

int main() {
	APCInject();
	system("pause");
	return 0;
}