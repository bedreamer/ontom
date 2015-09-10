; generated by ARM C/C++ Compiler, 4.1 [Build 894]
; commandline ArmCC [--debug -c --asm -o..\Output\system_stm32f10x.o --depend=..\Output\system_stm32f10x.d --cpu=Cortex-M3 --apcs=interwork -O0 -I.\inc -I.\src -I..\CMSIS -I"E:\Keil 4 ARM\ARM\RV31\Inc" -I"E:\Keil 4 ARM\ARM\CMSIS\Include" -I"E:\Keil 4 ARM\ARM\Inc\ST\STM32F10x" -DSTM32F10X_MD --omf_browse=..\Output\system_stm32f10x.crf ..\CMSIS\system_stm32f10x.c]
        THUMB
        REQUIRE8
        PRESERVE8

        AREA ||.text||, CODE, READONLY, ALIGN=2

SetSysClockTo72 PROC
        MOVS     r1,#0
        MOVS     r0,#0
        LDR      r2,|L1.420|
        LDR      r2,[r2,#0]
        ORR      r2,r2,#0x10000
        LDR      r3,|L1.420|
        STR      r2,[r3,#0]
        NOP      
|L1.18|
        LDR      r2,|L1.420|
        LDR      r2,[r2,#0]
        AND      r0,r2,#0x20000
        ADDS     r1,r1,#1
        CBNZ     r0,|L1.36|
        CMP      r1,#0x500
        BNE      |L1.18|
|L1.36|
        LDR      r2,|L1.420|
        LDR      r2,[r2,#0]
        TST      r2,#0x20000
        BEQ      |L1.50|
        MOVS     r0,#1
        B        |L1.52|
|L1.50|
        MOVS     r0,#0
|L1.52|
        CMP      r0,#1
        BNE      |L1.190|
        LDR      r2,|L1.424|
        LDR      r2,[r2,#0]
        ORR      r2,r2,#0x10
        LDR      r3,|L1.424|
        STR      r2,[r3,#0]
        MOV      r2,r3
        LDR      r2,[r2,#0]
        BIC      r2,r2,#3
        STR      r2,[r3,#0]
        MOV      r2,r3
        LDR      r2,[r2,#0]
        ORR      r2,r2,#2
        STR      r2,[r3,#0]
        LDR      r2,|L1.420|
        LDR      r2,[r2,#4]
        LDR      r3,|L1.420|
        STR      r2,[r3,#4]
        MOV      r2,r3
        LDR      r2,[r2,#4]
        STR      r2,[r3,#4]
        MOV      r2,r3
        LDR      r2,[r2,#4]
        ORR      r2,r2,#0x400
        STR      r2,[r3,#4]
        MOV      r2,r3
        LDR      r2,[r2,#4]
        BIC      r2,r2,#0x3f0000
        STR      r2,[r3,#4]
        MOV      r2,r3
        LDR      r2,[r2,#4]
        ORR      r2,r2,#0x1d0000
        STR      r2,[r3,#4]
        MOV      r2,r3
        LDR      r2,[r2,#0]
        ORR      r2,r2,#0x1000000
        STR      r2,[r3,#0]
        NOP      
|L1.144|
        LDR      r2,|L1.420|
        LDR      r2,[r2,#0]
        TST      r2,#0x2000000
        BEQ      |L1.144|
        LDR      r2,|L1.420|
        LDR      r2,[r2,#4]
        BIC      r2,r2,#3
        LDR      r3,|L1.420|
        STR      r2,[r3,#4]
        MOV      r2,r3
        LDR      r2,[r2,#4]
        ORR      r2,r2,#2
        STR      r2,[r3,#4]
        NOP      
|L1.178|
        LDR      r2,|L1.420|
        LDR      r2,[r2,#4]
        AND      r2,r2,#0xc
        CMP      r2,#8
        BNE      |L1.178|
|L1.190|
        BX       lr
        ENDP

SetSysClock PROC
        PUSH     {r4,lr}
        BL       SetSysClockTo72
        POP      {r4,pc}
        ENDP

SystemInit PROC
        PUSH     {r4,lr}
        LDR      r0,|L1.420|
        LDR      r0,[r0,#0]
        ORR      r0,r0,#1
        LDR      r1,|L1.420|
        STR      r0,[r1,#0]
        MOV      r0,r1
        LDR      r0,[r0,#4]
        LDR      r1,|L1.428|
        ANDS     r0,r0,r1
        LDR      r1,|L1.420|
        STR      r0,[r1,#4]
        MOV      r0,r1
        LDR      r0,[r0,#0]
        LDR      r1,|L1.432|
        ANDS     r0,r0,r1
        LDR      r1,|L1.420|
        STR      r0,[r1,#0]
        MOV      r0,r1
        LDR      r0,[r0,#0]
        BIC      r0,r0,#0x40000
        STR      r0,[r1,#0]
        MOV      r0,r1
        LDR      r0,[r0,#4]
        BIC      r0,r0,#0x7f0000
        STR      r0,[r1,#4]
        MOV      r0,#0x9f0000
        STR      r0,[r1,#8]
        BL       SetSysClock
        MOV      r0,#0x8000000
        LDR      r1,|L1.436|
        STR      r0,[r1,#0]
        POP      {r4,pc}
        ENDP

SystemCoreClockUpdate PROC
        PUSH     {r4,lr}
        MOVS     r1,#0
        MOVS     r0,#0
        MOVS     r2,#0
        LDR      r3,|L1.420|
        LDR      r3,[r3,#4]
        AND      r1,r3,#0xc
        CBZ      r1,|L1.306|
        CMP      r1,#4
        BEQ      |L1.314|
        CMP      r1,#8
        BNE      |L1.386|
        B        |L1.322|
|L1.306|
        LDR      r3,|L1.440|
        LDR      r4,|L1.444|
        STR      r3,[r4,#0]  ; SystemCoreClock
        B        |L1.394|
|L1.314|
        LDR      r3,|L1.440|
        LDR      r4,|L1.444|
        STR      r3,[r4,#0]  ; SystemCoreClock
        B        |L1.394|
|L1.322|
        LDR      r3,|L1.420|
        LDR      r3,[r3,#4]
        AND      r0,r3,#0x3c0000
        LDR      r3,|L1.420|
        LDR      r3,[r3,#4]
        AND      r2,r3,#0x10000
        MOVS     r3,#2
        ADD      r0,r3,r0,LSR #18
        CBNZ     r2,|L1.356|
        LDR      r3,|L1.448|
        MULS     r3,r0,r3
        LDR      r4,|L1.444|
        STR      r3,[r4,#0]  ; SystemCoreClock
        B        |L1.384|
|L1.356|
        LDR      r3,|L1.420|
        LDR      r3,[r3,#4]
        TST      r3,#0x20000
        BEQ      |L1.376|
        LDR      r3,|L1.448|
        MULS     r3,r0,r3
        LDR      r4,|L1.444|
        STR      r3,[r4,#0]  ; SystemCoreClock
        B        |L1.384|
|L1.376|
        LDR      r3,|L1.440|
        MULS     r3,r0,r3
        LDR      r4,|L1.444|
        STR      r3,[r4,#0]  ; SystemCoreClock
|L1.384|
        B        |L1.394|
|L1.386|
        LDR      r3,|L1.440|
        LDR      r4,|L1.444|
        STR      r3,[r4,#0]  ; SystemCoreClock
        NOP      
|L1.394|
        NOP      
        LDR      r3,|L1.420|
        LDR      r3,[r3,#4]
        UBFX     r3,r3,#4,#4
        LDR      r4,|L1.452|
        LDRB     r1,[r4,r3]
        LDR      r3,|L1.444|
        LDR      r3,[r3,#0]  ; SystemCoreClock
        LSRS     r3,r3,r1
        LDR      r4,|L1.444|
        STR      r3,[r4,#0]  ; SystemCoreClock
        POP      {r4,pc}
        ENDP

|L1.420|
        DCD      0x40021000
|L1.424|
        DCD      0x40022000
|L1.428|
        DCD      0xf8ff0000
|L1.432|
        DCD      0xfef6ffff
|L1.436|
        DCD      0xe000ed08
|L1.440|
        DCD      0x007a1200
|L1.444|
        DCD      SystemCoreClock
|L1.448|
        DCD      0x003d0900
|L1.452|
        DCD      AHBPrescTable

        AREA ||.arm_vfe_header||, DATA, READONLY, NOALLOC, ALIGN=2

        DCD      0x00000000

        AREA ||.data||, DATA, ALIGN=2

SystemCoreClock
        DCD      0x044aa200
AHBPrescTable
        DCB      0x00,0x00,0x00,0x00
        DCB      0x00,0x00,0x00,0x00
        DCB      0x01,0x02,0x03,0x04
        DCB      0x06,0x07,0x08,0x09

        EXPORT SystemInit [CODE]
        EXPORT SystemCoreClockUpdate [CODE]
        EXPORT SystemCoreClock [DATA,SIZE=4]
        EXPORT AHBPrescTable [DATA,SIZE=16]

        IMPORT ||Lib$$Request$$armlib|| [CODE,WEAK]

        KEEP SetSysClockTo72
        KEEP SetSysClock

        ATTR FILESCOPE
        ATTR SETVALUE Tag_ABI_PCS_wchar_t,2
        ATTR SETVALUE Tag_ABI_enum_size,1
        ATTR SETVALUE Tag_ABI_optimization_goals,6
        ATTR SETSTRING Tag_conformance,"2.06"
        ATTR SETVALUE AV,18,1

        ASSERT {ENDIAN} = "little"
        ASSERT {INTER} = {TRUE}
        ASSERT {ROPI} = {FALSE}
        ASSERT {RWPI} = {FALSE}
        ASSERT {IEEE_FULL} = {FALSE}
        ASSERT {IEEE_PART} = {FALSE}
        ASSERT {IEEE_JAVA} = {FALSE}
        END
