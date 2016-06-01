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
const long long N = 30;                  // number of closest nwords that will be shown
const long long max_w = 50;              // max length of vocabulary entries
long long nwords, size;
char *vocab;
float *M;

int loadModel(const char *filename ){
    FILE *f;

    f = fopen(filename, "rb");
    if (f == NULL){
        printf("Input file not found\n");
        return -1;
    }
    fscanf(f, "%lld", &nwords);
    fscanf(f, "%lld", &size);

    vocab = (char *)malloc((long long)nwords * max_w * sizeof(char));
    M = (float *)malloc((long long)nwords * (long long)size * sizeof(float));
    
    if ( M == NULL ) {
        printf("Cannot allocate memory: %lld MB    %lld  %lld\n", 
                (long long)nwords * size * sizeof(float) / 1048576, nwords, size);
        return -1;
    }
    for (int i=0; i<nwords; i++) {
        int j = 0;
        while(1) {
            vocab[i*max_w + j] = fgetc(f);
            if(feof(f) || (vocab[i*max_w + j] == ' ')) break;
            if((j<max_w) && (vocab[i*max_w + j] != '\n')) j++;
        }
        vocab[i*max_w + j] = 0; // use 0 to separate words
        for (j=0; j<size; j++) fread(&M[i*size + j], sizeof(float), 1, f);
        float len = 0.0;
        for (j=0; j<size; j++) len += M[i*size + j] * M[i*size + j];
        len = sqrt(len);
        for(j=0; j<size; j++) M[i*size + j] /= len;
    }
    fclose(f);
    return 0;
}

int similarity(char st1[], FILE *fout){
    long long i,j,k,cn, b=0, c=0, bi[100];
    char st[100][max_size], *bestw[N];
    float len, bestd[N] = {0}, vec[max_size];
    
    i=j=k=cn=0;
    for (i=0; i<N; i++) {
        bestw[i] = (char *)malloc(max_size * sizeof(char));
        bestw[i][0] = 0;
    }
    while(1) {
        st[cn][j++] = st1[k++];
        st[cn][j] = 0;
        if (st1[k] == 0) break;
        if (st1[k] == ' ') {
            cn++;
            j = 0;
            k++;
        }
    }
    cn++;
    for (i=0; i<cn; i++) {
        for ( j=0; j<nwords; j++) {
            if(!strcmp(&vocab[j*max_w], st[i])) break;
        }
        if ( j==nwords ) j=-1;
        bi[i]=j;
        if ( j==-1 ) break;
    }
    if ( j==-1 )  return 0;
    
    //--- Word Cosine distance ---
    memset(vec,0, sizeof(vec));
    for( i=0; i<cn; i++) {
        if( bi[i]==-1 ) continue;
        for( j=0; j<size; j++ ) vec[j] += M[ j + bi[i]*size ];
    }
    len = 0.0;
    for( i=0; i<size; i++ ) len += vec[i]*vec[i];
    len = sqrt(len);
    for( i=0; i<size; i++ ) vec[i] /= len;
    for( i=0; i<N; i++ ) {
        bestd[i] = -1;
        bestw[i][0] = 0;
    }
    for( int k=0; k<nwords; k++ ) {
        i=0;
        for( j=0; j<cn; j++ ) if( bi[j]==k ) i=1;
        if( i==1 ) continue;
        float dist=0;
        for( i=0; i<size; i++ ) dist += vec[i]*M[i + k*size];
        if ( dist<0.3) continue;
        for( i=0; i<N; i++ ) {
            if( dist>bestd[i] ) {
                for( int d=N-1; d>i; d--){
                    bestd[d] = bestd[d-1];
                    strcpy(bestw[d], bestw[d-1]);
                }
                bestd[i] = dist;
                strcpy( bestw[i], &vocab[k*max_w] );
                break;
            }
        }
    }
    for (i = 0; i < N-1; i++) 
        //printf("%s:%f ", bestw[i], bestd[i]);
        fprintf(fout, "%s ", bestw[i]);
    fprintf(fout, "%s", bestw[i]);
}

 
    

int main(int argc, char **argv) {
  FILE *fp, *fout;
  char file_name[max_size];

  if (argc < 4) {
    //printf("Usage: ./distance <FILE>\nwhere FILE contains word projections in the BINARY FORMAT\n");
    printf("Usage: ./distance <file_model> <file_text> <file_out>\n");
    return 0;
  }
  
  strcpy(file_name, argv[1]);
  if (loadModel(file_name)!=0 ) {
      printf("Cannot load model file, please retry.");
      return 0;
  }
  printf("words:%lld, size:%lld\n", nwords, size);

  strcpy(file_name, argv[2]);
  fp = fopen(file_name, "rb");
  strcpy(file_name, argv[3]);
  fout = fopen(file_name, "w");
  if ( fp==NULL || fout==NULL )  return 0;

  char line[max_size]={0};
  size_t length;
  int k = 0;
  while( fgets(line, max_size, fp) != NULL ) {
      length = strlen(line);
      if ( length==0 ) continue;
      if ( line[length-1]=='\n')
          line[strlen(line)-1]=0;
      fprintf(fout, "%s\t", line);
      similarity(line, fout);
      fprintf(fout, "\n");
      if( ++k%100==0 ){
          printf("read words: %d %c", k, 13);
          fflush(stdout);
      }   
  }
  fclose(fp);
  fclose(fout);
  free(vocab);
  free(M);
  return 0;
}
