
aarch64-elf.x:     file format elf64-littleaarch64


Disassembly of section .text:

0000000000001400 <_start>:
    1400:	58000800 	ldr	x0, 1500 <_stack_addr>
    1404:	9100001f 	mov	sp, x0
    1408:	940000a7 	bl	16a4 <main>

000000000000140c <__exit>:
    140c:	d4200000 	.word	0xd4200000
    1410:	d65f03c0 	ret

0000000000001414 <__read>:
    1414:	d28000c8 	mov	x8, #0x6                   	// #6
    1418:	d45e0000 	.word	0xd45e0000
    141c:	d65f03c0 	ret

0000000000001420 <__write>:
    1420:	d28000a8 	mov	x8, #0x5                   	// #5
    1424:	d45e0000 	.word	0xd45e0000
    1428:	d65f03c0 	ret

000000000000142c <__open>:
    142c:	d2800028 	mov	x8, #0x1                   	// #1
    1430:	d45e0000 	.word	0xd45e0000
    1434:	d65f03c0 	ret

0000000000001438 <__close>:
    1438:	d2800048 	mov	x8, #0x2                   	// #2
    143c:	d45e0000 	.word	0xd45e0000
    1440:	d65f03c0 	ret
    1444:	d503201f 	nop
    1448:	d503201f 	nop
    144c:	d503201f 	nop
    1450:	d503201f 	nop
    1454:	d503201f 	nop
    1458:	d503201f 	nop
    145c:	d503201f 	nop
    1460:	d503201f 	nop
    1464:	d503201f 	nop
    1468:	d503201f 	nop
    146c:	d503201f 	nop
    1470:	d503201f 	nop
    1474:	d503201f 	nop
    1478:	d503201f 	nop
    147c:	d503201f 	nop
    1480:	d503201f 	nop
    1484:	d503201f 	nop
    1488:	d503201f 	nop
    148c:	d503201f 	nop
    1490:	d503201f 	nop
    1494:	d503201f 	nop
    1498:	d503201f 	nop
    149c:	d503201f 	nop
    14a0:	d503201f 	nop
    14a4:	d503201f 	nop
    14a8:	d503201f 	nop
    14ac:	d503201f 	nop
    14b0:	d503201f 	nop
    14b4:	d503201f 	nop
    14b8:	d503201f 	nop
    14bc:	d503201f 	nop
    14c0:	d503201f 	nop
    14c4:	d503201f 	nop
    14c8:	d503201f 	nop
    14cc:	d503201f 	nop
    14d0:	d503201f 	nop
    14d4:	d503201f 	nop
    14d8:	d503201f 	nop
    14dc:	d503201f 	nop
    14e0:	d503201f 	nop
    14e4:	d503201f 	nop
    14e8:	d503201f 	nop
    14ec:	d503201f 	nop
    14f0:	d503201f 	nop
    14f4:	d503201f 	nop
    14f8:	d503201f 	nop
    14fc:	d503201f 	nop

0000000000001500 <_stack_addr>:
    1500:	00001c60 	.word	0x00001c60

0000000000001504 <exit>:
    1504:	d10043ff 	sub	sp, sp, #0x10
    1508:	f90003fe 	str	x30, [sp]
    150c:	97ffffc0 	bl	140c <__exit>

