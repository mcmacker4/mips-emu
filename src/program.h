#ifndef EMU_PROGRAM_H
#define EMU_PROGRAM_H

uint8_t program_text[] = {
        /* 00400000: */ 0x24, 0x08, 0x00, 0x05, // |input:11| addiu $t0, $zero, 5
        /* 00400004: */ 0x24, 0x09, 0x00, 0x00, // |input:13| addiu $t1, $zero, 0
        /* 00400008: */ 0x11, 0x09, 0x00, 0x0a, // |input:20| beq $t0, $t1, end_loop
        /* 0040000c: */ 0x00, 0x08, 0x20, 0x20, // |input:23| add $a0, $zero, $t0
        /* 00400010: */ 0x24, 0x02, 0x00, 0x01, // |input:24| addiu $v0, $zero, 1
        /* 00400014: */ 0x00, 0x00, 0x00, 0x0c, // |input:25| syscall
        /* 00400018: */ 0x24, 0x02, 0x00, 0x04, // |input:28| addiu $v0, $zero, 4
        /* 0040001c: */ 0x24, 0x04, 0x00, 0xc0, // |input:29| addiu $a0, $zero, new_line[hi]
        /* 00400020: */ 0x00, 0x04, 0x24, 0x00, // |input:30| sll $a0, $a0, 16
        /* 00400024: */ 0x24, 0x84, 0x00, 0x0c, // |input:31| addiu $a0, $a0, new_line[lo]
        /* 00400028: */ 0x00, 0x00, 0x00, 0x0c, // |input:32| syscall
        /* 0040002c: */ 0x21, 0x08, 0xff, 0xff, // |input:34| addi $t0, $t0, -1
        /* 00400030: */ 0x08, 0x10, 0x00, 0x01, // |input:36| j start_loop
        /* 00400030: */ 0x24, 0x02, 0x00, 0x04, // |input:42| addiu $v0, $zero, 4
        /* 00400034: */ 0x24, 0x04, 0x00, 0xc0, // |input:43| addiu $a0, $zero, go_str[hi]
        /* 00400038: */ 0x00, 0x04, 0x24, 0x00, // |input:44| sll $a0, $a0, 16
        /* 0040003c: */ 0x24, 0x84, 0x00, 0x00, // |input:45| addiu $a0, $a0, go_str[lo]
        /* 00400040: */ 0x00, 0x00, 0x00, 0x0c, // |input:46| syscall
        /* 00400044: */ 0x24, 0x02, 0x00, 0x00, // |input:48| addiu $v0, $zero, 0
        /* 00400048: */ 0x03, 0xe0, 0x00, 0x08, // |input:49| jr $ra
};

uint8_t program_data[] = {
        /* 00c00000: */ 0x47, 0x4f, 0x21, 0x21, // GO!!
        /* 00c00004: */ 0x21, 0x21, 0x21, 0x0a, // !!!\n
        /* 00c00008: */ 0x00, 0x00, 0x00, 0x00, //
        /* 00c0000c: */ 0x0a, 0x00, 0x00, 0x00 // \n
};

#endif //EMU_PROGRAM_H
