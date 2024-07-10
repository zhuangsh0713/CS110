#ifndef ASSEMBLER_H
#define ASSEMBLER_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

bool atoi_check(char *str);

bool translate(char* instruction, char **args, FILE *output_file) ;
uint32_t FindType(char* instruction, char **args) ;

uint32_t R_type(uint32_t opcode, uint32_t func3, uint32_t func7, char **args);
uint32_t I_type(uint32_t opcode, uint32_t func3, char **args) ;
uint32_t I_type_move(uint32_t opcode, uint32_t func3, uint32_t func7, char **args) ;
uint32_t S_type(uint32_t opcode, uint32_t func3, char **args) ;
uint32_t B_type(uint32_t opcode, uint32_t func3, char **args) ;
uint32_t U_type(uint32_t opcode, char **args) ;
uint32_t J_type(uint32_t opcode, char **args) ;



/* DO NOT MODIFY GIVEN API*/
int assembler(FILE *input_file, FILE *output_file);

#endif