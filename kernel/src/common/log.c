#include <common/log.h>
#include <common/string.h>
#include <common/panic.h>
#include <limine.h>

static volatile struct limine_terminal_request term_req = {
    .id = LIMINE_TERMINAL_REQUEST,
    .revision = 0
};


static struct limine_terminal* terminal = NULL;

static void puts(const char* str) 
{
    if (terminal == NULL) {
        terminal = term_req.response->terminals[0];
    }

    term_req.response->write(terminal, str, strlen(str)); 
}


void kprintf(char* fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);

    char* ptr;

    if (memcmp(fmt, KINFO, strlen(KINFO))) {
        puts(KINFO);
        fmt += strlen(KINFO);
    }

    for (ptr = fmt; *ptr != '\0'; ++ptr) {
        if (*ptr == '%') {
            ++ptr;
            switch (*ptr) {
                case 's':
                    puts(va_arg(ap, char*));
                    break;
                case 'd':
                	puts(dec2str(va_arg(ap, uint64_t)));
                	break;
                case 'x':
                	puts(hex2str(va_arg(ap, uint64_t)));
                	break;
            }
        } else {
            char terminated[2] = {*ptr, 0};
            puts(terminated);
        }
    }
}


void clear_term(void)
{
    kprintf("\033[H\033[2J\033[3J");
}
