#include "cache.h"
#include <string.h>
#include <stdlib.h>
#include <assert.h>

uint32_t log_(uint32_t n) {
    uint32_t count = 0;
    while (n > 1) {
        n >>= 1;
        count++;
    }
    return count;
}

/* Create a cache simulator according to the config */
struct cache * cache_create(struct cache_config config, struct cache * lower_level){
    /*YOUR CODE HERE*/
    struct cache* new_cache = (struct cache*)malloc(sizeof(struct cache));
    if(!new_cache){
        return NULL;
    }
    new_cache -> config = config;
    new_cache -> lower_cache = lower_level;

    new_cache -> offset_bits = log_(config.line_size);
    new_cache -> offset_mask = (1 << (new_cache -> offset_bits)) - 1;

    new_cache -> index_bits = log_(config.lines/config.ways);
    new_cache -> index_mask = ((1 << (new_cache -> index_bits)) - 1) << (new_cache -> offset_bits);

    new_cache -> tag_bits = config.address_bits - new_cache -> offset_bits - new_cache -> index_bits;
    new_cache -> tag_mask = ((1 << (new_cache -> tag_bits)) - 1) << (new_cache -> offset_bits + new_cache -> index_bits);

    new_cache -> lines = (struct cache_line*)malloc(config.lines * sizeof(struct cache_line)) ;
    if(!(new_cache -> lines)){
        free(new_cache);
        return NULL;
    }
    for(uint32_t i = 0; i < config.lines; i++){
        new_cache -> lines[i].dirty = false;
        new_cache -> lines[i].valid = false;
        new_cache -> lines[i].tag = 0;
        new_cache -> lines[i].last_access = 0;
        // if data allocation fails, free all allocated memory?
        new_cache -> lines[i].data = (uint8_t *)calloc(config.line_size, sizeof(uint8_t));
    }

    return new_cache;
}

/* 
Release the resources allocated for the cache simulator. 
Also writeback dirty lines

The order in which lines are evicted is:
set0-slot0, set1-slot0, set2-slot0, (the 0th way)
set0-slot1, set1-slot1, set2-slot1, (the 1st way)
set0-slot2, set1-slot2, set2-slot2, (the 2nd way)
and so on.
*/
void cache_destroy(struct cache* cache){
    /*YOUR CODE HERE*/
    if(!cache){
        return;
    }
    
    uint32_t num_set = cache -> config.lines/cache -> config.ways;

    for(uint32_t i = 0; i < cache -> config.lines; i++){
        if(cache -> config.write_back && cache -> lines[i].dirty && cache -> lines[i].valid){
            uint32_t store_addr = (cache -> lines[i].tag << (cache -> index_bits + cache -> offset_bits)) + ((i % num_set) << (cache -> offset_bits));
            if(cache -> lower_cache){
                cache_write_cacheline(cache -> lower_cache, store_addr, cache -> lines[i].data);
            }
            else{
                mem_store(cache -> lines[i].data, store_addr, cache -> config.line_size);
            }
        }
        free(cache -> lines[i].data);
    }
    free(cache -> lines);
    cache_destroy(cache -> lower_cache);
    free(cache);
    return;
}

