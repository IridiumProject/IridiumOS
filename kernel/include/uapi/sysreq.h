#ifndef UAPI_SYSREQ_H
#define UAPI_SYSREQ_H

#include <uapi/api_routine.h>
#include <uapi/service_def.h>

void sysreq_append_service(UAPI_SERVICE_NUM_T service_num, UAPI_STATUS_T(*service)(UAPI_REQ_T, UAPI_OUT_T*));
UAPI_STATUS_T sysreq(UAPI_SERVICE_NUM_T service_num, UAPI_REQ_T request, UAPI_OUT_T* output);



#endif
