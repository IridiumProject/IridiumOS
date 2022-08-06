#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H


void divide_error(void);
void debug_exception(void);
void overflow(void);
void bound_range_exceeded(void);
void invalid_opcode(void);
void no_mathcoprocessor(void);
void double_fault(void);
void invalid_tss(void);
void segment_not_present(void);
void stack_segment_fault(void);
void general_protection_fault(void);
void page_fault(void);


#endif
