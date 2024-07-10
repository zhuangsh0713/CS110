# Exer 1
- How big is your cache block? How many consecutive accesses fit within a single block?
- How big is your cache? How many jumps do you need to make before you "wrap around?"
- What is your cache's associativity? Where can a particular block fit?
- Have you accessed this piece of data before? If so, is it still in the cache or not?


## Scenario 1:

`ecall: a0 = 9` allocates `a1` bytes ?

Cache Parameters:

- Cache Levels: 1
- Block Size (Bytes): 8
- Number of blocks: 4
- Associativity: 1 (Venus won't let you change this, why?)
    - Direct Mapping
- Cache Size (Bytes): 32 (Why?)
    - Block Size * # blocks = 8*4 = 32
- Placement Policy: Direct Mapping
- Block Replacement Policy: LRU
- Enable current selected level of the cache.

Program Parameters:

    Array Size: 128
    Step Size: 8
    Rep Count: 4
    Option: 0


- 2 word, 8 consecutive access

Answer: 0
1. step size, block size, # sets
   index bit = 3
   8 * 4 = 32 = (100000)_2, 每次得到的都是相同的index, tag不同
2. 0
3. step size = 1

## Scenario 2:

Cache Parameters:

- Cache Levels: 1
- Block Size (Bytes): 16
- Number of blocks: 16
- Associativity: 4
- Cache Size (Bytes): 256
- Placement Policy: N-Way Set Associative
- Block Replacement Policy: LRU
- Enable current selected level of the cache.

Program Parameters:

    Array Size: 256
    Step Size: 2
    Rep Count: 1
    Option: 1


Answer: 0.75
1. `opt = 1`, 对于每个位置两次access, `load`操作一定是hit.

    step size == block size/2, 每两次`store`操作一次miss, 一次access

    array size/step size = 32, 32 * rep = 32, 对数组里的32个位置access

    hit = 16 + 32

    hit rate = $\frac{48}{32*2}$ = 0.75
2. converge to 1, 每次循环访问的数组元素都是相同的，miss只有最开始的16次，不需要重新从内存加载
3. 外层循环固定block, 内层循环rep count, 每个rep count循环offset
```{assembly}
Block loop:


Operation loop:
    #operation on current location 
    j Check2

Offset loop:
    #iterate in one block

Check2:
    blt t1, rep count, Operation loop

```


## Scenario 3:

Cache Parameters:

- Cache Levels: 1
- Block Size (Bytes): 16
- Number of blocks: 16
- Associativity: 4
- Cache Size (Bytes): 256
- Placement Policy: N-Way Set Associative
- Block Replacement Policy: Random
- Enable current selected level of the cache.

Program Parameters:

    Array Size: 256
    Step Size: 8
    Rep Count: 2
    Option: 0

Answer:
1. [0, 0.5] 
    
    0: 每次replacement同一位置，第二次循环时全部miss

    0.5: 第一次循环8个不同位置，第二次全部hit

2. direct mapped: 16 blocks -> index bit = 4
    
    一次循环占用8条cache line，没有replacement，第二次全部hit

    hit rate = 0.5


# Exer 2

- `cat  /proc/cpuinfo` cache alignment = 64 KB?

```{c}
    int block_size = 4096/4 ; # word = 1024
```

1. Why there is a gap between gb_v and gb_h ?
    - in `gb_h`, we get consecutive elements in array, iterating over elements within the block, so the cache line can be reused; `gb_v` get seperate elements, loading from memory to cache each iteration


2. Why the changed execution order will achieve a better performance even if we do more things(transpose)?
    - after transposition, `gb_h` can be applied for new matrix



# Exer 3

change ordering of members in `log_entry`


same ordering in `tranverse`, status, reference_time, src_ip;