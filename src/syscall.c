#include "syscall.h"

#include "register.h"

#include <stdio.h>

void syscall(cpu_t* cpu) {

    switch (R(V0)) {
        case 1:
        case 3:
            printf("%d\n", R(A0));
            break;
        case 4: {
            uint32_t addr = R(A0);
            uint32_t off = 0;
            uint8_t c = MEM_GET(uint8_t, addr);
            while (c != 0) {
                printf("%c", c);
                off++;
                c = MEM_GET(uint8_t, addr + off);
            }
            printf("\n");
            break;
        }
        case 0:
            cpu->halt = true;
            break;
        default:
            FATAL("Unsupported Syscall (%d).", R(V0))
    }

}