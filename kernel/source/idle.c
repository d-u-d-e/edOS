#include <kernel/include/kernel.h>
#include <kernel/include/sys/__assert.h>


void __weak arch_spin_relax(void)
{
	__ASSERT(!arch_irq_unlocked(arch_irq_lock()),
		 "this is meant to be called with IRQs disabled");

	arch_nop();
}
