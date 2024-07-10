.section .text
.globl splash_screen
.type splash_screen, @function
splash_screen:
    li a0,10
	li a1,50
	li a2,155
	li a3,65
	li a4,65535
	addi sp,sp,-4
	sw ra,0(sp)
	# jal LCD_DrawCircle
	jal LCD_DrawLine
	lw ra,0(sp)
	addi sp,sp,4
