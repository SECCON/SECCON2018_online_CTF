format PE console
entry start

include 'win32a.inc'

;=================================================
section '.text' code executable ; writable
;=================================================
start:
;Start Edit Area----------------------------------
push ebp
mov  ebp,esp
push esi

call [GetCommandLine]
mov  [ebp-4], eax
push     eax
push     34 ;"
call test1

push   0x00000040       ;Type
push   caption2          ;lpCaption
push   message2          ;lpText
push   0
call   [MessageBox]

call [ExitProcess]

pop esi
mov esp, ebp
pop ebp
ret

;-------------------------------------
test1:

push ebp
mov  ebp,esp
push esi 
movzx    ecx, BYTE [ebp+0x8]
mov      edx, [ebp+0xc]
movzx    edx, BYTE [edx]
cmp      ecx, edx
jne      exit

mov      ecx,1
mov      edx, [ebp+0xc]
inc      edx
push     edx
push     67 ;C
call test2

pop esi
mov esp, ebp
pop ebp
ret

;-------------------------------------
test2:

push ebp
mov  ebp,esp
push esi 
movzx    ecx, BYTE [ebp+0x8]
mov      edx, [ebp+0xc]
movzx    edx, BYTE [edx]
cmp      ecx, edx
jne      exit

mov      ecx,2
mov      edx, [ebp+0xc]
inc      edx
push     edx
push     58 ;:
call test3

pop esi
mov esp, ebp
pop ebp
ret

;-------------------------------------
test3:

push ebp
mov  ebp,esp
push esi 
movzx    ecx, BYTE [ebp+0x8]
mov      edx, [ebp+0xc]
movzx    edx, BYTE [edx]
cmp      ecx, edx
jne      exit

mov      ecx,3
mov      edx, [ebp+0xc]
inc      edx
push     edx
push     92 ;\
call test4

pop esi
mov esp, ebp
pop ebp
ret

;-------------------------------------
test4:

push ebp
mov  ebp,esp
push esi 
movzx    ecx, BYTE [ebp+0x8]
mov      edx, [ebp+0xc]
movzx    edx, BYTE [edx]
cmp      ecx, edx
jne      exit

mov      ecx,4
mov      edx, [ebp+0xc]
inc      edx
push     edx
push     84 ;T
call test5

pop esi
mov esp, ebp
pop ebp
ret

;-------------------------------------
test5:

push ebp
mov  ebp,esp
push esi 
movzx    ecx, BYTE [ebp+0x8]
mov      edx, [ebp+0xc]
movzx    edx, BYTE [edx]
cmp      ecx, edx
jne      exit

mov      ecx,5
mov      edx, [ebp+0xc]
inc      edx
push     edx
push     101 ;e
call test6

pop esi
mov esp, ebp
pop ebp
ret

;-------------------------------------
test6:

push ebp
mov  ebp,esp
push esi 
movzx    ecx, BYTE [ebp+0x8]
mov      edx, [ebp+0xc]
movzx    edx, BYTE [edx]
cmp      ecx, edx
jne      exit

mov      ecx,6
mov      edx, [ebp+0xc]
inc      edx
push     edx
push     109 ;m
call test7

pop esi
mov esp, ebp
pop ebp
ret

;-------------------------------------
test7:

push ebp
mov  ebp,esp
push esi 
movzx    ecx, BYTE [ebp+0x8]
mov      edx, [ebp+0xc]
movzx    edx, BYTE [edx]
cmp      ecx, edx
jne      exit

mov      ecx,7
mov      edx, [ebp+0xc]
inc      edx
push     edx
push     112 ;p
call test8

pop esi
mov esp, ebp
pop ebp
ret

;-------------------------------------
test8:

push ebp
mov  ebp,esp
push esi 
movzx    ecx, BYTE [ebp+0x8]
mov      edx, [ebp+0xc]
movzx    edx, BYTE [edx]
cmp      ecx, edx
jne      exit

