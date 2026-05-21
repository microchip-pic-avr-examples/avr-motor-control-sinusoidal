.section .text
  
    
.global fmul_16x16
; C prototype:
; fixp16_t fmul_16x16(fixp16_t a, fixp16_t b);
;     params: R25:R24=a, R23:R22=b
;     retval: R25:R24
;  temporary: R25:R24:R23:R22 tempresult
; arg copies: R19:R18 R21:R20 a, b copies
; fmul only works with r16-r23
fmul_16x16:
    movw    r18, r24  ;   r25:r24 -> r19:r18  a
    movw    r20, r22  ;   r23:r22 -> r21:r20  b
    clr     r26       ;  temporary zero
    fmul    r19, r21  ; r19*r21*2 -> r1:r0  ; (ah * bh) << 1
    movw    r24, r0   ;     r1:r0 -> r25:r24
    fmul    r18, r20  ; r18*r20*2 -> r1:r0  ; (al * bl) << 1
    movw    r22, r0   ;     r1:r0 -> r23:r22
    adc     r24, r26  ;   0+r24+C -> r24+C
    fmul    r19, r20  ; r19*r20*2 -> r1:r0  ; (ah * bl) << 1
    add     r23, r0   ;    r23+r0 -> r23+C
    adc     r24, r1   ;  r24+r1+C -> r24+C
    adc     r25, r26  ;   0+r25+C -> r25
    fmul    r18, r21  ; r18*r21*2 -> r1:r0  ; (al * bh) << 1
    add     r23, r0   ;    r23+r0 -> r23+C
    adc     r24, r1   ;  r24+r1+C -> r24+C
    adc     r25, r26  ;   0+r25+C -> r25
    clr     r1        ; restore zero reg
    ret
    
.global fmul_16x16k_n3
; C prototype:
; void fmul_16x16k_n3(fixp16_t *p, fixp16_t k);
;     params: R25:R24=p, R23:R22=k
;  temporary: R25:R24:R23:R22 tempresult
;   zero reg: R26
; iter count: R27
; R31:R30 - Z pointer register pair
; R1:R0 - multiplication intermediary result
;  fmul args: R19:R18 R21:R20
; fmul only works with r16-r23 inputs
fmul_16x16k_n3:
    movw    r30, r24  ;  param p
    movw    r18, r22  ;  param k , r19:r18=bh:bl
    clr     r26       ;  zero reg
    ldi     r27, 3    ;  iterations count
_fmul_16x16k_n3_loop:
    ldd     r20, Z+0  ; al
    ldd     r21, Z+1  ; ah
    fmul    r19, r21  ; r19*r21*2 -> r1:r0  ; (ah * bh) << 1
    movw    r24, r0   ;     r1:r0 -> r25:r24
    fmul    r18, r20  ; r18*r20*2 -> r1:r0  ; (al * bl) << 1
    movw    r22, r0   ;     r1:r0 -> r23:r22
    adc     r24, r26  ;   0+r24+C -> r24+C
    fmul    r19, r20  ; r19*r20*2 -> r1:r0  ; (ah * bl) << 1
    add     r23, r0   ;    r23+r0 -> r23+C
    adc     r24, r1   ;  r24+r1+C -> r24+C
    adc     r25, r26  ;   0+r25+C -> r25
    fmul    r18, r21  ; r18*r21*2 -> r1:r0  ; (al * bh) << 1
    add     r23, r0   ;    r23+r0 -> r23+C
    adc     r24, r1   ;  r24+r1+C -> r24+C
    adc     r25, r26  ;   0+r25+C -> r25
    st      Z+, r24
    st      Z+, r25
    dec     r27
    brne    _fmul_16x16k_n3_loop
    clr     r1        ; restore zero reg
    ret
   
.global add_u16k_n3
; C prototype:
; void add_u16k_n3(uint16_t *p, uint16_t k);
;     params: R25:R24=p, R23:R22=k
; R31:R30 - Z pointer register pair
add_u16k_n3:
    movw    r30, r24  ; param p, Z pointer

    ldd     r24, Z+0  ; low byte
    ldd     r25, Z+1  ; high byte
    add	    r24, r22
    adc     r25, r23
    std     Z+0, r24
    std     Z+1, r25

    ldd     r24, Z+2  ; low byte
    ldd     r25, Z+3  ; high byte
    add	    r24, r22
    adc     r25, r23
    std     Z+2, r24
    std     Z+3, r25

    ldd     r24, Z+4  ; low byte
    ldd     r25, Z+5  ; high byte
    add	    r24, r22
    adc     r25, r23
    std     Z+4, r24
    std     Z+5, r25

    ret

