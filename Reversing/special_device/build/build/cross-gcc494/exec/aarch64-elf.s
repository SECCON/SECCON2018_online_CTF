	.cpu generic+fp+simd
	.file	"aarch64-elf.c"
// GNU C (GCC) version 4.9.4 (aarch64-elf)
//	compiled by GNU C version 6.4.0, GMP version 6.1.2, MPFR version 3.1.5, MPC version 1.0.3
// GGC heuristics: --param ggc-min-expand=100 --param ggc-min-heapsize=131072
// options passed:  -nostdinc -D ___FreeBSD___ -D ARCH="aarch64-elf"
// aarch64-elf.c -mlittle-endian -mabi=lp64 -auxbase-strip aarch64-elf.s -O
// -Wall -fno-builtin -fverbose-asm -fomit-frame-pointer -fno-inline
// options enabled:  -faggressive-loop-optimizations -fauto-inc-dec
// -fbranch-count-reg -fcombine-stack-adjustments -fcommon -fcompare-elim
// -fcprop-registers -fdefer-pop -fdelete-null-pointer-checks
// -fdwarf2-cfi-asm -fearly-inlining -feliminate-unused-debug-types
// -fforward-propagate -ffunction-cse -fgcse-lm -fgnu-runtime -fgnu-unique
// -fguess-branch-probability -fident -fif-conversion -fif-conversion2
// -finline-atomics -finline-functions-called-once -fipa-profile
// -fipa-pure-const -fipa-reference -fira-hoist-pressure
// -fira-share-save-slots -fira-share-spill-slots -fivopts
// -fkeep-static-consts -fleading-underscore -flifetime-dse -fmath-errno
// -fmerge-constants -fmerge-debug-strings -fmove-loop-invariants
// -fomit-frame-pointer -fpeephole -fprefetch-loop-arrays
// -freg-struct-return -fsched-critical-path-heuristic
// -fsched-dep-count-heuristic -fsched-group-heuristic -fsched-interblock
// -fsched-last-insn-heuristic -fsched-rank-heuristic -fsched-spec
// -fsched-spec-insn-heuristic -fsched-stalled-insns-dep -fsection-anchors
// -fshow-column -fshrink-wrap -fsigned-zeros -fsplit-ivs-in-unroller
// -fsplit-wide-types -fstrict-volatile-bitfields -fsync-libcalls
// -ftoplevel-reorder -ftrapping-math -ftree-bit-ccp -ftree-ccp -ftree-ch
// -ftree-coalesce-vars -ftree-copy-prop -ftree-copyrename -ftree-cselim
// -ftree-dce -ftree-dominator-opts -ftree-dse -ftree-forwprop -ftree-fre
// -ftree-loop-if-convert -ftree-loop-im -ftree-loop-ivcanon
// -ftree-loop-optimize -ftree-parallelize-loops= -ftree-phiprop -ftree-pta
// -ftree-reassoc -ftree-scev-cprop -ftree-sink -ftree-slsr -ftree-sra
// -ftree-ter -funit-at-a-time -fverbose-asm -fzero-initialized-in-bss
// -mlittle-endian -mlra

	.text
	.align	2
	.global	exit
	.type	exit, %function
exit:
	sub	sp, sp, #16	//,,
	str	x30, [sp]	//,
	bl	__exit	//
	.size	exit, .-exit
	.align	2
	.global	write1
	.type	write1, %function
write1:
	sub	sp, sp, #32	//,,
	str	x30, [sp]	//,
	add	x2, sp, 32	// tmp76,,
	strb	w1, [x2,-1]!	// c, c
	mov	x1, x2	//, tmp76
	mov	w2, 1	//,
	bl	__write	//
	ldr	x30, [sp]	//,
	add	sp, sp, 32	//,,
	ret
	.size	write1, .-write1
	.align	2
	.global	putchar
	.type	putchar, %function
putchar:
	sub	sp, sp, #16	//,,
	stp	x19, x30, [sp]	//,,
	mov	w19, w1	// c, c
	bl	write1	//
	mov	w0, w19	//, c
	ldp	x19, x30, [sp]	//,,
	add	sp, sp, 16	//,,
	ret
	.size	putchar, .-putchar
	.align	2
	.global	puts
	.type	puts, %function
