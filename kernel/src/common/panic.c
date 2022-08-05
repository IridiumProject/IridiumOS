#include <common/panic.h>
#include <common/log.h>

void print_panic_msg(void) {
    kprintf(KERN_PANIC);
}
