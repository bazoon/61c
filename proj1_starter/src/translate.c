#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "tables.h"
#include "translate_utils.h"
#include "translate.h"


/* Writes instructions during the assembler's first pass to OUTPUT. The case
   for general instructions has already been completed, but you need to write
   code to translate the li and blt pseudoinstructions. Your pseudoinstruction 
   expansions should not have any side effects.

   NAME is the name of the instruction, ARGS is an array of the arguments, and
   NUM_ARGS specifies the number of items in ARGS.

   Error checking for regular instructions are done in pass two. However, for
   pseudoinstructions, you must make sure that ARGS contains the correct number
   of arguments. You do NOT need to check whether the registers / label are 
   valid, since that will be checked in part two.

   Also for li:
    - make sure that the number is representable by 32 bits. (Hint: the number 
        can be both signed or unsigned).
    - if the immediate can fit in the imm field of an addiu instruction, then
        expand li into a single addiu instruction. Otherwise, expand it into 
        a lui-ori pair.

   And for blt:
    - your expansion should use the fewest number of instructions possible.

   MARS has slightly different translation rules for li, and it allows numbers
   larger than the largest 32 bit number to be loaded with li. You should follow
   the above rules if MARS behaves differently.

   Use fprintf() to write. If writing multiple instructions, make sure that 
   each instruction is on a different line.

   Returns the number of instructions written (so 0 if there were any errors).
 */
unsigned write_pass_one(FILE* output, const char* name, char** args, int num_args) {
    if (strcmp(name, "li") == 0) {
        /* YOUR CODE HERE */
        return 0;
    } else if (strcmp(name, "blt") == 0) {
        /* YOUR CODE HERE */
        return 0;
    } else {
        write_inst_string(output, name, args, num_args);
        return 1;
    }
}

/* Writes the instruction in hexadecimal format to OUTPUT during pass #2.
   
   NAME is the name of the instruction, ARGS is an array of the arguments, and
   NUM_ARGS specifies the number of items in ARGS. 

   The symbol table (SYMTBL) is given for any symbols that need to be resolved
   at this step. If a symbol should be relocated, it should be added to the
   relocation table (RELTBL), and the fields for that symbol should be set to
   all zeros. 

   You must perform error checking on all instructions and make sure that their
   arguments are valid. If an instruction is invalid, you should not write 
   anything to OUTPUT but simply return -1. MARS may be a useful resource for
   this step.

   Note the use of helper functions. Consider writing your own! If the function
   definition comes afterwards, you must declare it first (see translate.h).

   Returns 0 on success and -1 on error. 
 */
int translate_inst(FILE* output, const char* name, char** args, size_t num_args, uint32_t addr,
    SymbolTable* symtbl, SymbolTable* reltbl) {
    // int64_t addr;
    // printf("writing: %s\n", name);
    if (strcmp(name, "addu") == 0)       return write_rtype (0x21, output, args, num_args);
    else if (strcmp(name, "or") == 0)    return write_rtype (0x25, output, args, num_args);
    else if (strcmp(name, "slt") == 0)   return write_rtype (0x2a, output, args, num_args);
    else if (strcmp(name, "sltu") == 0)  return write_rtype (0x2b, output, args, num_args);
    else if (strcmp(name, "sll") == 0)   return write_shift (0x00, output, args, num_args);
    else if (strcmp(name, "addiu") == 0)  return write_itype (9, output, args, num_args, symtbl);
    else if (strcmp(name, "ori") == 0)   return write_itype (13, output, args, num_args, symtbl);
    else if (strcmp(name, "lui") == 0)   return write_itype (15, output, args, num_args, symtbl);
    else if (strcmp(name, "lb") == 0)   return write_itype (32, output, args, num_args, symtbl);
    else if (strcmp(name, "lbu") == 0)   return write_itype (36, output, args, num_args, symtbl);
    else if (strcmp(name, "lw") == 0)   return write_itype (35, output, args, num_args, symtbl);
    else if (strcmp(name, "sb") == 0)   return write_itype (40, output, args, num_args, symtbl);
    else if (strcmp(name, "sw") == 0)   return write_itype (43, output, args, num_args, symtbl);
    else if (strcmp(name, "beq") == 0)   return write_itype (4, output, args, num_args, symtbl);
    else if (strcmp(name, "bne") == 0)   return write_itype (5, output, args, num_args, symtbl);   
    else if (strcmp(name, "j") == 0) {
        add_to_table(reltbl, args[0], get_addr_for_symbol(symtbl, args[0]));
        return write_jtype (2, output, args, num_args, symtbl);
    } else if (strcmp(name, "jal") == 0) {
        add_to_table(reltbl, args[0], get_addr_for_symbol(symtbl, args[0]));
        return write_jal (3, output, args, symtbl);
    }
    else if (strcmp(name, "jr") == 0)   return write_rtype (8, output, args, num_args);
    else                                 return -1;
}

void setBits(int *n, int from, int to, int value) {
    int bitCount = from - to + 1;
    int i;
    int mask = value << from;
}

/* A helper function for writing most R-type instructions. You should use
   translate_reg() to parse registers and write_inst_hex() to write to 
   OUTPUT. Both are defined in translate_utils.h.

   This function is INCOMPLETE. Complete the implementation below. You will
   find bitwise operations to be the cleanest way to complete this function.
 */

