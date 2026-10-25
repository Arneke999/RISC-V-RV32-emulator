#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define MEM_BASE 0x80000000u
#define MEM_SIZE 0x10000u   // 64 KB

typedef struct { uint32_t x[32], pc; uint8_t *mem; } Cpu;

int main(int argc, char **argv) {
    Cpu cpu = {0};
    cpu.mem = calloc(MEM_SIZE, 1);

    if (argc < 2) {
        // without argc check, running ./emu with no argument makes argv[1] NULL and crashes fopen
        fprintf(stderr, "usage: %s program.bin\n", argv[0]);
        return 1;
    }
    FILE *file = fopen(argv[1], "rb");
    if(file==NULL) {
        perror("Error opening program");
        return 1;
    }
    size_t n = fread(cpu.mem, 1, MEM_SIZE, file);
    fclose(file);
    if(n==0) {
        fprintf(stderr, "empty or unreadable file\n");
        return 1;
    }
    cpu.pc = MEM_BASE;

    for (;;) {
        uint32_t inst;
        memcpy(&inst, &cpu.mem[cpu.pc - MEM_BASE], 4);   // fetch the instruction

        uint32_t opcode = inst & 0x7f;
        uint32_t rd     = (inst >> 7)  & 0x1f;
        uint32_t funct3 = (inst >> 12) & 0x7;
        uint32_t rs1    = (inst >> 15) & 0x1f;
        uint32_t rs2    = (inst >> 20) & 0x1f;
        uint32_t funct7  = (inst >> 25);

        uint32_t next_pc = cpu.pc + 4;   // jumps overwrite this

        switch (opcode) {
        // TODO: one case per instruction
        default:
            fprintf(stderr, "unknown instruction %08x at %08x\n", inst, cpu.pc);
            return 1;
        }

        cpu.x[0] = 0;
        cpu.pc = next_pc;
    }
}