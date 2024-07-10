.section .data
welcome_message: .asciz "Welcome"
welcome_message1: .asciz "to"
name_message: .asciz "Flappy"
name_message1: .asciz "Bird"
enter_message: .asciz "-Enter-"

.section .text
.globl splash_screen
.type splash_screen, @function
splash_screen:
    addi sp, sp, -12
    sw a0, 0(sp)
    sw a1, 4(sp)
    sw a2, 8(sp)

    li a0, 0x0000
    addi sp,sp,-4
	sw ra,0(sp)
    jal LCD_Clear
    lw ra,0(sp)
	addi sp,sp,4

    li a0, 40
    li a1, 80
    li a2, 5
    ; li a3, 0x7FFF
    lw a3, 0(sp)
	sw ra,0(sp)
    jal LCD_DrawCircle
    lw ra,0(sp)
	addi sp,sp,4

    li a0, 500
    addi sp,sp,-4
	sw ra,0(sp)
    jal delay_1ms
    lw ra,0(sp)
	addi sp,sp,4


    li a0, 40
    li a1, 80
    li a2, 15
    ; li a3, 0x841F
    lw a3, 0(sp)
	sw ra,0(sp)
    jal LCD_DrawCircle
    lw ra,0(sp)
	addi sp,sp,4

    li a0, 500
    addi sp,sp,-4
	sw ra,0(sp)
    jal delay_1ms
    lw ra,0(sp)
	addi sp,sp,4


    li a0, 40
    li a1, 80
    li a2, 25
    ; li a3, 0x001F
    lw a3, 0(sp)
	sw ra,0(sp)
    jal LCD_DrawCircle
    lw ra,0(sp)
	addi sp,sp,4


    li a0, 2000
    addi sp,sp,-4
	sw ra,0(sp)
    jal delay_1ms
    lw ra,0(sp)
	addi sp,sp,4

    li a0, 0x0000
    addi sp,sp,-4
	sw ra,0(sp)
    jal LCD_Clear
    lw ra,0(sp)
	addi sp,sp,4


show_text:    
    li a0, 0x0000
    addi sp,sp,-4
	sw ra,0(sp)
    jal LCD_Clear
    lw ra,0(sp)
	addi sp,sp,4
    li a0, 13
    li a1, 15
    lui a2, %hi(welcome_message)
    addi a2, a2, %lo(welcome_message)
    li a3, 0xFFFF
    addi sp,sp,-4
	sw ra,0(sp)
    jal LCD_ShowString
    lw ra,0(sp)
	addi sp,sp,4

	li a0, 200
    addi sp,sp,-4
	sw ra,0(sp)
    jal delay_1ms
    lw ra,0(sp)
	addi sp,sp,4

    li a0, 30
    li a1, 35
    lui a2, %hi(welcome_message1)
    addi a2, a2, %lo(welcome_message1)
    li a3, 0xFFFF
    addi sp,sp,-4
	sw ra,0(sp)
    jal LCD_ShowString  
    lw ra,0(sp)
	addi sp,sp,4

	li a0, 200
    addi sp,sp,-4
	sw ra,0(sp)
    jal delay_1ms
    lw ra,0(sp)
	addi sp,sp,4

word_loop:
    li a0, 15
    li a1, 55
    lui a2, %hi(name_message)
    addi a2, a2, %lo(name_message)
    li a3, 0x001F
    addi sp,sp,-4
	sw ra,0(sp) 
    jal LCD_ShowString
    lw ra,0(sp)
	addi sp,sp,4

    li a0, 25
    li a1, 75
    lui a2, %hi(name_message1)
    addi a2, a2, %lo(name_message1)
    li a3, 0x001F
    addi sp,sp,-4
	sw ra,0(sp) 
    jal LCD_ShowString
    lw ra,0(sp)
	addi sp,sp,4

	li a0, 200
    addi sp,sp,-4
	sw ra,0(sp)
    jal delay_1ms
    lw ra,0(sp)
	addi sp,sp,4

	li a0, 15
    li a1, 55
    lui a2, %hi(name_message)
    addi a2, a2, %lo(name_message)
    li a3, 0X07FF
    addi sp,sp,-4
	sw ra,0(sp) 
    jal LCD_ShowString
    lw ra,0(sp)
	addi sp,sp,4

    li a0, 25
    li a1, 75
    lui a2, %hi(name_message1)
    addi a2, a2, %lo(name_message1)
    li a3, 0X07FF
    addi sp,sp,-4
	sw ra,0(sp) 
    jal LCD_ShowString
    lw ra,0(sp)
	addi sp,sp,4

	li a0, 200
    addi sp,sp,-4
	sw ra,0(sp)
    jal delay_1ms
    lw ra,0(sp)
	addi sp,sp,4


    li a0, 13
    li a1, 120
    lui a2, %hi(enter_message)
    addi a2, a2, %lo(enter_message)
    li a3, 0x07E0
    addi sp,sp,-4
	sw ra,0(sp) 
    jal LCD_ShowString
    lw ra,0(sp)
	addi sp,sp,4

    addi sp,sp,-4
	sw ra,0(sp) 
    jal get_button
    lw ra,0(sp)
	addi sp,sp,4
    li s0, 1
    beq a0, s0, stop

	j word_loop

stop:
    addi a0, x0, 0x0000
    addi sp,sp,-4
	sw ra,0(sp)
    jal LCD_Clear
    lw ra,0(sp)
	addi sp,sp,4

    // return value
    li a0, 300
    ret
