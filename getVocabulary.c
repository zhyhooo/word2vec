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
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <malloc.h>

const long long max_size = 2000;         // max length of strings
const long long N = 20;                  // number of closest nwords that will be shown
const long long max_w = 50;              // max length of vocabulary entries
long long nwords, size;
float *M;

int loadModel(const char *filename, const char *filevocab ){
    FILE *fin, *fout;

    fin = fopen(filename, "rb");
    fout = fopen(filevocab, "wb");
    if (fin == NULL || fout == NULL){
        printf("Input file or output file  not found\n");
        return -1;
    }
    fscanf(fin, "%lld", &nwords);
    fscanf(fin, "%lld", &size);
   
    float *M = (float *)malloc((long long)size * sizeof(float));
    for (int i=0; i<nwords; i++) {
        int j = 0;
        char vocab[max_w];
        while(1) {
            vocab[j] = fgetc(fin);
            if(feof(fin) || (vocab[j] == ' ')) break;
            if((j<max_w) && (vocab[j] != '\n')) j++;
        }
        vocab[j] = 0; // use 0 to separate words
        fprintf(fout, "%s\n", vocab);
        for (j=0; j<size; j++) fread(&M[j], sizeof(float), 1, fin);
        if ( i%100000==0 ){
            printf("read words #%lld %c", i, 13);
            fflush(stdout);
        }
    }
    fclose(fin);
    fclose(fout);
    free(M);
    return 0;
}


int main(int argc, char **argv) {
  FILE *fp;
  char file_name[max_size], file_vocab[max_size];

  if (argc < 3) {
    //printf("Usage: ./distance <FILE>\nwhere FILE contains word projections in the BINARY FORMAT\n");
    printf("Usage: ./distance <file_model> <file_vocab>\n");
    return 0;
  }
  
  strcpy(file_name, argv[1]);
  strcpy(file_vocab, argv[2]);
  if (loadModel(file_name, file_vocab)!=0 ) {
      printf("Cannot load model file, please retry.");
      return 0;
  }
  printf("words:%lld, size:%lld\n", nwords, size);
}
