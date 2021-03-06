; generated by ARM C/C++ Compiler, 4.1 [Build 894]
; commandline ArmCC [--debug -c --asm -o..\Output\test.o --depend=..\Output\test.d --cpu=Cortex-M3 --apcs=interwork -O0 -I.\src -I.\inc -I..\CMSIS -I"E:\Keil 4 ARM\ARM\RV31\Inc" -I"E:\Keil 4 ARM\ARM\CMSIS\Include" -I"E:\Keil 4 ARM\ARM\Inc\ST\STM32F10x" -DSTM32F10X_MD --omf_browse=..\Output\test.crf src\test.c]
        THUMB
        REQUIRE8
        PRESERVE8

        AREA ||.text||, CODE, READONLY, ALIGN=2

System_Myinit PROC
        LDR      r0,|L1.324|
        LDR      r0,[r0,#0]
        ORR      r0,r0,#1
        LDR      r1,|L1.324|
        STR      r0,[r1,#0]
        MOV      r0,r1
        LDR      r0,[r0,#0]
        LDR      r1,|L1.328|
        ANDS     r0,r0,r1
        LDR      r1,|L1.324|
        STR      r0,[r1,#0]
        MOV      r0,r1
        LDR      r0,[r0,#0]
        BIC      r0,r0,#0x40000
        STR      r0,[r1,#0]
        MOV      r0,#0x9f0000
        STR      r0,[r1,#8]
        MOV      r0,r1
        LDR      r0,[r0,#4]
        LDR      r1,|L1.332|
        ANDS     r0,r0,r1
        LDR      r1,|L1.324|
        STR      r0,[r1,#4]
        MOV      r0,r1
        LDR      r0,[r0,#4]
        ORR      r0,r0,#0x1d0000
        STR      r0,[r1,#4]
        MOV      r0,r1
        LDR      r0,[r0,#0]
        ORR      r0,r0,#0x10000
        STR      r0,[r1,#0]
        NOP      
|L1.74|
        LDR      r0,|L1.324|
        LDR      r0,[r0,#0]
        TST      r0,#0x20000
        BEQ      |L1.74|
        LDR      r0,|L1.324|
        LDR      r0,[r0,#4]
        ORR      r0,r0,#0x400
        LDR      r1,|L1.324|
        STR      r0,[r1,#4]
        LDR      r0,|L1.336|
        LDR      r0,[r0,#0]
        BIC      r0,r0,#0xf
        LDR      r1,|L1.336|
        STR      r0,[r1,#0]
        MOV      r0,r1
        LDR      r0,[r0,#0]
        ORR      r0,r0,#0x10
        STR      r0,[r1,#0]
        MOV      r0,r1
        LDR      r0,[r0,#0]
        ORR      r0,r0,#2
        STR      r0,[r1,#0]
        LDR      r0,|L1.324|
        LDR      r0,[r0,#0]
        ORR      r0,r0,#0x1000000
        LDR      r1,|L1.324|
        STR      r0,[r1,#0]
        NOP      
|L1.142|
        LDR      r0,|L1.324|
        LDR      r0,[r0,#0]
        TST      r0,#0x2000000
        BEQ      |L1.142|
        LDR      r0,|L1.324|
        LDR      r0,[r0,#4]
        ORR      r0,r0,#2
        LDR      r1,|L1.324|
        STR      r0,[r1,#4]
        NOP      
|L1.166|
        LDR      r0,|L1.324|
        LDR      r0,[r0,#4]
        TST      r0,#1
        BNE      |L1.166|
        LDR      r0,|L1.324|
        LDR      r0,[r0,#0]
        BIC      r0,r0,#1
        LDR      r1,|L1.324|
        STR      r0,[r1,#0]
        LSLS     r0,r1,#15
        LDR      r1,|L1.340|
        STR      r0,[r1,#0]
        BX       lr
        ENDP

WWDG_IRQHandler PROC
        ADDS     r2,r1,#1
        UXTB     r1,r2
        ADDS     r2,r0,#1
        UXTB     r0,r2
        BX       lr
        ENDP

||fun|| PROC
        PUSH     {r4,lr}
        SUB      sp,sp,#0x18
        MOVS     r2,#0x14
        LDR      r1,|L1.344|
        ADD      r0,sp,#4
        BL       __aeabi_memcpy4
        LDR      r0,[sp,#0xc]
        STR      r0,[sp,#0x10]
        LDR      r0,[sp,#8]
        STR      r0,[sp,#0xc]
        LDR      r0,[sp,#8]
        ADDS     r0,r0,#1
        STR      r0,[sp,#8]
        LDR      r0,[sp,#4]
        ADDS     r0,r0,#1
        STR      r0,[sp,#4]
        LDR      r0,[sp,#0x14]
        ADDS     r0,r0,#1
        STR      r0,[sp,#0x14]
        ADDS     r0,r4,#1
        MOV      r4,r0
        ADD      sp,sp,#0x18
        POP      {r4,pc}
        ENDP

main PROC
        PUSH     {r4,r6,lr}
        SUB      sp,sp,#0x1c
        MOVS     r6,#3
        MOVS     r2,#0x18
        LDR      r1,|L1.344|
        ADDS     r1,r1,#0x14
        ADD      r0,sp,#4
        BL       __aeabi_memcpy4
        LDR      r0,[sp,#0xc]
        STR      r0,[sp,#0x10]
        LDR      r0,[sp,#8]
        STR      r0,[sp,#0xc]
        LDR      r0,[sp,#8]
        ADDS     r0,r0,#1
        STR      r0,[sp,#8]
        LDR      r0,[sp,#4]
        ADDS     r0,r0,#1
        STR      r0,[sp,#4]
        LDR      r0,[sp,#0x14]
        ADDS     r0,r0,#1
        STR      r0,[sp,#0x14]
        LDR      r0,[sp,#0x18]
        ADDS     r0,r0,#1
        STR      r0,[sp,#0x18]
        CMP      r5,#3
        BEQ      |L1.310|
        MOV      r4,pc
|L1.310|
        AND      r4,r4,#3
        BL       ||fun||
        MOVS     r0,#0x36
        ADD      sp,sp,#0x1c
        POP      {r4,r6,pc}
        ENDP

|L1.324|
        DCD      0x40021000
|L1.328|
        DCD      0xfef6ffff
|L1.332|
        DCD      0xff80c00c
|L1.336|
        DCD      0x40022000
|L1.340|
        DCD      0xe000ed08
|L1.344|
        DCD      ||.constdata||

        AREA ||.arm_vfe_header||, DATA, READONLY, NOALLOC, ALIGN=2

        DCD      0x00000000

        AREA ||.constdata||, DATA, READONLY, ALIGN=2

        DCD      0x00000001
        DCD      0x00000002
        DCD      0x00000003
        DCD      0x00000004
        DCD      0x00000005
        DCD      0x00000001
        DCD      0x00000002
        DCD      0x00000003
        DCD      0x00000004
        DCD      0x00000005
        DCD      0x00000007

        AREA ||.data||, DATA, ALIGN=2

ms0
        %        8
ms1
        DCD      0x00000000
        DCB      0x00,0x00

__ARM_use_no_argv EQU 0

        EXPORT __ARM_use_no_argv
        EXPORT System_Myinit [CODE]
        EXPORT WWDG_IRQHandler [CODE]
        EXPORT ||fun|| [CODE]
        EXPORT main [CODE]
        EXPORT ms0 [DATA,SIZE=8]
        EXPORT ms1 [DATA,SIZE=6]

        IMPORT ||Lib$$Request$$armlib|| [CODE,WEAK]
        IMPORT __aeabi_memcpy4 [CODE]

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
