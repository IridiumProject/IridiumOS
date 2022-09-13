#ifndef INIT__SERVICES_H
#define INIT__SERVICES_H

#include <sys/process.h>
#include <perm.h>

// TODO: Dynamically allocate this.
#define MAX_SERVICES 50


struct Service {
    PID_T pid;
    PPERM_T perm_mask;
    const char* service_name;
    const char* service_description;
};

/*
 *  Any functions with '_' prefix means it could
 *  be deprecated or changed in the near future thus use 
 *  with caution.
 *
 */

PID_T _service_create(const char* path, const char* name, const char* description, PPERM_T pmask, uint8_t uses_console);

#endif