/* Read one byte at a specific address. return hit=true/miss=false */
bool cache_read_byte(struct cache * cache, uint32_t addr, uint8_t *byte){
    /*YOUR CODE HERE*/
    uint32_t num_set = cache -> config.lines/ cache -> config.ways;
    uint32_t index = (addr & cache -> index_mask) >> (cache -> offset_bits) ;
    uint32_t tag = (addr & cache -> tag_mask) >> (cache -> offset_bits + cache -> index_bits) ;
    uint32_t offset = (addr & cache -> offset_mask);

    uint64_t lru = 0xfffffffffff;
    uint32_t lru_location = 0;

    bool exist_invalid = false;
    uint32_t invalid_location = 0;


    for(uint32_t i = 0; i < cache -> config.ways; i++){
        uint32_t new_tag = cache -> lines[index + i * num_set].tag;
        uint64_t new_lru = cache -> lines[index + i * num_set].last_access;
        bool valid = cache -> lines[index + i * num_set].valid;

        if(valid && new_tag == tag){
            *byte = cache -> lines[index + i * num_set].data[offset];
            cache -> lines[index + i * num_set].last_access = get_timestamp();
            return true;
        }

        if(!valid && !exist_invalid){
            exist_invalid = true;
            invalid_location = i;
            break;
        }
        else if(new_lru < lru){
            lru = new_lru;
            lru_location = i;
        }
    }

    if(exist_invalid){
        lru_location = invalid_location;
    }

    // offset in store and load addr should be 0s?
    if(cache -> config.write_back && cache -> lines[index + lru_location * num_set].dirty && cache -> lines[index + lru_location * num_set].valid){
        uint32_t store_addr = (cache -> lines[index + lru_location * num_set].tag << (cache -> index_bits + cache -> offset_bits)) + (index << (cache -> offset_bits));
        if(cache -> lower_cache){
            cache_write_cacheline(cache -> lower_cache, store_addr, cache -> lines[index + lru_location * num_set].data);
        }
        else{
            mem_store(cache -> lines[index + lru_location * num_set].data, store_addr, cache -> config.line_size);
        }
    }
    cache -> lines[index + lru_location * num_set].tag = tag;
    cache -> lines[index + lru_location * num_set].last_access = get_timestamp();

    if(cache -> lower_cache){
        cache_read_cacheline(cache -> lower_cache, addr&(cache -> index_mask + cache -> tag_mask), &(cache -> lines[index + lru_location * num_set].data));
    }
    else{
        mem_load(cache -> lines[index + lru_location * num_set].data, addr&(cache -> index_mask + cache -> tag_mask), cache -> config.line_size);
    }

    cache -> lines[index + lru_location * num_set].valid = true;
    cache -> lines[index + lru_location * num_set].dirty = false;

    *byte = cache -> lines[index + lru_location * num_set].data[offset];

    return false;
}
/* Write one byte into a specific address. return hit=true/miss=false*/
bool cache_write_byte(struct cache * cache, uint32_t addr, uint8_t byte){
    /*YOUR CODE HERE*/
    uint32_t num_set = cache -> config.lines/ cache -> config.ways;

    uint32_t index = (addr & cache -> index_mask) >> (cache -> offset_bits) ;
    uint32_t tag = (addr & cache -> tag_mask) >> (cache -> offset_bits + cache -> index_bits) ;
    uint32_t offset = (addr & cache -> offset_mask);

    uint64_t lru = 0xfffffffffff;
    uint32_t lru_location = 0;

    bool exist_invalid = false;
    uint32_t invalid_location = 0;

    // bool exist_empty = false;
    // uint32_t empty_location = 0;
    for(uint32_t i = 0; i < cache -> config.ways; i++){
        uint32_t new_tag = cache -> lines[index + i * num_set].tag;
        uint64_t new_lru = cache -> lines[index + i * num_set].last_access;
        bool valid = cache -> lines[index + i * num_set].valid;

        if(valid && new_tag == tag){
            // if the object cache line is dirty, whether need to write back into memory?
            // do not write it into memory
            // testcase: 2-0,...2-3 

            // if(cache -> config.write_back && cache -> lines[index * cache -> config.ways + i].dirty){
            //     uint32_t store_addr = (cache -> lines[index * cache -> config.ways + i].tag << (cache -> index_bits + cache -> offset_bits)) + (index << (cache -> offset_bits));
            //     mem_store(cache -> lines[index * cache -> config.ways + i].data, store_addr, cache -> config.line_size);
            // }
            cache -> lines[index + i * num_set].data[offset] = byte;
            if(!cache -> config.write_back){
                if(cache -> lower_cache){
                    cache_write_cacheline(cache -> lower_cache, addr&(cache -> index_mask + cache -> tag_mask), cache -> lines[index + i * num_set].data);
                }
                else{
                    mem_store(cache -> lines[index + i * num_set].data, addr&(cache -> index_mask + cache -> tag_mask), cache -> config.line_size);
                }
            }
            else{
                cache -> lines[index + i * num_set].dirty = true;
            }
            cache -> lines[index + i * num_set].last_access = get_timestamp() ;
            return true;
        }

        if(!valid && !exist_invalid){
            exist_invalid = true;
            invalid_location = i;
            break;
        }
        else if(new_lru < lru){
            lru = new_lru;
            lru_location = i;
        }
    }
    
    if(exist_invalid){
        lru_location = invalid_location;
    }

    if(cache -> config.write_back && cache -> lines[index + lru_location * num_set].dirty && cache -> lines[index + lru_location * num_set].valid){
        uint32_t store_addr = (cache -> lines[index + lru_location * num_set].tag << (cache -> index_bits + cache -> offset_bits)) + (index << (cache -> offset_bits));
        if(cache -> lower_cache){
            cache_write_cacheline(cache -> lower_cache, store_addr, cache -> lines[index + lru_location * num_set].data);
        }
        else{
            mem_store(cache -> lines[index + lru_location * num_set].data, store_addr, cache -> config.line_size);
        }
        
        // mem_store(cache -> lines[index + lru_location * num_set].data, store_addr, cache -> config.line_size);
    }

    cache -> lines[index + lru_location * num_set].tag = tag;
    cache -> lines[index + lru_location * num_set].last_access = get_timestamp();

    // mem_load(cache -> lines[index + lru_location * num_set].data, addr&(cache -> index_mask + cache -> tag_mask), cache -> config.line_size);

    if(cache -> lower_cache){
        cache_read_cacheline(cache -> lower_cache, addr&(cache -> index_mask + cache -> tag_mask), &(cache -> lines[index + lru_location * num_set].data));
    }
    else{
        mem_load(cache -> lines[index + lru_location * num_set].data, addr&(cache -> index_mask + cache -> tag_mask), cache -> config.line_size);
    }

    cache -> lines[index + lru_location * num_set].valid = true;
    cache -> lines[index + lru_location * num_set].data[offset] = byte;
    if(!cache -> config.write_back){
        if(cache -> lower_cache){
            cache_write_cacheline(cache -> lower_cache, addr&(cache -> index_mask + cache -> tag_mask), cache -> lines[index + lru_location * num_set].data);
        }
        else{
            mem_store(cache -> lines[index + lru_location * num_set].data, addr&(cache -> index_mask + cache -> tag_mask), cache -> config.line_size);
        }
        // mem_store(cache -> lines[index + lru_location * num_set].data, addr&(cache -> index_mask + cache -> tag_mask), cache -> config.line_size);
    }
    else{
        cache -> lines[index + lru_location * num_set].dirty = true;
    }

    return false;
}


