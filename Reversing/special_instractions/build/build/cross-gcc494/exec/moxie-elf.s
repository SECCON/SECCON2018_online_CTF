	.file	"moxie-elf.c"
# GNU C (GCC) version 4.9.4 (moxie-elf)
#	compiled by GNU C version 6.4.0, GMP version 6.1.2, MPFR version 3.1.5, MPC version 1.0.3
# GGC heuristics: --param ggc-min-expand=100 --param ggc-min-heapsize=131072
# options passed:  -nostdinc -D ___FreeBSD___ -D ARCH="moxie-elf"
# moxie-elf.c -auxbase-strip moxie-elf.s -O -Wall -fno-builtin
# -fverbose-asm -fomit-frame-pointer -fno-inline
# options enabled:  -faggressive-loop-optimizations -fauto-inc-dec
# -fbranch-count-reg -fcombine-stack-adjustments -fcommon -fcompare-elim
# -fcprop-registers -fdefer-pop -fdelete-null-pointer-checks
# -fearly-inlining -feliminate-unused-debug-types -fforward-propagate
# -ffunction-cse -fgcse-lm -fgnu-runtime -fgnu-unique
# -fguess-branch-probability -fident -fif-conversion -fif-conversion2
# -finline-atomics -finline-functions-called-once -fipa-profile
# -fipa-pure-const -fipa-reference -fira-hoist-pressure
# -fira-share-save-slots -fira-share-spill-slots -fivopts
# -fkeep-static-consts -fleading-underscore -flifetime-dse -fmath-errno
# -fmerge-constants -fmerge-debug-strings -fmove-loop-invariants
# -fomit-frame-pointer -fpcc-struct-return -fpeephole
# -fprefetch-loop-arrays -fsched-critical-path-heuristic
# -fsched-dep-count-heuristic -fsched-group-heuristic -fsched-interblock
# -fsched-last-insn-heuristic -fsched-rank-heuristic -fsched-spec
# -fsched-spec-insn-heuristic -fsched-stalled-insns-dep -fshow-column
# -fshrink-wrap -fsigned-zeros -fsplit-ivs-in-unroller -fsplit-wide-types
# -fstrict-volatile-bitfields -fsync-libcalls -ftoplevel-reorder
# -ftrapping-math -ftree-bit-ccp -ftree-ccp -ftree-ch -ftree-coalesce-vars
# -ftree-copy-prop -ftree-copyrename -ftree-dce -ftree-dominator-opts
# -ftree-dse -ftree-forwprop -ftree-fre -ftree-loop-if-convert
# -ftree-loop-im -ftree-loop-ivcanon -ftree-loop-optimize
# -ftree-parallelize-loops= -ftree-phiprop -ftree-pta -ftree-reassoc
# -ftree-scev-cprop -ftree-sink -ftree-slsr -ftree-sra -ftree-ter
# -funit-at-a-time -fverbose-asm -fzero-initialized-in-bss -meb

	.text
	.p2align	1
	.global	exit
	.type	exit, @function
exit:
	dec    $sp, 24	#,
	jsra   __exit	#
	.size	exit, .-exit
	.p2align	1
	.global	write1
	.type	write1, @function
write1:
	dec    $sp, 24	#,
	sto.b  16($fp), $r1	# c, c
	mov    $r1, $fp	#,
	inc    $r1, 16	#,
	ldi.l  $r2, 1	#,
	jsra   __write	#
	ret
	.size	write1, .-write1
	.p2align	1
	.global	putchar
	.type	putchar, @function
putchar:
	push   $sp, $r6	#
	dec    $sp, 24	#,
	mov    $r6, $r1	# c, c
	;
	jsra   write1	#
	mov    $r0, $r6	#, c
	mov    $r12, $fp	#,
	dec    $r12, 4	#,
	pop    $r12, $r6	#,
	ret
	.size	putchar, .-putchar
	.p2align	1
	.global	puts
	.type	puts, @function
puts:
	push   $sp, $r6	#
	push   $sp, $r7	#
	push   $sp, $r8	#
	push   $sp, $r9	#
	push   $sp, $r10	#
	dec    $sp, 24	#,
	mov    $r7, $r0	# fd, fd
	mov    $r6, $r1	# str, str
	ld.b   $r1, ($r1)	#, *str_3(D)
	;
	mov    $r2, $r1	#, D.1045
	;
	xor    $r0, $r0	# tmp58
	cmp    $r2, $r0	# D.1045, tmp58
	beq   .L5	#
	ldi.l  $r10, putchar	# tmp64,
	mov    $r8, $r0	# tmp65, tmp58
.L7:
	mov    $r0, $r7	#, fd
	;
	jsr    $r10	# tmp64
	inc    $r6, 1	# str,
	ld.b   $r1, ($r6)	#, MEM[base: str_9, offset: 0B]
	;
	mov    $r0, $r1	#, D.1045
	;
	cmp    $r0, $r8	# D.1045, tmp65
	bne   .L7	#
