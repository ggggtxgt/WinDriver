#pragma once
#include <ntifs.h>

// APC环境枚举，指定APC执行时使用的进程上下文环境
typedef enum KAPC_ENVIRONMENT {
	OriginalApcEnvironment,    // 原始APC环境：使用APC创建时的原始进程上下文
	AttachedApcEnvironment,    // 附加APC环境：使用当前线程已挂靠的进程上下文
	CurrentApcEnvironment,     // 当前APC环境：使用当前线程的当前进程上下文
	InsertApcEnvironment       // 插入APC环境：在APC插入队列时的环境
} KAPC_ENVIRONMENT;

// 普通APC例程函数指针类型
typedef VOID(*PKNORMAL_ROUTINE) (
	IN PVOID NormalContext,    // 用户定义的上下文参数
	IN PVOID SystemArgument1,  // 系统参数1
	IN PVOID SystemArgument2   // 系统参数2
);

// 内核APC例程函数指针类型
typedef VOID(*PKKERNEL_ROUTINE)(
	IN struct _KAPC* Apc,						// 指向当前APC对象的指针
	IN OUT PKNORMAL_ROUTINE* NormalRoutine,		// 指向普通例程指针的指针（可修改）
	IN OUT PVOID* NormalContext,				// 指向普通上下文指针的指针（可修改）
	IN OUT PVOID* SystemArgument1,				// 指向系统参数1指针的指针（可修改）
	IN OUT PVOID* SystemArgument2				// 指向系统参数2指针的指针（可修改）
);

// 将APC对象插入到目标线程的APC队列中
BOOLEAN KeInsertQueueApc(
	__inout PRKAPC Apc,               // 指向KAPC结构体的指针（输入/输出参数）
	__in_opt PVOID SystemArgument1,   // 可选输入：系统参数1
	__in_opt PVOID SystemArgument2,   // 可选输入：系统参数2
	__in KPRIORITY Increment          // 输入：线程优先级提升值
);

// APC终止例程函数指针类型
typedef VOID(*PKRUNDOWN_ROUTINE)(
	IN struct _KAPC* Apc      // 指向要清理的APC对象的指针
);

// 初始化APC对象
VOID KeInitializeApc(
	__out PRKAPC Apc,							// 输出：指向KAPC结构体的指针
	__in PRKTHREAD Thread,						// 输入：目标线程对象指针
	__in KAPC_ENVIRONMENT Environment,			// 输入：APC环境类型
	__in PKKERNEL_ROUTINE KernelRoutine,		// 输入：内核例程函数指针
	__in_opt PKRUNDOWN_ROUTINE RundownRoutine,	// 输入可选：终止例程函数指针
	__in_opt PKNORMAL_ROUTINE NormalRoutine,	// 输入可选：普通例程函数指针
	__in_opt KPROCESSOR_MODE ProcessorMode,		// 输入可选：处理器模式（用户/内核）
	__in_opt PVOID NormalContext				// 输入可选：普通例程上下文参数
);