puts:
	sub	sp, sp, #32	//,,
	stp	x19, x20, [sp]	//,,
	str	x30, [sp,16]	//,
	mov	w20, w0	// fd, fd
	mov	x19, x1	// str, str
	ldrb	w1, [x1]	// D.2259, *str_3(D)
	cbz	w1, .L5	// D.2259,
.L7:
	mov	w0, w20	//, fd
	bl	putchar	//
	ldrb	w1, [x19,1]!	// D.2259, MEM[base: str_9, offset: 0B]
	cbnz	w1, .L7	// D.2259,
.L5:
	mov	w0, 0	//,
	ldp	x19, x20, [sp]	//,,
	ldr	x30, [sp,16]	//,
	add	sp, sp, 32	//,,
	ret
	.size	puts, .-puts
	.align	2
	.global	putxval
	.type	putxval, %function
putxval:
	sub	sp, sp, #48	//,,
	str	x30, [sp]	//,
	strb	wzr, [sp,40]	//, MEM[(char *)&buf + 16B]
	cbnz	x1, .L10	// value,
	cmp	w2, wzr	// column,
	cset	w3, eq	// D.2268,
	add	w2, w2, w3	// column, column, D.2268
.L10:
	add	x4, sp, 39	// p,,
	adrp	x6, .LC0	// tmp109,
	add	x6, x6, :lo12:.LC0	// tmp110, tmp109,
	b	.L11	//
.L13:
	and	x5, x1, 15	// D.2269, value,
	ldrb	w5, [x6,x5]	//,* D.2269
	strb	w5, [x4],-1	// tmp97, MEM[base: p_20, offset: 1B]
	lsr	x1, x1, 4	// value, value,
	sub	w2, w2, w3	// column, column, D.2268
.L11:
	cmp	w2, wzr	// column,
	cset	w3, ne	// D.2268,
	cbnz	w3, .L13	// D.2268,
	cbnz	x1, .L13	// value,
	add	x1, x4, 1	//, p,
	bl	puts	//
	mov	w0, 0	//,
	ldr	x30, [sp]	//,
	add	sp, sp, 48	//,,
	ret
	.size	putxval, .-putxval
	.align	2
	.global	get_random_value
	.type	get_random_value, %function
get_random_value:
	sub	sp, sp, #32	//,,
	str	x30, [sp]	//,
	add	x1, sp, 24	//,,
	mov	w2, 8	//,
	bl	__read	//
	ldr	x0, [sp,24]	//, value
	ldr	x30, [sp]	//,
	add	sp, sp, 32	//,,
	ret
	.size	get_random_value, .-get_random_value
	.align	2
	.global	decode
	.type	decode, %function
decode:
	sub	sp, sp, #64	//,,
	stp	x19, x20, [sp]	//,,
	stp	x21, x22, [sp,16]	//,,
	stp	x23, x24, [sp,32]	//,,
	str	x30, [sp,48]	//,
	mov	x22, x0	// str, str
	mov	x23, x1	// key, key
	mov	w24, w2	// fd, fd
	ldrb	w0, [x0]	// *str_5(D), *str_5(D)
	cbz	w0, .L16	// *str_5(D),
	mov	x21, x22	// D.2280, str
	mov	x3, 0	// D.2279,
	mov	w20, w3	// i, D.2279
.L17:
	ldrb	w19, [x23,x3]	// D.2282, *_9
	mov	w0, w24	//, fd
	bl	get_random_value	//
	eor	x0, x0, x19	// D.2282,, D.2282
	ldrb	w3, [x21]	//, *_30
	eor	w3, w0, w3	// tmp95, D.2282, *_30
	strb	w3, [x21]	// tmp95, *_30
	add	w20, w20, 1	// i, i,
	sxtw	x3, w20	// D.2279, i
	add	x21, x22, x3	// D.2280, str, D.2279
	ldrb	w0, [x22,x3]	// *_6, *_6
	cbnz	w0, .L17	// *_6,
.L16:
	mov	x0, x22	//, str
	ldp	x19, x20, [sp]	//,,
	ldp	x21, x22, [sp,16]	//,,
	ldp	x23, x24, [sp,32]	//,,
	ldr	x30, [sp,48]	//,
	add	sp, sp, 64	//,,
	ret
	.size	decode, .-decode
	.align	2
	.global	main
	.type	main, %function
