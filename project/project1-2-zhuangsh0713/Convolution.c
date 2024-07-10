#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(int argc,char ** argv){
  if(argc<3){
    printf("Usage:\n");
    printf("./main <input file> <output file>\n");
    exit(0);
  }

  char * input_file_name = argv[1];
  char * output_file_name = argv[2];

  FILE * input_file = fopen(input_file_name,"r");
  FILE * output_file = fopen(output_file_name,"w");
  
  if(input_file == NULL){
    printf("Error: unable to open input file %s\n",input_file_name);
    exit(0);
  }

  if(output_file == NULL){
    printf("Error: unable to open output file %s\n",output_file_name);
    fclose(input_file);
    exit(0);
  }

  /* YOUR CODE HERE */

  char input1[100] = {0} ;
  fscanf(input_file, "%[^\n]", input1) ;
  fscanf(input_file, "%*c");

  int image_length = atoi(strtok(input1, " ")) ;
  int image_width = atoi(strtok(NULL, " ")) ;

  char *StrImage = NULL ;
  int **Image = (int **)malloc(image_width * sizeof(int *));
  for (int i = 0; i < image_width; i++) {
    Image[i] = (int *)malloc(image_length * sizeof(int));
  }
  for(int i = 0; i < image_width; i++){
    fscanf(input_file, "%[^\n]", input1);
    int j = 0;
    StrImage = strtok(input1, " ");
    while (StrImage != NULL) {
      Image[i][j] = atoi(StrImage);
      StrImage = strtok(NULL, " ");
      j++;
    }
    fscanf(input_file, "%*c");
  }

  char input2[100] = {0} ;
  fscanf(input_file, "%[^\n]", input2) ;
  fscanf(input_file, "%*c");

  int kernel_length = atoi(strtok(input2, " ")) ;
  int kernel_width = atoi(strtok(NULL, " ")) ;

  char *StrKernel = NULL;
  int **Kernel = (int **)malloc(kernel_width * sizeof(int *));
  for (int i = 0; i < kernel_width; i++) {
    Kernel[i] = (int *)malloc(kernel_length * sizeof(int));
  }
 

  for(int i = 0; i < kernel_width; i++){
    fscanf(input_file, "%[^\n]", input2);
    int j = 0;
    StrKernel = strtok(input2, " ");
    while (StrKernel != NULL) {
      Kernel[i][j] = atoi(StrKernel);
      StrKernel = strtok(NULL, " ");
      j++;
    }    
    fscanf(input_file, "%*c");
  }

  int result_width = image_width - kernel_width + 1;
  int result_length = image_length - kernel_length + 1;

  int **result = (int **)malloc(result_width * sizeof(int *));
  for (int i = 0; i < result_width; i++) {
    result[i] = (int *)malloc(result_length * sizeof(int));
  }

  for (int x = 0; x < result_width; x++) {
    for (int y = 0; y < result_length; y++) {
      result[x][y] = 0;
      for (int i = 0; i < kernel_width; i++) {
        for (int j = 0; j < kernel_length; j++) {
          result[x][y] += Kernel[i][j] * Image[x + i][y + j];
        }
      }
    }
  }

  for(int i = 0; i < image_width + 1 - kernel_width; i++){
    for(int j = 0; j < image_length + 1 - kernel_length; j++){
        fprintf(output_file, "%d ", result[i][j]);
    }
    fprintf(output_file, "\n") ;
  }

  for (int i = 0; i < image_width; i++) {
    free(Image[i]);
  }
  free(Image);

  for (int i = 0; i < kernel_width; i++) {
    free(Kernel[i]);
  }
  free(Kernel);

  for (int i = 0; i < image_width + 1 - kernel_width; i++) {
    free(result[i]);
  }
  free(result);

  fclose(input_file);
  fclose(output_file);

  return 0;
}
