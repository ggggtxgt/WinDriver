#include "PeTools.h"

NTSTATUS PeLoadFile() {
	HANDLE hFile = 0;
	OBJECT_ATTRIBUTES attr = { 0 };
	IO_STATUS_BLOCK ioStatus = { 0 };
	LARGE_INTEGER larg = { 0 };
	LARGE_INTEGER offset = { 0 };
	UNICODE_STRING filePath = RTL_CONSTANT_STRING(L"\\??\\C:\\Windows\\System32\\ntdll.dll");
	InitializeObjectAttributes(&attr, &filePath, OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE, NULL, NULL);
	// 打开文件
	NTSTATUS status = ZwCreateFile(&hFile, GENERIC_READ, &attr, &ioStatus, &larg, FILE_ATTRIBUTE_NORMAL, 
								   FILE_SHARE_READ, FILE_OPEN, FILE_NON_DIRECTORY_FILE | FILE_SYNCHRONOUS_IO_NONALERT, NULL, NULL);
	if (!NT_SUCCESS(status)) {
		return status;
	}
	FILE_STANDARD_INFORMATION fileInfo = { 0 };
	// 查询文件大小
	ZwQueryInformationFile(hFile, &ioStatus, &fileInfo, sizeof(FILE_STANDARD_INFORMATION), FileStandardInformation);
	fileBuffer = ExAllocatePool(PagedPool, fileInfo.EndOfFile.QuadPart);
	if (NULL == fileBuffer) {
		return -1;
	}
	// 读取文件
	status = ZwReadFile(hFile, NULL, NULL, NULL, &ioStatus, fileBuffer, fileInfo.EndOfFile.QuadPart, &offset, NULL);
	return status;
}

ULONG GetFuncAddress() {
	if (NULL == fileBuffer) {
		return 0;
	}
	// 获取 PE 头部
	PIMAGE_DOS_HEADER pDosHeader = fileBuffer;
	// 获取 NT 头部
	PIMAGE_NT_HEADERS pNtHeader = pDosHeader->e_lfanew + fileBuffer;
	// 获取 FileHader 头部
	PIMAGE_FILE_HEADER pFileHeader = &(pNtHeader->FileHeader);
	// 获取可选 PE 头部
	PIMAGE_OPTIONAL_HEADER pOptionHeader = &(pNtHeader->OptionalHeader);
	// 获取导出表
	IMAGE_DATA_DIRECTORY dataDirectory = pOptionHeader->DataDirectory[0];
	PIMAGE_EXPORT_DIRECTORY exportTable = (RvaToFoa(dataDirectory.VirtualAddress) + fileBuffer);
	char* dllName = (RvaToFoa(exportTable->Name) + fileBuffer);
	PeDeleteFileBuffer();
	DbgPrint("%s", dllName);
	return 0;
}

ULONG RvaToFoa(ULONG rva) {
	ULONG Foa = 0;
	// 获取 PE 头部
	PIMAGE_DOS_HEADER pDosHeader = fileBuffer;
	// 获取 NT 头部
	PIMAGE_NT_HEADERS pNtHeader = pDosHeader->e_lfanew + fileBuffer;
	// 获取 FileHader 头部
	PIMAGE_FILE_HEADER pFileHeader = &(pNtHeader->FileHeader);
	// 获取第一个区段头部
	PIMAGE_SECTION_HEADER pSectionHeader = IMAGE_FIRST_SECTION(pNtHeader);
	for (SIZE_T i = 0; i < pFileHeader->NumberOfSections; i++) {
		if (rva >= pSectionHeader->VirtualAddress && rva < pSectionHeader->Misc.VirtualSize + pSectionHeader->VirtualAddress) {
			Foa = pSectionHeader->PointerToRawData + rva - pSectionHeader->VirtualAddress;
			break;
		}
		pSectionHeader++;
	}
	return Foa;
}

void PeDeleteFileBuffer() {
	if (fileBuffer) {
		ExFreePool(fileBuffer);
	}
}