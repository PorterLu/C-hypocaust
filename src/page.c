#include "hypervisor.h"

extern uint64_t TEXT_START;
extern uint64_t TEXT_END;
extern uint64_t RODATA_START;
extern uint64_t RODATA_END;
extern uint64_t BSS_START;
extern uint64_t BSS_END;
extern uint64_t HEAP_START;
extern uint64_t HEAP_SIZE;

static uint64_t _alloc_start = 0;
static uint64_t _alloc_end = 0;
static uint64_t _num_pages = 0;

#define PAGE_SIZE 4096
#define PAGE_ORDER 12

#define PAGE_TAKEN (uint8_t)(1 << 0) 			//这个页是否被占用
#define PAGE_LAST (uint8_t)(1 << 1)				//这个页是否是申请的一系列页中的最后一位

struct Page{
	uint8_t flags;								
};

static inline void _clear(struct Page *page){	//只需将标志位清0
	page->flags = 0;
}

static inline int _is_free(struct Page *page){	//检测该页Page的第0位检测是否被占用
	if(page->flags & PAGE_TAKEN){
		return 0;
	} else {
		return 1;
	}
}

static inline void _set_flag(struct Page *page, uint8_t flags){	//标记第0位表示已经被占用
	page->flags |= flags;
}

static inline int _is_last(struct Page *page){					//检测第1位判断是否是这一系列页中
	if(page->flags & PAGE_LAST){
		return 1;
	} else {
		return 0;
	}
}

static inline uint64_t _align_page(uint64_t address){			//对地址进行对齐，对地址进行向上去整，进行4kb的对齐
	uint64_t order = (1 << PAGE_ORDER) - 1;
	return (address + order) & (~order);
}

void page_init(){												//初始化，计算剩余的存储空间，够分配几页，其中8页用于管理其它的页
	_num_pages = (HEAP_SIZE / PAGE_SIZE) - 8;
	printf("HEAP_START = %x, HEAP_SIZE = %x, num of pages = %d\n", HEAP_START, HEAP_SIZE, _num_pages);

	struct Page *page = (struct Page*) HEAP_START;				//初始化这些页结构
	for(int i = 0; i < _num_pages; i++){
		_clear(page);
		page++;
	}

	_alloc_start = _align_page(HEAP_START + 8 * PAGE_SIZE);		//实际使用的页的起始地址
	_alloc_end = _alloc_start + (PAGE_SIZE * _num_pages);		//实际使用的页的结束地址

	printf("TEXT:	0x%x -> 0x%x\n", TEXT_START, TEXT_END);		
	printf("RODATA: 0x%x -> 0x%x\n", RODATA_START, RODATA_END);
	printf("BSS:	0x%x -> 0x%x\n", BSS_START, BSS_END);
	printf("HEAP:	0x%x -> 0x%x\n", _alloc_start, _alloc_end);
}

void *page_alloc(int npages){									//分配n个pages
	int found = 0;
	struct Page *page_i = (struct Page*) HEAP_START;			//检测页管理单元
	for(int i = 0; i <= (_num_pages - npages); i++){			//两层循环，用于检测使用
		if(_is_free(page_i)){
			found = 1;
			struct Page *page_j = page_i;						
			for(int j = i; j < (i + npages); j++){
				if(!_is_free(page_j)){
					found = 0;
					break;
				}
				page_j++;
			}

			if(found){											//如果找到了连续的npages个页
				struct Page *page_k = page_i;
				for(int k = i; k < (i + npages); k++){			//将这几个位标记为已经被占用
					_set_flag(page_k, PAGE_TAKEN);
					page_k ++;
				}
				page_k --;
				_set_flag(page_k, PAGE_LAST);					//将最后一个位标记为最后一个位，方便进行free的时候进行处理
				return (void*)(_alloc_start + i * PAGE_SIZE);	//返回起始地址
			}
		}
		page_i++;
	}
	return NULL;
}

void page_free(void *p){
	if(!p || (uint64_t)p >= _alloc_end){						//空页和界限检查
		return;
	}
	struct Page *page = (struct Page*) HEAP_START;
	page += ((uint64_t)p - _alloc_start)/PAGE_SIZE;
	while(!_is_free(page)){										//循环检查直至最后一个页
		if(_is_last(page)){
			_clear(page);
			break;
		} else {
			_clear(page);
			page++;
		}
	}
}

