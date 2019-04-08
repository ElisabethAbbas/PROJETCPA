#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define LIGNE_MAX 200

struct graph_metadata{
  unsigned nombre_noeuds;
  unsigned nombre_aretes;
  unsigned noeud_max;
  unsigned *noeuds;
};

struct graph_metadata noeud_max_et_nb_aretes(FILE* f){
  int noeud_max=-1, a, b;
  struct graph_metadata res={0,0};
  char buf[LIGNE_MAX];
  
  fseek(f, 0, SEEK_SET);
  while(fgets(buf, LIGNE_MAX, f)!=NULL){
    sscanf(buf, "%d %d\n", &a, &b); 		
    if(noeud_max<a) noeud_max=a;
    if(noeud_max<b) noeud_max=b;
    res.nombre_aretes++;
  }
  res.noeud_max=noeud_max;

  // ré-indexation des noeuds
  unsigned int *noeuds = (unsigned int*)malloc(sizeof(unsigned int)*(noeud_max+1));
  int i;
  for(i=0; i<=noeud_max; i++)
    noeuds[i]=-1;
  
  fseek(f, 0, SEEK_SET);
  i=0;
  while(fgets(buf, LIGNE_MAX, f)!=NULL){
    sscanf(buf, "%d %d\n", &a, &b); 		
    if(noeuds[a]==-1)
      noeuds[a]=i++;
    if(noeuds[b]==-1)
      noeuds[b]=i++;
  }

  res.noeuds=noeuds;
  res.nombre_noeuds=i;
  
  return res;
}

int main(int argc, char** argv){
  if(argc!=2){
    printf("commande : ./nombre_noeuds_et_aretes fichier\n");
    exit(-1);
  }
  
  FILE* f = fopen(argv[1], "r");
  
  struct graph_metadata metadata = noeud_max_et_nb_aretes(f);
  
  printf("Nombre de noeuds : %d\n", metadata.nombre_noeuds);
  printf("Nombre d'arêtes : %d\n", metadata.nombre_aretes);
  
  fclose(f);
}
