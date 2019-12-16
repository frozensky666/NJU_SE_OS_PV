
; ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
;                               syscall.asm
; ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
;                                                     Forrest Yu, 2005
; ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

%include "sconst.inc"

_NR_get_ticks       equ 0 ; 要跟 global.c 中 sys_call_table 的定义相对应！
_NR_print			equ 1
_NR_color_print		equ 2
INT_VECTOR_SYS_CALL equ 0x90

; 导出符号
global	get_ticks
global	print
global 	color_print

bits 32
[section .text]

; ====================================================================
;                              get_ticks
; ====================================================================
get_ticks:
	mov	eax, _NR_get_ticks
	int	INT_VECTOR_SYS_CALL
	ret

;print
print:
	mov	eax,_NR_print
	mov ebx, dword[esp + 4]
	int	INT_VECTOR_SYS_CALL
	ret

;color_print
color_print:
	mov	eax,_NR_color_print
	mov ebx, dword[esp + 4]
	mov ecx, dword[esp + 8]
	int	INT_VECTOR_SYS_CALL
	ret