# Lab 1

### Exercise 1

- Linux-Ubuntu 22.04 installed
- gcc 11.4.0 installed

### Exercise 2

- Gradescope HW1 finished: 1.0/1.0

### Exercise 3

- `cd` change directory
  - `cd ../../` switch to upper directory
  - `cd ~` switch to user home directory
  - `cd -` switch to last-accessed directory

- `ls` list directory contents
  - `ls -a`  显示当前目录中的所有文件和目录，包括隐藏文件
  - `ls -A`  同`ls -a` 但是不列出"."（目前目录）".."（父目录）
  - `ls -l` 以长格式显示当前目录中的文件和目录
  - `ls -lh`  以人类可读的方式显示当前目录中的文件和目录大小
  - `ls -t`  按照修改时间排序显示当前目录中的文件和目录
  - `ls -r`  倒序显示文件和目录
  - `ls -R`  递归显示当前目录中的所有文件和子目录
  - `ls -l /etc/passwd`  显示/etc/passwd文件的详细信息
  - `ls *.txt `   列出所有扩展名为.txt的文件
  - `ls file?.txt`  列出文件名为file?.txt的文件，其中?表示任意一个字符
  - `ls [abc]*.txt`  列出以a、b或c开头、扩展名为.txt的文件

- `pwd`  print work directory

- `mkdir`  make directory
  - `mkdir -p cs110P/lab1/test`  确保目录名称存在，若不存在则新建

- `rm`  remove

  - `rm -i`  删除前逐一询问确认
  - `rm -f`  强制删除（只读）
  - `rm -r`  将目录以及以下档案逐一删除
  - `rm -r *`  删除当前目录下的所有文件及目录

- `mv`  move

  - `mv source_file dest_file`  将源文件名source_file改为目标文件名dest_file
  - `mv source_file dest_directory`  将文件source_file移动到目标目录
  - `mv source_directory dest_directory` 
    - 目录名dest_directory已存在：将source_directory移动到目录名dest_directory中
    - 目录名dest_directory不存在：将source_directory改名为dest_directroy
  - `-b` 当目标文件或目录存在时，在执行覆盖前，会为其创建一个备份
  - `-i` 如果制定移动的源目录或文件与目标的目录或文件同名，则先询问是否覆盖就文件
  - `-f` 如果制定移动的源目录或文件与目标的文件同名，不会询问直接覆盖旧文件
  - `-n` 不要覆盖任何已存在的文件或目录
  - `-u` 当源文件比目标文件新或者目标文件不存在时，才执行移动操作

- `cat`  concatenate 连接文件并打印到标准输出设备上

  - `-n`  由1开始对所有输出的行数编号
  - `-b`(`--number-nonblank`) 和`-n`相似，但是对于空白行不编号
  - `-s`  (`--squeeze-blank`) 如果有连续两行以上的空白行，就替换为一行的空白行
  - `-E`  (`--show-ends`) 在每行结束处显示$
  - `cat -n textfile1 > textfile2`  把textfile1的文档内容加上行号后输入到textfile2这个文档里
  - `cat -b textfile1 textfile2 >> textfile3`  把textfile1和textfile2加上行号（空白行不加）之后将内容附加到textfile3文档里
  - `cat /dev/null > /etc/text.txt`  清空/etc/file.txt文档内容

-  `man`  manual 查看各种命令、函数和配置文件的手册页面

  | Common Options | Meaning                            |
  | -------------- | :--------------------------------- |
  | `-f`           | 显示与指定关键字相关的手册界面     |
  | `-k`           | 搜索手册页中与关键字匹配的条目     |
  | `-a`           | 显示所有匹配的手册页面             |
  | `-w`           | 仅显示手册页的位置，而不显示其内容 |

  | Common Section Numbers | Meaning        |
  | :--------------------: | -------------- |
  |           1            | 用户命令       |
  |           2            | 系统调用       |
  |           3            | C 库函数       |
  |           4            | 设备和特殊文件 |
  |           5            | 文件格式和约定 |
  |           6            | 游戏和演示     |
  |           7            | 杂项           |
  |           8            | 系统管理命令   |

  - `man ls`  查看ls命令的手册页面
  - `man 3 printf`  查看C语言标准库函数printf的手册页面
  - `man -k keyborad`  搜索包含特定关键字的手册页面条目

### Exercise 4

- `man grep` display the manual of `grep`
- `grep [OPTIONs...] PATTERNS [FILEs...]`
  - `-i` 忽略大小写进行匹配
  - `-v` 反向查找，只打印不匹配的行
  - `-n` 显示匹配行的行号
  - `-r` 递归查找子目录中的文件
  - `-c` 只打印匹配的行数
  - `grep hello file.txt` 在文件file.txt中查找字符串"hello"，并打印匹配的行
  - `grep -r -n pattern dir/` 在文件夹dir中递归查找所有文件中匹配正则表达式pattern的行，并打印匹配行所在的文件名和行号

### Exercise 5

