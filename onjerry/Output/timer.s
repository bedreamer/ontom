; generated by ARM C/C++ Compiler, 4.1 [Build 894]
; commandline ArmCC [--debug -c --asm -o..\Output\timer.o --depend=..\Output\timer.d --cpu=Cortex-M3 --apcs=interwork -O1 -I.\src -I.\inc -I..\CMSIS -I"E:\Keil 4 ARM\ARM\RV31\Inc" -I"E:\Keil 4 ARM\ARM\CMSIS\Include" -I"E:\Keil 4 ARM\ARM\Inc\ST\STM32F10x" -DSTM32F10X_CL --omf_browse=..\Output\timer.crf src\Timer.c]
        THUMB
        REQUIRE8
        PRESERVE8

        AREA ||.text||, CODE, READONLY, ALIGN=2

TIM1_UP_IRQHandler PROC
        PUSH     {r4-r8,lr}
        LDR      r0,|L1.632|
        LDRH     r1,[r0,#0]
        BIC      r1,r1,#1
        STRH     r1,[r0,#0]
        LDR      r4,|L1.636|
        MOVS     r6,#0
        LDRH     r0,[r4,#0xc]  ; cont1
        ADDS     r1,r0,#1
        STRH     r1,[r4,#0xc]
        CMP      r0,#0x1f4
        BCC      |L1.42|
        LDR      r0,|L1.640|
        LDR      r1,[r0,#0xc]
        EOR      r1,r1,#0x2000
        STR      r1,[r0,#0xc]
        STRH     r6,[r4,#0xc]
|L1.42|
        LDRB     r1,[r4,#2]  ; Systimememb
        LDR      r5,|L1.644|
        LSLS     r0,r1,#31
        BEQ      |L1.156|
        LDRH     r0,[r4,#8]  ; ADSMstartcont
        SUBS     r0,r0,#1
        UXTH     r0,r0
        STRH     r0,[r4,#8]
        CMP      r0,#0x28
        BHI      |L1.156|
        CBZ      r0,|L1.70|
        BL       ADS7953_SMstart
        B        |L1.156|
|L1.70|
        LDR      r3,|L1.648|
        LDR      r2,[r4,#0x10]  ; predata
        MOV      r12,#0x3e8
        LDR      r0,[r3,#0]  ; Samp_Databuff
        MOVS     r7,#0x28
        SUB      r8,r2,r0
        CMP      r8,r12
        BCC      |L1.96|
        SUBS     r2,r0,r2
        CMP      r2,r12
        BCS      |L1.140|
|L1.96|
        UDIV     r2,r0,r7
        MLS      r0,r7,r2,r0
        MOVS     r7,#0x14
        UDIV     r0,r0,r7
        ADD      r0,r0,r2
        LDRB     r2,[r5,#1]  ; Samp_ADdata_Stream
        LDR      r7,|L1.652|
        ADD      r2,r7,r2,LSL #4
        STRH     r0,[r2,#4]
        AND      r0,r1,#0xfe
        STRB     r0,[r4,#2]
        STR      r6,[r3,#0]  ; Samp_Databuff
        MOV      r0,#0xffffffff
        STR      r0,[r4,#0x10]  ; predata
        STRB     r6,[r4,#3]
        B        |L1.156|
|L1.140|
        STR      r0,[r4,#0x10]  ; predata
        STRH     r7,[r4,#8]
        STR      r6,[r3,#0]  ; Samp_Databuff
        LDRB     r0,[r4,#3]  ; cont
        ADDS     r0,r0,#1
        STRB     r0,[r4,#3]
        BL       ADS7953_SMstart
|L1.156|
        LDRB     r1,[r4,#2]  ; Systimememb
        LSLS     r0,r1,#30
        BPL      |L1.192|
        LDRH     r0,[r4,#0xa]  ; COMMbreaktime
        SUBS     r2,r0,#1
        STRH     r2,[r4,#0xa]
        CMP      r0,#0
        BNE      |L1.192|
        LDRH     r0,[r5,#4]  ; Samp_ADdata_Stream
        ORR      r0,r0,#1
        STRH     r0,[r5,#4]
        AND      r0,r1,#0xfd
        STRB     r0,[r4,#2]
        MOV      r0,#0x1388
        STRH     r0,[r4,#0xa]
|L1.192|
        POP      {r4-r8,pc}
        ENDP

TIM2_IRQHandler PROC
        PUSH     {r4-r6,lr}
        MOV      r0,#0x40000000
        LDRH     r1,[r0,#0x10]
        BIC      r1,r1,#1
        STRH     r1,[r0,#0x10]
        MOVS     r4,#0
        LDR      r5,|L1.656|
        LDR      r6,|L1.660|
|L1.216|
        LDRB     r0,[r5,r4,LSL #2]
        CMP      r0,#4
        BEQ      |L1.238|
        CMP      r0,#5
        BEQ      |L1.238|
        LDR      r1,|L1.664|
        LDR      r1,[r1,r0,LSL #2]
        MOV      r0,r4
        BLX      r1
|L1.238|
        ADD      r0,r4,r4,LSL #1
        ADD      r1,r6,r0,LSL #3
        LDRB     r0,[r1,#5]
        LSLS     r0,r0,#30
        BEQ      |L1.284|
        LDRB     r0,[r1,#0x11]
        AND      r0,r0,#7
        CMP      r0,#6
        BNE      |L1.284|
        LDRB     r0,[r1,#0x12]
        LSLS     r0,r0,#30
        BMI      |L1.284|
        LDRH     r0,[r1,#0x14]
        SUBS     r2,r0,#1
        STRH     r2,[r1,#0x14]
        CBNZ     r0,|L1.284|
        MOVS     r0,#0
        STRH     r0,[r1,#0x14]
        MOVS     r0,#2
        STRB     r0,[r1,#0x12]
|L1.284|
        ADDS     r4,r4,#1
        UXTB     r4,r4
        CMP      r4,#2
        BCC      |L1.216|
        POP      {r4-r6,pc}
        ENDP

TIM3_IRQHandler PROC
        PUSH     {r4-r7,lr}
        LDR      r0,|L1.668|
        LDRH     r1,[r0,#0]
        BIC      r1,r1,#1
        STRH     r1,[r0,#0]
        LDR      r4,|L1.636|
        MOVS     r6,#0
        LDRB     r1,[r4,#0]  ; RXtimememb
        CBZ      r1,|L1.344|
        MOV      r0,r4
        LDR      r5,|L1.676|
        LDRH     r2,[r0,#4]
        LDR      r7,|L1.672|
        SUBS     r0,r2,#1
        UXTH     r3,r0
        LDRB     r0,[r5,#0]
        MOV      r12,#0xffff
        CMP      r1,#1
        BEQ      |L1.424|
        CMP      r1,#2
        BEQ      |L1.398|
        CMP      r1,#3
        BEQ      |L1.460|
|L1.344|
        LDRB     r0,[r4,#1]  ; TXtimememb
        CMP      r0,#0
        BEQ      |L1.396|
        LDR      r3,|L1.636|
        LDR      r7,|L1.680|
        LDRH     r5,[r3,#6]
        LDRB     r2,[r7,#0]
        SUBS     r3,r5,#1
        AND      r1,r2,#3
        UXTH     r3,r3
        CMP      r0,#1
        BEQ      |L1.490|
        CMP      r0,#2
        BNE      |L1.396|
        CMP      r1,#1
        BNE      |L1.396|
        STRH     r3,[r4,#6]
        CMP      r5,#0
        BNE      |L1.396|
        STRB     r6,[r4,#1]
        ORR      r0,r2,#8
        STRB     r0,[r7,#0]
        MOVS     r0,#0x14
        STRH     r0,[r4,#6]
|L1.396|
        POP      {r4-r7,pc}
|L1.398|
        LSLS     r1,r0,#30
        BNE      |L1.396|
        STRH     r3,[r4,#4]
        CMP      r2,#0
        BNE      |L1.396|
        ORR      r0,r0,#1
        STRB     r0,[r5,#0]
        MOVS     r0,#0xa
        STRH     r0,[r4,#4]
        MOVS     r0,#1
        STRB     r0,[r4,#0]
|L1.422|
        POP      {r4-r7,pc}
|L1.424|
        AND      r1,r0,#3
        CMP      r1,#1
        BNE      |L1.422|
        STRH     r3,[r4,#4]
        CMP      r2,#0
        BNE      |L1.422|
        ORR      r0,r0,#2
        STRB     r0,[r5,#0]
        STRB     r6,[r4,#0]
        STRH     r12,[r4,#4]
        LDRH     r0,[r7,#0]
        BIC      r0,r0,#4
        STRH     r0,[r7,#0]
|L1.458|
        POP      {r4-r7,pc}
|L1.460|
        LSLS     r0,r0,#30
        BNE      |L1.458|
        STRH     r3,[r4,#4]
        CMP      r2,#0
        BNE      |L1.458|
        STRB     r6,[r4,#0]
        MOVS     r0,#2
        STRB     r0,[r5,#0]
        LDRH     r0,[r7,#0]
        ORR      r0,r0,#4
        STRH     r0,[r7,#0]
        STRH     r12,[r4,#4]
|L1.488|
        POP      {r4-r7,pc}
|L1.490|
        CMP      r1,#1
        BNE      |L1.488|
        STRH     r3,[r4,#6]
        CMP      r5,#0
        BNE      |L1.488|
        STRB     r6,[r4,#1]
        ORR      r0,r2,#2
        STRB     r0,[r7,#0]
        MOVS     r0,#0x6e
        STRH     r0,[r4,#6]
        POP      {r4-r7,pc}
        ENDP

Timer_Init PROC
        LDR      r0,|L1.684|
        LDR      r1,[r0,#0x18]
        ORR      r1,r1,#0x800
        STR      r1,[r0,#0x18]
        LDR      r1,[r0,#0x1c]
        ORR      r1,r1,#1
        STR      r1,[r0,#0x1c]
        LDR      r1,[r0,#0x1c]
        ORR      r1,r1,#2
        STR      r1,[r0,#0x1c]
        LDR      r0,|L1.688|
        LDR      r1,[r0,#4]
        ORR      r1,r1,#0x1400
        STR      r1,[r0,#4]
        LDR      r0,|L1.632|
        MOV      r1,#0x2d0
        ADDS     r0,r0,#0x1c
        STRH     r1,[r0,#0]
        LDR      r2,|L1.632|
        MOVS     r0,#0xa
        ADDS     r2,r2,#0x18
        STRH     r0,[r2,#0]
        LDR      r2,|L1.632|
        ADDS     r2,r2,#0x20
        STRH     r0,[r2,#0]
        LSLS     r0,r0,#29
        STRH     r1,[r0,#0x2c]
        MOVS     r1,#0xfa
        STRH     r1,[r0,#0x28]
        LDR      r2,|L1.668|
        MOV      r1,#0x168
        ADDS     r2,r2,#0x1c
        STRH     r1,[r2,#0]
        MOVS     r1,#0x64
        SUBS     r2,r2,#4
        STRH     r1,[r2,#0]
        LDR      r2,|L1.632|
        MOVS     r1,#1
        SUBS     r2,r2,#4
        STRH     r1,[r2,#0]
        LDR      r3,|L1.632|
        MOVS     r2,#0x95
        SUBS     r3,r3,#0x10
        STRH     r2,[r3,#0]
        STRH     r1,[r0,#0xc]
        STRH     r2,[r0,#0]
        LDR      r0,|L1.668|
        SUBS     r0,r0,#4
        STRH     r1,[r0,#0]
        LDR      r0,|L1.668|
        SUBS     r0,r0,#0x10
        STRH     r2,[r0,#0]
        BX       lr
        ENDP

|L1.632|
        DCD      0x40012c10
|L1.636|
        DCD      ||.data||
|L1.640|
        DCD      0x40011000
|L1.644|
        DCD      Samp_ADdata_Stream
|L1.648|
        DCD      Samp_Databuff
|L1.652|
        DCD      Samp_DataArr
|L1.656|
        DCD      BMSCom_NC
|L1.660|
        DCD      BMSCom_MC_Rsp
|L1.664|
        DCD      BMSCom_GBStageTxTimerProc
|L1.668|
        DCD      0x40000410
|L1.672|
        DCD      0x4001380c
|L1.676|
        DCD      U1_RXfin
|L1.680|
        DCD      U1_TXfin
|L1.684|
        DCD      0x40021000
|L1.688|
        DCD      0xe0042000

        AREA ||.arm_vfe_header||, DATA, READONLY, NOALLOC, ALIGN=2

        DCD      0x00000000

        AREA ||.data||, DATA, ALIGN=2

RXtimememb
        DCB      0x00
TXtimememb
        DCB      0x00
Systimememb
        DCB      0x00
cont
        DCB      0x00
U1_RXresettime
        DCW      0xffff
U1_TXresettime
        DCW      0xffff
ADSMstartcont
        DCW      0x0028
COMMbreaktime
        DCW      0x1388
cont1
        DCB      0x00,0x00,0x00,0x00
predata
        DCD      0xffffffff

        EXPORT TIM1_UP_IRQHandler [CODE]
        EXPORT TIM2_IRQHandler [CODE]
        EXPORT TIM3_IRQHandler [CODE]
        EXPORT Timer_Init [CODE]
        EXPORT RXtimememb [DATA,SIZE=1]
        EXPORT TXtimememb [DATA,SIZE=1]
        EXPORT Systimememb [DATA,SIZE=1]
        EXPORT U1_RXresettime [DATA,SIZE=2]
        EXPORT U1_TXresettime [DATA,SIZE=2]
        EXPORT ADSMstartcont [DATA,SIZE=2]
        EXPORT COMMbreaktime [DATA,SIZE=2]

        IMPORT ||Lib$$Request$$armlib|| [CODE,WEAK]
        IMPORT ADS7953_SMstart [CODE]
        IMPORT Samp_ADdata_Stream [DATA]
        IMPORT Samp_Databuff [DATA]
        IMPORT Samp_DataArr [DATA]
        IMPORT BMSCom_NC [DATA]
        IMPORT BMSCom_MC_Rsp [DATA]
        IMPORT BMSCom_GBStageTxTimerProc [DATA]
        IMPORT U1_RXfin [DATA]
        IMPORT U1_TXfin [DATA]

        KEEP cont
        KEEP cont1
        KEEP predata

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
