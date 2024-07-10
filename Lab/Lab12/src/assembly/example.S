.section .text
.globl draw
.type draw, @function
draw:
	li a0,40
	li a1,15
	li a2,140
	li a3,65
	li a4,65535
	addi sp,sp,-4
	sw ra,0(sp)
	# jal LCD_DrawCircle
	jal LCD_DrawRectangle
	lw ra,0(sp)
	addi sp,sp,4
