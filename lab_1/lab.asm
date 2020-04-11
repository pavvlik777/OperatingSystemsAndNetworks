.model small

.stack 100h
.386

.data
    DTA db 128h dup(0)  
    max   db   80      
    len   db   0 
    inputString   db   254, 0, 255 DUP (0) 
    outputString   db   254, 0, 255 DUP (0) 
    handleInput    dw 1 
    handleOutput    dw 1 
    buffer   db   255, 0, 256 DUP (0) 
    bufferEncrypt   db   255, 0, 256 DUP (0) 
    XorKey db 'Astapenko'
    sEnterInput      db   "Enter input file: $" 
    sEnterOutput      db   "Enter output file: $" 
    sInputExists db "Input file doesn't exists$"
    sOutputExists db "Output file exists$"
    sFinish db "Encrypting has been completed$"
    sError db "Error$"
    file_spec db '*.*', 0 
.code

NewLine proc
    push dx
    push ax
    mov dx,13 ; new Line
    mov ah,2
    int 21h  
    mov dx,10
    mov ah,2
    int 21h
    pop ax
    pop dx
    ret
    NewLine endp

EnterInputString   proc  
    push dx
    push ax
    lea   dx, sEnterInput   
    mov   ah, 9 
    int   21h
    mov dx, offset inputString    ;????????? ?????? ?????? ? ??????????
    mov AH, 0ah 
    int 21h
     
    mov al,10
    int 29h
    pop ax
    pop dx
    ret 
EnterInputString   endp  

EnterOutputString   proc  
    push dx
    push ax
    lea   dx, sEnterOutput   
    mov   ah, 9 
    int   21h
    mov dx, offset outputString    ;????????? ?????? ?????? ? ??????????
    mov AH, 0ah 
    int 21h
     
    mov al,10
    int 29h
    pop ax
    pop dx
    ret 
EnterOutputString   endp  

Encrypt proc
    push cx
    push dx
    push bx
    
    lea si, buffer
    lea di, bufferEncrypt
    
    xor cx, cx
    cmp ax, 0
    je encr_skip
    mov dx, ax
    mov bx, offset XorKey
    push ax
    cld
    loop_encr:
    lodsb
    xor al, [bx]
    stosb
    inc cx
    inc bx
    cmp cx, 9
    jne key_skip
    sub bx, 9
    sub cx, 9
    sub dx, 9
    key_skip:
    cmp cx, dx
    jne loop_encr
    
    pop ax
    encr_skip:
    pop bx
    pop dx
    pop cx
    ret
Encrypt endp

PrintBuffer proc
push bx
push ax
push cx
    mov bx, offset buffer
    add bx,ax               ;? AX ?????????? ??????????? ??????
    mov byte ptr [bx],'$'        ;?????????? ??????? 
 
    mov ah,9
    mov dx, offset buffer
    int 21h                 ;????? ??????????? ?????
 
    mov cx,256
    call newLine   
pop cx
pop ax
pop bx
    ret
PrintBuffer endp

error_msg proc  ;TODO
    mov dx,offset sError
    mov ah,9
    int 21h
    call NewLine
    mov ah, 02h
    mov dl, al
    add dl, '0'
    int 21h
    ret
error_msg endp

start:
    mov ax, @Data
    mov ds, ax
    MOV ES, AX 
    
    ;MOV AX, 3
    ;INT 10h
    
    CALL EnterInputString ;???? ????????? ?????
    CALL NewLine
    CALL EnterOutputString ;???? ??????????????? ?????
    CALL NewLine
    
    mov dx,offset DTA
    mov ah,1Ah
    int 21h ; set DTA location

    lea dx, file_spec
    xor cx, cx
    mov ah,4Eh

    int 21h
    jc  quit

check_name:
    xor bx, bx
    cld
    mov ESI, offset inputString+2
    mov EDI, offset DTA + 30
    mov ECX, 6 
    repe cmpsb 
    jne not_equal_1
    
    pop bx
    cmp bx, 1
    je skip_1
    mov bx, 1
    skip_1:
    push bx
    
    not_equal_1:
    cld
    mov ESI, offset outputString+2
    mov EDI, offset DTA + 30
    mov ECX, 6 
    repe cmpsb 
    jne not_equal_2
    
    mov dx,offset sOutputExists
    mov ah,9
    int 21h
    call NewLine
    
    not_equal_2:
next_char:
    lodsb
    test al, al
    jnz next_char

    mov dx, offset file_spec
    xor cx, cx
    mov ah, 4fh
    int 21h
    jnc check_name
    ;after check name
    pop bx
    cmp bx, 1
    je input_exists
    
    mov dx,offset sInputExists
    mov ah,9
    int 21h
    call NewLine
    jmp quit
    
    input_exists: ;;Input file exists
    ;Create output
    xor cx, cx
    lea bx, outputString + 2
    mov cl, outputString + 1
    add bx, cx
    mov byte ptr [bx], '$'
    mov ah,3Ch              
    mov dx, offset outputString + 2
    xor cx,cx               
    int 21h                 
    jnc CreateErrorSkip                  
    call error_msg          
    jmp quit                
CreateErrorSkip:
    mov [handleOutput],ax         ;?????????? ??????????? ?????
    
    xor cx, cx
    lea bx, inputString + 2
    mov cl, inputString + 1
    add bx, cx
    mov byte ptr [bx], '$'
    mov ah, 3dh
    mov al, 0 ; for read mode
    mov dx, offset inputString + 2
    int 21h
    jc error
    mov [handleInput], ax
    
    ;Read input file
    
read_loop:
mov bx, [handleInput]          ;read     
    mov ah, 3Fh              
    mov dx, offset buffer           
    mov cx, 256              
    int 21h                 
    jnc error_skip    
    call error_msg          
    jmp quit 
error_skip:                  
    ;call PrintBuffer
    
    call Encrypt
    
    push ax
    
    mov bx, [handleOutput] ;print
    mov dx, offset bufferEncrypt
    mov cx, ax
    mov ah, 40h
    int 21h
    
    pop ax
    cmp ax, 0
    jne read_loop
    jmp close_files
error:
    mov dx,offset sError
    mov ah,9
    int 21h
    call NewLine
    mov ah, 02h
    mov dl, al
    add dl, '0'
    int 21h
    jmp quit
    
close_files:
    mov ah,3Eh              ;close
    mov bx,[handleInput]         ;
    int 21h                 ;
    jnc continue_close                ;
    call error        
continue_close:
    mov ah,3Eh              ;close
    mov bx,[handleOutput]         ;
    int 21h  
    lea   dx, sFinish   
    mov   ah, 9 
    int   21h    ;
    jnc quit                ;
    call error       
quit:   
    mov ax, 4c00h
    int 21h
end start