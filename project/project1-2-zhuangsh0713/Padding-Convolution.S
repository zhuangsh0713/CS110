.import input.S

.text
main:

    lw a2, image_length
    lw a3, image_width
    la a4, image

    lw a5, kernel_length
    lw a6, kernel_width
    la a7, kernel

# length of result
#    add a2, a5, a5
#    add a2, a2, t0
#    addi a2, a2, -2

# width of result
#    add a3, a6, a6
#    add a3, a3, t1
#    addi a3, a3, -2

# store in memory
    add s0, x0, x0
    addi sp, sp, -4
    sw ra, 0(sp)

# begin the loop for convolution
    jal ra, 2d_convolution_1


    lw ra, 0(sp)
    addi sp, sp, 4

    li a0 10
    ecall


# loop of x, from 0 to result_width
2d_convolution_1:

# initialize parameter of inner loop
    add s1, x0, x0

# store in memory
    addi sp, sp, -4
    sw ra, 0(sp)

# begin inner loop
    jal ra, 2d_convolution_2

# print a \n
    addi a0, x0, 11
    addi a1, x0, 10
    ecall

    lw ra, 0(sp)
    addi sp, sp, 4

    addi s0, s0, 1

#compare current x with image_width
    blt s0, a3, 2d_convolution_1
    jr ra

# loop of y, from 0 to result_length
2d_convolution_2:

# initialize parameter of inner loop
    add s2, x0, x0

# store ra in memory    
    addi sp, sp, -4
    sw ra, 0(sp)

# initialize element to be printed
    add a1, x0, x0

# begin inner loop
    jal ra, 2d_convolution_3

# print_int
    addi a0, x0, 1
    ecall

# print space
    addi a0, x0, 11
    addi a1, x0, 32
    ecall

    lw ra, 0(sp)
    addi sp, sp, 4

    addi s1, s1, 1

#compare current y with image_length
    blt s1, a2, 2d_convolution_2
    jr ra


2d_convolution_3:

# initialize parameter of inner loop
    add s3, x0, x0

# store ra in memory
    addi sp, sp, -4
    sw ra, 0(sp)

# begin inner loop
    jal ra, 2d_convolution_4

    lw ra, 0(sp)
    addi sp, sp, 4

    addi s2, s2, 1

# compare current i with kernel_width
    blt s2, a6, 2d_convolution_3
    jr ra

2d_convolution_4:
    add t5, x0, x0


    addi s9, x0, 2
# t5 = x + i - (kernel_width - 1)/2
    add t5, s0, s2

# s7 = (kernel_width - 1)/2
    addi s7, a6, -1
    div s7, s7, s9
    sub t5, t5, s7

# t6 = y + j
    add t6, s1, s3
    sub t6, t6, s7

    blt t5, x0, not_satisfy
    blt t6, x0, not_satisfy
    bge t5, a3, not_satisfy
    bge t6, a2, not_satisfy

# every number occupy 4 bytes, s7 = 4(constant)
    addi s7, x0, 4

# t5 *= 4
    mul t5, t5, s7

# (x+i)*length + (y+j)
    mul t0, t5, a2
    mul t6, t6, s7
    add t0, t0, t6

# pointer to image[x+i][y+j]
    add t0, t0, a4

# t5 = i * 4
    mul t5, s2, s7

# i*length + j
    mul t1, t5, a5
    mul t6, s3, s7
    add t1, t1, t6

# pointer to kernel[i][j]
    add t1, t1, a7

# image[x+i][y+j], kernel[i][j]
    lw t2, 0(t0)
    lw t3, 0(t1)

# += image[x+i][y+j] * kernel[i][j]
    mul t4, t2, t3
    add a1, a1, t4
    
    addi s3, s3, 1

# compare current j with kernel_length
    blt s3, a5, 2d_convolution_4
    jr ra



not_satisfy:
    addi s3, s3, 1
    blt s3, a5, 2d_convolution_4
    jr ra