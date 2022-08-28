#include <fs/initrd.h>
#include <common/string.h>
#include <common/log.h>
#include <common/panic.h>
#include <common/asm.h>
#include <mm/kheap.h>
#include <limine.h>


static struct limine_module_request mod_req = {
    .id = LIMINE_MODULE_REQUEST,
    .revision = 0
};


struct TarHeader {
    char filename[100];
    char mode[8];
    char uid[8];
    char gid[8];
    char size[12];
    char mtime[12];
    char chksum[8];
    char typeflag[1];
};


static struct limine_file* initrd = NULL;

static size_t getsize(const char* in) {
    size_t size = 0;
    size_t j;
    size_t count = 1;
 
    for (j = 11; j > 0; j--, count *= 8) {
        size += ((in[j - 1] - '0') * count);
    }

    return size;
 
}

// Returns null terminated buffer holding file contents.
static void* get_buf(const char* data, size_t n_bytes) {
    data += 0x200;          // Get past header/padding.

    char* buf = kmalloc(sizeof(char) * (n_bytes + 1));

    for (size_t i = 0; i < n_bytes; ++i) {
        buf[i] = data[i];
    }

    buf[n_bytes] = 0;
    return (void*)buf;
}


void initrd_init(void) {
    for (uint64_t i = 0; i < mod_req.response->module_count; ++i) {
        struct limine_file* cur = mod_req.response->modules[i];

        if (strcmp(cur->path, "/Iridium/initrd.sys") == 0) {
            initrd = cur;
            break;
        }
    }

    if (initrd == NULL) {
        kprintf(KERN_PANIC "Unable to load initial ramdisk!\n");
        CLI; HLT;
    }
}


void* initrd_open(const char* path) {
    if (strlen(path) > 99) {
        return NULL;
    }

    char* initrd_data = initrd->address + 0x200;        // Get passed directory name.
    struct TarHeader* header = (struct TarHeader*)initrd_data;

    while (header->filename[0] != '\0') { 
        if (strcmp(header->filename, path) == 0) {
            return get_buf(initrd_data, getsize(header->size));
        }

        initrd_data += 0x400;                       // Get passed first file.
        header = (struct TarHeader*)initrd_data;
    }

    // File not found.
    return NULL;
}

void initrd_close(void* fp) {
    kfree(fp);
}