.L5:
	xor    $r0, $r0	#
	mov    $r12, $fp	#,
	dec    $r12, 20	#,
	pop    $r12, $r10	#,
	pop    $r12, $r9	#,
	pop    $r12, $r8	#,
	pop    $r12, $r7	#,
	pop    $r12, $r6	#,
	ret
	.size	puts, .-puts
	.section	.rodata.str1.4,"aMS",@progbits,1
	.p2align	2
.LC0:
	.string	"0123456789abcdef"
	.text
	.p2align	1
	.global	putxval
	.type	putxval, @function
putxval:
	push   $sp, $r6	#
	push   $sp, $r7	#
	push   $sp, $r8	#
	push   $sp, $r9	#
	push   $sp, $r10	#
	dec    $sp, 44	#,
	xor    $r3, $r3	# tmp35
	sto.b  -21($fp), $r3	# MEM[(char *)&buf + 16B], tmp35
	xor    $r3, $r3	# tmp36
	cmp    $r1, $r3	# value, tmp36
	bne   .L10	#
	cmp    $r2, $r3	# column, tmp37
	bne   .L10	#
	ldi.l  $r2, 1	# column,
.L10:
	mov    $r3, $fp	# ivtmp.24,
	dec	 $r3, --22	# ivtmp.24,
	xor    $r4, $r4	# tmp52
	ldi.b  $r10, 48	# tmp53,
	ldi.l  $r8, .LC0	# tmp54,
	ldi.l  $r7, 15	# tmp55,
	ldi.l  $r6, 4	# tmp56,
	jmpa   .L11	#
.L13:
	mov    $r5, $r1	# D.1054, value
	and    $r5, $r7	# D.1054, tmp55
	mov    $r9, $r8	#, tmp54
	add    $r9, $r5	#, D.1054
	ld.b   $r5, ($r9)	#,
	;
	st.b   ($r3), $r5	# MEM[base: p_3, offset: 0B], tmp43
	lshr   $r1, $r6	# value, tmp56
	cmp    $r2, $r4	# column, tmp52
	beq   .L12	#
.L15:
	dec	 $r2, --1	# column,
.L12:
	dec	 $r3, --1	# ivtmp.24,
.L11:
	cmp    $r1, $r4	# value, tmp52
	bne   .L13	#
	cmp    $r2, $r4	# column, tmp52
	beq   .L17	#
	st.b   ($r3), $r10	# MEM[base: p_3, offset: 0B], tmp53
	jmpa   .L15	#
.L17:
	mov    $r1, $r3	#, p
	inc    $r1, 1	#,
	jsra   puts	#
	xor    $r0, $r0	#
	mov    $r12, $fp	#,
	dec    $r12, 20	#,
	pop    $r12, $r10	#,
	pop    $r12, $r9	#,
	pop    $r12, $r8	#,
	pop    $r12, $r7	#,
	pop    $r12, $r6	#,
	ret
	.size	putxval, .-putxval
	.p2align	1
	.global	set_random_seed
	.type	set_random_seed, @function
set_random_seed:
# 127 "moxie-elf.c" 1
	.byte 0x16
# 0 "" 2
# 128 "moxie-elf.c" 1
	.byte 0x20
# 0 "" 2
	ret
	.size	set_random_seed, .-set_random_seed
	.p2align	1
	.global	get_random_value
	.type	get_random_value, @function
get_random_value:
# 133 "moxie-elf.c" 1
	.byte 0x17
# 0 "" 2
# 134 "moxie-elf.c" 1
	.byte 0x20
# 0 "" 2
	ret
	.size	get_random_value, .-get_random_value
	.p2align	1
	.global	decode
	.type	decode, @function
decode:
	push   $sp, $r6	#
	push   $sp, $r7	#
	push   $sp, $r8	#
	push   $sp, $r9	#
	push   $sp, $r10	#
	push   $sp, $r11	#
	dec    $sp, 24	#,
	mov    $r11, $r0	# str, str
	ld.b   $r2, ($r0)	# *str_5(D), *str_5(D)
	;
	xor    $r0, $r0	# tmp42
	cmp    $r2, $r0	# *str_5(D), tmp42
	beq   .L21	#
	mov    $r8, $r1	# ivtmp.34, key
	mov    $r7, $r11	# D.1066, str
	ldi.l  $r10, get_random_value	# tmp52,
.L22:
	ld.b   $r6, ($r8)	# D.1068, MEM[base: _21, offset: 0B]
	;
	xor    $r0, $r0	#
	jsr    $r10	# tmp52
	xor    $r6, $r0	# D.1068,
	ld.b   $r0, ($r7)	#, MEM[base: _29, offset: 0B]
	;
	xor    $r6, $r0	# tmp47,
	st.b   ($r7), $r6	# MEM[base: _29, offset: 0B], tmp47
	inc    $r7, 1	# D.1066,
	inc    $r8, 1	# ivtmp.34,
	ld.b   $r1, ($r7)	# MEM[base: _6, offset: 0B], MEM[base: _6, offset: 0B]
	;
	xor    $r0, $r0	# tmp50
	cmp    $r1, $r0	# MEM[base: _6, offset: 0B], tmp50
	bne   .L22	#
