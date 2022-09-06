#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "vm_assets.h"

void fetch() {
    reg.mar = reg.pc;
    reg.mbr = reg.cir = mem[reg.mar];
    reg.pc++;
}

Instruction_t parse_instruction(uint16_t address, uint32_t raw_instruction) {
    union i32 i; i.v = (int32_t)mem[address];

    Instruction_t parsed_instruction = {
        .address = address,
        .opcode  = i.n.n2,      // instruction's last 16 bits
        .operand = i.n.n1       // instruction's first 16 bits
    };
    return parsed_instruction;
}

void execute(Instruction_t instruction) {
    switch (instruction.opcode) {
        case LDA:
            printf(" => lda %04x\n", instruction.operand);
            reg.acc = mem[instruction.operand];         // acc = mem[S]       
            break;
        case STO:
            printf(" => sto %04x\n", instruction.operand);
            mem[instruction.operand] = reg.acc;         // mem[S] = acc       
            break;
        case CLA:
            printf(" => cla\n");                        // acc = 0
            reg.acc = 0;
            break;
        case ADD:
            printf(" => add %04x\n", instruction.operand);
            reg.acc += mem[instruction.operand];        // acc += mem[S]
            break;
        case SUB:
            printf(" => sub %04x\n", instruction.operand);
            reg.acc -= mem[instruction.operand];        // acc -= mem[S]
            break;
        case MUL:
            printf(" => mul %04x\n", instruction.operand);
            reg.acc *= mem[instruction.operand];        // acc *= mem[S]
            break;
        case DIV:
            printf(" => div %04x\n", instruction.operand);
            reg.acc /= mem[instruction.operand];        // acc /= mem[S]
            break;
        case AND:
            printf(" => and %04x\n", instruction.operand);
            reg.acc &= mem[instruction.operand];        // acc &= mem[S]
            break;
        case OR:
            printf(" => or  %04x\n", instruction.operand);
            reg.acc |= mem[instruction.operand];        // acc |= mem[S]
            break;
        case XOR:
            printf(" => xor %04x\n", instruction.operand);
            reg.acc ^= mem[instruction.operand];        // acc ^= mem[S]
            break;
        case SHL:
            printf(" => shl %04x\n", instruction.operand);
            reg.acc <<= mem[instruction.operand];       // acc <<= mem[S]
            break;
        case SHR:
            printf(" => shr %04x\n", instruction.operand);
            reg.acc >>= mem[instruction.operand];       // acc >>= mem[S]
            break;
        case JMP:
            printf(" => jmp %04x\n", instruction.operand);
            reg.pc = instruction.operand;               // pc = mem[S]
            break;
        case JGE: {
            if (reg.acc >= 0) {
                printf(" => jge %04x\n", instruction.operand);
                reg.pc = instruction.operand;           // if acc >= 0; pc = mem[S]
                break;
            }
            break;
        }
        case JNE: {
            if (reg.acc != 0) {
                printf(" => jne %04x\n", instruction.operand);
                reg.pc = instruction.operand;           // if acc != 0; pc = mem[S]
                break;
            }
            break;
        }
        case HALT:
            printf(" => halt\n");
            running = false;
            break;

        case 0:
            printf(" => "ERRORMSG"blank memory space\n");
            exit(EXIT_FAILURE);
        default:
            printf(" => "ERRORMSG"unknown instruction\n");
            exit(EXIT_FAILURE);
    }
}

int main(int argc, char const *argv[]) {
    mem[0x0000] = 0x2000;       // mem 0x0 = 0x2000
    mem[0x0001] = 0x2;          // mem 0x1 = 0x2

    mem[0x1111] = 0x00010000;   // lda 0000    
    mem[0x1112] = 0x00020003;   // sto 0003
    mem[0x1113] = 0x00030000;   // cla     
    mem[0x1114] = 0x00040000;   // add 0000
    mem[0x1115] = 0x00050000;   // sub 0000
    mem[0x1116] = 0x00060000;   // mul 0000
    mem[0x1117] = 0x00070000;   // div 0000
    mem[0x1118] = 0x00080000;   // and 0000
    mem[0x1119] = 0x00090000;   // or  0000
    mem[0x111a] = 0x000a0000;   // xor 0000
    mem[0x111b] = 0x000b0000;   // shl 0000
    mem[0x111c] = 0x000c0000;   // shr 0000
    mem[0x111d] = 0x000d111e;   // jmp 111e
    mem[0x111e] = 0x000e111f;   // jge 111f
    mem[0x111f] = 0x00010001;   // lda 0000
    mem[0x1120] = 0x000f1121;   // jne 1121 
    mem[0x1121] = 0x00100000;   // halt     
    
    reg.pc = 0x1111;
    
    while (running) {
        // make sure pc is in instruction memory space
        if (reg.pc < 0x1111 || reg.pc > 0xffff) {
            printf("\n"ERRORMSG"program counter = %x\n", reg.pc);
            exit(EXIT_FAILURE);
        }
        // fetch
        fetch();
        
        // decode
        Instruction_t instruction = parse_instruction(reg.mar, reg.cir);
        
        printf("pc = %04x acc = %04x ADDR[%04x] CONTENT: %08x | opcode = %04x operand = %04x", 
               reg.pc, reg.acc, reg.mar, mem[reg.mar], instruction.opcode, instruction.operand);
        
        // execute
        execute(instruction);
    }

    return 0;
}
