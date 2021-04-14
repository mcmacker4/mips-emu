#include <stdio.h>

#include "cpu.h"
#include <memory.h>

uint32_t program[] = {
/* 00000000 */ 0x2408001e, // ; |input:12| addiu $t0, $zero, 30
/* 00000004 */ 0x24090014, // ; |input:14| addiu $t1, $zero, 20
/* 00000008 */ 0x01092020, // ; |input:18| add $a0, $t0, $t1
/* 0000000c */ 0x24020001, // ; |input:22| addiu $v0, $zero, 1
/* 00000010 */ 0x0000000c, // ; |input:26| syscall
/* 00000014 */ 0x24040000, // ; |input:33| addiu $a0, $zero, str[hi]

/* 00000018 */ 0x00042400, // ; |input:34| sll $a0, $a0, 16
/* 0000001c */ 0x24840030, // ; |input:35| addiu $a0, $a0, str[lo]
/* 00000020 */ 0x24020004, // ; |input:38| addiu $v0, $zero, 4
/* 00000024 */ 0x0000000c, // ; |input:42| syscall
/* 00000028 */ 0x00001020, // ; |input:45| add $v0, $zero, $zero
/* 0000002c */ 0x0000000c, // ; |input:46| syscall

/* 00000030 */ 0x6c6c6548, // ; lleH
/* 00000034 */ 0x6f57206f, // ; oW o
/* 00000038 */ 0x21646c72, // ; !dlr
/* 0000003c */ 0x0000000a, // ; ___\n
};

int main() {

    cpu_t* cpu = cpu_new();

    memcpy(cpu->memory, program, sizeof(program));

    while (!cpu->halt) {
        cpu_next(cpu);
    }

    free(cpu);

}
