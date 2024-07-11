.data
n: .word 5  # You can change this variable to move different number of disks
startState: .asciiz "Number of disks: "
totalStep: .asciiz "\nTotal steps used: "

.text
main:
    # Move n disks from peg A to peg C using B as auxiliary
    li a0, 4
    la a1, startState
    ecall
    li a0, 1
    la t0, n
    lw a1, 0(t0)
    ecall
    li a0, 4
    la a1, totalStep
    ecall

    lw a0, 0(t0)
    li a1, 0  # Initial step count is 0
    jal ra, hanoi_tower
    
    # Print the total step used
    li a0, 1
    ecall
    # Exit program
    li a0, 10
    ecall


# Define the recursive function to count total steps for Tower of Hanoi
hanoi_tower:
    # YOUR CODE HERE #
    addi s0, x0, 1
    beq s0, a0, terminate

    addi sp, sp, -8
    sw a0, 0(sp)
    sw ra, 4(sp)

    addi a0, a0, -1
    jal ra, hanoi_tower
    addi a1, a1, 1
    jal ra, hanoi_tower

    lw a0, 0(sp)
    lw ra, 4(sp)    
    addi sp, sp, 8

    jr ra

terminate: 
    addi a1, a1, 1
    ret