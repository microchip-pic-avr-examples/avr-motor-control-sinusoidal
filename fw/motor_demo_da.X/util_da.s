.section .text
.global TCA_sync_start
; atomic start of TCA0 and TCA1
TCA_sync_start:
    in      r18,    0x3f   ; save SREG
    cli
    lds     r24,    0x0A00 ; TCA0 
    ori     r24,    0x01
    sts     0x0A00, r24
    lds     r24,    0x0A40 ; TCA1
    ori     r24,    0x01
    sts     0x0A40, r24
    out     0x3f,   r18    ; restore SREG
    ret

.global TCA_compare_set
TCA_compare_set:
    ret
