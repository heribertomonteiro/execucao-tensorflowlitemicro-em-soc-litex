#include <stddef.h>
struct _reent;

extern char _end;      // end of .bss, start of heap
extern char _fstack;   // top of stack (heap must not cross this)

static char *heap_ptr = NULL;

void * _sbrk(ptrdiff_t incr) {
    if (heap_ptr == NULL) {
        heap_ptr = &_end;
    }
    char *prev = heap_ptr;
    char *next = prev + incr;
    // simple collision check: do not pass stack base
    if (next >= &_fstack) {
        return (void *)-1;
    }
    heap_ptr = next;
    return (void *)prev;
}

void * _sbrk_r(struct _reent *r, ptrdiff_t incr) {
    (void)r;
    return _sbrk(incr);
}
