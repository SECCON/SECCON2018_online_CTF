/*
 * Use swi instruction.
 * See gdb/sim/moxie/interp.c:sim_engine_run()
 * (case 0x30:)
 */

#define SYS_exit  1
#define SYS_open  2
#define SYS_close /* not defined */
#define SYS_read  4
#define SYS_write 5

	.section .text

	.globl	_start
	.type	_start, @function
_start:
	ldi.l	$sp, _estack
	jsra	main

	/*
	 * Registers as return value and first argument are same.
	 * Setup of first argument is unneed.
	 */
	/* fall through */

	.globl	__exit
	.type	__exit, @function
__exit:
	mov	$r2, $r0
	swi	SYS_exit
	mov	$r0, $r2
	ret

	.globl	__read
	.type	__read, @function
__read:
	mov	$r4, $r2
	mov	$r3, $r1
	mov	$r2, $r0
	swi	SYS_read
	mov	$r0, $r2
	ret

	.globl	__write
	.type	__write, @function
__write:
	mov	$r4, $r2
	mov	$r3, $r1
	mov	$r2, $r0
	swi	SYS_write
	mov	$r0, $r2
	ret

	.globl	__open
	.type	__open, @function
__open:
	mov	$r4, $r2
	mov	$r3, $r1
	mov	$r2, $r0
	swi	SYS_open
	mov	$r0, $r2
	ret

	.globl	__close
	.type	__close, @function
__close:
	/* not defined */
	ret
