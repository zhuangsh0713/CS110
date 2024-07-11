gcc -> assembly -> machine code

.s RISC-V file

**Venus** : RISC-V stimulator
## Exercise 1
1.  - `.data` 	Store subsequent items in the **static segment** at the next available address.  
    - `.text` 	Store subsequent instructions in the **text** segment at the next available address.
    - `.word` 	Store listed values as unaligned 32-bit words. **Data Segment**
2.  - output:34, PC

    - x10 存储系统调用号？
3. - 0x10000010
    - contents of the registers??
4. ？运行过程中修改register

## Exercise 2
1. `addi t0, x0, 0`, `t0` represent k
2. t1, t2
3. - t3数组下标，4的倍数
    - t5 = source[t4]
4.  判断奇偶
    ``` 
	andi s1, t0, 1
    beq s1, x0, multiply
    ```