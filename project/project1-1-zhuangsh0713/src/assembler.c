#include "../inc/assembler.h"
#include "../inc/util.h"

// store the name of register
// enum may be a better choice?
const char* reg_name[32][3] = {
  {"x0", "zero"}, {"x1", "ra"}, {"x2", "sp"}, {"x3", "gp"},
  {"x4", "tp"}, {"x5", "t0"}, {"x6", "t1"}, {"x7", "t2"},
  {"x8", "s0", "fp"}, {"x9", "s1"}, {"x10", "a0"}, {"x11", "a1"},
  {"x12", "a2"}, {"x13", "a3"}, {"x14", "a4"}, {"x15", "a5"},
  {"x16", "a6"}, {"x17", "a7"}, {"x18", "s2"}, {"x19", "s3"}, 
  {"x20", "s4"}, {"x21", "s5"}, {"x22", "s6"}, {"x23", "s7"},
  {"x24", "s8"}, {"x25", "s9"}, {"x26", "s10"}, {"x27", "s11"}, 
  {"x28", "t3"}, {"x29", "t4"}, {"x30", "t5"}, {"x31", "t6"}
};

// translate register name into number; if not illegal, return -1
int trans_reg(const char* name){
  for(int i = 0; i <= 31; i++){
    for(int j = 0; j <= 2; j++){
      if(reg_name[i][j] && strcmp(name, reg_name[i][j]) == 0){
        return i ;
      }
    }
  }
  return -1 ;
}

// Bad immediate or offset: atoi return 0 when arg is not a number, check str == "0" to determine whether illegal
bool atoi_check(char *str){
  long long number = atoi(str) ;
  if(number == 0){
    if(strcmp(str, "0")){
      return false ;
    }
  }
  return true ;
}

// translate the pseudo-instructions into real instrcution
// if pseudo, it call Findtype, output the answer and return true, Findtype in assembler would not be called again
bool translate(char* instruction, char **args, FILE *output_file){
  bool pseudo = false ;
  uint32_t code = 0 ;
  if(!strcmp(instruction, "beqz")){
    char *new_args[] = {"beq", args[1], "x0", args[2]} ;
    code = FindType("beq", new_args) ;
    pseudo = true ;
  }
  else if(!strcmp(instruction, "bnez")){
    char *new_args[] = {"bne", args[1], "x0", args[2]} ;
    code = FindType("bne", new_args) ;
    pseudo = true ;
  }
  else if(!strcmp(instruction, "j")){
    char *new_args[] = {"jal", "x0", args[1]} ;
    code = FindType("jal", new_args) ;
    pseudo = true ;
  }
  else if(!strcmp(instruction, "jr")){
    char *new_args[] = {"jalr", "x0", args[1], "0"} ;
    code = FindType("jalr", new_args) ;
    pseudo = true ;
  }
  else if(!strcmp(instruction, "li")){
    // li may have 2 corresponding sentence, output in if
    long long imm = atoi(args[2]) ;
    bool check = atoi_check(args[2]) ;
    long long complment_imm = imm ;

    // li should be translated into only one addi instruction
    if(imm >= -2048 && imm < 2047){
      // rs1 = x0 since there is no lui
      char *new_args_addi[] = {"addi", args[1], "x0", args[2]} ;
      code = FindType("addi", new_args_addi) ;
      if(code && check){
        dump_code(output_file, code) ;
      }
      else{
        dump_error_information(output_file) ;
      } 
    }
    else{
      // 2's complement
      if(imm < 0){
        complment_imm = (1ll << 32) + imm ; 
      }
      long long lui_imm = complment_imm >> 12 ;
      long long addi_imm = ((1 << 12) - 1) & complment_imm ;
      // from 2's complement to origin number
      if(addi_imm > 2047){
        addi_imm -= 4096 ;
      }
      // 
      if(addi_imm < 0){
        lui_imm += 1 ;
      }

      char lui_imm_str[32] = {0}, addi_imm_str[32] = {0} ;

      sprintf(lui_imm_str, "%lld", lui_imm);
      sprintf(addi_imm_str, "%lld", addi_imm);

      char *new_args_lui[] = {"lui", args[1], lui_imm_str} ;
      code = FindType("lui", new_args_lui) ;
      if(code && check){
        dump_code(output_file, code) ;
      }
      else{
        dump_error_information(output_file) ;
      } 
      char *new_args_addi[] = {"addi", args[1], args[1], addi_imm_str} ;
      code = FindType("addi", new_args_addi) ;
      if(code){
        dump_code(output_file, code) ;
      }
      else{
        dump_error_information(output_file) ;
      } 
    }
    return true ;
  }
  else if(!strcmp(instruction, "mv")){
    char *new_args[] = {"addi", args[1], args[2], "0"} ;
    code = FindType("addi", new_args) ;
    pseudo = true ;
  }
  if(pseudo){
    if(code){
      dump_code(output_file, code) ;
    }
    else{
      dump_error_information(output_file) ;
    } 
  }
  return pseudo ;
}

