#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunknown-pragmas"
#pragma ide diagnostic ignored "cppcoreguidelines-narrowing-conversions"

#include "cpu.h"
#include "register.h"
#include "syscall.h"

#include <memory.h>


void cpu_exec_r(cpu_t *cpu, uint8_t op, uint8_t rs, uint8_t rt, uint8_t rd, uint8_t sh, uint8_t fun);

cpu_t* cpu_new() {
    cpu_t *cpu = calloc(1, sizeof(cpu_t));
    memset(cpu->registers, 0, sizeof(cpu->registers));
    cpu->mmu = mmu_new(2);
    cpu->halt = false;
    return cpu;
}

void cpu_del(cpu_t* cpu) {
    mmu_del(&cpu->mmu);
}

void cpu_next(cpu_t* cpu) {
    uint32_t instr = cpu_mem_get_uword(cpu, R(PC));
    cpu_exec(cpu, instr);
}


void cpu_exec(cpu_t *cpu, uint32_t instr) {
    uint8_t op = (instr >> 26) & 0x3F;
    uint8_t rs = (instr >> 21) & 0x1F;
    uint8_t rt = (instr >> 16) & 0x1F;
    uint16_t imm = instr & 0xFFFF;

    switch (op) {
        case 0x00:
        case 0x10: {
            uint8_t rd = (instr >> 11) & 0x1F;
            uint8_t sh = (instr >> 6) & 0x1F;
            uint8_t fn = (instr >> 0) & 0x3F;
            cpu_exec_r(cpu, op, rs, rt, rd, sh, fn);
            break;
        }
        case 0x02: { // j
            S(PC, (RU(PC) & 0xF0000000) | ((instr & 0x03FFFFFF) << 2));
            break;
        }
        case 0x03: { // jal
            S(RA, RU(PC));
            S(PC, (RU(PC) & 0xF0000000) | ((instr & 0x03FFFFF) << 2));
            break;
        }
        case 0x08: { // addi
            S(rt, R(rs) + (int32_t) (int16_t) imm);
            PCINC;
            break;
        }
        case 0x09: { // addiu
            S(rt, RU(rs) + (uint16_t) imm);
            PCINC;
            break;
        }
        case 0x0C: { // andi
            // Cast to u16 then to u32 so it compiles as `movzx` instead of `movsx`
            S(rt, R(rs) & ((uint32_t) (uint16_t) imm));
            PCINC;
            break;
        }
        case 0x0D: { // ori
            S(rt, R(rs) | ((uint32_t) (uint16_t) imm));
            PCINC;
            break;
        }
        case 0x0E: { // xori
            S(rt, R(rs) ^ ((uint32_t) (uint16_t) imm));
            PCINC;
            break;
        }
        case 0x01: { // bltz
            if (R(rs) < 0) {
                S(PC, RU(PC) + 4 + ((int32_t) imm << 2));
            } else {
                PCINC;
            }
            break;
        }
        case 0x04: { // beq
            if (R(rs) == R(rt)) {
                S(PC, RU(PC) + 4 + ((int32_t) imm << 2));
            } else {
                PCINC;
            }
            break;
        }
        case 0x05: { // bne
            if (R(rs) != R(rt)) {
                S(PC, RU(PC) + 4 + ((int32_t) imm << 2));
            } else {
                PCINC;
            }
            break;
        }
        case 0x06: { // blez
            if (R(rs) <= 0) {
                S(PC, RU(PC) + 4 + ((int32_t) imm << 2));
            } else {
                PCINC;
            }
            break;
        }
        case 0x07: { // bgtz
            if (R(rs) > 0) {
                S(PC, RU(PC) + 4 + ((int32_t) imm << 2));
            } else {
                PCINC;
            }
            break;
        }
        case 0x20: { // lb
            S(rt, cpu_mem_get_byte(cpu, imm + R(rs)));
            PCINC;
            break;
        }
        case 0x24: { // lbu
            S(rt, (uint32_t) cpu_mem_get_ubyte(cpu, imm + R(rs)));
            PCINC;
            break;
        }
        case 0x21: { // lh
            S(rt, cpu_mem_get_hword(cpu, imm + R(rs)));
            PCINC;
            break;
        }
        case 0x25: { // lhu
            S(rt, (uint32_t) cpu_mem_get_uhword(cpu, imm + R(rs)));
            break;
        }
        case 0x23: { // lw
            S(rt, cpu_mem_get_word(cpu, R(rs) + imm));
            PCINC;
            break;
        }
        case 0x0F: { // lui
            S(rt, R(rs) << 16);
            PCINC;
            break;
        }
        case 0x28: { // sb
            cpu_mem_set_byte(cpu, R(rs) + imm, R(rt));
            PCINC;
            break;
        }
        case 0x29: { // sh
            cpu_mem_set_hword(cpu, R(rs) + imm, R(rt));
            PCINC;
            break;
        }
        case 0x2B: { // sw
            cpu_mem_set_word(cpu, R(rs) + imm, R(rt));
            PCINC;
            break;
        }
        case 0x0A: { // slti
            S(rt, R(rs) < imm ? 1 : 0);
            PCINC;
            break;
        }
        case 0x0B: { // sltiu
            S(rt, RU(rs) < (uint16_t) imm ? 1 : 0);
            PCINC;
            break;
        }
        default:
            printf("Invalid opcode: 0x%02x\n", op);
            break;
    }

#undef IOPER
#undef IOPER_U
#undef BRANCH

}


