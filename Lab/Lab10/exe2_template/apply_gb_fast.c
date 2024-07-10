#include "main.h"

Image transpose(Image a){
    /* img_sc() will return a copy of the given image*/
    Image b = img_sc(a);
    b.dimX = a.dimY;
    b.dimY = a.dimX;
    
    for(int x=0;x<(int)a.dimX;x++){
        for(int y=0;y<(int)a.dimY;y++){
            b.data[x*b.dimX+y] = a.data[y*a.dimX+x];
        }
    }

    return b;
}

Image transpose_new(Image a){
    /* img_sc() will return a copy of the given image*/
    Image b = img_sc(a);
    b.dimX = a.dimY;
    b.dimY = a.dimX;

    int block_size = 1024 ;

    for(int y_block = 0; y_block < (int)a.dimY; y_block += block_size){
        for(int x=0;x<(int)a.dimX;x++){
            for(int y=0;y<block_size && (y+y_block) < (int)a.dimY;y++){
                b.data[x*b.dimX+y+y_block] += a.data[(y+y_block)*a.dimX+x];
            }
        }
    }
    return b;
}

Image apply_gb(Image a, FVec gv)
{
    // Image b = gb_h(a, gv);
    // Image c = gb_v(b,gv);
    // free(b.data);
    // return c;




    struct timeval start_time, stop_time, elapsed_time; 
    gettimeofday(&start_time,NULL);
    
    Image b = gb_h(a, gv);

    gettimeofday(&stop_time,NULL);
    timersub(&stop_time, &start_time, &elapsed_time); 
    printf("horizontal gaussian blur time: %f \n", elapsed_time.tv_sec+elapsed_time.tv_usec/1000000.0);

    gettimeofday(&start_time,NULL);

    Image b_trans = transpose(b) ;
    Image c = gb_h(b_trans, gv) ;
    Image result = transpose(c) ;

    gettimeofday(&stop_time,NULL);
    timersub(&stop_time, &start_time, &elapsed_time); 
    printf("vertical gaussian blur time: %f \n", elapsed_time.tv_sec+elapsed_time.tv_usec/1000000.0);


    gettimeofday(&start_time,NULL);

    Image b_trans_new = transpose_new(b) ;
    Image c_new = gb_h(b_trans, gv) ;
    Image result_new = transpose_new(c) ;

    gettimeofday(&stop_time,NULL);
    timersub(&stop_time, &start_time, &elapsed_time); 
    printf("vertical gaussian blur time with cache blocking: %f \n", elapsed_time.tv_sec+elapsed_time.tv_usec/1000000.0);

    
    free(b.data);
    return result;
}
