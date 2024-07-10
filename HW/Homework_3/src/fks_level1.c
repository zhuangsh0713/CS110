#include "../inc/fks_level1.h"
#include "../inc/hash_func.h"
#include <stdio.h>

static int get_list_len(list_node *node) {
  // TODO
  int count = 0;
  list_node *ptr = node ;
  for(; ptr != NULL;){
    count ++;
    ptr = ptr -> next;
  }
  return count ;
}

fks_level1 *fks_level1_build(hash_chaining *hash_chaining_table) {
  // TODO
  fks_level1 *ptr = (fks_level1 *)malloc(sizeof(fks_level1));  
  ptr -> size = hash_chaining_table -> size ;
  ptr -> level2_tables = (fks_level2 **)malloc(sizeof(fks_level2 *) * (ptr -> size)) ;
  for(uint32_t i = 0; i < ptr -> size; i++){
    // printf("i = %d\n", i) ;
    uint32_t size_2level = get_list_len((hash_chaining_table -> slots)[i]) ;
    // (ptr -> level2_tables)[i] = (fks_level2 *)malloc(sizeof(fks_level2)) ;
    if(size_2level){
      fks_level2 *new_head = fks_level2_build((hash_chaining_table -> slots)[i], size_2level * size_2level, generate_hash_parameters()) ;
      while(new_head == NULL){
        hash_parameters new_parameters = generate_hash_parameters() ;
        new_head = fks_level2_build((hash_chaining_table -> slots)[i], size_2level * size_2level, new_parameters) ;
        // printf("i = %d\n", i) ;
      }
      (ptr -> level2_tables)[i] = new_head ;
    }
    else{
      (ptr -> level2_tables)[i] = NULL ;
    }
  }
  ptr -> parameters = hash_chaining_table -> parameters ;
  return ptr ;
}

bool fks_level1_search(fks_level1 *table, uint32_t key) {
  // TODO
  uint32_t hash_number = hash_func(key, table -> parameters, table -> size) ;
  return fks_level2_search((table -> level2_tables)[hash_number], key) ; 
}

void fks_level1_destroy(fks_level1 *table) {
  // TODO
  for(uint32_t i = 0; i < table -> size; i++){
    fks_level2_destroy((table -> level2_tables)[i]) ;
    // printf("finished0") ;
    // free((table -> level2_tables)[i]) ;
  }
  // printf("finished -1\n") ;
  free(table -> level2_tables) ;
  free(table) ;
  return ;
}