void cpu_exec_r(cpu_t *cpu, uint8_t op, uint8_t rs, uint8_t rt, uint8_t rd, uint8_t sh, uint8_t fun) {
    if (op == 0x00) {
        switch (fun) {
            case 0x20: { // add
                //TODO: Overflow
                S(rd, R(rs) + R(rt));
                PCINC;
                break;
            }
            case 0x21: // addu
                S(rd, RU(rs) + RU(rt));
                PCINC;
                break;
            case 0x22: // sub
                // TODO: Underflow
                S(rd, R(rs) - R(rt));
                PCINC;
                break;
            case 0x23: // subu
                S(rd, RU(rs) - RU(rt));
                PCINC;
                break;
            case 0x18: { // mul
                int64_t res = R(rs) * R(rt);
                S(HI, (res >> 32) & 0xFF);
                S(LO, res & 0xFF);
                PCINC;
                break;
            }
            case 0x19: { // mulu
                uint64_t res = RU(rs) * RU(rt);
                S(HI, (res >> 32) & 0xFF);
                S(LO, res & 0xFF);
                PCINC;
                break;
            }
            case 0x1A: // div
                S(LO, R(rs) / R(rt));
                S(HI, R(rs) % R(rt));
                PCINC;
                break;
            case 0x1B: // divu
                S(LO, RU(rs) / RU(rt));
                S(HI, RU(rs) % RU(rt));
                PCINC;
                break;
            case 0x2A: {// slt
                S(rd, R(rs) < R(rt) ? 1 : 0);
                PCINC;
                break;
            }
            case 0x2B: // sltu
                S(rd, RU(rs) < RU(rt) ? 1 : 0);
                PCINC;
                break;
            case 0x24: // and
                S(rd, R(rs) & R(rt));
                PCINC;
                break;
            case 0x25: // or
                S(rd, R(rs) | R(rt));
                PCINC;
                break;
            case 0x27: // nor
                S(rd, ~(R(rs) | R(rt)));
                PCINC;
                break;
            case 0x28: // xor
                S(rd, R(rs) ^ R(rt));
                PCINC;
                break;
            case 0x00: // sll
                S(rd, R(rt) << sh);
                PCINC;
                break;
            case 0x02: // srl
                S(rd, RU(rt) >> sh);
                PCINC;
                break;
            case 0x03: // sra
                S(rd, R(rt) >> sh);
                PCINC;
                break;
            case 0x08: // jr
                S(PC, RU(rs));
                break;
            case 0x10: // mfhi
                S(rd, R(HI));
                PCINC;
                break;
            case 0x11: // mthi
                S(HI, R(rs));
                PCINC;
                break;
            case 0x12: // mflo
                S(rd, R(LO));
                PCINC;
                break;
            case 0x13: // mtlo
                S(LO, R(rs));
                PCINC;
                break;
            case 0x0C:
                syscall(cpu);
                PCINC;
                break;
            default: FATAL("Invalid FN type for 0x00 opcode: 0x%02x", fun)
        }
    } else {
        FATAL("Coprocessors not implemented")
    }

#undef OPER
#undef OPER_U

}


#define CPU_MEM_GET(dtype, tname)                                           \
dtype cpu_mem_get_##tname(cpu_t* cpu, uint32_t addr) {                      \
    for (uint32_t pi = 0; pi < mmu_size(&cpu->mmu); pi++) {                 \
        page_t* page = mmu_get(&cpu->mmu, pi);                              \
        if (page->base == PAGE_ALIGN(addr)) {                               \
            return page_mem_get_##tname(page, addr - page->base);           \
        }                                                                   \
    }                                                                       \
    FATAL("Memory read: address 0x%08x not mapped.", addr);                       \
}

CPU_MEM_GET(int8_t, byte)
//CPU_MEM_GET(uint8_t, ubyte)
uint8_t cpu_mem_get_ubyte(cpu_t *cpu, uint32_t addr) {
    for (uint32_t pi = 0; pi < mmu_size(&cpu->mmu); pi++) {
        page_t *page = mmu_get(&cpu->mmu, pi);
        if (page->base == PAGE_ALIGN(addr)) {
            return page_mem_get_ubyte(page, addr - page->base);
        }
    }
    FATAL("Memory read: address 0x%08x not mapped.", addr);
}
CPU_MEM_GET(int16_t, hword)
CPU_MEM_GET(uint16_t, uhword)
CPU_MEM_GET(int32_t, word)
CPU_MEM_GET(uint32_t, uword)


#define CPU_MEM_SET(dtype, tname)                                           \
void cpu_mem_set_##tname(cpu_t* cpu, uint32_t addr, dtype value) {          \
    for (uint32_t pi = 0; pi < mmu_size(&cpu->mmu); pi++) {                 \
        page_t* page = mmu_get(&cpu->mmu, pi);                              \
        if (page->base == PAGE_ALIGN(addr)) {                               \
            page_mem_set_##tname(page, addr - page->base, value);           \
        }                                                                   \
    }                                                                       \
    FATAL("Memory write: address 0x%08x not mapped.", addr);                \
}

CPU_MEM_SET(int8_t, byte)
CPU_MEM_SET(uint8_t, ubyte)
CPU_MEM_SET(int16_t, hword)
CPU_MEM_SET(uint16_t, uhword)
CPU_MEM_SET(int32_t, word)
CPU_MEM_SET(uint32_t, uword)

#pragma clang diagnostic pop