.L21:
	mov    $r0, $r11	#, str
	mov    $r12, $fp	#,
	dec    $r12, 24	#,
	pop    $r12, $r11	#,
	pop    $r12, $r10	#,
	pop    $r12, $r9	#,
	pop    $r12, $r8	#,
	pop    $r12, $r7	#,
	pop    $r12, $r6	#,
	ret
	.size	decode, .-decode
	.section	.rodata.str1.4
	.p2align	2
.LC1:
	.string	"This program uses special instructions.\n\n"
	.p2align	2
.LC2:
	.string	"SETRSEED: (Opcode:0x16)\n"
	.p2align	2
.LC3:
	.string	"\tRegA -> SEED\n\n"
	.p2align	2
.LC4:
	.string	"GETRAND: (Opcode:0x17)\n"
	.p2align	2
.LC5:
	.string	"\txorshift32(SEED) -> SEED\n"
	.p2align	2
.LC6:
	.string	"\tSEED -> RegA\n\n"
	.p2align	2
.LC7:
	.string	"\n"
	.text
	.p2align	1
	.global	main
	.type	main, @function
main:
	push   $sp, $r6	#
	dec    $sp, 24	#,
	ldi.l  $r0, -1831433054	#,
	jsra   set_random_seed	#
	ldi.l  $r6, puts	# tmp29,
	ldi.l  $r0, 1	#,
	ldi.l  $r1, .LC1	#,
	jsr    $r6	# tmp29
	ldi.l  $r0, 1	#,
	ldi.l  $r1, .LC2	#,
	jsr    $r6	# tmp29
	ldi.l  $r0, 1	#,
	ldi.l  $r1, .LC3	#,
	jsr    $r6	# tmp29
	ldi.l  $r0, 1	#,
	ldi.l  $r1, .LC4	#,
	jsr    $r6	# tmp29
	ldi.l  $r0, 1	#,
	ldi.l  $r1, .LC5	#,
	jsr    $r6	# tmp29
	ldi.l  $r0, 1	#,
	ldi.l  $r1, .LC6	#,
	jsr    $r6	# tmp29
	ldi.l  $r0, flag	#,
	ldi.l  $r1, randval	#,
	jsra   decode	#
	mov    $r1, $r0	# D.1071,
	ldi.l  $r0, 1	#,
	jsr    $r6	# tmp29
	ldi.l  $r0, 1	#,
	ldi.l  $r1, .LC7	#,
	jsr    $r6	# tmp29
	xor    $r0, $r0	#
	jsra   exit	#
	.size	main, .-main
	.global	flag
	.data
	.p2align	2
	.type	flag, @object
	.size	flag, 32
flag:
	.byte	109
	.byte	114
	.byte	-61
	.byte	-30
	.byte	-49
	.byte	-107
	.byte	84
	.byte	-99
	.byte	-74
	.byte	-84
	.byte	3
	.byte	-124
	.byte	-61
	.byte	-62
	.byte	53
	.byte	-109
	.byte	-61
	.byte	-41
	.byte	124
	.byte	-30
	.byte	-35
	.byte	-44
	.byte	-84
	.byte	94
	.byte	-103
	.byte	-55
	.byte	-91
	.byte	52
	.byte	-34
	.byte	6
	.byte	78
	.byte	0
	.global	randval
	.p2align	2
	.type	randval, @object
	.size	randval, 64
randval:
	.byte	61
	.byte	5
	.byte	-36
	.byte	49
	.byte	-47
	.byte	-118
	.byte	-81
	.byte	41
	.byte	-106
	.byte	-6
	.byte	-53
	.byte	27
	.byte	1
	.byte	-20
	.byte	-30
	.byte	-9
	.byte	21
	.byte	112
	.byte	108
	.byte	-12
	.byte	126
	.byte	-95
	.byte	-98
	.byte	14
	.byte	1
	.byte	-7
	.byte	-62
	.byte	76
	.byte	-70
	.byte	-96
	.byte	-95
	.byte	8
	.byte	112
	.byte	36
	.byte	-123
	.byte	-118
	.byte	77
	.byte	45
	.byte	60
	.byte	2
	.byte	-4
	.byte	111
	.byte	32
	.byte	-16
	.byte	-57
	.byte	-83
	.byte	47
	.byte	-105
	.byte	43
	.byte	-52
	.byte	-93
	.byte	52
	.byte	35
	.byte	83
	.byte	-55
	.byte	-73
	.byte	12
	.byte	16
	.byte	108
	.byte	14
	.byte	-6
	.byte	-7
	.byte	-95
	.byte	-102
	.ident	"GCC: (GNU) 4.9.4"
