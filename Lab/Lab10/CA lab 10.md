## CA lab 10

#### 1

- Venus won't let you change this, why? 

  > direct mapping

- cache size

  > block size * number of blocks

##### 1.1

- What combination of parameters is producing the hit rate you observe? (Hint: Your answer should be the process of your calculation.)

  > hit rate: 0
  >
  > cache_size = 32, step_size_in_bytes = 32, so each memory access is mapped to the first cache block.

- What is our hit rate if we increase Rep Count arbitrarily? Why?

  > No change. Data are overwritten in each loop.

- How could we modify our program parameters to maximize our hit rate?

  > Block size = 32
  >
  > Fully associative

##### 1.2

- What combination of parameters is producing the hit rate you observe? (Hint: Your answer should be the process of your calculation.)

  > hit rate: 0.75
  >
  > block_size = 16, # of sets = 4; step_size_in_bytes = 8, iterations_in_a_loop = 32, ...

- What happens to our hit rate as Rep Count goes to infinity? Why?

  > tend to 1. The whole array(size: 256 bytes) is stored in the cache with no block ever overwritten.

- Suppose we have a program that uses a very large array and during each Rep, we apply a different operator to the elements of our array (e.g. if Rep Count = 1024, we apply 1024 different operations to each of the array elements). How can we restructure our program to achieve a hit rate like that achieved in this scenario? (Assume that the number of operations we apply to each element is very large and that the result for each element can be computed independently of the other elements.) What is this technique called?

  > Blocking. Process the array block by block. Finish processing the current block before proceeding to the next.

##### 1.3

- Run the simulation a few times. Every time, set a different seed value (bottom of the cache window). Note that the hit rate is non-deterministic. What is the range of its hit rate? Why is this the case? ("The cache eviction is random" is not a sufficient answer)

  > 0 ~ 0.5
  >
  > 0: every memory access maps to either the 0th or the 8th block, overwriting the previous one. 
  >
  > 0.5: Least Recently used / any policy that maps an access to an unused block in its set. 
  >
  > > step_size_in_bytes = 32, so it hops back and forth between the 1st and the 3rd set. 
  > >
  > > iterations_in_a_loop = 8, so it exactly fills all of the 1st and the 3rd set.

- Which Cache parameter can you modify in order to get a constant hit rate? Record the parameter and its value (and be prepared to show your TA a few runs of the simulation). How does this parameter allow us to get a constant hit rate? And explain why the constant hit rate value is that value.

  > - block size = 256/128/64/
  >
  >   64 — the total 256 bytes are exactly partitioned into 4 sets, each of 64 bytes.
  >
  > - associativity = 1
  >
  > - any combination such that $\text{block size} \times \#\text{ of sets} = 256$

  > original seed = 3320754448268424113



#### 2

```c
Image apply_gb(Image a, FVec gv) {
    Image b = gb_h(a, gv);
    // Image c = gb_v(b, gv);
    b = transpose(b);
    Image c = gb_h(b, gv);
    c = transpose(c);
    free(b.data);
    return c;
}
```

> row major



#### 3

```c
/*
struct log_entry {
    int src_ip;
    char URL[128];
    long reference_time;
    char bro[128];
    int status;
} logs[NUM_ENTRIES];
 */
struct LogArrays {
    int src_ip[NUM_ENTRIES];
    char URL[NUM_ENTRIES][128];
    long reference_time[NUM_ENTRIES];
    char bro[NUM_ENTRIES][128];
    int status[NUM_ENTRIES];
} logs;

void traverse() {
    for (int i = 0; i < NUM_ENTRIES; i++) {
        if (logs.status[i]) {
            logs.reference_time[i] = 0;
            logs.src_ip[i] = 0;
        }
/*
        if (logs[i].status) {
            logs[i].reference_time = 0;
            logs[i].src_ip = 0;
        }
*/
    }
}

void make_logs() {
    for (int i = 0; i < NUM_ENTRIES; i++) {
        logs.src_ip[i] = i * 123;
        logs.reference_time[i] = i * 123;
        logs.status[i] = 1;
/*
        logs[i].src_ip = i * 123;
        logs[i].reference_time = i * 123;
        logs[i].status = 1;
*/
    }
}
```

> **Contiguous Memory Allocation**: Instead of an array of structs, you can use a separate array for each member of the struct. This approach is known as the Structure of Arrays (SoA) layout, as opposed to the Array of Structures (AoS) layout you're currently using. This can improve spatial locality and reduce the number of cache misses, especially if you're accessing the members in a predictable pattern.