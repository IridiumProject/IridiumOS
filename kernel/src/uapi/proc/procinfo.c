#include <uapi/proc/info.h>
#include <uapi/service.h>
#include <uapi/api_routine.h>
#include <proc/proc.h>

extern struct Process* queue_head;

static UAPI_STATUS_T procinfo_uapi(UAPI_REQ_T req, UAPI_OUT_T* output) {
    switch (req) {
        case PROCINFO_UAPI_GET_PIDTOP:
            *output = queue_head->pid;
            return UAPI_OK;
    }

    return UAPI_INVALID_ARG;
}


void procinfo_uapi_init(void) {
    UAPI_APPEND_SERVICE(UAPI_PROCINFO_SERVICE, procinfo_uapi);
}
