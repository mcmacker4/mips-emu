#ifndef EMU_CPU_H
#define EMU_CPU_H

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

#define FATAL(...) { \
    fprintf(stderr, __VA_ARGS__); \
    exit(-1); }

#define R(r) (cpu_reg_get(cpu, r))
#define RU(r) ((uint32_t) cpu_reg_get(cpu, r))
#define S(r, v) (cpu_reg_set(cpu, r, v))

#define MEM_GET(type, addr) \
    (*(type*) (cpu->memory + (addr)))
#define MEM_SET(type, addr, v) \
    (*(type*) (cpu->memory + (addr)) = (v))

typedef struct cpu {
    int32_t registers[36];
    uint8_t memory[4096];

    bool halt;
} cpu_t;

cpu_t* cpu_new();

void cpu_next(cpu_t* cpu);
void cpu_exec(cpu_t* cpu, uint32_t instr);

static inline int32_t cpu_reg_get(cpu_t* cpu, uint8_t r) {
    if (r == 0) return 0;
    return cpu->registers[r];
}

static inline void cpu_reg_set(cpu_t* cpu, uint8_t r, int32_t v) {
    if (r != 0) {
        cpu->registers[r] = v;
    }
}

#endif //EMU_CPU_H
