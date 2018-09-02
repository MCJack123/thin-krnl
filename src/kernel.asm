;;kernel.asm
;;Entry point

;nasm directive - 32 bit
bits 32
section .text
        ;multiboot spec
        align 4
        dd 0x1BADB002            ;magic
        dd 0x00                  ;flags
        dd - (0x1BADB002 + 0x00) ;checksum. m+f+c should be zero

global start
extern kmain	        ;kmain is defined in the c file

global usleep
usleep:
    push ebp
    mov ebp,esp
    mov eax,ebp
    mov ah,0x86
    mov edx,eax
    int 0x15
    mov esp,ebp
    pop ebp
    ret

global get_mem_size
get_mem_size:
    mov edx,0xdfffffff ;set edx to 0xdfffff
    mov esi,0x00100000 ;set esi to 0x100000
    mov byte [0x100000],0xab ;set address 0xfffff to 0xab
    call probeRAM ;call probeRAM function
    mov eax,[0x100004]
    shl eax,12
    mov [0x100004],eax ;set address 0xffff8 to eax
    mov [0x100008],esi ;set address 0xffffc to esi
    mov eax,0x100004 ;set eax (return value) to 0xffff8
    ret ;exit
probeRAM: 
    push eax
    push ebx
    push edx
    push ebp 
    mov ebp,esi             ;ebp = starting address
    add esi,0x00000FFF      ;round esi up to block boundary
    and esi, ~0x00000FFF    ;truncate to block boundary
    push esi                ;Save corrected starting address for later
    mov eax, esi            ;eax = corrected starting address
    sub eax, ebp            ;eax = bytes to skip from original starting address, due to rounding
    xor ecx,ecx             ;ecx = number of bytes of RAM found so far (none)
    sub edx,eax             ;edx = number of bytes left to test
    jc .done                ;  all done if nothing left after rounding
    or esi,0x00000FFC       ;esi = address of last uint32_t in first block
    shr edx,12              ;edx = number of blocks to test (rounded down)
    je .done                ; Is there anything left after rounding?
 
.testAddress:
    mov ebx,[0x100004]
    inc ebx
    mov [0x100004],ebx
    mov eax,[esi]           ;eax = original value
    mov ebx,eax             ;ebx = original value
    not eax                 ;eax = reversed value
    mov [esi],eax           ;Modify value at address
    mov [0x100000],ebx         ;Do dummy write (that's guaranteed to be a different value)
    wbinvd                  ;Flush the cache
    mov ebp,[esi]           ;ebp = new value
    mov [esi],ebx           ;Restore the original value (even if it's not RAM, in case it's a memory mapped device or something)
    cmp ebp,eax             ;Was the value changed?
    jne .done               ; no, definitely not RAM -- exit to avoid damage
                            ; yes, assume we've found some RAM
 
    add ecx,0x00001000      ;ecx = new number of bytes of RAM found
    add esi,0x00001000      ;esi = new address to test
    dec edx                 ;edx = new number of blocks remaining
    jne .testAddress        ;more blocks remaining?
                            ;If not, we're done
 
.done:
    pop esi                 ;esi = corrected starting address (rounded up)
    pop ebp
    pop edx
    pop ebx
    pop eax
    ret

start:
  cli 			;block interrupts
  mov esp, stack_space	;set stack pointer
  call kmain
  hlt		 	;halt the CPU

section .bss
resb 8192		;8KB for stack
stack_space: