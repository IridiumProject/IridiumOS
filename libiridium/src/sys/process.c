#include <sys/process.h>
#include <sys/sysreq.h>


PID_T get_top_pid(void) {
    uint64_t out;
    sysreq(UAPI_PROCINFO_SERVICE, 0, &out);
    return (PID_T)out;
}
