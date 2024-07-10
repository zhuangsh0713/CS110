#include "../inc/simulator.h"
#include "../inc/TLB.h"
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

addr_t get_L1_PTP(addr_t address){
    return address >> (L2_BITS + OFFSET_BITS);
}

addr_t get_L2_PTE(addr_t address){
    return  (address >> (OFFSET_BITS)) & ((1 << L2_BITS) - 1);
}

status_t allocate_page(Process *process, addr_t address,
                        addr_t physical_address) {
    // This is guaranteed by the simulator
    assert(address >> OFFSET_BITS << OFFSET_BITS == address);
    assert(physical_address >> OFFSET_BITS << OFFSET_BITS == physical_address);
    // 1. Check if the process is valid
    if (process == NULL) {
        return ERROR;
    }
    // TODO: Implement me!
    addr_t L1_PTP = get_L1_PTP(address);

    // 2.
    // 2.1 check L2 page table exist
    if(process -> page_table.entries[L1_PTP].entries == NULL){
        process -> page_table.entries[L1_PTP].valid_count = 0;
        process -> page_table.entries[L1_PTP].entries = (PTE *)calloc(L2_PAGE_TABLE_SIZE, sizeof(PTE));
        for(size_t i = 0; i < L2_PAGE_TABLE_SIZE; i++){
            process -> page_table.entries[L1_PTP].entries[i].valid = 0;
        }
    }

    // 2.2 check PTE in L2 page table exist
    L2_PAGE_TABLE L2_page_table = process -> page_table.entries[L1_PTP];
    addr_t L2_PTE = get_L2_PTE(address);
    // virtual page has already been allocated
    if(L2_page_table.entries[L2_PTE].valid){
        return ERROR;
    }

    // 3
    // 3.1 the physical memory is not large enough to provide the required physical page
    if(physical_address >> (L1_BITS + L2_BITS) > main_memory->size){
      return ERROR;
    }

    // 3.2 store PPN in L2 page table
    L2_page_table.entries[L2_PTE].frame = physical_address >> OFFSET_BITS;
    L2_page_table.entries[L2_PTE].valid = 1;
    L2_page_table.valid_count += 1;
    
    return SUCCESS;
}

status_t deallocate_page(Process *process, addr_t address) {
    // This is guaranteed by the simulator
    assert(address >> OFFSET_BITS << OFFSET_BITS == address);
    // 1. Check if the process is valid
    if (process == NULL) {
        return ERROR;
    }
    // TODO: Implement me!
    addr_t L1_PTP = get_L1_PTP(address);
    
    // 2.
    // 2.1 check L2 page table exist
    if(process -> page_table.entries[L1_PTP].valid_count){
        return ERROR;
    }

    // 2.2 check PTE in L2 page table exist
    L2_PAGE_TABLE L2_page_table = process -> page_table.entries[L1_PTP];
    addr_t L2_PTE = get_L2_PTE(address);
    // virtual page has already been deallocated
    if(!L2_page_table.entries || !L2_page_table.entries[L2_PTE].valid){
      return ERROR;
    }

    // 3.
    // 3.1 deallocate virtual page
    L2_page_table.entries[L2_PTE].valid = 0;
    L2_page_table.valid_count -= 1;
    remove_TLB(process -> pid, address >> OFFSET_BITS);

    // 3.2 if the last page in the L2 page table is deallocated
    if(L2_page_table.valid_count == 0){
        free(L2_page_table.entries);
    }

    return SUCCESS;
}

status_t read_byte(Process *process, addr_t address, byte_t *byte) {
    // 1. Check if the process is valid
    if (process == NULL) {
        return ERROR;
    }
    // TODO: Implement me!
    status_t hit = SUCCESS;

    // 2. Find in TLB
    unsigned  physical_pn = read_TLB(process -> pid, address >> OFFSET_BITS);
    if(physical_pn >= main_memory -> size){
        return ERROR;
    }

    // 3. TLB miss -> Page Walk and Update TLB
    if(physical_pn == 0){
        if(process -> page_table.entries[get_L1_PTP(address)].entries && process -> page_table.entries[get_L1_PTP(address)].entries[get_L2_PTE(address)].valid){
            physical_pn = process -> page_table.entries[get_L1_PTP(address)].entries[get_L2_PTE(address)].frame;
        }
        else{
            return  ERROR;
        }
        if(physical_pn >= main_memory -> size){
            return  ERROR;
        }
        write_TLB(process -> pid, address >> OFFSET_BITS, physical_pn);
    }else{
        hit = TLB_HIT;
    }

    // 4. Read Byte
    *byte = main_memory -> pages[physical_pn] -> data[address & ((1 << OFFSET_BITS) -1)];

    return hit;
}

status_t write_byte(Process *process, addr_t address, const byte_t *byte) {
    // 1. Check if the process is valid
    if (process == NULL) {
        return ERROR;
    }
    // TODO: Implement me!
    status_t hit = SUCCESS;

    // 2. Find in TLB
    unsigned  physical_pn = read_TLB(process -> pid, address >> OFFSET_BITS);
    if(physical_pn >= main_memory -> size){
      return ERROR;
    }

    // 3. TLB miss -> Page Walk and Update TLB
    if(!physical_pn){
        if(process -> page_table.entries[get_L1_PTP(address)].entries && process -> page_table.entries[get_L1_PTP(address)].entries[get_L2_PTE(address)].valid){
            physical_pn = process -> page_table.entries[get_L1_PTP(address)].entries[get_L2_PTE(address)].frame;
        }
        else{
            return  ERROR;
        }

        if(physical_pn >= main_memory -> size){
            return  ERROR;
        }
        write_TLB(process -> pid, address >> OFFSET_BITS, physical_pn);
    }
    else{
        hit = TLB_HIT;
    }

    // 4. Write Byte
    main_memory -> pages[physical_pn] -> data[address & ((1 << OFFSET_BITS) -1)] = *byte;
    
    return hit;
}
