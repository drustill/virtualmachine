/* vm.h */
#define _GNU_SOURCE

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>
#include <errno.h>
#include <utils.h>

#define NoErr  0x00
#define SysHlt 0x01  /* System halt */
#define ErrMem 0x02
#define ErrSegv 0x04 /* Segmentation fault */

#define segfault(x) error((x), ErrSegv)

typedef unsigned char int8;
typedef unsigned short int int16;
typedef unsigned int int32;
typedef unsigned long long int int64;

/* Macros for typecasting */
#define $1 (int8 *)
#define $2 (int16)
#define $4 (int32)
#define $8 (int64)
#define $c (char *)
#define $i (int)

/*

    16 bit
        AX
        BX
        CX
        DX
        SP
        IP
    65 KB memory
    (Serial COM port)
    (Floppy drive)

*/

/* Macros to access registers */
#define $ax ->c.r.ax
#define $bx ->c.r.bx
#define $cx ->c.r.cx
#define $dx ->c.r.dx
#define $sp ->c.r.sp
#define $ip ->c.r.ip

/**
 * @typedef Reg
 * @brief Represents a 16-bit general-purpose register.
 */
typedef unsigned short int Reg;

/**
 * @typedef Registers
 * @brief Shorthand for `struct s_registers`.
 *
 * @struct s_registers
 * @brief Defines the CPU's general-purpose registers.
 *
 * Mmmmmmmmmmmmmmmmmodel an x86-like architecture with six key registers:
 * - AX: Accumulator Register.
 * - BX: Base Register.
 * - CX: Count Register.
 * - DX: Data Register.
 * - SP: Stack Pointer.
 * - IP: Instruction Pointer.
 */
typedef struct s_registers {
    Reg ax;
    Reg bx;
    Reg cx;
    Reg dx;
    Reg sp;
    Reg ip;
} Registers;

/**
 * @struct s_cpu
 * @brief Represents the CPU, containing its register set.
 */
typedef struct s_cpu {
    Registers r; /**< General-purpose registers */
} CPU;

/**
 * @enum e_opcode
 * @typedef Opcode
 * @brief Enumeration of supported opcodes (instructions).
 *
 * These opcodes represent the minimal instruction set architecture (ISA) for the VM:
 * - `mov`: Moves data between memory locations or registers.
 * - `nop`: No-operation; advances to the next instruction.
 * - `hlt`: Halts execution.
 */
typedef enum e_opcode {
	mov = 0x01,
	nop = 0x02,
	hlt = 0x03
} Opcode;

/**
 * @typedef Args
 * @brief Represents the arguments passed to an instruction.
 *
 * Each argument is a 16-bit value for flexibility in addressing or data manipulation.
 */
typedef int16 Args;

/**
 * @struct s_instrmap
 * @brief Maps opcodes to stack requirements for execution.
 *
 * Each instruction in the VM's ISA is mapped to its required stack size.
 */
typedef struct s_instrmap {
    Opcode o; /**< Opcode for the instruction */
    int8 s; /**< Required stack size for the instruction */
} InstructionMap;

/**
 * @struct s_instruction
 * @brief Represents a single instruction for the VM.
 *
 * One instruction contains:
 * - asm opcode
 * - argumemts
 */
typedef struct s_instruction {
	Opcode o;
	Args a[]; /* 0-2 byte */
} Instruction;


/**
 * @typedef Errorcode
 * @brief Represents error codes used in the VM.
 */
typedef unsigned char Errorcode;

/**
 * @typedef Memory
 * @brief Represents the virtual machine's memory space.
 *
 * Memory is modeled as an array of bytes, allowing access to both instructions
 * and data. The size is determined by the maximum addressable range for a signed
 * 16-bit value (`int16`).
 */
typedef int8 Memory[((int16)(-1))];

/**
 * @typedef Program
 * @brief Represents a program to be loaded and executed by the VM.
 *
 * A program consists of a sequence of instructions stored in memory.
 */
typedef int8 Program;

/**
 * @typedef VM
 * @brief shorthand for struct s_vm
 *
 * @struct s_vm
 * @brief Represents the virtual machine state.
 *
 * The VM includes:
 * - A CPU for executing instructions.
 * - A memory space for storing code and data.
 * - A break line (`b`) that separates code and data sections in memory.
 */
typedef struct s_vm {
    CPU c;
    Memory m;
    int16 b;  /* Break line between code and data sections */
} VM;

/**
 * @typedef Stack
 * @brief Represents a stack pointer into the VM's memory.
 */
typedef Memory *Stack;

/**
 * @var instrmap
 * @brief Defines the mapping between opcodes and stack requirements.
 *
 * Each instruction is mapped to its stack size, enabling the VM to handle
 * stack-based operations efficiently.
 */
static InstructionMap instrmap[] = {
    { mov, 0x03 }, /**< Move instruction requires 3 bytes */
    { nop, 0x01 }, /**< No-operation requires 1 byte */
    { hlt, 0x01 }  /**< Halt instruction requires 1 byte */
};

/**
 * @def InstructionMaps
 * @brief Defines the number of instruction mappings.
 */
#define InstructionMaps (sizeof(instrmap) / sizeof(struct s_instrmap))

/* Function Declarations */

void __mov(VM*,Opcode,Args,Args);

void error(VM*,Errorcode);
void execinstr(VM*,Program*);
void execute(VM*);
 Program *exampleprogram(VM*);
int8 map(Opcode);
VM *virtualmachine(void);
int main(int,char**);

/* Breakline visualization */

/*
Section .text (code) +read +exec -write
 ########
 #######
 ------------------------------

 ****
 *********
 **********
Section .data +read -exec +write
*/
