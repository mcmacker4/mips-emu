#include "syscall.h"

#include "register.h"

#include <stdio.h>

void syscall(cpu_t* cpu) {

    switch (RU(V0)) {
        case 0:
            cpu->halt = true;
            break;
        case 1:
        case 3:
            printf("%d", R(A0));
            break;
        case 4: {
            uint32_t addr = R(A0);
            uint32_t off = 0;
            uint8_t c = cpu_mem_get_ubyte(cpu, addr);
            while (c != 0) {
                printf("%c", c);
                off++;
                c = cpu_mem_get_ubyte(cpu, addr + off);
            }
            break;
        }
        default:
            FATAL("Unsupported Syscall (%d).", R(V0))
    }

}