/* main.c */
#include <stdio.h>
#include "main.h"

/**
 * @brief Maps an opcode to its corresponding size
 * 
 * @param o Opcode to map
 * @return int8 Size of the opcode
 */
int8 map(Opcode o) {
    int8 n, ret;
    InstructionMap *p;

    ret = 0;
    for (n=InstructionMaps, p=instrmap; n; n--, p++) {
        if (p->o == o) {
            ret = p->s;
            break;
        }
    }
    
    return ret;
}

void __mov(VM *vm, Opcode opcode, Args a1, Args a2) {
    /* MOV instruction moves data between registers or memory
       a1: destination (register)
       a2: source (immediate value or register)
       
       For now this only implements MOV AX, imm16
       where imm16 is a 16-bit immediate value loaded into AX */
       
    /* Cast the immediate value to a 16-bit register value
       and store it in the AX register */
    vm $ax = (Reg)a1;

    return;
}

/**
 * @brief Handles VM errors and exits with appropriate status
 * 
 * @param vm Pointer to VM instance to clean up
 * @param e Error code indicating type of error
 * 
 * Prints error message to stderr, frees VM memory if needed,
 * and exits with status code (-1 for errors, 0 for normal halt)
 */
void error(VM *vm, Errorcode e) {
    int8 exitcode;

    exitcode = -1;
    switch(e) {
        case ErrSegv:
            fprintf(stderr, "%s\n", "VM Segmentation fault");
            break;

        case SysHlt:
            fprintf(stderr, "%s\n", "System halted");
            exitcode = 0;
            printf("ax = %.04hx\n", $i vm $ax);

            break;

        default:
            break;
    }
    if (vm)
        free(vm);

    exit($i exitcode);
}

/**
 * @brief Executes a single instruction from the program
 *
 * Memory layout for instructions:
 * +--------+--------+--------+--------+
 * | opcode | arg1   | arg2   | ...    |
 * +--------+--------+--------+--------+
 * 1 byte    1-2 bytes depending on instruction
 *
 * Instruction formats:
 * Size 1: [opcode]              e.g. nop, hlt
 * Size 2: [opcode][arg1]        
 * Size 3: [opcode][arg1][arg2]  e.g. mov ax,0x05
 *
 * @param vm Pointer to VM instance
 * @param p Pointer to current instruction
 */
void execinstr(VM* vm, Program *p) {
    Args a1, a2;
    int16 size;

    a1=a2 = 0;
    size = map(*p);

    switch (size) {
        case 1:
            break;

        case 2:
            a1 = *(p+1);
            break;

        case 3:
            a1 = *(p+1);
            a2 = *(p+3);
            break;

        default:
            segfault(vm);
            break;
    }

    switch (*p) {
        case mov: 
            __mov(vm, (Opcode)*p, a1, a2);
            break;

        case nop:
            break;
        
        case hlt:
            error(vm, SysHlt);
            break;
    }

    return;
}

/**
 * @brief Main execution loop for the VM
 *
 * Memory layout:
 * +------------------------+------------------+
 * |     Code Section      |   Data Section   |
 * +------------------------+------------------+
 * ^                       ^                  ^
 * |                       |                  |
 * vm->m               breakline          end of mem
 *                     (vm->b)
 *
 * Example program in memory:
 * mov ax,0x05; nop; hlt;
 * +----+----+----+----+----+
 * |0x01|0x00|0x05|0x02|0x03|
 * +----+----+----+----+----+
 *  mov  arg1 arg2  nop  hlt
 *
 * @param vm Pointer to VM instance
 */
void execute(VM *vm) {
    int32 brkaddr;
    Program *pp;
    int16 size;
    Instruction ip;

    assert(vm && *vm->m);
    size = 0;
    brkaddr = ((int32)vm->m + vm->b);
    pp = (Program *)&vm->m;

    do {
        vm $ip += size;
        pp += size;

        if ((int32)pp > brkaddr)
            segfault(vm);
        size = map(*pp);
        execinstr(vm, pp);
    } while (*pp != (Opcode)hlt);

    return;
}


/**
 * @brief Creates a new VM instance
 * 
 * @return Pointer to VM instance, or NULL if allocation fails
 */
VM *virtualmachine() {
    VM *p;
    int16 size;

    size = $2 sizeof(struct s_vm);
    p = (VM *)malloc($i size);
    if (!p) {
        errno = ErrMem;
        return (VM *)0;
    }
    zero($1 p, size);

    return p;
}


/**
 * @brief Creates an example program to demonstrate VM instruction execution
 * 
 * This function creates a simple test program that:
 * 1. Moves the value 0x05 into register AX (mov instruction)
 * 2. Executes a no-op instruction (nop)
 * 3. Halts execution (hlt)
 *
 * The program is assembled directly into the VM's memory space by:
 * - Allocating space for each instruction
 * - Setting the opcode and arguments
 * - Copying the encoded instructions sequentially into memory
 * - Setting up initial register values
 *
 * @param vm Pointer to the virtual machine instance
 * @return Program* Pointer to the assembled program in VM memory
 */
Program *exampleprogram(VM *vm) {
    Program *p;
    Instruction *i1, *i2, *i3;
    Args a1;
    int16 s1, s2, sa1;

    a1 = 0;
    s1 = map(mov);
    s2 = map(nop);

    i1 = (Instruction *)malloc($i s1);
    i2 = (Instruction *)malloc( s2);
    i3 = (Instruction *)malloc( s2);
    assert(i1 && i2);
    zero($1 i1, s1);
    zero($1 i2, s2);
    zero($1 i3, s2);

    i1->o = mov;
    sa1 = (s1-1);
    a1 = 0x0005;
    // 0000 0001 mov eax
    // 0000 0000
    // 0000 0005  mov eax,0x05

    p = vm->m;
    copy($1 p, $1 i1, 1);
    p++;

    if (a1) {
        copy($1 p, $1 &a1, sa1);
        p += sa1;
    }

    i2->o = nop;
    copy($1 p, $1 i2, 1);

    p++;
    i3->o = hlt;
    copy($1 p, $1 i3, 1);

    int16 total = (s1+sa1+s2+s2);
    vm $ip = (Reg)vm->m;
    vm $sp = (Reg)-1;

    /* Why does this work in example */
    // free(i1);
    free(i2);

    vm->b = total;

    for (int i = 0; i < vm->b; i++) {
        printf("%02x ", vm->m[i]);
    }
    printf("\n");

    return (Program *)&vm->m;
}

int main(int argc, char *argv[]) {
	Program *prog;
	VM *vm;

	vm = virtualmachine();
	printf("vm   = %p (sz: %d)\n", vm, sizeof(struct s_vm));

    prog = exampleprogram(vm);

    printf("Memory layout: ");
    for(int i = 0; i < vm->b; i++) {
        printf("%02x ", vm->m[i]);
    }
    printf("\n");

    execute(vm);

    printhex($1 prog, (map(mov)+map(nop)+map(hlt)), ' ');

    return 0;
}