mov      ecx,8
mov      edx, [ebp+0xc]
inc      edx
push     edx
push     92 ;\
call test9

pop esi
mov esp, ebp
pop ebp
ret

;-------------------------------------
test9:

push ebp
mov  ebp,esp
push esi 
movzx    ecx, BYTE [ebp+0x8]
mov      edx, [ebp+0xc]
movzx    edx, BYTE [edx]
cmp      ecx, edx
jne      exit

mov      ecx,9
mov      edx, [ebp+0xc]
inc      edx
push     edx
push     83 ;S
call test10

pop esi
mov esp, ebp
pop ebp
ret

;-------------------------------------
test10:

push ebp
mov  ebp,esp
push esi 
movzx    ecx, BYTE [ebp+0x8]
mov      edx, [ebp+0xc]
movzx    edx, BYTE [edx]
cmp      ecx, edx
jne      exit

mov      ecx,10
mov      edx, [ebp+0xc]
inc      edx
push     edx
push     69 ;E
call test11

pop esi
mov esp, ebp
pop ebp
ret

;-------------------------------------
test11:

push ebp
mov  ebp,esp
push esi 
movzx    ecx, BYTE [ebp+0x8]
mov      edx, [ebp+0xc]
movzx    edx, BYTE [edx]
cmp      ecx, edx
jne      exit

mov      ecx,11
mov      edx, [ebp+0xc]
inc      edx
push     edx
push     67 ;C
call test12

pop esi
mov esp, ebp
pop ebp
ret

;-------------------------------------
test12:

push ebp
mov  ebp,esp
push esi 
movzx    ecx, BYTE [ebp+0x8]
mov      edx, [ebp+0xc]
movzx    edx, BYTE [edx]
cmp      ecx, edx
jne      exit

mov      ecx,12
mov      edx, [ebp+0xc]
inc      edx
push     edx
push     67 ;C
call test13

pop esi
mov esp, ebp
pop ebp
ret

;-------------------------------------
test13:

push ebp
mov  ebp,esp
push esi 
movzx    ecx, BYTE [ebp+0x8]
mov      edx, [ebp+0xc]
movzx    edx, BYTE [edx]
cmp      ecx, edx
jne      exit

mov      ecx,13
mov      edx, [ebp+0xc]
inc      edx
push     edx
push     79 ;O
call test14

pop esi
mov esp, ebp
pop ebp
ret

;-------------------------------------
test14:

push ebp
mov  ebp,esp
push esi 
movzx    ecx, BYTE [ebp+0x8]
mov      edx, [ebp+0xc]
movzx    edx, BYTE [edx]
cmp      ecx, edx
jne      exit

mov      ecx,14
mov      edx, [ebp+0xc]
inc      edx
push     edx
push     78 ;N
call test15

pop esi
mov esp, ebp
pop ebp
ret

;-------------------------------------
test15:

push ebp
mov  ebp,esp
push esi 
movzx    ecx, BYTE [ebp+0x8]
mov      edx, [ebp+0xc]
movzx    edx, BYTE [edx]
cmp      ecx, edx
jne      exit

mov      ecx,15
mov      edx, [ebp+0xc]
inc      edx
push     edx
push     50 ;2
call test16

pop esi
mov esp, ebp
pop ebp
ret

;-------------------------------------
test16:

push ebp
mov  ebp,esp
push esi 
movzx    ecx, BYTE [ebp+0x8]
mov      edx, [ebp+0xc]
movzx    edx, BYTE [edx]
cmp      ecx, edx
jne      exit

mov      ecx,16
mov      edx, [ebp+0xc]
inc      edx
push     edx
push     48 ;0
call test17

pop esi
mov esp, ebp
pop ebp
ret

;-------------------------------------
test17:

push ebp
mov  ebp,esp
push esi 
movzx    ecx, BYTE [ebp+0x8]
mov      edx, [ebp+0xc]
movzx    edx, BYTE [edx]
cmp      ecx, edx
jne      exit

