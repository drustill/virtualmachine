/* main.c */
#include <stdio.h>
#include "main.h"

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

int main(int argc, char *argv[]) {
	Program *prog;
	VM *vm;

	vm = virtualmachine();
	printf("vm = %p (sz: %d)\n", vm, sizeof(struct s_vm));
}
