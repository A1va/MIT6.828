#ifndef JOS_INC_TYPES_H
#define JOS_INC_TYPES_H

#ifndef NULL
#define NULL ((void*) 0)
#endif

// Represents true-or-false values
typedef _Bool bool;
enum { false, true };

// 整数类型的显式大小版本
typedef __signed char int8_t;
typedef unsigned char uint8_t;
typedef short int16_t;
typedef unsigned short uint16_t;
typedef int int32_t;
typedef unsigned int uint32_t;
typedef long long int64_t;
typedef unsigned long long uint64_t;

// 指针和地址的长度均为32位。我们使用指针类型表示虚拟地址，使用 uintptr_t 表示虚拟地址的数值，使用 physaddr_t 表示物理地址。
typedef int32_t intptr_t;
typedef uint32_t uintptr_t;
typedef uint32_t physaddr_t;

// 页码长度为32位。
typedef uint32_t ppn_t;

// size _ t 用于表示内存对象的大小。
typedef uint32_t size_t;

// ssize_t是size_t的签名版本，用于可能会产生错误的情况。
typedef int32_t ssize_t;

// off_t 用于文件偏移和长度
typedef int32_t off_t;

// 高效的比大小操作
#define MIN(_a, _b)						\
({								\
	typeof(_a) __a = (_a);					\
	typeof(_b) __b = (_b);					\
	__a <= __b ? __a : __b;					\
})
#define MAX(_a, _b)						\
({								\
	typeof(_a) __a = (_a);					\
	typeof(_b) __b = (_b);					\
	__a >= __b ? __a : __b;					\
})

// 实际作用就是将 sz 向下取整成 PGSIZE 的倍数，如 sz=5369, PGSIZE=4096, 那么 addr=4096
#define ROUNDDOWN(a, n)						\
({								\
	uint32_t __a = (uint32_t) (a);				\
	(typeof(a)) (__a - __a % (n));				\
})
// 实际作用就是将 sz 向上取整(对齐)成 PGSIZE 的倍数，如 sz=5369, PGSIZE=4096, 那么 addr=8192
#define ROUNDUP(a, n)						\
({								\
	uint32_t __n = (uint32_t) (n);				\
	(typeof(a)) (ROUNDDOWN((uint32_t) (a) + __n - 1, __n));	\
})

#define ARRAY_SIZE(a)	(sizeof(a) / sizeof(a[0]))

// 返回 成员属性 相对于结构类型开头的偏移量
#define offsetof(type, member)  ((size_t) (&((type*)0)->member))

#endif /* !JOS_INC_TYPES_H */