mov      ecx,17
mov      edx, [ebp+0xc]
inc      edx
push     edx
push     49 ;1
call test18

pop esi
mov esp, ebp
pop ebp
ret

;-------------------------------------
test18:

push ebp
mov  ebp,esp
push esi 
movzx    ecx, BYTE [ebp+0x8]
mov      edx, [ebp+0xc]
movzx    edx, BYTE [edx]
cmp      ecx, edx
jne      exit

mov      ecx,18
mov      edx, [ebp+0xc]
inc      edx
push     edx
push     56 ;8
call test19

pop esi
mov esp, ebp
pop ebp
ret

;-------------------------------------
test19:

push ebp
mov  ebp,esp
push esi 
movzx    ecx, BYTE [ebp+0x8]
mov      edx, [ebp+0xc]
movzx    edx, BYTE [edx]
cmp      ecx, edx
jne      exit

mov      ecx,19
mov      edx, [ebp+0xc]
inc      edx
push     edx
push     79 ;O
call test20

pop esi
mov esp, ebp
pop ebp
ret

;-------------------------------------
test20:

push ebp
mov  ebp,esp
push esi 
movzx    ecx, BYTE [ebp+0x8]
mov      edx, [ebp+0xc]
movzx    edx, BYTE [edx]
cmp      ecx, edx
jne      exit

mov      ecx,20
mov      edx, [ebp+0xc]
inc      edx
push     edx
push     110 ;n
call test21

pop esi
mov esp, ebp
pop ebp
ret

;-------------------------------------
test21:

push ebp
mov  ebp,esp
push esi 
movzx    ecx, BYTE [ebp+0x8]
mov      edx, [ebp+0xc]
movzx    edx, BYTE [edx]
cmp      ecx, edx
jne      exit

mov      ecx,21
mov      edx, [ebp+0xc]
inc      edx
push     edx
push     108 ;l
call test22

pop esi
mov esp, ebp
pop ebp
ret

;-------------------------------------
test22:

push ebp
mov  ebp,esp
push esi 
movzx    ecx, BYTE [ebp+0x8]
mov      edx, [ebp+0xc]
movzx    edx, BYTE [edx]
cmp      ecx, edx
jne      exit

mov      ecx,22
mov      edx, [ebp+0xc]
inc      edx
push     edx
push     105 ;i
call test23

pop esi
mov esp, ebp
pop ebp
ret

;-------------------------------------
test23:

push ebp
mov  ebp,esp
push esi 
movzx    ecx, BYTE [ebp+0x8]
mov      edx, [ebp+0xc]
movzx    edx, BYTE [edx]
cmp      ecx, edx
jne      exit

mov      ecx,23
mov      edx, [ebp+0xc]
inc      edx
push     edx
push     110 ;n
call test24

pop esi
mov esp, ebp
pop ebp
ret

;-------------------------------------
test24:

push ebp
mov  ebp,esp
push esi 
movzx    ecx, BYTE [ebp+0x8]
mov      edx, [ebp+0xc]
movzx    edx, BYTE [edx]
cmp      ecx, edx
jne      exit

mov      ecx,24
mov      edx, [ebp+0xc]
inc      edx
push     edx
push     101 ;e
call test25

pop esi
mov esp, ebp
pop ebp
ret

;-------------------------------------
test25:

push ebp
mov  ebp,esp
push esi 
movzx    ecx, BYTE [ebp+0x8]
mov      edx, [ebp+0xc]
movzx    edx, BYTE [edx]
cmp      ecx, edx
jne      exit

mov      ecx,25
mov      edx, [ebp+0xc]
inc      edx
push     edx
push     46 ;.
call test26

pop esi
mov esp, ebp
pop ebp
ret

;-------------------------------------
test26:

push ebp
mov  ebp,esp
push esi 
movzx    ecx, BYTE [ebp+0x8]
mov      edx, [ebp+0xc]
movzx    edx, BYTE [edx]
cmp      ecx, edx
jne      exit

