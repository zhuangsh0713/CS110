#include "FloatCalculate.h"
#include <stdbool.h>
#include <math.h>

const size_t SIGN_BIT = 1;
const size_t EXPONENT_BITS = 8;
const size_t MANTISSA_BITS = 23;

static int32_t get_norm_bias(void) { return 1 - (1 << (EXPONENT_BITS - 1)); }

static int32_t get_denorm_bias(void) { return 1 + get_norm_bias(); }

static bool test_rightmost_all_zeros(uint32_t number, size_t bits) {
  uint32_t mask = (1ull << bits) - 1;
  return (number & mask) == 0;
}

static bool test_rightmost_all_ones(uint32_t number, size_t bits) {
  uint32_t mask = (1ull << bits) - 1;
  return (number & mask) == mask;
}


static void build_bitstring(Float input, char *output){
  output[0] = input.sign + '0' ;
  if(input.type == DENORMALIZED_T){
    for(size_t i = EXPONENT_BITS; i >= SIGN_BIT; i--){
      output[i] = '0' ;
    }
  }
  else{
    uint32_t expo = input.exponent - get_norm_bias() ;
    for(size_t i = EXPONENT_BITS; i >= SIGN_BIT; i--){
        output[i] = (expo & 1) + '0';
        expo = expo >> 1 ;
    }
  }

  uint32_t mantissa = input.mantissa ;
  for(size_t i = EXPONENT_BITS + MANTISSA_BITS; i >= SIGN_BIT + EXPONENT_BITS; i--){
    output[i] = (mantissa & 1) + '0' ;
    mantissa >>= 1 ;
  }
  output[EXPONENT_BITS + MANTISSA_BITS + SIGN_BIT] = '\0' ;

  return ;
}

// You can also design a function of your own.
static Float parse_bitstring(const char *input){
  Float output ;
  output.sign = input[0] - '0' ;
  uint32_t expo = 0 ;
  uint32_t mantissa = 0 ;
  for(size_t i = SIGN_BIT; i <= EXPONENT_BITS + MANTISSA_BITS; i++){
    if(i <= EXPONENT_BITS){
      expo += (input[i] - '0') * (1u << (EXPONENT_BITS - i)) ;
    }
    else{
      mantissa += (input[i] - '0') * (1u << (EXPONENT_BITS + MANTISSA_BITS - i)) ;
    }
  }
  output.exponent = expo + get_norm_bias() ;
  output.mantissa = mantissa ;
  if(test_rightmost_all_zeros(expo, 8)){
    output.type = DENORMALIZED_T ;
    output.exponent = get_denorm_bias() ;
  }
  else if(test_rightmost_all_ones(expo, 8)){
    if(test_rightmost_all_zeros(mantissa, 23)){
      output.type = INFINITY_T ;
    }
    else{
      output.type = NAN_T ; 
    }
  }
  else{
    output.type = NORMALIZED_T ;
  }
  
  return output ;
}

static void shift(Float x, char* output, int32_t expo){
  char origin[32] ;
  build_bitstring(x, origin);

  for(size_t i = 0; i <= 26; i++){
    output[i] = '0' ;
  }

  for(size_t i = 1; i <= 23; i++){
    if(i + expo - x.exponent <= 25){
      output[i + expo - x.exponent] = origin[i + EXPONENT_BITS] ;
    }
    else{
      if(origin[i + EXPONENT_BITS]-'0'){
        output[26] = '1' ;
        break ;
      }
    }
  }
  if(((expo - x.exponent) >= 0) && ((expo - x.exponent) <= 26)){
    output[expo - x.exponent] = '0' + 1 - x.type ;
  }
  output[27] = '\0' ;
  return ;
}

// You can also design a function of your own.
static Float float_add_impl(Float a, Float b){
  Float output ;
  char new[27] = {0};
  char max_shift[27] = {0};
  char min_shift[27] = {0};

  uint32_t NumA = a.mantissa + (1 - a.type) * (1u << 23) ;
  uint32_t NumB = b.mantissa + (1 - b.type) * (1u << 23) ;
  int32_t Expo = fmax(a.exponent, b.exponent) ;
  NumA >>= Expo - a.exponent ;
  NumB >>= Expo - b.exponent ;
  // uint32_t new_sign = 0 ;

  if(NumA < NumB){
    shift(b, max_shift, Expo) ;
    output.sign = b.sign ;
    shift(a, min_shift, Expo) ;
    // new_sign = a.sign ;
  }
  else{
    shift(a, max_shift, Expo) ;
    output.sign = a.sign ;
    shift(b, min_shift, Expo) ;
    // new_sign = b.sign ;
  }
  bool SameSign = true ;
  if(a.sign != b.sign){
    SameSign = false ;
  }

  int carry = 0 ; 
  for(int i = 26; i >= 0; i--){
    int sum = 0 ;
    if(SameSign){
      sum = (max_shift[i]-'0') + (min_shift[i]-'0') + carry ;
    }
    else{
      sum = (max_shift[i]-'0') - (min_shift[i]-'0') + carry ;
    }
    
    carry = sum >> 1 ;
    sum = sum&1 ;
    new[i] = sum + '0' ; 
  }
  output.type = NORMALIZED_T ;
  output.mantissa = 0 ;
  if(carry){
    if(test_rightmost_all_ones(Expo - get_norm_bias(), 8)){
      output.exponent = Expo ;
      output.mantissa = 0;
      output.type = INFINITY_T ;
    }
    else{
      Expo ++ ;
      for(int i = 22; i >= 0; i--){
        output.mantissa += (1u << (MANTISSA_BITS - i - 1)) * (new[i] - '0') ;
      }
      output.exponent = Expo ;
    }
  }
  else{
    int index = 27 ;
    for(int i = 0; i <= 26; i++){
      if(new[i] - '0'){
        index = i ;
        break ;
      }
    }
    if((Expo - index) < get_denorm_bias()){
      index = Expo - get_denorm_bias() ;
      output.exponent = get_norm_bias() ;
      for(int i = 23; i >= 1; i--){
        output.mantissa += (1u << (MANTISSA_BITS - i)) * (new[i] - '0') ;
      }
      if(index < 23){
        output.mantissa <<= (index) ;
        for(int i = 1; i <= 3 && i <= index; i++){
          output.mantissa += (1u << (index - i)) * (new[i + MANTISSA_BITS] -'0') ;
        }
      }
    }
    else{
      for(int i = 23; i >= 1; i--){
        output.mantissa += (1u << (MANTISSA_BITS - i)) * (new[i] - '0') ;
      }
      if(index < 23){
        output.mantissa <<= (index) ;
        for(int i = 1; i <= 3 && i <= index; i++){
          output.mantissa += (1u << (index - i)) * (new[i + MANTISSA_BITS] -'0') ;
        }
      }    
      Expo -= index ;
      output.exponent = Expo ;
    }
  }
  return output ;
}


// You should not modify the signature of this function
void float_add(const char *a, const char *b, char *result) {
  // TODO: Implement this function
  // A possible implementation of the function:
  Float fa = parse_bitstring(a);
  Float fb = parse_bitstring(b);
  Float fresult = float_add_impl(fa, fb);
  build_bitstring(fresult, result);
  return ;
}
