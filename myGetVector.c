//  Copyright 2013 Google Inc. All Rights Reserved.
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <malloc.h>

const long long max_size = 2000;         // max length of strings
const long long N = 20;                  // number of closest words that will be shown
const long long max_w = 50;              // max length of vocabulary entries

int loadModel(const char *filename ){
    FILE *f;
    float len = 0.0, *M;
    char *word;
    long long nwords, size;

    f = fopen(filename, "rb");
    if (f == NULL){
        printf("Input file not found\n");
        return -1;
    }
    fscanf(f, "%lld", &nwords);
    fscanf(f, "%lld", &size);

    word = (char *)malloc( max_w * sizeof(char));
    M = (float *)malloc( (long long)size * sizeof(float));
    
    for (int i=0; i<nwords; i++) {
        int j = 0;
        while(1) {
            word[j] = fgetc(f);
            if(feof(f) || (word[j] == ' ')) break;
            if((j<max_w) && (word[j] != '\n')) j++;
        }
        word[j] = 0; // use 0 to separate words
        printf("%s", word);

        for (j=0; j<size; j++) fread(&M[j], sizeof(float), 1, f);
        len = 0;
        for (j=0; j<size; j++) len += M[j] * M[j];
        len = sqrt(len);
        for(j=0; j<size; j++) printf(" %.6f", M[j]/=len);
        printf("\n");

    }
    fclose(f);
    free(word);
    free(M);
    return 0;
}

 
    

int main(int argc, char **argv) {
  char file_name[max_size];

  if (argc < 2) {
    printf("Usage: ./getVector <FILE>\nwhere FILE contains word projections in the BINARY FORMAT\n");
    return 0;
  }
  
  strcpy(file_name, argv[1]);
  if ( loadModel(file_name)!=0 ) 
      printf("Cannot load model file, please retry.");

  return 0;
}