- | Common GCC compiler options | Meaning                                                      |
  | --------------------------- | ------------------------------------------------------------ |
  | `-Wpedantic`                | issue all the warnings demanded by strict ISO C and ISO C++  |
  | `-Wall`                     | enables all the warnings about constructions that some users consider questionable, and that are easy to avoid (or modify to prevent the warning), even in conjunction with macros. |
  | `-Wextra`                   | enables some extra warning flags that are not enabled by `-Wall` |
  | `-Wvla`                     | warn if a variable-length array is used in the code          |
  | `-Werror`                   | make all warnings into errors                                |
  | `-std=c11`                  | determine the language standard                              |

https://gcc.gnu.org/onlinedocs/gcc/Warning-Options.html

- struct tm	https://en.cppreference.com/w/c/chrono/tm
- macros	https://gcc.gnu.org/onlinedocs/gcc-7.5.0/cpp/Macros.html#Macros
  - macro arguments	https://gcc.gnu.org/onlinedocs/gcc-7.5.0/cpp/Macro-Arguments.html#Macro-Arguments
  - stringizing	https://gcc.gnu.org/onlinedocs/gcc-7.5.0/cpp/Stringizing.html#Stringizing
- source code:

```c
#include <stdio.h>
#include <stdint.h>
#include <time.h>

#define printSize(type) printf("Size of "#type": %zu\n", sizeof(type))

int main(void) {
    printSize(char);
    printSize(short);
    printSize(short int);   //equal to short, at least 16 bits
    printSize(int);
    printSize(long int);
    printSize(unsigned int);
    printSize(unsigned long);
    printSize(void *);
    printSize(size_t);  //unsigned integer type of the result of sizeof
    printSize(double);
    printSize(int8_t);
    printSize(int16_t);
    printSize(int32_t);
    printSize(int64_t);
    printSize(time_t);
    printSize(clock_t);
    printSize(struct tm);
    printSize(NULL);    // implementation-defined; `0` or `void *`

    return 0;
}

```

- compile commands:
  - default: `gcc main.c -Wpedantic -Wall -Wextra -Wvla -Werror -std=c11 -o main`	`./main`
  
  - 32bit version: `gcc main.c -Wpedantic -Wall -Wextra -Wvla -Werror -std=c11 -m32 -o main_32`  `./main_32`
  
    Output:
  
    ```
    Size of char: 1
    Size of short: 2
    Size of short int: 2
    Size of int: 4
    Size of long int: 4
    Size of unsigned int: 4
    Size of void *: 4
    Size of size_t: 4
    Size of double: 8
    Size of int8_t: 1
    Size of int16_t: 2
    Size of int32_t: 4
    Size of int64_t: 8
    Size of time_t: 4
    Size of clock_t: 4
    Size of struct tm: 44
    Size of NULL: 4
    ```
    
  - 64bit version: `gcc main.c -Wpedantic -Wall -Wextra -Wvla -Werror -std=c11 -m64 -o main_64`  `./main_64`
  
    Output:
  
    ```
    Size of char: 1
    Size of short: 2
    Size of short int: 2
    Size of int: 4
    Size of long int: 8
    Size of unsigned int: 4
    Size of void *: 8
    Size of size_t: 8
    Size of double: 8
    Size of int8_t: 1
    Size of int16_t: 2
    Size of int32_t: 4
    Size of int64_t: 8
    Size of time_t: 8
    Size of clock_t: 8
    Size of struct tm: 56
    Size of NULL: 8
    ```
  
- reason of differences: 

  **32bit** computer & **64bit** computer: the size of register(also called 'word size') indicates the amount of data with which the computer can manipulate at a time 

  - `char`

     `sizeof(char)`, `sizeof(signed char)`, and `sizeof(unsigned char)` always return 1.

  - `short int` = `short`  at least 16	

  - `int`, `unsigned int` at least 16, may relate to the width of the register

  - `long int` may relate to the width of the register

  - `size_t` store the maximum size of a theoretically possible object of any type (including array); aliasing; relate to address bit width; concrete implementation on the device: `unsigned long`

  - `void *` relate to the address bit width(maximum; enough soundness)

  - `double` standard: IEEE-754, which requires 64bits

  - `int8_t`, `int16_t`, `int32_t`, `int64_t` provided only if the implementation directly supports the type

  - `time_t` unspecified by the standard; implementation: `long`

  - `clock_t` implementation-defined range and precision(unspecified); implementation: `long`

  - `struct tm` contains at least 9 `int` members(regulated by the standard); the difference results form memory alignment issue
  
     the concrete implementation: 9 `int`  +  1`long int`  + 1`char *`'
  
     - 32bit computers: `int` - 4 bytes, `long int` - 4 bytes, `char *`  - 4 bytes	 $\longrightarrow 4 * 11 =44$
     - 64bit computers: `int` - 4 bytes, `long int` - 8 bytes, `char *`  - 8 bytes	 $\longrightarrow 4 * 9 + 4 + 8 + 8 =56$
  
  - `NULL` `(void *) 0` *or* `0` or `0L`; implementation: `(void *)0`, whose size is same to `void *`