// pass the opcode(can be a default in each function), func3 and func7
uint32_t FindType(char* instruction, char **args) {
  if(!strcmp(instruction, "add")){
    return R_type(0x33, 0x0, 0x00, args);
  }
  else if(!strcmp(instruction, "mul")){
    return R_type(0x33, 0x0, 0x01, args);
  }
  else if(!strcmp(instruction, "sub")){
    return R_type(0x33, 0x0, 0x20, args) ;
  }
  else if(!strcmp(instruction, "sll")){
    return R_type(0x33, 0x1, 0x00, args) ;
  }
  else if(!strcmp(instruction, "mulh")){
    return R_type(0x33, 0x1, 0x01, args) ;
  }
  else if(!strcmp(instruction, "slt")){
    return R_type(0x33, 0x2, 0x00, args) ;
  }
  else if(!strcmp(instruction, "sltu")){
    return R_type(0x33, 0x3, 0x00, args) ;
  }
  else if(!strcmp(instruction, "xor")){
    return R_type(0x33, 0x4, 0x00, args) ;
  }
  else if(!strcmp(instruction, "div")){
    return R_type(0x33, 0x4, 0x01, args) ;
  }
  else if(!strcmp(instruction, "srl")){
    return R_type(0x33, 0x5, 0x00, args) ;
  }
  else if(!strcmp(instruction, "sra")){
    return R_type(0x33, 0x5, 0x20, args) ;
  }
  else if(!strcmp(instruction, "or")){
    return R_type(0x33, 0x6, 0x00, args) ;
  }
  else if(!strcmp(instruction, "rem")){
    return R_type(0x33, 0x6, 0x01, args) ;
  }
  else if(!strcmp(instruction, "and")){
    return R_type(0x33, 0x7, 0x00, args) ;
  } // R-type
  else if(!strcmp(instruction, "lb")){
    return I_type(0x03, 0x0, args) ;
  }
  else if(!strcmp(instruction, "lh")){
    return I_type(0x03, 0x1, args) ;
  }
  else if(!strcmp(instruction, "lw")){
    return I_type(0x03, 0x2, args) ;
  }
  else if(!strcmp(instruction, "lbu")){
    return I_type(0x03, 0x4, args) ;
  }
  else if(!strcmp(instruction, "lhu")){
    return I_type(0x03, 0x5, args) ;
  }
  else if(!strcmp(instruction, "addi")){
    return I_type(0x13, 0x0, args) ;
  }
  else if(!strcmp(instruction, "slti")){
    return I_type(0x13, 0x2, args) ;
  }
  else if(!strcmp(instruction, "sltiu")){
    return I_type(0x13, 0x3, args) ;
  }
  else if(!strcmp(instruction, "xori")){
    return I_type(0x13, 0x4, args) ;
  }
  else if(!strcmp(instruction, "ori")){
    return I_type(0x13, 0x6, args) ;
  }
  else if(!strcmp(instruction, "andi")){
    return I_type(0x13, 0x7, args) ;
  } 
  else if(!strcmp(instruction, "jalr")){
    return I_type(0x67, 0x0, args);
  } // I-type decide by func3
  else if(!strcmp(instruction, "slli")){
    return I_type_move(0x13, 0x1, 0x00, args) ;
  }
  else if(!strcmp(instruction, "srli")){
    return I_type_move(0x13, 0x5, 0x00, args) ;
  }
  else if(!strcmp(instruction, "srai")){
    return I_type_move(0x13, 0x5, 0x20, args) ;
  }
  else if(!strcmp(instruction, "ecall")){
    return I_type_move(0x73, 0x0, 0x000, NULL) ;
  } // I-type decide by func3 and func7
  else if(!strcmp(instruction, "sb")){
    return S_type(0x23, 0x0, args) ;
  }
  else if(!strcmp(instruction, "sh")){
    return S_type(0x23, 0x1, args) ;
  }
  else if(!strcmp(instruction, "sw")){
    return S_type(0x23, 0x2, args) ;
  } // S-type: same as I-type 
  else if(!strcmp(instruction, "beq")){
    return B_type(0x63, 0x0, args) ;
  }
  else if(!strcmp(instruction, "bne")){
    return B_type(0x63, 0x1, args) ;
  }
  else if(!strcmp(instruction, "blt")){
    return B_type(0x63, 0x4, args) ;
  }
  else if(!strcmp(instruction, "bge")){
    return B_type(0x63, 0x5, args) ;
  }
  else if(!strcmp(instruction, "bltu")){
    return B_type(0x63, 0x6, args) ;
  }
  else if(!strcmp(instruction, "bgeu")){
    return B_type(0x63, 0x7, args) ;
  } // B-type: same
  else if(!strcmp(instruction, "auipc")){
    return U_type(0x17, args) ;
  }
  else if(!strcmp(instruction, "lui")){
    return U_type(0x37, args) ;
  } // U_type: no func3
  else if(!strcmp(instruction, "jal")){
    return J_type(0x6f, args) ;
  } // J-type
  // Non-existent instruction will return 0 as an error
  return 0 ;
}