main:
	sub	sp, sp, #48	//,,
	stp	x19, x20, [sp]	//,,
	stp	x21, x30, [sp,16]	//,,
	mov	x0, 31300	// tmp77,
	movk	x0, 0xcbbf, lsl 16	// tmp77,,
	movk	x0, 0x408d, lsl 32	// tmp77,,
	movk	x0, 0x139, lsl 48	// tmp77,,
	add	x20, sp, 48	// tmp80,,
	str	x0, [x20,-8]!	// tmp77, seed
	adrp	x19, .LC1	// tmp79,
	add	x19, x19, :lo12:.LC1	// tmp78, tmp79,
	mov	x0, x19	//, tmp78
	mov	w1, 1	//,
	mov	w2, 0	//,
	bl	__open	//
	mov	w21, w0	// fd,
	mov	x1, x20	//, tmp80
	mov	w2, 8	//,
	bl	__write	//
	mov	w0, w21	//, fd
	bl	__close	//
	mov	x0, x19	//, tmp78
	mov	w1, 0	//,
	mov	w2, w1	//,
	bl	__open	//
	mov	w19, w0	// fd,
	adrp	x1, .LANCHOR0	// tmp84,
	add	x1, x1, :lo12:.LANCHOR0	// tmp83, tmp84,
	mov	x0, x1	//, tmp83
	add	x1, x1, 32	//, tmp83,
	mov	w2, w19	//, fd
	bl	decode	//
	mov	x1, x0	// D.2284,
	mov	w0, 1	//,
	bl	puts	//
	mov	w0, 1	//,
	adrp	x1, .LC2	// tmp90,
	add	x1, x1, :lo12:.LC2	//, tmp90,
	bl	puts	//
	mov	w0, w19	//, fd
	bl	__close	//
	mov	w0, 0	//,
	bl	exit	//
	.size	main, .-main
	.global	flag
	.global	randval
	.data
	.align	3
.LANCHOR0 = . + 0
	.type	flag, %object
	.size	flag, 32
flag:
	.byte	-2
	.byte	117
	.byte	-120
	.byte	-87
	.byte	90
	.byte	-86
	.byte	16
	.byte	82
	.byte	-100
	.byte	106
	.byte	103
	.byte	-12
	.byte	-126
	.byte	-66
	.byte	33
	.byte	86
	.byte	89
	.byte	11
	.byte	-105
	.byte	50
	.byte	33
	.byte	70
	.byte	-109
	.byte	-82
	.byte	64
	.byte	13
	.byte	46
	.byte	31
	.byte	-125
	.byte	67
	.byte	64
	.byte	0
	.type	randval, %object
	.size	randval, 64
randval:
	.byte	29
	.byte	-85
	.byte	27
	.byte	15
	.byte	-89
	.byte	-39
	.byte	26
	.byte	-80
	.byte	97
	.byte	126
	.byte	-74
	.byte	72
	.byte	-92
	.byte	86
	.byte	-49
	.byte	126
	.byte	73
	.byte	5
	.byte	-3
	.byte	5
	.byte	-100
	.byte	-7
	.byte	84
	.byte	69
	.byte	-6
	.byte	36
	.byte	-58
	.byte	29
	.byte	104
	.byte	-14
	.byte	70
	.byte	-50
	.byte	-63
	.byte	-83
	.byte	-85
	.byte	8
	.byte	36
	.byte	-122
	.byte	-100
	.byte	-8
	.byte	88
	.byte	101
	.byte	98
	.byte	-120
	.byte	73
	.byte	34
	.byte	-126
	.byte	17
	.byte	41
	.byte	20
	.byte	99
	.byte	116
	.byte	-82
	.byte	40
	.byte	-50
	.byte	-116
	.byte	121
	.byte	45
	.byte	-85
	.byte	7
	.byte	-69
	.byte	117
	.byte	37
	.byte	-99
	.section	.rodata.str1.8,"aMS",%progbits,1
	.align	3
.LC0:
	.string	"0123456789abcdef"
	.zero	7
.LC1:
	.string	"/dev/xorshift64"
.LC2:
	.string	"\n"
	.ident	"GCC: (GNU) 4.9.4"
