" Vim syntax file
" Language:	NASM: The Netwide Assembler (v0.97)
" Maintainer:	C. Laurence Gonsalves <clgonsal@kami.com>
" Last Change:	1999 Jun 14
" URL:		http://www.cryogen.com/clgonsal/vim/syntax/nasm.vim
" NASM's home:	http://www.cryogen.com/Nasm

" Remove any old syntax stuff hanging around
syn clear
syn case ignore

syn match nasmPreProc		"^\s*%[^;]*"

syn keyword nasmOperator	SEG WRT INCBIN EQU TIMES FAR SHORT
syn keyword nasmOperator	BYTE WORD DWORD NOSPLIT

syn keyword nasmDirective	BITS SECTION SEGMENT ABSOLUTE ORG EXTERN
syn keyword nasmDirective	STRUC ENDSTRUC COMMON GLOBAL GROUP

syn keyword nasmStorage		DB DW DD DQ DT RESB RESW RESD RESQ REST

syn match nasmLabel		"[?.a-z_][a-z0-9_$?.@#~]*"

syn match nasmRegister		"\<[ABCD][LH]\>"
syn match nasmRegister		"\<E\=\([ABCD]X\|[SD]I\|[BS]P\)\>"
syn match nasmRegister		"\<[C-GS]S\>"

syn match nasmDecNumber		"[0-9]\+"
syn match nasmOctNumber		"[0-7]\+Q"
syn match nasmBinNumber		"[0-1]\+B"
syn match nasmHexNumber		"[0-9][0-9a-fA-F]\+H"
syn match nasmHexNumber		"0[xX][0-9a-fA-F]\+"
syn match nasmHexNumber		"\$[0-9][0-9a-fA-F]*"

syn match nasmComment		";.*$"

syn match nasmString		"'[^']\+'"


" instructions that take cc's
syn match nasmInstruction "\<\(CMOV\|FCMOV\|J\|PMVZB\|SET\)\(A\|AE\|B\|BE\|E\|G\|GE\|L\|LE\|NA\|NAE\|NB\|NBE\|NE\|NG\|NGE\|NL\|NLE\|NO\|NP\|NS\|NZ\|O\|P\|PE\|PO\|S\|Z\)\>"

" other instructions
syn keyword nasmInstruction AAA AAD AAM AAS ADC ADD AND ARPL BOUND BSF BSR
syn keyword nasmInstruction BSWAP BT BTC BTR BTS CALL CBW CDQ CLC CLD CLI
syn keyword nasmInstruction CLTS CMC CMP CMPSB CMPSD CMPSW CMPXCHG
syn keyword nasmInstruction CMPXCHG486 CMPXCHG8B CPUID CWD CWDE DAA DAS DEC
syn keyword nasmInstruction DIV EMMS ENTER F2XM1 FABS FADD FADDP FBLD FBSTP
syn keyword nasmInstruction FCHS FCLEX FCOM FCOMI FCOMIP FCOMP FCOMPP FCOS
syn keyword nasmInstruction FDECSTP FDISI FDIV FDIVP FDIVR FDIVRP FENI
syn keyword nasmInstruction FFREE FIADD FICOM FICOMP FIDIV FIDIVR FILD
syn keyword nasmInstruction FIMUL FINCSTP FINIT FIST FISTP FISUB FLD FLD1
syn keyword nasmInstruction FLDCW FLDENV FLDL2E FLDL2T FLDLG2 FLDLN2 FLDPI
syn keyword nasmInstruction FLDZ FMUL FMULP FNCLEX FNDISI FNENI FNINIT FNOP
syn keyword nasmInstruction FPATAN FPREM FPREM1 FPTAN FRNDINT FRSTOR FSAVE
syn keyword nasmInstruction FSCALE FSETPM FSIN FSINCOS FSQRT FST FSTCW
syn keyword nasmInstruction FSTENV FSTP FSTSW FSUB FSUBP FSUBR FSUBRP FTST
syn keyword nasmInstruction FUCOM FUCOMI FUCOMIP FUCOMP FUCOMPP FXAM FXCH
syn keyword nasmInstruction FXTRACT FYL2X FYL2XP1 HLT IBTS ICEBP IDIV IMUL
syn keyword nasmInstruction IN INC INSB INSD INSW INT INT01 INT1 INT3 INTO
syn keyword nasmInstruction INVD INVLPG IRET IRETD IRETW JCXZ JECXZ JMP
syn keyword nasmInstruction LAHF LAR LDS LEA LEAVE LES LFS LGDT LGS LIDT
syn keyword nasmInstruction LLDT LMSW LOADALL LOADALL286 LODSB LODSD LODSW
syn keyword nasmInstruction LOOP LOOPE LOOPNE LOOPNZ LOOPZ LSL LSS LTR MOV
syn keyword nasmInstruction MOVD MOVQ MOVSB MOVSD MOVSW MOVSX MOVZX MUL NEG
syn keyword nasmInstruction NOP NOT OR OUT OUTSB OUTSD OUTSW PACKSSDW
syn keyword nasmInstruction PACKSSWB PACKUSWB PADDB PADDD PADDSB PADDSIW
syn keyword nasmInstruction PADDSW PADDUSB PADDUSW PADDW PAND PANDN PAVEB
syn keyword nasmInstruction PCMPEQB PCMPEQD PCMPEQW PCMPGTB PCMPGTD PCMPGTW
syn keyword nasmInstruction PDISTIB PMACHRIW PMADDWD PMAGW PMULHRIW PMULHRW
syn keyword nasmInstruction PMULHW PMULLW POP POPA POPAD POPAW POPF POPFD
syn keyword nasmInstruction POPFW POR PSLLD PSLLQ PSLLW PSRAD PSRAW PSRLD
syn keyword nasmInstruction PSRLQ PSRLW PSUBB PSUBD PSUBSB PSUBSIW PSUBSW
syn keyword nasmInstruction PSUBUSB PSUBUSW PSUBW PUNPCKHBW PUNPCKHDQ
syn keyword nasmInstruction PUNPCKHWD PUNPCKLBW PUNPCKLDQ PUNPCKLWD PUSH
syn keyword nasmInstruction PUSHA PUSHAD PUSHAW PUSHF PUSHFD PUSHFW PXOR
syn keyword nasmInstruction RCL RCR RDMSR RDPMC RDTSC RET RETF RETN ROL ROR
syn keyword nasmInstruction RSM SAHF SAL SALC SAR SBB SCASB SCASD SCASW
syn keyword nasmInstruction SGDT SHL SHLD SHR SHRD SIDT SLDT SMI SMSW STC
syn keyword nasmInstruction STD STI STOSB STOSD STOSW STR SUB TEST UMOV
syn keyword nasmInstruction VERR VERW WAIT WBINVD WRMSR XADD XBTS XCHG
syn keyword nasmInstruction XLATB XOR

if !exists("did_nasm_syntax_inits")
    let did_nasm_syntax_inits = 1

    hi link nasmLabel		Label
    hi link nasmComment		Comment
    hi link nasmInstruction	Statement
    hi link nasmDirective	Operator
    hi link nasmOperator	Operator

    hi link nasmString		String

    hi link nasmRegister	Function

    hi link nasmInclude		Include
    hi link nasmCond		PreCondit
    hi link nasmMacro		Macro

    hi link nasmHexNumber	Number
    hi link nasmDecNumber	Number
    hi link nasmOctNumber	Number
    hi link nasmBinNumber	Number

    hi link nasmPreProc		PreProc
endif

let b:current_syntax = "nasm"

