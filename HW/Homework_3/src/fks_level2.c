#include "../inc/fks_level2.h"
#include "../inc/hash_func.h"
#include <string.h>

fks_level2 *fks_level2_init(uint32_t size, hash_parameters parameters) {
  // TODO
  fks_level2 *ptr = (fks_level2 *)malloc(sizeof(fks_level2)) ;
  ptr -> slots = (uint32_t *)malloc(sizeof(uint32_t) * size) ;
  for(uint32_t i = 0; i < size; i++){
    (ptr -> slots)[i] = FKS_LEVEL2_EMPTY ;
  }
  ptr -> parameters = parameters ;
  ptr -> size = size ;
  return ptr;
}

fks_level2 *fks_level2_build(list_node *head, uint32_t size,
                             hash_parameters parameters) {
  // TODO
  fks_level2 *new = fks_level2_init(size, parameters) ;
  for(list_node *ptr = head; ptr != NULL; ){
    bool flag = fks_level2_insert(new, ptr -> key) ;
    if(!flag){
      fks_level2_destroy(new) ;
      return NULL ;
    }
    ptr = ptr -> next ;
  }
  return new ;
}

bool fks_level2_insert(fks_level2 *table, uint32_t key) {
  // TODO
  uint32_t hash_number = hash_func(key, table -> parameters, table -> size) ;
  if((table -> slots)[hash_number] == FKS_LEVEL2_EMPTY){
    (table -> slots)[hash_number] = key ;
    return true ;
  }
  else if(key == (table -> slots)[hash_number]){
    return true ;
  }
  return false ;
}

bool fks_level2_search(fks_level2 *table, uint32_t key) {
  // TODO
  uint32_t hash_number = hash_func(key, table -> parameters, table -> size) ;
  if((table -> slots)[hash_number] != key){
    return false ;
  }
  return true ;
}

void fks_level2_destroy(fks_level2 *table) {
  // TODO
  // for(uint32_t i = 0; i < table -> size; i++){
  //   free((table -> slots)[i]) ;
  // }
  if(table){
    free(table -> slots) ;
    // printf("finished - slots") ;

    free(table) ;
    // printf("finished table") ;
  }
  

  return ;
}
