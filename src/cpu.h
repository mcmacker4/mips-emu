#ifndef EMU_CPU_H
#define EMU_CPU_H

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

#include "memory.h"

#define FATAL(...) { \
    fprintf(stderr, __VA_ARGS__); \
    exit(-1); }

#define R(r) (cpu_reg_get(cpu, r))
#define RU(r) ((uint32_t) cpu_reg_get(cpu, r))
#define S(r, v) (cpu_reg_set(cpu, r, v))
#define PCINC S(PC, RU(PC) + 4)

typedef struct cpu {
    int32_t registers[36];
    mmu_t mmu;

    bool halt;
} cpu_t;

cpu_t* cpu_new();
void cpu_del(cpu_t* cpu);

void cpu_next(cpu_t* cpu);
void cpu_exec(cpu_t* cpu, uint32_t instr);

#define CPU_MEM_GET(dtype, tname) \
dtype cpu_mem_get_##tname(cpu_t* cpu, uint32_t addr);

CPU_MEM_GET(int8_t, byte)
CPU_MEM_GET(uint8_t, ubyte)
CPU_MEM_GET(int16_t, hword)
CPU_MEM_GET(uint16_t, uhword)
CPU_MEM_GET(int32_t, word)
CPU_MEM_GET(uint32_t, uword)


#define CPU_MEM_SET(dtype, tname)                                           \
void cpu_mem_set_##tname(cpu_t* cpu, uint32_t addr, dtype value);

CPU_MEM_SET(int8_t, byte)
CPU_MEM_SET(uint8_t, ubyte)
CPU_MEM_SET(int16_t, hword)
CPU_MEM_SET(uint16_t, uhword)
CPU_MEM_SET(int32_t, word)
CPU_MEM_SET(uint32_t, uword)

static inline int32_t cpu_reg_get(cpu_t* cpu, uint8_t r) {
    if (r == 0) return 0;
    return cpu->registers[r];
}

static inline void cpu_reg_set(cpu_t* cpu, uint8_t r, int32_t v) {
    if (r != 0) {
        cpu->registers[r] = v;
    }
}

#undef CPU_MEM_GET
#undef CPU_MEM_SET

#endif //EMU_CPU_H
