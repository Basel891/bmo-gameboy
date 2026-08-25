#pragma once

#define CLR_BIT(REG, PIN) ((REG)) &= ~(1 << (PIN))
#define TOG_BIT(REG, PIN) ((REG) ^= (1 << (PIN)))
#define SET_BIT(REG, PIN) ((REG)) |= (1 << (PIN))
#define GET_BIT(REG, PIN) (((REG) >> (PIN)) & 1)