0000000000001510 <write1>:
    1510:	d10083ff 	sub	sp, sp, #0x20
    1514:	f90003fe 	str	x30, [sp]
    1518:	910083e2 	add	x2, sp, #0x20
    151c:	381ffc41 	strb	w1, [x2, #-1]!
    1520:	aa0203e1 	mov	x1, x2
    1524:	52800022 	mov	w2, #0x1                   	// #1
    1528:	97ffffbe 	bl	1420 <__write>
    152c:	f94003fe 	ldr	x30, [sp]
    1530:	910083ff 	add	sp, sp, #0x20
    1534:	d65f03c0 	ret

0000000000001538 <putchar>:
    1538:	d10043ff 	sub	sp, sp, #0x10
    153c:	a9007bf3 	stp	x19, x30, [sp]
    1540:	2a0103f3 	mov	w19, w1
    1544:	97fffff3 	bl	1510 <write1>
    1548:	2a1303e0 	mov	w0, w19
    154c:	a9407bf3 	ldp	x19, x30, [sp]
    1550:	910043ff 	add	sp, sp, #0x10
    1554:	d65f03c0 	ret

0000000000001558 <puts>:
    1558:	d10083ff 	sub	sp, sp, #0x20
    155c:	a90053f3 	stp	x19, x20, [sp]
    1560:	f9000bfe 	str	x30, [sp, #16]
    1564:	2a0003f4 	mov	w20, w0
    1568:	aa0103f3 	mov	x19, x1
    156c:	39400021 	ldrb	w1, [x1]
    1570:	340000a1 	cbz	w1, 1584 <puts+0x2c>
    1574:	2a1403e0 	mov	w0, w20
    1578:	97fffff0 	bl	1538 <putchar>
    157c:	38401e61 	ldrb	w1, [x19, #1]!
    1580:	35ffffa1 	cbnz	w1, 1574 <puts+0x1c>
    1584:	52800000 	mov	w0, #0x0                   	// #0
    1588:	a94053f3 	ldp	x19, x20, [sp]
    158c:	f9400bfe 	ldr	x30, [sp, #16]
    1590:	910083ff 	add	sp, sp, #0x20
    1594:	d65f03c0 	ret

0000000000001598 <putxval>:
    1598:	d100c3ff 	sub	sp, sp, #0x30
    159c:	f90003fe 	str	x30, [sp]
    15a0:	3900a3ff 	strb	wzr, [sp, #40]
    15a4:	b5000081 	cbnz	x1, 15b4 <putxval+0x1c>
    15a8:	6b1f005f 	cmp	w2, wzr
    15ac:	1a9f17e3 	cset	w3, eq  // eq = none
    15b0:	0b030042 	add	w2, w2, w3
    15b4:	91009fe4 	add	x4, sp, #0x27
    15b8:	90000006 	adrp	x6, 1000 <_.tmp+0xfc0>
    15bc:	911d40c6 	add	x6, x6, #0x750
    15c0:	14000006 	b	15d8 <putxval+0x40>
    15c4:	92400c25 	and	x5, x1, #0xf
    15c8:	386568c5 	ldrb	w5, [x6, x5]
    15cc:	381ff485 	strb	w5, [x4], #-1
    15d0:	d344fc21 	lsr	x1, x1, #4
    15d4:	4b030042 	sub	w2, w2, w3
    15d8:	6b1f005f 	cmp	w2, wzr
    15dc:	1a9f07e3 	cset	w3, ne  // ne = any
    15e0:	35ffff23 	cbnz	w3, 15c4 <putxval+0x2c>
    15e4:	b5ffff01 	cbnz	x1, 15c4 <putxval+0x2c>
    15e8:	91000481 	add	x1, x4, #0x1
    15ec:	97ffffdb 	bl	1558 <puts>
    15f0:	52800000 	mov	w0, #0x0                   	// #0
    15f4:	f94003fe 	ldr	x30, [sp]
    15f8:	9100c3ff 	add	sp, sp, #0x30
    15fc:	d65f03c0 	ret

0000000000001600 <get_random_value>:
    1600:	d10083ff 	sub	sp, sp, #0x20
    1604:	f90003fe 	str	x30, [sp]
    1608:	910063e1 	add	x1, sp, #0x18
    160c:	52800102 	mov	w2, #0x8                   	// #8
    1610:	97ffff81 	bl	1414 <__read>
    1614:	f9400fe0 	ldr	x0, [sp, #24]
    1618:	f94003fe 	ldr	x30, [sp]
    161c:	910083ff 	add	sp, sp, #0x20
    1620:	d65f03c0 	ret

0000000000001624 <decode>:
    1624:	d10103ff 	sub	sp, sp, #0x40
    1628:	a90053f3 	stp	x19, x20, [sp]
    162c:	a9015bf5 	stp	x21, x22, [sp, #16]
    1630:	a90263f7 	stp	x23, x24, [sp, #32]
    1634:	f9001bfe 	str	x30, [sp, #48]
    1638:	aa0003f6 	mov	x22, x0
    163c:	aa0103f7 	mov	x23, x1
    1640:	2a0203f8 	mov	w24, w2
    1644:	39400000 	ldrb	w0, [x0]
    1648:	34000200 	cbz	w0, 1688 <decode+0x64>
    164c:	aa1603f5 	mov	x21, x22
    1650:	d2800003 	mov	x3, #0x0                   	// #0
    1654:	2a0303f4 	mov	w20, w3
    1658:	38636af3 	ldrb	w19, [x23, x3]
    165c:	2a1803e0 	mov	w0, w24
    1660:	97ffffe8 	bl	1600 <get_random_value>
    1664:	ca130000 	eor	x0, x0, x19
    1668:	394002a3 	ldrb	w3, [x21]
    166c:	4a030003 	eor	w3, w0, w3
    1670:	390002a3 	strb	w3, [x21]
    1674:	11000694 	add	w20, w20, #0x1
    1678:	93407e83 	sxtw	x3, w20
    167c:	8b0302d5 	add	x21, x22, x3
    1680:	38636ac0 	ldrb	w0, [x22, x3]
    1684:	35fffea0 	cbnz	w0, 1658 <decode+0x34>
    1688:	aa1603e0 	mov	x0, x22
    168c:	a94053f3 	ldp	x19, x20, [sp]
    1690:	a9415bf5 	ldp	x21, x22, [sp, #16]
    1694:	a94263f7 	ldp	x23, x24, [sp, #32]
    1698:	f9401bfe 	ldr	x30, [sp, #48]
    169c:	910103ff 	add	sp, sp, #0x40
    16a0:	d65f03c0 	ret

00000000000016a4 <main>:
    16a4:	d100c3ff 	sub	sp, sp, #0x30
    16a8:	a90053f3 	stp	x19, x20, [sp]
    16ac:	a9017bf5 	stp	x21, x30, [sp, #16]
    16b0:	d28f4880 	mov	x0, #0x7a44                	// #31300
    16b4:	f2b977e0 	movk	x0, #0xcbbf, lsl #16
    16b8:	f2c811a0 	movk	x0, #0x408d, lsl #32
    16bc:	f2e02720 	movk	x0, #0x139, lsl #48
    16c0:	9100c3f4 	add	x20, sp, #0x30
    16c4:	f81f8e80 	str	x0, [x20, #-8]!
    16c8:	90000013 	adrp	x19, 1000 <_.tmp+0xfc0>
    16cc:	911da273 	add	x19, x19, #0x768
    16d0:	aa1303e0 	mov	x0, x19
    16d4:	52800021 	mov	w1, #0x1                   	// #1
    16d8:	52800002 	mov	w2, #0x0                   	// #0
    16dc:	97ffff54 	bl	142c <__open>
    16e0:	2a0003f5 	mov	w21, w0
    16e4:	aa1403e1 	mov	x1, x20
    16e8:	52800102 	mov	w2, #0x8                   	// #8
    16ec:	97ffff4d 	bl	1420 <__write>
    16f0:	2a1503e0 	mov	w0, w21
    16f4:	97ffff51 	bl	1438 <__close>
    16f8:	aa1303e0 	mov	x0, x19
    16fc:	52800001 	mov	w1, #0x0                   	// #0
    1700:	2a0103e2 	mov	w2, w1
    1704:	97ffff4a 	bl	142c <__open>
    1708:	2a0003f3 	mov	w19, w0
    170c:	90000001 	adrp	x1, 1000 <_.tmp+0xfc0>
    1710:	91200021 	add	x1, x1, #0x800
    1714:	aa0103e0 	mov	x0, x1
    1718:	91008021 	add	x1, x1, #0x20
    171c:	2a1303e2 	mov	w2, w19
    1720:	97ffffc1 	bl	1624 <decode>
    1724:	aa0003e1 	mov	x1, x0
    1728:	52800020 	mov	w0, #0x1                   	// #1
    172c:	97ffff8b 	bl	1558 <puts>
    1730:	52800020 	mov	w0, #0x1                   	// #1
    1734:	90000001 	adrp	x1, 1000 <_.tmp+0xfc0>
    1738:	911de021 	add	x1, x1, #0x778
    173c:	97ffff87 	bl	1558 <puts>
    1740:	2a1303e0 	mov	w0, w19
    1744:	97ffff3d 	bl	1438 <__close>
    1748:	52800000 	mov	w0, #0x0                   	// #0
    174c:	97ffff6e 	bl	1504 <exit>
