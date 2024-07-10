# Lab 11

### Exer 1

Try to tell the difference of the following "load" intrisics:

Load 128-bits of integer data from memory into dst. mem_addr

1. `_mm_load_si128`: 
    - must be aligned on a 16-byte boundary or a general-protection exception may be generated.
    - 0x1010, 最低位一定是0
2. `_mm_loadu_si128`:
    - "u": unaligned

3. `_mm_load_pd`
   - 2 packed double-precision (64-bit) floating-point elements
4. `_mm_load1_pd`
   - Load a double-precision (64-bit) floating-point element from memory into both elements of dst.

### Exer 2

`__m128 `: 4 * float 
 `__m128d` : 2 * double

1. `_mm_setzero_ps`

   Return vector of type `__m128` with all elements set to zero.

2.  `_mm_set1_ps`

   Broadcast single-precision (32-bit) floating-point value a to all elements of dst.

3.  `_mm_loadu_ps`

4.  `_mm_add_ps` 

5.  `_mm_mul_ps`

6.  `_mm_storeu_ps`

\# inner loop:

- origin: n\*p\*4

- improve: n*p

### Exer 3

> increment a pointer or index to the next element in an array ([pointer arithmetic](https://en.wikipedia.org/wiki/Pointer_arithmetic)), as well as "end of loop" tests. If an optimizing compiler or assembler is able to pre-calculate offsets to each *individually referenced* array variable, these can be built into the [machine code](https://en.wikipedia.org/wiki/Machine_code) instructions directly, therefore requiring no additional arithmetic operations at run time.

### Exer 4

`-O0`

```assembly
a:
        .zero   4
modify:
        addi    sp,sp,-32
        sw      s0,28(sp)
        addi    s0,sp,32
        sw      a0,-20(s0)
        lui     a5,%hi(a)
        lw      a4,%lo(a)(a5)
        lw      a5,-20(s0)
        add     a4,a4,a5
        lui     a5,%hi(a)
        sw      a4,%lo(a)(a5)
        nop
        lw      s0,28(sp)
        addi    sp,sp,32
        jr      ra
main:
        addi    sp,sp,-32
        sw      s0,28(sp)
        addi    s0,sp,32
        sw      zero,-20(s0)
        j       .L3
.L4:
        lui     a5,%hi(a)
        lw      a5,%lo(a)(a5)
        addi    a4,a5,1
        lui     a5,%hi(a)
        sw      a4,%lo(a)(a5)
        lw      a5,-20(s0)
        addi    a5,a5,1
        sw      a5,-20(s0)
.L3:
        lw      a4,-20(s0)
        li      a5,999
        ble     a4,a5,.L4
        sw      zero,-24(s0)
        j       .L5
.L6:
        lui     a5,%hi(a)
        lw      a4,%lo(a)(a5)
        lw      a5,-24(s0)
        add     a4,a4,a5
        lui     a5,%hi(a)
        sw      a4,%lo(a)(a5)
        lw      a5,-24(s0)
        addi    a5,a5,1
        sw      a5,-24(s0)
.L5:
        lw      a4,-24(s0)
        li      a5,999
        ble     a4,a5,.L6
        lui     a5,%hi(a)
        lw      a5,%lo(a)(a5)
        mv      a0,a5
        lw      s0,28(sp)
        addi    sp,sp,32
        jr      ra
```



`-O3`

```assembly
modify:
        lui     a4,%hi(a)
        lw      a5,%lo(a)(a4)
        add     a5,a5,a0
        sw      a5,%lo(a)(a4)
        ret
main:
        lui     a4,%hi(a)
        lw      a0,%lo(a)(a4)
        li      a5,499712
        addi    a5,a5,788
        add     a0,a0,a5
        sw      a0,%lo(a)(a4)
        ret
a:
        .zero   4
```

