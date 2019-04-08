#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LIGNE_MAX 20

int main(int argc, char** argv){
  FILE* f = fopen("degres.txt", "r");
  FILE* fres = fopen("degree_distribution.txt", "w");
  unsigned int a;
  int i;
  char buf[LIGNE_MAX];

  unsigned int max_deg=0;
  while(fgets(buf, LIGNE_MAX, f)!=NULL){max_deg++;}
  
  long long *degree_distribution = (long long*)malloc((max_deg+1)*sizeof(long long)); 

  if (degree_distribution==NULL){
    printf("erreur malloc");
    exit(1);
  }

  memset(degree_distribution, 0, (max_deg+1)*sizeof(long long));
  
  int nb_degres=0;

  fseek(f, 0, SEEK_SET);
  while(fgets(buf, LIGNE_MAX, f)!=NULL){
    sscanf(buf, "%d\n", &a);	
    degree_distribution[a]++;
    nb_degres++;		
  }
  
  for(i=0; i<nb_degres; i++)
    if(degree_distribution[i]!=0)
      fprintf(fres, "%d %lld\n", i, degree_distribution[i]);
  
  free(degree_distribution);
  fclose(f);
  fclose(fres);
}
