.model small
.stack 1000
.data
        old dd 0 
.code
.486

 
new_handle proc far      
        push ds si es di dx cx bx ax 
        
        xor ax, ax 
        xor dx, dx
        in  ax, 60h        

        cmp al, 1Eh  
        jne skip
            mov ah, 2
            int 16h
            cmp al, 2
            je allGood
            mov ah, 2
            int 16h
            cmp al, 1
            jne skip
            allGood:
            mov dl, 'B'
            je new_handler
        skip:
        jmp old_handler

        new_handler:         
                push cx
                push ax
                push cx
                xor cx, cx
                mov ah, 05h
                mov cl, dl
                int 16h
                pop cx
                pop ax
                pop cx
            
                jmp exit       
        old_handler: 
                pop ax bx cx dx di es si ds                       
                jmp dword ptr cs:old        
                
        exit:
                xor ax, ax
                mov al, 20h
                out 20h, al 
                pop ax bx cx dx di es si ds 
        iret
new_handle endp
 
 
new_end:
 
start:
        mov ax, @data
        mov ds, ax
        
        cli ;????? ????? IF
        pushf 
        push 0        
        pop ds
        mov eax, ds:[09h*4] 
        mov cs:[old], eax 
        
        mov ax, cs
        shl eax, 16
        mov ax, offset new_handle
        mov ds:[09h*4], eax 
        sti 
        
        xor ax, ax
        mov ah, 31h
        MOV DX, (New_end - @code + 10FH) / 16 
        INT 27H 
end start