mov      ecx,26
mov      edx, [ebp+0xc]
inc      edx
push     edx
push     101 ;e
call test27

pop esi
mov esp, ebp
pop ebp
ret

;-------------------------------------
test27:

push ebp
mov  ebp,esp
push esi 
movzx    ecx, BYTE [ebp+0x8]
mov      edx, [ebp+0xc]
movzx    edx, BYTE [edx]
cmp      ecx, edx
jne      exit

mov      ecx,27
mov      edx, [ebp+0xc]
inc      edx
push     edx
push     120 ;x
call test28

pop esi
mov esp, ebp
pop ebp
ret

;-------------------------------------
test28:

push ebp
mov  ebp,esp
push esi 
movzx    ecx, BYTE [ebp+0x8]
mov      edx, [ebp+0xc]
movzx    edx, BYTE [edx]
cmp      ecx, edx
jne      exit

mov      ecx,28
mov      edx, [ebp+0xc]
inc      edx
push     edx
push     101 ;e
call test29

pop esi
mov esp, ebp
pop ebp
ret

;-------------------------------------
test29:

push ebp
mov  ebp,esp
push esi 
movzx    ecx, BYTE [ebp+0x8]
mov      edx, [ebp+0xc]
movzx    edx, BYTE [edx]
cmp      ecx, edx
jne      exit

mov      ecx,29
mov      edx, [ebp+0xc]
inc      edx
push     edx
push     34 ;"
call test30

pop esi
mov esp, ebp
pop ebp
ret

;-------------------------------------
test30:

push ebp
mov  ebp,esp
push esi 
movzx    ecx, BYTE [ebp+0x8]
mov      edx, [ebp+0xc]
movzx    edx, BYTE [edx]
cmp      ecx, edx
jne      exit

mov      ecx,30
mov      edx, [ebp+0xc]
inc      edx
push     edx
push     32 ; 
call test31

pop esi
mov esp, ebp
pop ebp
ret

;-------------------------------------
test31:

push ebp
mov  ebp,esp
push esi 
movzx    ecx, BYTE [ebp+0x8]
mov      edx, [ebp+0xc]
movzx    edx, BYTE [edx]
cmp      ecx, edx
jne      exit

mov      ecx,31
mov      edx, [ebp+0xc]
inc      edx
push     edx
push     83 ;S
call test32

pop esi
mov esp, ebp
pop ebp
ret

;-------------------------------------
test32:

push ebp
mov  ebp,esp
push esi 
movzx    ecx, BYTE [ebp+0x8]
mov      edx, [ebp+0xc]
movzx    edx, BYTE [edx]
cmp      ecx, edx
jne      exit

mov      ecx,32
mov      edx, [ebp+0xc]
inc      edx
push     edx
push     69 ;E
call test33

pop esi
mov esp, ebp
pop ebp
ret

;-------------------------------------
test33:

push ebp
mov  ebp,esp
push esi 
movzx    ecx, BYTE [ebp+0x8]
mov      edx, [ebp+0xc]
movzx    edx, BYTE [edx]
cmp      ecx, edx
jne      exit

mov      ecx,33
mov      edx, [ebp+0xc]
inc      edx
push     edx
push     67 ;C
call test34

pop esi
mov esp, ebp
pop ebp
ret

;-------------------------------------
test34:

push ebp
mov  ebp,esp
push esi 
movzx    ecx, BYTE [ebp+0x8]
mov      edx, [ebp+0xc]
movzx    edx, BYTE [edx]
cmp      ecx, edx
jne      exit

mov      ecx,34
mov      edx, [ebp+0xc]
inc      edx
push     edx
push     67 ;C
call test35

pop esi
mov esp, ebp
pop ebp
ret

;-------------------------------------
test35:

push ebp
mov  ebp,esp
push esi 
movzx    ecx, BYTE [ebp+0x8]
mov      edx, [ebp+0xc]
movzx    edx, BYTE [edx]
cmp      ecx, edx
jne      exit