uint32_t R_type(uint32_t opcode, uint32_t func3, uint32_t func7, char **args){
  uint32_t machine_code = opcode ;
  machine_code += func3 << 12 ;
  machine_code += func7 << 25 ;
  int rd = trans_reg(args[1]), rs1 = trans_reg(args[2]), rs2 = trans_reg(args[3]) ;
  if(rd == -1||rs1 == -1||rs2 == -1){
    return 0 ;
  }
  machine_code += rd << 7 ;
  machine_code += rs1 << 15 ;
  machine_code += rs2 << 20 ;

  return machine_code ;
}

uint32_t I_type(uint32_t opcode, uint32_t func3, char **args){
  uint32_t machine_code = opcode ;
  int rd = trans_reg(args[1]), rs1 = trans_reg(args[2]), imm = atoi(args[3]);
  bool check = atoi_check(args[3]) ;
  // order of args differs in 0x03 and 0x13
  if(opcode == 0x03){
    rs1 = trans_reg(args[3]) ;
    imm = atoi(args[2]) ;
    check = atoi_check(args[2]) ;
  }

  if(rd == -1||rs1 == -1||(!check)||imm < -2048||imm > 2047){ 
    return 0 ;
  } 
  machine_code += rd << 7 ;
  machine_code += func3 << 12 ;
  machine_code += rs1 << 15 ;

  // imm store in 2's complement
  if(imm < 0){
    machine_code += ((1u << 12) + imm) << 20 ;
  }
  else{
    machine_code += imm << 20 ;
  }
  return machine_code ;
}

uint32_t I_type_move(uint32_t opcode, uint32_t func3, uint32_t func7, char **args){
  uint32_t machine_code = opcode ;
  int rd = 0, rs1 = 0, imm = 0 ;
  bool check = true ;
  // remove if, it will cause segmentation fault, why?
  if(args){
    rd = trans_reg(args[1]), rs1 = trans_reg(args[2]), imm = atoi(args[3]) ;
    check = atoi_check(args[3]) ;
  }
  // int rd = trans_reg(args[1]), rs1 = trans_reg(args[2]), imm = atoi(args[3]) ;
  if(rd == -1||rs1 == -1||(!check)||imm < 0||imm > 31){
    return 0 ;
  }
  machine_code += rd << 7 ;
  machine_code += func3 << 12 ;
  machine_code += rs1 << 15 ;
  machine_code += imm << 20 ;
  machine_code += func7 << 25 ;
  return machine_code ;
}

uint32_t S_type(uint32_t opcode, uint32_t func3, char **args){
  uint32_t machine_code = opcode ;
  int rs2 = trans_reg(args[1]), rs1 = trans_reg(args[3]), imm = atoi(args[2]) ;
  bool check = atoi_check(args[2]) ;

  if(rs2 == -1||rs1 == -1||(!check)||imm < -2048||imm > 2047){
    return 0 ;
  } 
  // cope with 2's complement
  if(imm < 0){
    imm = (1u << 12) + imm ;
  }
  // split imm into [0:4] and [5:11]
  uint32_t lower_5_bit = ((1u << 5) - 1) & imm ;
  uint32_t higher_7_bit = imm >> 5; 
  machine_code += lower_5_bit << 7;
  machine_code += func3 << 12 ;
  machine_code += rs1 << 15 ;
  machine_code += rs2 << 20;
  machine_code += higher_7_bit << 25 ; // 12-bit, only move left by 20 bit
  return machine_code ;
}


