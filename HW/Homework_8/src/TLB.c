#include "../inc/TLB.h"
#include <stdio.h>
#include <stdlib.h>

unsigned global_TLB_init(void) {
  global_tlb = calloc(1, sizeof(TLB));
  return 0;
}

void global_TLB_free(void) {
  if (global_tlb == NULL) {
    return;
  }
  free(global_tlb);
  global_tlb = NULL;
}

unsigned read_TLB(proc_id_t pid, unsigned vpn) {
    // how to use pid?
    // 0. Context Switch 
    if(pid != global_tlb -> pid){
        flush_TLB(pid);
    }
    global_tlb -> clock++;

    // 1.
    for(size_t i = 0; i < TLB_SIZE; i++){
        if(global_tlb -> entries[i].valid){

            // Hit
            if(global_tlb -> entries[i].vpn == vpn){
                global_tlb -> entries[i].lut = global_tlb -> clock;
                return global_tlb -> entries[i].ppn;
            }
        }
    }
    return 0;
}

void write_TLB(proc_id_t pid, unsigned vpn, unsigned ppn) {
    // 0. Context Switch 
    if(pid != global_tlb -> pid){
        flush_TLB(pid);
    }
    global_tlb -> clock++;

    // 1. 
    for(size_t i = 0; i < TLB_SIZE; i++){
        if(global_tlb -> entries[i].valid && global_tlb -> entries[i].vpn == vpn){
            global_tlb -> entries[i].ppn = ppn;
            global_tlb -> entries[i].lut = global_tlb -> clock;
            return;
        }
    }

    // 2. LRU and Invalid
    size_t invalid_location = 0;
    size_t exist_invalid = 0;
    size_t lru = SIZE_MAX;
    size_t lru_location = 0;
    for(size_t i = 0; i < TLB_SIZE; i++){
        if(!exist_invalid && !global_tlb->entries[i].valid){
            invalid_location = i;
            exist_invalid = 1;
            break;
        }
        if(global_tlb->entries[i].lut < lru){
            lru_location = i;
            lru = global_tlb->entries[i].lut;
        }
    }
    if(exist_invalid){
        lru_location = invalid_location;
    }
    global_tlb -> entries[lru_location].valid = 1;
    global_tlb -> entries[lru_location].vpn = vpn;
    global_tlb -> entries[lru_location].ppn = ppn;
    global_tlb -> entries[lru_location].lut = global_tlb -> clock;
}

void remove_TLB(proc_id_t pid, unsigned vpn) {
    global_tlb -> clock ++;
    // 0. Context Switch 
    if(pid != global_tlb -> pid){
        flush_TLB(pid);
    }

    // 1.
    for(size_t i = 0; i < TLB_SIZE; i++){
        if(global_tlb -> entries[i].valid && global_tlb -> entries[i].vpn == vpn){
            global_tlb -> entries[i].valid = 0;
            global_tlb -> entries[i].lut = global_tlb -> clock;
            return;
        }
    }
}

void flush_TLB(proc_id_t pid){
    for(size_t i = 0; i < TLB_SIZE; i++){
        global_tlb -> entries[i].valid = 0;
    }
    global_tlb -> pid = pid;
}
