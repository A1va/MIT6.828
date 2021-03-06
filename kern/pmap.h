/* See COPYRIGHT for copyright information. */

#ifndef JOS_KERN_PMAP_H
#define JOS_KERN_PMAP_H
#ifndef JOS_KERNEL
# error "This is a JOS kernel header; user programs should not #include it"
#endif

#include <inc/memlayout.h>
#include <inc/assert.h>
struct Env;

extern char bootstacktop[], bootstack[];

extern struct PageInfo *pages;
extern size_t npages;

extern pde_t *kern_pgdir;


/**
 * PADDR/_paddr: 将虚拟地址转化为物理地址，宏函数 PADDR 在输入的虚拟地址上减去 KERNBASE。
 */
#define PADDR(kva) _paddr(__FILE__, __LINE__, kva)

static inline physaddr_t
_paddr(const char *file, int line, void *kva)
{
	if ((uint32_t)kva < KERNBASE)
		_panic(file, line, "PADDR called with invalid kva %08lx", kva);
	return (physaddr_t)kva - KERNBASE;
}

/**
 * 宏函数 KADDR 调用了函数 _kaddr，将物理地址转化成虚拟地址，也就是在物理地址的数值上加上了 KERNBAE.
 * 
 * JOS内核有时候在仅知道物理地址的情况下，想要访问该物理地址，但是没有办法绕过MMU的线性地址转换机制，所以没有办法用物理地址直接访问。
 * JOS将虚拟地址0xF0000000映射到物理地址0x0处的一个原因就是希望能有一个简便的方式实现物理地址和线性地址的转换。
 * 在知道物理地址pa的情况下可以加0xF0000000得到对应的线性地址，可以用KADDR(pa)宏实现。
 * 在知道线性地址va的情况下减0xF0000000可以得到物理地址，可以用宏PADDR(va)实现。
 * 该宏采用虚拟地址 -- 指向 KERNBASE 之上的地址，在该地址上映射了机器最大 256MB 的物理内存并返回相应的物理地址。
 * 如果向其传递一个非虚拟地址，它会产生 panic 异常。
 */ 
#define KADDR(pa) _kaddr(__FILE__, __LINE__, pa)

static inline void*
_kaddr(const char *file, int line, physaddr_t pa)
{
	if (PGNUM(pa) >= npages)
		_panic(file, line, "KADDR called with invalid pa %08lx", pa);
	return (void *)(pa + KERNBASE);
}


enum {
	// 用于 page_alloc 函数，将返回的物理页清零。
	ALLOC_ZERO = 1<<0,
};

void	mem_init(void);

void	page_init(void);
struct PageInfo *page_alloc(int alloc_flags);
void	page_free(struct PageInfo *pp);
int	page_insert(pde_t *pgdir, struct PageInfo *pp, void *va, int perm);
void	page_remove(pde_t *pgdir, void *va);
struct PageInfo *page_lookup(pde_t *pgdir, void *va, pte_t **pte_store);
void	page_decref(struct PageInfo *pp);

void	tlb_invalidate(pde_t *pgdir, void *va);

int	user_mem_check(struct Env *env, const void *va, size_t len, int perm);
void	user_mem_assert(struct Env *env, const void *va, size_t len, int perm);

// 每个物理页对应一个strcut PageInfo和一个物理页号PPN和唯一的物理首地址
// 返回PageInfo结构pp对应物理页的物理首地址，physaddr_t(uint32_t) 自定义的物理地址类型
static inline physaddr_t
page2pa(struct PageInfo *pp)
{
	// pages: 物理页的状态描述数组首地址，(pp - pages)就是pp 在数组中的索引
	// 又因分配的所有物理内存都是页对齐，因此 索引*PGSIZE 就是对应的物理页首地址
	// (<<12 = *1000H = *PGSIZE)
	return (pp - pages) << PGSHIFT;	
}

// 返回物理地址pa所在物理页对应的PageInfo结构
static inline struct PageInfo*
pa2page(physaddr_t pa)
{
	if (PGNUM(pa) >= npages)
		panic("pa2page called with invalid pa");
	return &pages[PGNUM(pa)];
}

// 返回PageInfo结构pp对应物理页的虚拟首地址：获取PageInfo结构对应的物理地址，再调用 KADDR 获取物理地址对应的虚拟地址
// 与 page2pa 类似，只不过返回的是 PageInfo 结构 pp 所对应的物理页的虚拟首地址PPN(20)
static inline void*
page2kva(struct PageInfo *pp)
{
	return KADDR(page2pa(pp));
}

pte_t *pgdir_walk(pde_t *pgdir, const void *va, int create);

#endif /* !JOS_KERN_PMAP_H */
