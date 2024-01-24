#ifndef INTERRUPT_H
#define INTERRUPT_H

void divide_error();
void debug();
void nmi();
void int3();
void overflow();
void bounds();
void opcode();
void device_not_present();
void double_fault();
void i387_overrun();
void illegal_tss();
void illegal_segment();
void stack_overflow();
void general_protect();
void page_fault();
void reserved();

#endif