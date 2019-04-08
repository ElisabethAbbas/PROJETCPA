#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#define LIGNE_MAX 200

struct adj_arr {
  unsigned n;
  unsigned m;
  unsigned *cd;
  unsigned *adj;
};

struct graph_metadata{
  unsigned nombre_noeuds;
  unsigned nombre_aretes;
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
  res.nombre_noeuds=noeud_max+1;
  return res;
}

void remplir_cd(FILE* f, unsigned* cd, int taille){
  char buf[LIGNE_MAX];
  memset(cd, 0, taille * sizeof(unsigned));
  int a, b, i;
  fseek(f, 0, SEEK_SET);
  while(fgets(buf, LIGNE_MAX, f)!=NULL){
    sscanf(buf, "%d %d\n", &a, &b); 		
    cd[a+1]++;
    cd[b+1]++;
  }
  for(i=1; i<taille; i++)
    cd[i]=cd[i]+cd[i-1];

}

int main(int argc, char** argv){ 
  if(argc!=2){
    printf("commande : ./adjacency_array fichier\n");
    exit(-1);
  }
  
  FILE* f =fopen(argv[1], "r");
  char buf[LIGNE_MAX];
  int a, b;
  struct adj_arr arr;

  time_t t1=time(NULL);
  
  struct graph_metadata metadata = noeud_max_et_nb_aretes(f);
  arr.n=metadata.nombre_noeuds;
  arr.m=metadata.nombre_aretes;  

  int taille_cd = arr.n+1;
  int taille_adj = 2*arr.m;
  
  arr.cd = (unsigned*)malloc(taille_cd*sizeof(unsigned)); // nombre de noeuds +1
  arr.adj= (unsigned*)malloc(taille_adj*sizeof(unsigned)); // somme des degrés = 2*m    
  memset(arr.adj, 0, taille_adj*sizeof(unsigned));
  
  int* d = malloc(taille_cd*sizeof(int));
  memset(d, 0, taille_cd*sizeof(int));	

  remplir_cd(f, arr.cd, taille_cd);

  // on remplit adj
  fseek(f, 0, SEEK_SET);
  while(fgets(buf, LIGNE_MAX, f)!=NULL){
    sscanf(buf, "%d %d\n", &a, &b); 	
 
    arr.adj[arr.cd[a]+d[a]]=b;
    d[a]++;
   
    arr.adj[arr.cd[b]+d[b]]=a;
    d[b]++;
  }

  char temps[200];
  sprintf(temps, "echo temps : %ld > temps_adj_array", time(NULL)-t1);  
  system(temps);

  // affichage de i et ses voisins
  int i, j, k=0;
  for(i=0; i<arr.n; i++){
    printf("\n%d : ", i);
    for(j=0; j<d[i]; j++)	
      printf("%d ", arr.adj[arr.cd[i]+j]); 
    k++;
  }
  
  free(arr.adj);
  free(arr.cd);
  free(d);
  fclose(f);
}
