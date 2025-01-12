#include <printk.h>
#include <panic.h>

void
do_divide_error()
{
	panic("divide error caught");
}

void
do_debug()
{
	panic("debug trap caught");
}

void
do_nmi()
{
	panic("nmi error caught");
}

void
do_int3()
{
	panic("int3 trap caught");
}

void
do_overflow()
{
	panic("overflow trap caught");
}

void
do_bounds()
{
	panic("bound check trap caught");
}

void
do_opcode()
{
	panic("opcode error caught");
}

void
do_device_not_present()
{
	panic("device not present error caught");
}

void
do_double_fault(unsigned int err_code)
{
	panic("double fault caught");
}

void
do_i387_overrun()
{
	panic("i387 overrun caught");
}

void
do_illegal_tss(unsigned int err_code)
{
	panic("illegal tss caught");
}

void
do_illegal_segment(unsigned int err_code)
{
	panic("illegal segment caught");
}

void
do_stack_overflow(unsigned int err_code)
{
	panic("stack overflow caught");
}

void
do_general_protect(unsigned int err_code)
{
	panic("general protection exception caught");
}

void
do_page_fault(unsigned int err_code)
{
	panic("page fault caught");
}

void
do_reserved()
{
	panic("unknow interrupt caught");
}
