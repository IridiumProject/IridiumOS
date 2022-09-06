#ifndef PROC_PSIGNAL_H
#define PROC_PSIGNAL_H

#define PSIGNAL_EXTRACT_SOURCE(psignal) ((psignal >> 32) & 0xFFFF)
#define PSIGNAL_EXTRACT_SIGPAYLOAD(psignal) (psignal & 0xFFFFFFFF)
#define PSIGNAL_TOP_BITS_UNTOUCHED(psignal) (psignal >> 48 == 0)


#endif