void cache_read_cacheline(struct cache * cache, uint32_t addr, uint8_t** line){
    uint32_t num_set = cache -> config.lines/ cache -> config.ways;
    uint32_t index = (addr & cache -> index_mask) >> (cache -> offset_bits) ;
    uint32_t tag = (addr & cache -> tag_mask) >> (cache -> offset_bits + cache -> index_bits) ;

    uint64_t lru = 0xfffffffffff;
    uint32_t lru_location = 0;

    bool exist_invalid = false;
    uint32_t invalid_location = 0;

    for(uint32_t i = 0; i < cache -> config.ways; i++){
        uint32_t new_tag = cache -> lines[index + i * num_set].tag;
        uint64_t new_lru = cache -> lines[index + i * num_set].last_access;
        bool valid = cache -> lines[index + i * num_set].valid;

        if(valid && new_tag == tag){
            *line = cache -> lines[index + i * num_set].data;
            cache -> lines[index + i * num_set].last_access = get_timestamp();
            return;
        }

        if(!valid && !exist_invalid){
            exist_invalid = true;
            invalid_location = i;
            break;
        }
        else if(new_lru < lru){
            lru = new_lru;
            lru_location = i;
        }
    }

    if(exist_invalid){
        lru_location = invalid_location;
    }


    if(cache -> config.write_back && cache -> lines[index + lru_location * num_set].dirty && cache -> lines[index + lru_location * num_set].valid){
        uint32_t store_addr = (cache -> lines[index + lru_location * num_set].tag << (cache -> index_bits + cache -> offset_bits)) + (index << (cache -> offset_bits));
        mem_store(cache -> lines[index + lru_location * num_set].data, store_addr, cache -> config.line_size);
    }
    cache -> lines[index + lru_location * num_set].tag = tag;
    cache -> lines[index + lru_location * num_set].last_access = get_timestamp();

    mem_load(cache -> lines[index + lru_location * num_set].data, addr, cache -> config.line_size);

    cache -> lines[index + lru_location * num_set].valid = true;
    cache -> lines[index + lru_location * num_set].dirty = false;

    *line = cache -> lines[index + lru_location * num_set].data;

    return;
}


