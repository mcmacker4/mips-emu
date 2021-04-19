#include <stdio.h>

#include "cpu.h"
#include "register.h"

#include <memory.h>

#include "program.h"

#ifndef NDEBUG
void cpu_print_status(cpu_t* cpu) {
    printf("===============================\n");
    printf("REGISTERS:\n");
    for (uint32_t i = 0; i < 32; i++) {
        printf("%02d: 0x%08x", i, cpu_reg_get(cpu, i));
        if (i % 4 == 3) printf("\n"); else printf("\t");
    }
    printf("PC: 0x%08x\t", cpu_reg_get(cpu, PC));
    printf("I: 0x%08x\n", cpu_mem_get_uword(cpu, cpu_reg_get(cpu, PC)));
}
#endif

int main() {

    cpu_t* cpu = cpu_new();

    page_t textpage = page_new_memory(0x00400000);
    memcpy(textpage.physical, program_text, sizeof(program_text));

    page_t datapage = page_new_memory(0x00c00000);
    memcpy(datapage.physical, program_data, sizeof(program_data));

    mmu_add(&cpu->mmu, &textpage);
    mmu_add(&cpu->mmu, &datapage);

    cpu_reg_set(cpu, PC, 0x00400000);
    cpu_reg_set(cpu, RA, UINT32_MAX);

#ifndef NDEBUG
    cpu_print_status(cpu);
#endif

    while ((uint32_t) cpu_reg_get(cpu, PC) != UINT32_MAX) {
        cpu_next(cpu);
        //cpu_print_status(cpu);
    }

#ifndef NDEBUG
    cpu_del(cpu);
#endif
    free(cpu);

}
