#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#define LIGNE_MAX 200

struct edge{
  int a;
  int b;
};

int main(int argc, char** argv){
  unsigned int nombre_aretes=0;
  time_t t1;
  
  if(argc!=2){
    printf("commande : ./list_of_edges fichier\n");
    exit(-1);
  }

  FILE* f = fopen(argv[1], "r");
  if(f==NULL){
    printf("Erreur ouverture fichier.\n");
    exit(-1);
  } 

  t1 = time(NULL);
  char buf[LIGNE_MAX];
  while(fgets(buf, LIGNE_MAX, f)!=NULL){ nombre_aretes++; }
   
  int a, b, i=0;
  struct edge *tab = (struct edge *)malloc(nombre_aretes*sizeof(struct edge));
   
  fseek(f, 0, SEEK_SET);
  while(fgets(buf, LIGNE_MAX, f)!=NULL){
    sscanf(buf, "%d %d\n", &a, &b);
    tab[i].a=a;
    tab[i].b=b;
    i++;
  }
  char temps[200];
  sprintf(temps, "echo temps : %ld > temps_list_of_edges", time(NULL)-t1);

  system(temps);

  // affichage des arêtes
  for(i=0; i<nombre_aretes; i++)
    printf("%d %d\n", tab[i].a, tab[i].b);

  free(tab);
  fclose(f);
}