uint32_t B_type(uint32_t opcode, uint32_t func3, char **args){
  uint32_t machine_code = opcode ;
  int rs1 = trans_reg(args[1]), rs2 = trans_reg(args[2]), imm = atoi(args[3]) ;
  bool check = atoi_check(args[3]) ;  
  if(rs1 == -1||rs2 == -1||(!check)||imm < -4096||imm > 4095){
    return 0 ;
  } 
  if(imm < 0){
    imm = (1u << 13) + imm ;
  }
  // split into [1:4], [5:10], [11], [12]
  uint32_t lower_4_bit = ((1u << 4) - 1) & (imm >> 1);
  uint32_t middle_5to10 = ((1u << 6) - 1) & (imm >> 5) ;
  uint32_t middle_11 = 1 & (imm >> 11) ;
  uint32_t higher_12 = imm >> 12 ;

  machine_code += middle_11 << 7 ;
  machine_code += lower_4_bit << 8 ;
  machine_code += func3 << 12 ;
  machine_code += rs1 << 15 ;
  machine_code += rs2 << 20 ;
  machine_code += middle_5to10 << 25 ;
  machine_code += higher_12 << 31 ;

  return machine_code ;
}


uint32_t U_type(uint32_t opcode, char **args){
  uint32_t machine_code = opcode ;
  int rd = trans_reg(args[1]);
  long long imm = atoi(args[2]) ;
  bool check = atoi_check(args[2]) ;

  if(rd == -1||(!check)||imm < 0||imm > 1048575){
    return 0 ;
  }

  // store [0:19] of imm to higher 2o bit of machine code, why [20:31] in slides ? 
  machine_code += rd << 7 ;
  machine_code += (((1 << 20) - 1) & imm) << 12 ;
  return machine_code ;
}


uint32_t J_type(uint32_t opcode, char **args){
  uint32_t machine_code = opcode ;
  int rd = trans_reg(args[1]);
  long long imm = atoi(args[2]) ;
  bool check = atoi_check(args[2]) ;

  if(rd == -1||(!check)||imm < -(1ll << 20)||imm > (1ll << 20) - 1){
    return 0 ;
  }
  if(imm < 0){
    imm = (1ll << 21) + imm ;
  }
  // do not store imm[0] 
  uint32_t lower_1to10 = ((1u << 10) - 1) & (imm >> 1);
  uint32_t middle_11 = 1 & (imm >> 11) ;
  uint32_t middle_12to19 = ((1u << 8) - 1) & (imm >> 12) ;
  uint32_t higher_20 = imm >> 20 ;

  machine_code += rd << 7 ;
  machine_code += middle_12to19 << 12 ;
  machine_code += middle_11 << 20 ;
  machine_code += lower_1to10 << 21 ;
  machine_code += higher_20 << 31 ;

  return machine_code ;
}


/* DO NOT MODIFY THE GIVEN API*/
int assembler(FILE *input_file,
              FILE *output_file) {
  /*YOUR CODE HERE*/
  const char* delimiters = "   ()" ;
  char* token[5];

  // split the instruction by space and bracket
  // strtok: Call multiple times to obtain successive tokens from the same string. 
  int success ; 
  // int j = 0 ;
  do{
    char input[50] = {0};
    success = fscanf(input_file, "%[^\n]", input) ;
    // fprintf(output_file, "%s\n", input) ;
    // fprintf(output_file, "%d %d %d\n", trans_reg("a2"), trans_reg("t0"), atoi("9")) ;
    // j++ ;
    // if(j >= 35){
    int i = 0 ;
    if(success > 0){
      token[i] = strtok(input, delimiters) ;
      while(token[i]){
        // fprintf(output_file, "%d: %s\n", i, token[i]) ;
        token[++i] = strtok(NULL, delimiters);
      }
    }

    bool pseudo = translate(token[0], token, output_file) ;
    if(success>0 && (!pseudo)){
      uint32_t code = FindType(token[0], token) ;
      if(code){
        dump_code(output_file, code) ;
      }
      else{
        dump_error_information(output_file) ;
      }      
    }
    // }
    fscanf(input_file, "%*c");
  }while(success != EOF); // && j <= 45
  // }while(j <= 3) ;

  return 0;
}
