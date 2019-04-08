#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

#define LIGNE_MAX 200

int recherche_max(FILE *f){
  int max=0;
  char buf[LIGNE_MAX];
  int a,b;
  
  while(fgets(buf, LIGNE_MAX, f)!=NULL){
    sscanf(buf, "%d %d\n", &a, &b);
    if(max<a)
      max = a;
    if(max<b)
      max = b;
  }

  return max;
}

char* voisins(char** t, int nombre_noeuds, int n){
  if(n>=nombre_noeuds){
    printf("Le noeud %d n'existe pas.\n", n);
    exit(-1);
  }
  return t[n];
}

void affiche_voisins(char** t, int nombre_noeuds, int n){
  char* voisins_ = voisins(t, nombre_noeuds, n);
  int i,j;

  printf("%d : ", n);
  for(i=0; i<(nombre_noeuds/8); i++)
    for(j=0; j<8; j++){
      if(((voisins_[i])>>j)&1){
	printf("%d ", (i*8+j));
      }
    }
    
  printf("\n");
}

int main(int argc, char** argv){
  char buf[LIGNE_MAX];
  int a, b, i=0;

  time_t t1;
  
  if(argc!=2){
    printf("commande : ./adjacency_matrix fichier\n");
    exit(-1);
  }
  
  FILE* f = fopen(argv[1], "r");

  t1=time(NULL);
  
  int nombre_noeuds = recherche_max(f)+1; // on compte 0
  
  if(f==NULL)
    printf("erreur");
  
  char **tab = (char**)malloc(nombre_noeuds*sizeof(char*));
  
  for(i=0;i<nombre_noeuds; i++){
    tab[i]=(char*)malloc((floor(nombre_noeuds/8)+1)*sizeof(char));
    memset(tab[i], 0, (floor(nombre_noeuds/8)+1)*sizeof(char));	
  }	

  fseek(f, 0, SEEK_SET);
  while(fgets(buf, LIGNE_MAX, f)!=NULL){
    sscanf(buf, "%d %d\n", &a, &b);
    (voisins(tab, nombre_noeuds, a)[b/8])|=(1<<(b%8));
    (voisins(tab, nombre_noeuds, b)[a/8])|=(1<<(a%8));
  }

  char temps[200];
  sprintf(temps, "echo temps : %ld > temps_adj_matrix", time(NULL)-t1);
  system(temps);

  for(i=0; i<nombre_noeuds; i++)
    affiche_voisins(tab, nombre_noeuds, i);
  
  for(i=0; i<nombre_noeuds; i++)
    free(tab[i]);
  
  free(tab);
  fclose(f);
}