mov      ecx,35
mov      edx, [ebp+0xc]
inc      edx
push     edx
push     79 ;O
call test36

pop esi
mov esp, ebp
pop ebp
ret

;-------------------------------------
test36:

push ebp
mov  ebp,esp
push esi 
movzx    ecx, BYTE [ebp+0x8]
mov      edx, [ebp+0xc]
movzx    edx, BYTE [edx]
cmp      ecx, edx
jne      exit

mov      ecx,36
mov      edx, [ebp+0xc]
inc      edx
push     edx
push     78 ;N
call test37

pop esi
mov esp, ebp
pop ebp
ret

;-------------------------------------
test37:

push ebp
mov  ebp,esp
push esi 
movzx    ecx, BYTE [ebp+0x8]
mov      edx, [ebp+0xc]
movzx    edx, BYTE [edx]
cmp      ecx, edx
jne      exit

mov      ecx,37
mov      edx, [ebp+0xc]
inc      edx
push     edx
push     123 ;{
call test38

pop esi
mov esp, ebp
pop ebp
ret

;-------------------------------------
test38:

push ebp
mov  ebp,esp
push esi 
movzx    ecx, BYTE [ebp+0x8]
mov      edx, [ebp+0xc]
movzx    edx, BYTE [edx]
cmp      ecx, edx
jne      exit

mov      ecx,38
mov      edx, [ebp+0xc]
inc      edx
push     edx
push     82 ;R
call test39

pop esi
mov esp, ebp
pop ebp
ret

;-------------------------------------
test39:

push ebp
mov  ebp,esp
push esi 
movzx    ecx, BYTE [ebp+0x8]
mov      edx, [ebp+0xc]
movzx    edx, BYTE [edx]
cmp      ecx, edx
jne      exit

mov      ecx,39
mov      edx, [ebp+0xc]
inc      edx
push     edx
push     117 ;u
call test40

pop esi
mov esp, ebp
pop ebp
ret

;-------------------------------------
test40:

push ebp
mov  ebp,esp
push esi 
movzx    ecx, BYTE [ebp+0x8]
mov      edx, [ebp+0xc]
movzx    edx, BYTE [edx]
cmp      ecx, edx
jne      exit

mov      ecx,40
mov      edx, [ebp+0xc]
inc      edx
push     edx
push     110 ;n
call test41

pop esi
mov esp, ebp
pop ebp
ret

;-------------------------------------
test41:

push ebp
mov  ebp,esp
push esi 
movzx    ecx, BYTE [ebp+0x8]
mov      edx, [ebp+0xc]
movzx    edx, BYTE [edx]
cmp      ecx, edx
jne      exit

mov      ecx,41
mov      edx, [ebp+0xc]
inc      edx
push     edx
push     110 ;n
call test42

pop esi
mov esp, ebp
pop ebp
ret

;-------------------------------------
test42:

push ebp
mov  ebp,esp
push esi 
movzx    ecx, BYTE [ebp+0x8]
mov      edx, [ebp+0xc]
movzx    edx, BYTE [edx]
cmp      ecx, edx
jne      exit

mov      ecx,42
mov      edx, [ebp+0xc]
inc      edx
push     edx
push     49 ;1
call test43

pop esi
mov esp, ebp
pop ebp
ret

;-------------------------------------
test43:

push ebp
mov  ebp,esp
push esi 
movzx    ecx, BYTE [ebp+0x8]
mov      edx, [ebp+0xc]
movzx    edx, BYTE [edx]
cmp      ecx, edx
jne      exit

mov      ecx,43
mov      edx, [ebp+0xc]
inc      edx
push     edx
push     110 ;n
call test44

pop esi
mov esp, ebp
pop ebp
ret

;-------------------------------------
test44:

push ebp
mov  ebp,esp
push esi 
movzx    ecx, BYTE [ebp+0x8]
mov      edx, [ebp+0xc]
movzx    edx, BYTE [edx]
cmp      ecx, edx
jne      exit

mov      ecx,44
mov      edx, [ebp+0xc]
inc      edx
push     edx
push     54 ;6
call test45

pop esi
mov esp, ebp
pop ebp
ret

;-------------------------------------
test45:

push ebp
mov  ebp,esp
push esi 
movzx    ecx, BYTE [ebp+0x8]
mov      edx, [ebp+0xc]
movzx    edx, BYTE [edx]
cmp      ecx, edx
jne      exit

mov      ecx,45
mov      edx, [ebp+0xc]
inc      edx
push     edx
push     95 ;_
call test46

pop esi
mov esp, ebp
pop ebp
ret

;-------------------------------------
test46:

push ebp
mov  ebp,esp
push esi 
movzx    ecx, BYTE [ebp+0x8]
mov      edx, [ebp+0xc]
movzx    edx, BYTE [edx]
cmp      ecx, edx
jne      exit

mov      ecx,46
mov      edx, [ebp+0xc]
inc      edx
push     edx
push     80 ;P
call test47

pop esi
mov esp, ebp
pop ebp
ret

;-------------------------------------
test47:

push ebp
mov  ebp,esp
push esi 
movzx    ecx, BYTE [ebp+0x8]
mov      edx, [ebp+0xc]
movzx    edx, BYTE [edx]
cmp      ecx, edx
jne      exit

mov      ecx,47
mov      edx, [ebp+0xc]
inc      edx
push     edx
push     52 ;4
call test48

pop esi
mov esp, ebp
pop ebp
ret

;-------------------------------------
test48:

push ebp
mov  ebp,esp
push esi 
movzx    ecx, BYTE [ebp+0x8]
mov      edx, [ebp+0xc]
movzx    edx, BYTE [edx]
cmp      ecx, edx
jne      exit

mov      ecx,48
mov      edx, [ebp+0xc]
inc      edx
push     edx
push     55 ;7
call test49

pop esi
mov esp, ebp
pop ebp
ret

;-------------------------------------
test49:

push ebp
mov  ebp,esp
push esi 
movzx    ecx, BYTE [ebp+0x8]
mov      edx, [ebp+0xc]
movzx    edx, BYTE [edx]
cmp      ecx, edx
jne      exit

mov      ecx,49
mov      edx, [ebp+0xc]
inc      edx
push     edx
push     104 ;h
call test50

pop esi
mov esp, ebp
pop ebp
ret

;-------------------------------------
test50:

push ebp
mov  ebp,esp
push esi 
movzx    ecx, BYTE [ebp+0x8]
mov      edx, [ebp+0xc]
movzx    edx, BYTE [edx]
cmp      ecx, edx
jne      exit

mov      ecx,50
mov      edx, [ebp+0xc]
inc      edx
push     edx
push     125 ;}

pop esi
mov esp, ebp
pop ebp
ret

;-------------------------------------





exit:
push   0x00000040       ;Type
push   caption          ;lpCaption
push   message          ;lpText
push   0
call   [MessageBox]


call [ExitProcess]





;push 0                  ;nShowCmd
;push 0                  ;lpDirectory
;push commandline        ;lpParameters
;push application        ;lpFile
;push verb               ;lpOperation
;push 0
;call    [ShellExecute]
;push    eax
;
;push    60000
;call    [Sleep]
;
;push   0x00000040       ;Type
;push   caption          ;lpCaption
;push   message          ;lpText
;push   0
;call   [MessageBox]



section '.data' data readable

caption         db      'Failed',0
message         db      'The environment is not correct.',0

caption2         db      'Congratz',0
message2         db      'You know the flag!',0


;================================================
section '.import' data readable import
;================================================
library kernel32, 'kernel32.dll',\
        shell32, 'shell32.dll',\
        user32, 'user32.dll'


import  kernel32, ExitProcess, 'ExitProcess',\
        Sleep, 'Sleep',\
        GetCommandLine ,'GetCommandLineA'
import  user32,  MessageBox, 'MessageBoxA'