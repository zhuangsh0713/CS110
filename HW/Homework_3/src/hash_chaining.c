#include "../inc/hash_chaining.h"
#include "../inc/hash_func.h"

hash_chaining *hash_chaining_init(uint32_t size) {
  // TODO
  hash_chaining *ptr = (hash_chaining *)malloc(sizeof(hash_chaining));  
  ptr -> size = size ;
  ptr -> slots = (list_node **)malloc(sizeof(list_node *) * size) ;
  for(uint32_t i = 0; i < size; i++){
    // (ptr -> slots)[i] = (list_node *)malloc(sizeof(list_node)) ;
    (ptr -> slots)[i] = NULL ;
  }
  ptr -> parameters = generate_hash_parameters() ;
  return ptr ;
}

void hash_chaining_insert(hash_chaining *table, uint32_t key) {
  // TODO
  uint32_t hash_number = hash_func(key, table -> parameters, table -> size) ;
  list_node *new = (list_node *)malloc(sizeof(list_node)) ;
  new -> key = key ;
  new -> next = NULL ;

  list_node *prev = NULL;
  list_node *curr = table->slots[hash_number];
  while (curr != NULL) {
    prev = curr;
    curr = curr->next;
  }

  if(prev == NULL){
    table->slots[hash_number] = new ;
  } 
  else{
    prev->next = new ;  
  }
  return ;
}

bool hash_chaining_search(hash_chaining *table, uint32_t key) {
  // TODO
  uint32_t hash_number = hash_func(key, table -> parameters, table -> size) ;
  for(list_node *ptr = (table -> slots)[hash_number]; ptr != NULL;){
    if(ptr -> key == key){
      return true ;
    }
    ptr = ptr -> next ;
  }
  return false ;
}

void hash_chaining_destroy(hash_chaining *table) {
  // TODO
  for(uint32_t i = 0; i < table -> size; i++){
    for(list_node *ptr = (table -> slots)[i]; ptr != NULL;){
      list_node *next = ptr -> next ;
      free(ptr) ;
      ptr = next ;
    }
  }
  free(table -> slots) ;
  free(table) ;
  return ;
}
