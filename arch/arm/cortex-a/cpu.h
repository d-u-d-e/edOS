#pragma once

#define MODE_USR	0x10
#define MODE_FIQ	0x11
#define MODE_IRQ	0x12
#define MODE_SVC	0x13
#define MODE_ABT	0x17
#define MODE_HYP	0x1a
#define MODE_UND	0x1b
#define MODE_SYS	0x1f
#define MODE_MASK	0x1f

#define A_BIT	(1 << 8)
#define I_BIT	(1 << 7)
#define F_BIT	(1 << 6)
#define T_BIT	(1 << 5)