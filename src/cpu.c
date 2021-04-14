#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunknown-pragmas"
#pragma ide diagnostic ignored "cppcoreguidelines-narrowing-conversions"

#include "cpu.h"
#include "register.h"
#include "syscall.h"

#include <memory.h>


void cpu_exec_r(cpu_t *cpu, uint8_t op, uint8_t rs, uint8_t rt, uint8_t rd, uint8_t sh, uint8_t fun) {

#define OPER(a, b, c, o) \
    cpu->registers[a] = R(b) o R(c)
#define OPER_U(a, b, c, o) \
    cpu->registers[a] = RU(b) o RU(c)

    if (op == 0x00) {
        switch (fun) {
            case 0x00: // sll
                S(rd, R(rt) << sh);
                break;
            case 0x1A: // div
                OPER(HI, rs, rt, %);
                OPER(LO, rs, rt, /);
                break;
            case 0x1B: // divu
                OPER_U(HI, rs, rt, %);
                OPER_U(LO, rs, rt, /);
                break;
            case 0x02: // srl
                S(rd, RU(rt) >> sh);
                break;
            case 0x2A: // slt
                OPER(rd, rs, rt, <);
                break;
            case 0x2B: // sltu
                OPER_U(rd, rs, rt, <);
                break;
            case 0x03: // sra
                S(rd, R(rt) >> sh);
                break;
            case 0x08: // jr
                S(PC, R(rs));
                break;
            case 0x10: // mfhi
                S(rd, R(HI));
                break;
            case 0x11: // mthi
                S(HI, R(rs));
                break;
            case 0x12: // mflo
                S(rd, R(LO));
                break;
            case 0x13: // mtlo
                S(LO, R(rs));
                break;
            case 0x18: { // mult
                int64_t res = R(rs) * R(rt);
                S(HI, (res >> 32) & 0xFF);
                S(LO, res & 0xFF);
                break;
            }
            case 0x19: { // multu
                uint64_t res = RU(rs) * RU(rt);
                S(HI, (res >> 32) & 0xFF);
                S(LO, res & 0xFF);
                break;
            }
            case 0x20: // add
                OPER(rd, rs, rt, +);
                break;
            case 0x21: // addu
                OPER_U(rd, rs, rt, +);
                break;
            case 0x22: // sub
                OPER(rd, rs, rt, -);
                break;
            case 0x23: // subu
                OPER_U(rd, rs, rt, -);
                break;
            case 0x24: // and
                OPER_U(rd, rs, rt, &);
                break;
            case 0x25: // or
                OPER_U(rd, rs, rt, |);
                break;
            case 0x26: // xor
                OPER_U(rd, rs, rt, ^);
                break;
            case 0x27: // nor
                S(rd, ~(RU(rs) | RU(rt)));
                break;
            case 0x0C:
                syscall(cpu);
                break;
            default: FATAL("Invalid FN type for 0x00 opcode: 0x%02x", fun)
        }
    } else {
        FATAL("Coprocessors not implemented")
    }

#undef OPER
#undef OPER_U

}

cpu_t* cpu_new() {
    cpu_t *cpu = malloc(sizeof(cpu_t));
    memset(cpu, 0, sizeof(cpu_t));
    return cpu;
}

void cpu_next(cpu_t* cpu) {
    uint32_t instr = MEM_GET(uint32_t, R(PC));
    cpu_exec(cpu, instr);
    S(PC, R(PC) + 4); // Increment PC
}

void cpu_exec(cpu_t *cpu, uint32_t instr) {
    uint8_t op = (instr >> 26) & 0x3F;
    uint8_t rs = (instr >> 21) & 0x1F;
    uint8_t rt = (instr >> 16) & 0x1F;
    int16_t imm = instr & 0xFFFF;

#define IOPER(a, b, i, o) \
    S(a, R(b) o i)
#define IOPER_U(a, b, i, o) \
    S(a, R(b) o (uint16_t) (i))
#define BRANCH(cond, off)                \
    if (cond) {                             \
        S(PC, R(PC) + ((uint32_t) (off) << 2));   \
    }

    switch (op) {
        case 0x00:
        case 0x10: {
            uint8_t rd = (instr >> 11) & 0x1F;
            uint8_t sh = (instr >> 6) & 0x1F;
            uint8_t fn = (instr >> 0) & 0x3F;
            cpu_exec_r(cpu, op, rs, rt, rd, sh, fn);
            break;
        }
        case 0x02: {
            uint32_t hi4 = R(PC) & 0xFC000000;
            S(PC, hi4 | ((instr & 0x03FFFFF) << 2));
            break;
        }
        case 0x03: {
            S(RA, R(PC));
            S(PC, ((instr & 0x03FFFFF) << 2));
            break;
        }
        case 0x08: { // addi
            IOPER(rt, rs, imm, +);
            break;
        }
        case 0x09: { // addiu
            IOPER_U(rt, rs, imm, +);
            break;
        }
        case 0x0C: { // andi
            IOPER_U(rt, rs, imm, &);
            break;
        }
        case 0x0D: { // ori
            IOPER(rt, rs, imm, |);
        }
        case 0x01: { // bgez - bgezal - bltz - bltzal
            uint8_t link = rt & 0b10000;
            uint8_t neg = rt & 0b00001;
            if (link)
                S(RA, R(PC));
            if (neg) {
                BRANCH(R(rs) < 0, imm)
            } else {
                BRANCH(R(rs) >= 0, imm)
            }
            break;
        }
        case 0x04: { // beq
            BRANCH(R(rs) == R(rt), imm)
        }
        case 0x06: { // blez
            BRANCH(R(rs) <= 0, imm)
        }
        case 0x05: { // bne
            BRANCH(R(rs) != R(rt), imm)
        }
        case 0x07: { // bgtz
            BRANCH(R(rs) > 0, imm)
        }
        case 0x20: { // lb
            S(rt, MEM_GET(int8_t, imm + R(rs)));
        }
        case 0x24: { // lbu
            S(rt, MEM_GET(uint8_t, imm + R(rs)));
        }
        case 0x21: { // lh
            S(rt, MEM_GET(int16_t, imm + R(rs)));
        }
        case 0x25: { // lhu
            S(rt, MEM_GET(uint16_t, imm + R(rs)));
        }
        case 0x23: { // lw
            S(rt, MEM_GET(int32_t, imm + R(rs)));
        }
        case 0x28: { // sb
            MEM_SET(int8_t, imm + R(rs), R(rt));
        }
        case 0x29: { // sh
            MEM_SET(int16_t, imm + R(rs), R(rt));
        }
        case 0x2B: { // sw
            MEM_SET(int32_t, imm + R(rs), R(rt));
        }
        case 0x0A: { // slti
            IOPER(rt, rs, imm, <);
        }
        case 0x0B: { // sltiu
            IOPER_U(rt, rs, imm, <);
        }
        default:
            printf("Invalid opcode: 0x%02x\n", op);
    }

#undef IOPER
#undef IOPER_U
#undef BRANCH

}

#pragma clang diagnostic pop