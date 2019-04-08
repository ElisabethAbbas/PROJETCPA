#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define LIGNE_MAX 20

int sum(unsigned int *tab, int taille){
  int i=0;
  int res=0;	
  while(i<taille)
    res+=tab[i++];
  return res;
}

int main(int argc, char** argv){
  FILE* f = fopen("degres.txt", "r");
  char buf[LIGNE_MAX];	

  int nombre_noeuds=0;
  while(fgets(buf, LIGNE_MAX, f)!=NULL){nombre_noeuds++;}; 
  
  unsigned int *degres = malloc(nombre_noeuds*sizeof(unsigned int));

  if(degres==NULL){
    printf("erreur malloc");
    exit(1);
  }
  
  int i=0;
  unsigned int a;
  fseek(f, 0, SEEK_SET);
  while(fgets(buf, LIGNE_MAX, f)!=NULL){
    sscanf(buf, "%u\n", &a);	
    degres[i++]=a;		
  }
	
  int s = sum(degres, nombre_noeuds);	
  long long res = 0;
  i=0;
  int tmp = s;
  while(i<nombre_noeuds-1){
    tmp=(tmp-degres[i]);
    res+=tmp*degres[i];
    i++;
  }	

  printf("Q = %lld\n", res);
	
  free(degres);
  fclose(f);
}
