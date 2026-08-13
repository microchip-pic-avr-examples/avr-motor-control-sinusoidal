#ifndef FRAC_LIB_H
#define FRAC_LIB_H

#include <stdint.h>

typedef uint16_t fixp16_t;  /* U1Q15 0...32768 / 0.0...1.0000 */

/* CAUTION: no saturation, no overflow/underflow protection! */

extern fixp16_t   fmul_16x16(fixp16_t a, fixp16_t b);
/*  C equivalent:
 fixp16_t fmul_16x16(fixp16_t a, fixp16_t b)
{
    fixp16_t result = ((uint32_t)a * (uint32_t)b) >> 15;
    return   result;
}
 */

extern void       fmul_16x16k_n3(fixp16_t *p, fixp16_t k);
/*  C equivalent:
void fmul_16x16k_n3(fixp16_t *p, fixp16_t k)
{
    uint8_t len = 3;
    while(len--)
    {
        *p = c_fmul_16x16(*p, k);
        p++;
    }
}
 */

extern void       add_u16k_n3(uint16_t *p, uint16_t k);
/*  C equivalent:
void    add_u16k_n3(uint16_t *p, uint16_t k)
{
    uint8_t i = 3;
    while(i--)
        *p++ += k;
}
 */

#endif /* FRAC_LIB_H */

