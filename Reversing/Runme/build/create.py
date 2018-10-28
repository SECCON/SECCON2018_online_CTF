function_start='''
push ebp
mov  ebp,esp
push esi '''

function_end='''
pop esi
mov esp, ebp
pop ebp
ret
'''

answer='"C:\\Temp\\SECCON2018Online.exe" SECCON{Runn1n6_P47h}'
print len(answer)

exit
for x in range(len(answer)):
    print 'test' + str(x) + ':'
    print function_start
    print 'movzx    ecx, BYTE [ebp+0x8]'
    print 'mov      edx, [ebp+0xc]'
    print 'movzx    edx, BYTE [edx]'
    print 'cmp      ecx, edx'
    print 'jne      exit'
    print ''
    print 'mov      ecx,' + str(x)
    print 'mov      edx, [ebp+0xc]'
    print 'inc      edx'
    print 'push     edx'
    print 'push     ' +  str(ord(answer[x:x+1])) + ' ;' + answer[x:x+1]
    print 'call test' + str(x+1)
    print function_end
    print ';-------------------------------------'


