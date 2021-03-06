; generated by ARM C/C++ Compiler, 4.1 [Build 894]
; commandline ArmCC [--debug -c --asm -o..\Output\uart.o --depend=..\Output\uart.d --cpu=Cortex-M3 --apcs=interwork -O1 -I.\src -I.\inc -I..\CMSIS -I"E:\Keil 4 ARM\ARM\RV31\Inc" -I"E:\Keil 4 ARM\ARM\CMSIS\Include" -I"E:\Keil 4 ARM\ARM\Inc\ST\STM32F10x" -DSTM32F10X_CL --omf_browse=..\Output\uart.crf src\UART.c]
        THUMB
        REQUIRE8
        PRESERVE8

        AREA ||.text||, CODE, READONLY, ALIGN=2

USART1_Send PROC
        LDR      r1,|L1.628|
        LDRB     r0,[r1,#1]  ; U1_TXfin
        AND      r2,r0,#0xf
        CMP      r2,#0xd
        BNE      |L1.46|
        AND      r0,r0,#0xf5
        STRB     r0,[r1,#1]
        LDR      r1,|L1.632|
        MOVS     r0,#0x6e
        STRH     r0,[r1,#0]
        LDR      r1,|L1.636|
        MOVS     r0,#1
        STRB     r0,[r1,#0]
        LDR      r1,|L1.640|
        LSLS     r0,r0,#8
        STR      r0,[r1,#0]
        LDR      r0,|L1.644|
        LDRH     r1,[r0,#0]
        ORR      r1,r1,#0x80
        STRH     r1,[r0,#0]
|L1.46|
        BX       lr
        ENDP

USART1_IRQHandler PROC
        PUSH     {r4-r11,lr}
        LDR      r7,|L1.644|
        LDRH     r1,[r7,#0]
        LDR      r12,|L1.628|
        LDR      r3,|L1.648|
        LDR      r5,|L1.628|
        MOVS     r0,#0x88
        BICS     r0,r0,r1
        ADD      r12,r12,#0xa
        SUB      r4,r3,#4
        ADD      r10,r5,#6
        BNE      |L1.130|
        LDRH     r0,[r4,#0]
        LSLS     r0,r0,#24
        BPL      |L1.130|
        LDRH     r0,[r5,#8]  ; U1_TXlen
        ADDS     r1,r0,#1
        UXTH     r1,r1
        STRH     r1,[r5,#8]
        LDRB     r0,[r12,r0]
        STRH     r0,[r3,#0]
        LDRH     r0,[r10,#0]  ; U1_RxTxLen
        CMP      r0,r1
        BNE      |L1.126|
        LDRH     r0,[r7,#0]
        BIC      r0,r0,#0x80
        STRH     r0,[r7,#0]
        LDRH     r0,[r7,#0]
        ORR      r0,r0,#0x40
        STRH     r0,[r7,#0]
|L1.126|
        POP      {r4-r11,pc}
|L1.130|
        LDRH     r0,[r7,#0]
        MOVS     r1,#0x48
        BICS     r1,r1,r0
        MOV      r8,#2
        MOV      r0,#0
        BNE      |L1.226|
        LDRH     r1,[r4,#0]
        LSLS     r1,r1,#25
        BPL      |L1.226|
        LDRH     r1,[r7,#0]
        BIC      r1,r1,#0x40
        STRH     r1,[r7,#0]
        LDRH     r1,[r4,#0]
        BIC      r1,r1,#0x40
        STRH     r1,[r4,#0]
        LDR      r1,|L1.652|
        LDR      r2,[r1,#0xc]
        EOR      r2,r2,#0x4000
        STR      r2,[r1,#0xc]
        STRH     r0,[r5,#8]
        LDR      r1,|L1.636|
        STRB     r0,[r1,#0]
        LDR      r2,|L1.632|
        MOV      r1,#0xffff
        STRH     r1,[r2,#0]
        LDRB     r1,[r5,#1]  ; U1_TXfin
        AND      r1,r1,#0xf8
        STRB     r1,[r5,#1]
        STRB     r8,[r5,#0]
        STRH     r0,[r10,#0]
        LDR      r1,|L1.640|
        MOV      r0,#0x1000000
        STR      r0,[r1,#0]
        LDRH     r0,[r7,#0]
        ORR      r0,r0,#4
        STRH     r0,[r7,#0]
|L1.224|
        B        |L1.126|
|L1.226|
        LDRH     r2,[r7,#0]
        MOVS     r1,#0x24
        BICS     r1,r1,r2
        BNE      |L1.126|
        LDRH     r1,[r4,#0]
        LSLS     r1,r1,#26
        BPL      |L1.126|
        LDRB     r1,[r5,#0]  ; U1_RXfin
        LDR      r6,|L1.656|
        ANDS     r2,r1,#3
        LDR      r1,|L1.660|
        MOV      r9,#4
        BEQ      |L1.270|
        CMP      r2,#1
        BEQ      |L1.356|
        CMP      r2,#2
        BEQ      |L1.280|
        CMP      r2,#3
        BNE      |L1.300|
        B        |L1.368|
|L1.270|
        STRH     r9,[r6,#0]
        STRB     r8,[r1,#0]
        B        |L1.300|
|L1.280|
        STRB     r0,[r5,#2]
        STRB     r0,[r5,#3]
        STRB     r0,[r5,#4]
        STRH     r0,[r10,#0]
        STRB     r0,[r5,#0]
        STRH     r9,[r6,#0]
        STRB     r8,[r1,#0]
|L1.300|
        LDRH     r2,[r4,#0]
        MOV      r11,#0x14
        LSLS     r2,r2,#28
        MOV      lr,#3
        BEQ      |L1.374|
        LDRH     r2,[r3,#0]
        LDRH     r2,[r4,#0]
        STRH     r0,[r10,#0]
        STRB     r0,[r5,#4]
        LDRB     r2,[r5,#3]  ; Errcount
        ADDS     r2,r2,#1
        UXTB     r2,r2
        STRB     r2,[r5,#3]
        CMP      r2,#5
        BCC      |L1.126|
        STRB     r0,[r5,#0]
        LDRH     r0,[r7,#0]
        BIC      r0,r0,#4
        STRH     r0,[r7,#0]
        STRH     r11,[r6,#0]
        STRB     lr,[r1,#0]
|L1.354|
        B        |L1.126|
|L1.356|
        LDRH     r2,[r4,#0]
        LDRH     r2,[r3,#0]
        STRB     r8,[r5,#0]
        STRB     r0,[r1,#0]
        B        |L1.126|
|L1.368|
        LDRH     r0,[r4,#0]
        LDRH     r0,[r3,#0]
        B        |L1.126|
|L1.374|
        LDRB     r2,[r5,#3]  ; Errcount
        SUBS     r4,r2,#1
        STRB     r4,[r5,#3]
        CBNZ     r2,|L1.384|
        STRB     r0,[r5,#3]
|L1.384|
        LDRH     r2,[r3,#0]
        UXTB     r3,r2
        LDRB     r8,[r5,#2]  ; U1_RXflag
        LSLS     r2,r8,#30
        BMI      |L1.472|
        LDRH     r2,[r10,#0]  ; U1_RxTxLen
        CBNZ     r2,|L1.418|
        LDRB     r4,[r5,#4]  ; Headcapfail
        ADD      r9,r4,#1
        STRB     r9,[r5,#4]
        CMP      r4,#0xa
        BEQ      |L1.446|
|L1.418|
        ADDS     r1,r2,#1
        UXTH     r1,r1
        STRH     r1,[r10,#0]
        LDRB     r2,[r12,r2]
        CMP      r2,r3
        BNE      |L1.466|
        CMP      r1,#5
        BNE      |L1.224|
        ORR      r0,r8,#2
        STRB     r0,[r5,#2]
        B        |L1.126|
|L1.446|
        STRB     r0,[r5,#0]
        LDRH     r0,[r7,#0]
        BIC      r0,r0,#4
        STRH     r0,[r7,#0]
        STRH     r11,[r6,#0]
        STRB     lr,[r1,#0]
        B        |L1.126|
|L1.466|
        STRH     r0,[r10,#0]
        B        |L1.126|
|L1.472|
        LDRB     r2,[r5,#0]  ; U1_RXfin
        LSLS     r1,r2,#24
        BMI      |L1.354|
        LDRH     r1,[r10,#0]  ; U1_RxTxLen
        CMP      r1,#0x43
        BCS      |L1.498|
        ADDS     r0,r1,#1
        STRH     r0,[r10,#0]
        STRB     r3,[r12,r1]
        B        |L1.126|
|L1.498|
        STRH     r0,[r10,#0]
        ORR      r0,r2,#0x80
        STRB     r0,[r5,#0]
        B        |L1.126|
        ENDP

UART_Init PROC
        PUSH     {r4,lr}
        LDR      r0,|L1.664|
        LDR      r1,[r0,#0x18]
        MOV      r2,#0x4005
        ORRS     r1,r1,r2
        STR      r1,[r0,#0x18]
        LDR      r1,|L1.640|
        MOV      r2,#0x1000000
        STR      r2,[r1,#0]
        ASRS     r0,r0,#21
        STR      r0,[r1,#0]
        LSLS     r0,r0,#1
        STR      r0,[r1,#0]
        LDR      r0,|L1.640|
        SUBS     r0,r0,#0xc
        LDR      r3,[r0,#0]
        MOV      r4,#0x333
        BICS     r3,r3,r4
        STR      r3,[r0,#0]
        LDR      r3,[r0,#0]
        ORR      r3,r3,#0x32
        STR      r3,[r0,#0]
        LDR      r3,[r0,#0]
        LSLS     r4,r4,#2
        BICS     r3,r3,r4
        STR      r3,[r0,#0]
        LDR      r3,[r0,#0]
        ORR      r3,r3,#0x880
        STR      r3,[r0,#0]
        LDR      r0,|L1.668|
        LDR      r3,[r0,#4]
        BIC      r3,r3,#4
        STR      r3,[r0,#4]
        LDR      r3,|L1.644|
        MOV      r0,#0x1d4c
        SUBS     r3,r3,#4
        STRH     r0,[r3,#0]
        LDR      r3,|L1.644|
        MOVS     r0,#0
        ADDS     r3,r3,#0xc
        STRH     r0,[r3,#0]
        SUBS     r3,r3,#4
        STRH     r0,[r3,#0]
        SUBS     r3,r3,#4
        STRH     r0,[r3,#0]
        SUBS     r3,r3,#4
        MOV      r0,#0x202c
        STRH     r0,[r3,#0]
        STR      r2,[r1,#0]
        POP      {r4,pc}
        ENDP

        DCW      0x0000
|L1.628|
        DCD      ||.data||
|L1.632|
        DCD      U1_TXresettime
|L1.636|
        DCD      TXtimememb
|L1.640|
        DCD      0x40010810
|L1.644|
        DCD      0x4001380c
|L1.648|
        DCD      0x40013804
|L1.652|
        DCD      0x40011000
|L1.656|
        DCD      U1_RXresettime
|L1.660|
        DCD      RXtimememb
|L1.664|
        DCD      0x40021000
|L1.668|
        DCD      0x40010000

        AREA ||.arm_vfe_header||, DATA, READONLY, NOALLOC, ALIGN=2

        DCD      0x00000000

        AREA ||.data||, DATA, ALIGN=1

U1_RXfin
        DCB      0x02
U1_TXfin
        DCB      0x00
U1_RXflag
        DCB      0x00
Errcount
        DCB      0x00
Headcapfail
        DCB      0x00,0x00
U1_RxTxLen
        DCW      0x0000
U1_TXlen
        DCB      0x00,0x00
U1_FIFOBuff
        DCB      0xf0,0xe1
        DCB      0xd2,0xc3,0x05,0x00
        %        40
        DCB      0x00,0x00,0x00,0x00
        %        16
        DCB      0x00

        EXPORT USART1_Send [CODE]
        EXPORT USART1_IRQHandler [CODE]
        EXPORT UART_Init [CODE]
        EXPORT U1_RXfin [DATA,SIZE=1]
        EXPORT U1_TXfin [DATA,SIZE=1]
        EXPORT U1_RxTxLen [DATA,SIZE=2]
        EXPORT U1_FIFOBuff [DATA,SIZE=67]

        IMPORT ||Lib$$Request$$armlib|| [CODE,WEAK]
        IMPORT U1_TXresettime [DATA]
        IMPORT TXtimememb [DATA]
        IMPORT U1_RXresettime [DATA]
        IMPORT RXtimememb [DATA]

        KEEP U1_RXflag
        KEEP Errcount
        KEEP Headcapfail
        KEEP U1_TXlen

        ATTR FILESCOPE
        ATTR SETVALUE Tag_ABI_PCS_wchar_t,2
        ATTR SETVALUE Tag_ABI_enum_size,1
        ATTR SETVALUE Tag_ABI_optimization_goals,5
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
