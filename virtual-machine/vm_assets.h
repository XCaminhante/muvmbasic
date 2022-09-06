#ifndef __ASSETS_H
#define __ASSETS_H

#define SET_BIT(BF, N) BF |= ((uint32_t)0x00000001 << N)
#define CLR_BIT(BF, N) BF &= ~((uint32_t)0x00000001 << N)
#define IS_BIT_SET(BF, N) ((BF >> N) & 0x1)

#define ERRORMSG "[\e[1;31mERROR\e[0m]: "

// address in memory | opcode (last 16 bits) | operand (fist 16 bits)
typedef struct Instruction_t {
    uint32_t address;
    uint16_t opcode;
    uint16_t operand;
} Instruction_t;

// split an uint32_t into 2x uint16_t 
union i32 {
    uint32_t v;
    struct {
        int n1:16;
        int n2:16;
    } n;
};

// instructions
enum {
    LDA  = 1, 
    STO  = 2, 
    CLA  = 3,
    ADD  = 4, 
    SUB  = 5, 
    MUL  = 6, 
    DIV  = 7,
    AND  = 8, 
    OR   = 9, 
    XOR  = 10,  
    SHL  = 11, 
    SHR  = 12,
    JMP  = 13, 
    JGE  = 14, 
    JNE  = 15, 
    HALT = 16
};

// registers
struct reg {
    int32_t pc, cir, mar, mbr, acc;
} reg;

// memory
int32_t mem[INT16_MAX];

bool running = true;

/* this function fetches the instruction from memory */
void fetch();

/* this function parses a raw instruction into address/opcode/operand
 *  @param address: address in memory
 *  @param raw_instruction: contents of that address in memory */
Instruction_t parse_instruction(uint16_t address, uint32_t raw_instruction);

/* this function decodes the opcode and executes some action based on it
 * @param instruction: struct containing information about what needs to be done */
void execute(Instruction_t instruction);

#endif //__ASSETS_H
