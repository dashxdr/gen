#include <inttypes.h>

extern unsigned char cpu_readmem24(uint32_t);
extern unsigned short cpu_readmem24_word(uint32_t);
extern uint32_t cpu_readmem24_dword(uint32_t);
extern void cpu_writemem24(uint32_t, unsigned char);
extern void cpu_writemem24_word(uint32_t, unsigned short);
extern void cpu_writemem24_dword(uint32_t, uint32_t);
