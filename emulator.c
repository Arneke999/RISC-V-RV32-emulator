#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define MEM_BASE 0x80000000u
#define MEM_SIZE 0x10000u   // 64 KB

typedef struct { uint32_t x[32], pc; uint8_t *mem; } Cpu;

int main(int argc, char **argv) {
    Cpu cpu = {0};  //Initiate new cpu with all values = 0
    cpu.mem = calloc(MEM_SIZE, 1); //reserves MEM_SIZEx1 bytes in memory, all set to zero

    if (cpu.mem==NULL){  //allocation can fail
        perror("calloc");
        return 1;
    }

    if (argc < 2) {
        // without argc check, running ./emu with no argument makes argv[1] NULL and crashes fopen
        fprintf(stderr, "usage: %s program.bin\n", argv[0]);
        return 1;
    }
    FILE *file = fopen(argv[1], "rb");
    if (file==NULL) {
        perror("Error opening program");
        return 1;
    }
    size_t n = fread(cpu.mem, 1, MEM_SIZE, file); //read up to MEM_SIZEx1 bytes and stores into cpu.mem from file, n is the amount of bytes read
    fclose(file);
    if (n==0) {
        fprintf(stderr, "empty or unreadable file\n");
        return 1;
    }
    //need program counter for the linker
    cpu.pc = MEM_BASE;

    for (;;) {
        uint32_t inst;
        memcpy(&inst, &cpu.mem[cpu.pc - MEM_BASE], 4);   // fetch the 4 byte instruction into inst

        uint32_t opcode = inst & 0x7f;
        uint32_t rd     = (inst >> 7)  & 0x1f;
        uint32_t funct3 = (inst >> 12) & 0x7;
        uint32_t rs1    = (inst >> 15) & 0x1f;
        uint32_t rs2    = (inst >> 20) & 0x1f;
        uint32_t funct7  = (inst >> 25);

        int32_t imm = (int32_t)inst >> 20;

        uint32_t next_pc = cpu.pc + 4;   // jumps overwrite this

        switch (opcode) {
        // TODO: lui, auipc, jal, jalr, ecall
            case 0x13:  {
                uint32_t shamt = (inst >> 20) & 0x1f;
                switch (funct3) {
                    case 0x0: cpu.x[rd] = cpu.x[rs1] + imm; break;  //addi
                    case 0x1: cpu.x[rd] = cpu.x[rs1] << shamt; break; //slli
                    case 0x5:
                        if (funct7 == 0x00) cpu.x[rd] = cpu.x[rs1] >> shamt;    //srli
                        else if (funct7 == 0x20) cpu.x[rd] = (int32_t)cpu.x[rs1] >> shamt; //srai
                        else goto bad;
                        break;
                    default:
                    bad:
                        fprintf(stderr, "uknown OP-IMM instruction %08x (funct3 %u, funct7 %02x) at %08x\n", inst, funct3, funct7, cpu.pc);
                        return 1;
                }
            }
            case 0x37: cpu.x[rd] = imm << 12; break; //lui
            case 0x17: cpu.x[rd]  //auipc

            case 0x6f:  //jal

            case 0x67:  //jalr

            case 0x73:  //ecall


            break;
            default:
                fprintf(stderr, "uknown instruction %08x (opcode %02x) at %08x\n", inst, opcode, cpu.pc);
                return 1;
        }
        cpu.x[0] = 0;
        cpu.pc = next_pc;
    }
    free(cpu.mem);
}
