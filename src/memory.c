#include "memory.h"

#include <assert.h>

page_t page_new_memory(uint32_t base) {
    page_t map;
    map.type = PAGE_MEMORY;
    map.base = base;
    map.physical = malloc(PAGE_MEMSIZE);
    return map;
}

void page_del(page_t* page) {
    switch (page->type) {
        case PAGE_MEMORY:
            free(page->physical);
            break;
        default:
            fprintf(stderr, "Invalid page type (%d)\n", page->type);
    }
}

#define PAGE_MEM_GET(dtype, tname)                                          \
dtype page_mem_get_##tname(page_t* page, uint32_t addr) {                  \
    assert(page->type == PAGE_MEMORY);                                       \
    dtype result = 0;                                                       \
    for (uint32_t i = 0; i < sizeof(dtype); i++) {                          \
        uint8_t byte = *(page->physical + addr + i);                         \
        result = (result << 8) | byte;                                      \
    }                                                                       \
    return result;                                                          \
}

PAGE_MEM_GET(int8_t, byte)
//PAGE_MEM_GET(uint8_t, ubyte)
uint8_t page_mem_get_ubyte(page_t *page, uint32_t addr) {
    assert(page->type == PAGE_MEMORY);
    uint8_t result = 0;
    for (uint32_t i = 0; i < sizeof(uint8_t); i++) {
        uint8_t byte = *(page->physical + addr + i);
        result = (result << 8) | byte;
    }
    return result;
}
PAGE_MEM_GET(int16_t, hword)
PAGE_MEM_GET(uint16_t, uhword)
PAGE_MEM_GET(int32_t, word)
PAGE_MEM_GET(uint32_t, uword)


#define PAGE_MEM_SET(dtype, tname)                                          \
void page_mem_set_##tname(page_t* page, uint32_t addr, dtype value) {       \
    assert(page->type == PAGE_MEMORY);                                      \
    for (uint32_t i = 0; i < sizeof(dtype); i++) {                          \
        uint8_t byte = addr >> (8 * (sizeof(dtype) - i - 1));               \
        *(page->physical + addr + i) = byte;                                \
    }                                                                       \
}

PAGE_MEM_SET(int8_t, byte)
PAGE_MEM_SET(uint8_t, ubyte)
PAGE_MEM_SET(int16_t, hword)
PAGE_MEM_SET(uint16_t, uhword)
PAGE_MEM_SET(int32_t, word)
PAGE_MEM_SET(uint32_t, uword)

/*
 * MAPLIST
 */

mmu_t mmu_new(uint32_t capacity) {
    mmu_t list;
    list.size = 0;
    list.capacity = capacity;
    list.mappings = malloc(capacity * sizeof(page_t));
    return list;
}

void mmu_del(mmu_t* list) {
    for (uint32_t i = 0; i < list->size; i++) {
        page_t* page = mmu_get(list, i);
        page_del(page);
    }
    free(list->mappings);
}

static inline void maplist_resize(mmu_t *list, uint32_t newcap) {
    list->mappings = realloc(list->mappings, newcap * sizeof(page_t));
    list->capacity = newcap;
}

void mmu_add(mmu_t *list, page_t *mapping) {
    if (list->size == list->capacity)
        maplist_resize(list, list->capacity * 3 / 2);
    list->mappings[list->size] = *mapping;
    list->size++;
}

void mmu_remove(mmu_t *list, uint32_t index) {
    if (index >= 0 && index < list->size) {
        memmove(list->mappings + index, list->mappings + index + 1, (list->size - index - 1) * sizeof(page_t));
        list->size--;
    }
}