void binprintf(int v)
{
    unsigned int mask=1<<((sizeof(int)<<3)-1);
    while(mask) {
        printf("%d", (v&mask ? 1 : 0));
        mask >>= 1;
    }
}

int write_rtype(uint8_t funct, FILE* output, char** args, size_t num_args) {
    // Perhaps perform some error checking?
    int opcode = 0;
    int shamt = 0;

    int rd = 0; 
    int rs = 0; 
    int rt = 0; 

    if (num_args == 3) {
        rd = translate_reg(args[0]);
        rs = translate_reg(args[1]);
        rt = translate_reg(args[2]);
    } else if (num_args == 1) {
        rs = translate_reg(args[0]);
    }

    if (rt < 0 || rs < 0 || rd < 0 || rt > 31 || rs > 31 || rd > 31) {
        printf("Error on %d out of bounds, rt=%d rs=%d, rd=%d\n", opcode, rt, rs, rd);
        printf("args: args[0]: %s args[1]: %s args[2]: %s \n", args[0], args[1], args[2]);
        return -1;
    }

    // printf("%d, %d, %d, %s\n", rd, rs, rt, args[2]);
    
    uint32_t instruction = (opcode << 26) | (rs << 21) | (rt << 16) | (rd << 11) | (shamt << 6) | funct;
    
    write_inst_hex(output, instruction);

    printf("\n");
    return 0;
}

int write_itype(int opcode, FILE* output, char** args, size_t num_args, SymbolTable* symtbl) {
    int imm;
    int rs;
    int rt;
    int err;

    // printf("write_itype %s %s %s \n", args[0], args[1], args[2]);

    if (num_args == 2) {
        rs = 0;
        rt = translate_reg(args[0]);
        err = translate_num(&imm, args[1], -32768, 32767);
    } else {
        rt = translate_reg(args[0]);
        if (isdigit(args[1][0]) != 0 || args[1][0] == '-') {
            rs = translate_reg(args[2]);
            err = translate_num(&imm, args[1], -32768, 32767);
        } else {
            rs = translate_reg(args[1]);
            err = translate_num(&imm, args[2], -32768, 32767);
            if (err == -1) {
                err = get_addr_for_symbol(symtbl, args[2]);
                imm = err;
                if (err == -1) {
                    printf("label not found! /n");
                    return -1;
                } 
            }
        }
    }

    if (err == -1 || rt > 31 || rs > 31 || rs < 0 || rt < 0) {
        printf("Error on %d out of bounds,  err=%d, rt=%d rs=%d\n", opcode, err, rt, rs);
        printf("args: %s %s %s \n", args[0], args[1], args[2]);
        return -1;
    }

    // printf("rt=%s, rs=%s imm=%d opcode=%d, rt=%d  rs=%d \n", args[0], args[1], imm, opcode, rt, rs);
    // printf("args: %s %s %s \n", args[0], args[1], args[2]);
    
    uint32_t instruction = (opcode << 26) | (rs << 21) | (rt << 16) | (imm & 0xffff);
  
    write_inst_hex(output, instruction);

    printf("\n");
    return 0;
}

/* A helper function for writing shift instructions. You should use 
   translate_num() to parse numerical arguments. translate_num() is defined
   in translate_utils.h.

   This function is INCOMPLETE. Complete the implementation below. You will
   find bitwise operations to be the cleanest way to complete this function.
 */
int write_shift(uint8_t funct, FILE* output, char** args, size_t num_args) {
	// Perhaps perform some error checking?

    long int shamt;
    int rd = translate_reg(args[0]);
    int rt = translate_reg(args[1]);
    int err = translate_num(&shamt, args[2], 0, 31);

    if (rt > 31 || shamt > 31 || rt > 31 || err == -1) {
        return -1;
    }

    printf("rd=%d rt=%d shamt=%d\n", rd, rt, shamt);
    printf("args: 0: %s 1: %s 2: %s \n", args[0], args[1], args[2]);


    uint32_t instruction = (rt << 16) | (rd << 11) | (shamt << 6) | funct;
    write_inst_hex(output, instruction);
    return 0;
}

int write_jtype(int opcode, FILE* output, char** args, size_t num_args, SymbolTable* symtbl) {
    int imm;
    int err = translate_num(&imm, args[0], 0, 65535);

    if (err == -1) {
        err = get_addr_for_symbol(symtbl, args[0]);
        imm = err;
    }
    
    // printf("writing jtype %s \n ", args[0]);
    
    if (err < 0) {
        return -1;
    }

    // printf("opcode=%d, imm=%d err=%d\n", opcode << 26, imm, err);

    uint32_t instruction = (opcode << 26) | imm;

    write_inst_hex(output, instruction);
    return 0;
}

int write_jal(int opcode, FILE* output, char **args, SymbolTable *symtbl) {
    int imm = get_addr_for_symbol(symtbl, args[0]);
    // printf("opcode=%d, imm=%d\n", opcode << 26, imm);

    uint32_t instruction = (opcode << 26) | imm;

    write_inst_hex(output, instruction);
    return 0;
}