void cache_write_cacheline(struct cache * cache, uint32_t addr, uint8_t* line){
    uint32_t num_set = cache -> config.lines/ cache -> config.ways;

    uint32_t index = (addr & cache -> index_mask) >> (cache -> offset_bits) ;
    uint32_t tag = (addr & cache -> tag_mask) >> (cache -> offset_bits + cache -> index_bits) ;

    uint64_t lru = 0xfffffffffff;
    uint32_t lru_location = 0;

    bool exist_invalid = false;
    uint32_t invalid_location = 0;

    // bool exist_empty = false;
    // uint32_t empty_location = 0;
    for(uint32_t i = 0; i < cache -> config.ways; i++){
        uint32_t new_tag = cache -> lines[index + i * num_set].tag;
        uint64_t new_lru = cache -> lines[index + i * num_set].last_access;
        bool valid = cache -> lines[index + i * num_set].valid;

        if(valid && new_tag == tag){

            cache -> lines[index + i * num_set].data = line;
            if(!cache -> config.write_back){
                mem_store(cache -> lines[index + i * num_set].data, addr&(cache -> index_mask + cache -> tag_mask), cache -> config.line_size);
            }
            else{
                cache -> lines[index + i * num_set].dirty = true;
            }
            cache -> lines[index + i * num_set].last_access = get_timestamp() ;
            return ;
        }

        if(!valid && !exist_invalid){
            exist_invalid = true;
            invalid_location = i;
            break;
        }
        else if(new_lru < lru){
            lru = new_lru;
            lru_location = i;
        }
    }
    if(exist_invalid){
        lru_location = invalid_location;
    }

    if(cache -> config.write_back && cache -> lines[index + lru_location * num_set].dirty && cache -> lines[index + lru_location * num_set].valid){
        uint32_t store_addr = (cache -> lines[index + lru_location * num_set].tag << (cache -> index_bits + cache -> offset_bits)) + (index << (cache -> offset_bits));
        mem_store(cache -> lines[index + lru_location * num_set].data, store_addr, cache -> config.line_size);
    }

    cache -> lines[index + lru_location * num_set].tag = tag;
    cache -> lines[index + lru_location * num_set].last_access = get_timestamp();

    // mem_load(cache -> lines[index + lru_location * num_set].data, addr&(cache -> index_mask + cache -> tag_mask), cache -> config.line_size);

    if(cache -> lower_cache){
        cache_read_cacheline(cache -> lower_cache, addr&(cache -> index_mask + cache -> tag_mask), &(cache -> lines[index + lru_location * num_set].data));
    }
    else{
        mem_load(cache -> lines[index + lru_location * num_set].data, addr&(cache -> index_mask + cache -> tag_mask), cache -> config.line_size);
    }

    cache -> lines[index + lru_location * num_set].valid = true;
    cache -> lines[index + lru_location * num_set].data = line;
    if(!cache -> config.write_back){
        mem_store(cache -> lines[index + lru_location * num_set].data, addr&(cache -> index_mask + cache -> tag_mask), cache -> config.line_size);
    }
    else{
        cache -> lines[index + lru_location * num_set].dirty = true;
    }

    return;
}