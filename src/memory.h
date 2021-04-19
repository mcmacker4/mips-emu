#ifndef EMU_MEMORY_H
#define EMU_MEMORY_H

#include <stdint.h>
#include <malloc.h>
#include <memory.h>
#include <assert.h>


/*
 *  MAPPINGS
 */

#define PAGE_MEMSIZE 4096
#define PAGE_ALIGN(x) (x & 0xFFFFF000)


typedef enum pagetype {
    PAGE_MEMORY
} pagetype_t;

typedef struct page {
    pagetype_t type;
    union {
        struct {
            uint32_t base;
            uint8_t* physical;
        };
    };
} page_t;

page_t page_new_memory(uint32_t base);
void page_del(page_t* page);

#define PAGE_MEM_GET(type, tname) \
type page_mem_get_##tname(page_t* map, uint32_t addr);

PAGE_MEM_GET(int8_t, byte)
PAGE_MEM_GET(uint8_t, ubyte)
PAGE_MEM_GET(int16_t, hword)
PAGE_MEM_GET(uint16_t, uhword)
PAGE_MEM_GET(int32_t, word)
PAGE_MEM_GET(uint32_t, uword)

#define PAGE_MEM_SET(dtype, tname) \
void page_mem_set_##tname(page_t* map, uint32_t addr, dtype value);

PAGE_MEM_SET(int8_t, byte)
PAGE_MEM_SET(uint8_t, ubyte)
PAGE_MEM_SET(int16_t, hword)
PAGE_MEM_SET(uint16_t, uhword)
PAGE_MEM_SET(int32_t, word)
PAGE_MEM_SET(uint32_t, uword)

#undef PAGE_MEM_GET
#undef PAGE_MEM_SET


/*
 *  MAPLIST
 */

typedef struct mmu {
    page_t* mappings;
    uint32_t size;
    uint32_t capacity;
} mmu_t;

mmu_t                       mmu_new(uint32_t capacity);
void                        mmu_del(mmu_t* list);
static inline page_t*       mmu_get(mmu_t* list, uint32_t idx) {
    assert(idx >= 0 && idx < list->size);
    return list->mappings + idx;
}
static inline uint32_t      mmu_size(mmu_t* list) {
    return list->size;
}
void                        mmu_add(mmu_t* list, page_t* mapping);
void                        mmu_remove(mmu_t* list, uint32_t index);

#endif //EMU_MEMORY_H
