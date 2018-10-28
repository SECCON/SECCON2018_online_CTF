/*
 * Use halt handler.
 * See gdb/sim/aarch64/simulator.c:handle_halt()
 */

#define AngelSVC_Reason_Open  0x01
#define AngelSVC_Reason_Close 0x02
#define AngelSVC_Reason_Write 0x05
#define AngelSVC_Reason_Read  0x06

/*
 * See gdb/sim/aarch64/simulator.c:
 * aarch64_decode_and_execute() (case GROUP_BREXSYS_1010:)
 * dexBr() (case BR_REG_110:)
 * dexExcpnGen()
 */
#define HALT_GROUP  (10 << 25) /* GROUP_BREXSYS_1010 */
#define HALT_GROUP2 ( 6 << 29) /* BR_REG_110 */
#define HALT_OPC2   ( 0 <<  2) /* OK */
#define HALT_LL     ( 0 <<  0)
#define HALT_OPC(opc)     ((opc)   << 21) /* 1:BRK, 2:HLT */
#define HALT_IMM16(imm16) ((imm16) << 5)

#define CALL_ExcpnGen(opc, imm16) \
	.long (HALT_GROUP | HALT_GROUP2 | HALT_OPC2 | HALT_LL | \
	HALT_OPC(opc) | HALT_IMM16(imm16))

/* See gdb/sim/aarch64/simulator.c:dexExcpnGen() (opc == 1 && LL == 0) */
#define CALL_BRK CALL_ExcpnGen(1, 0)

/*
 * See gdb/sim/aarch64/simulator.c:dexExcpnGen() (opc == 1 && LL == 0)
 * See gdb/sim/aarch64/simulator.c:handle_halt() (imm16 == 0xf000)
 */
#define CALL_HLT CALL_ExcpnGen(2, 0xf000)

	.section .text

	.globl	_start
	.type	_start, %function
_start:
	ldr	x0, _stack_addr
	mov	sp, x0
	bl	main

	/*
	 * Registers as return value and first argument are same.
	 * Setup of first argument is unneed.
	 */
	/* fall through */

	.globl	__exit
	.type	__exit, %function
__exit:
	CALL_BRK
	ret

	.globl	__read
	.type	__read, %function
__read:
	mov	x8, #AngelSVC_Reason_Read
	CALL_HLT
	ret

	.globl	__write
	.type	__write, %function
__write:
	mov	x8, #AngelSVC_Reason_Write
	CALL_HLT
	ret

	.globl	__open
	.type	__open, %function
__open:
	mov	x8, #AngelSVC_Reason_Open
	CALL_HLT
	ret

	.globl	__close
	.type	__close, %function
__close:
	mov	x8, #AngelSVC_Reason_Close
	CALL_HLT
	ret

	.align 8
_stack_addr:
	.long	_estack
