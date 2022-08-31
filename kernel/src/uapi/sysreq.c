#include <uapi/sysreq.h>
#include <uapi/service.h>

static UAPI_STATUS_T(*services[SERVICE_COUNT])(UAPI_REQ_T, UAPI_OUT_T*);


void sysreq_append_service(UAPI_SERVICE_NUM_T service_num, UAPI_STATUS_T(*service)(UAPI_REQ_T, UAPI_OUT_T*)) {
    if (services[service_num] != 0 || service_num >= SERVICE_COUNT) {
        return;
    }

    services[service_num] = service;
}


UAPI_STATUS_T sysreq(UAPI_SERVICE_NUM_T service_num, UAPI_REQ_T request, UAPI_OUT_T* output) {
    if (service_num >= SERVICE_COUNT || services[service_num] == 0) {
        return UAPI_INVALID_SERVICE;
    }
    
    return services[service_num](request, output);
}
