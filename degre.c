#define LIGNE_MAX 60
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char** argv){
  if(argc!=2){
    printf("commande: ./degre fichier\n");
    exit(-1);
  }
  
  FILE* f = fopen(argv[1], "r");
  char buf[LIGNE_MAX];
  int a,b;
  
  // recherche du noeud max
  int max = 0;
  while(fgets(buf, LIGNE_MAX, f)!=NULL){
    sscanf(buf, "%d %d\n", &a, &b);
    max=(max<a)?a:max;
    max=(max<b)?b:max;
  }

  fseek(f, 0, SEEK_SET);
  // mise des degres en mémoire
  unsigned int *degres = (unsigned int*)malloc(sizeof(int)*(max+1));
  memset(degres, 0, sizeof(unsigned int)*(max+1));
  while(fgets(buf, LIGNE_MAX, f)!=NULL){
    sscanf(buf, "%u %u\n", &a, &b);
    degres[a]++;
    degres[b]++;
  }

  // écriture des degres
  FILE* res = fopen("degres.txt", "w");
  int i;
  for(i=0; i<max+1; i++)
    fprintf(res, "%u\n", degres[i]);
  
  free(degres);
  fclose(f);
  fclose(res);
}
