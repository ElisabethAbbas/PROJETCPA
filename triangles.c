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
  memset(cd, 0, taille*sizeof(unsigned int));
  int a, b, i;
  fseek(f, 0, SEEK_SET);
  while(fgets(buf, LIGNE_MAX, f)!=NULL){
    sscanf(buf, "%d %d\n", &a, &b); 		
    if(b<a)
      cd[a+1]++;
    if(a<b)
      cd[b+1]++;
  }
  for(i=1; i<taille; i++)
    cd[i]=cd[i]+cd[i-1];

}

void echanger(unsigned int *t, int i1, int i2){
  int d = t[i1];
  t[i1]=t[i2];
  t[i2]=d;
}

int partitionner(unsigned int *t, int deb, int fin, int pivot){
  echanger(t, pivot, fin);
  int i, j= deb;

  for(i=deb; i<=fin; i++){
    if(t[i]<t[fin]){
      echanger(t, i, j);
      j++;
    }      
  }
  
  echanger(t, fin, j);
  return j;
}

int choix_pivot(int deb, int fin){
  return (int)((rand()%(fin-deb+1))+deb);
}

void tri_rapide(unsigned int *t, int deb, int fin){
  int pivot;
  if(deb<fin){
    pivot = choix_pivot(deb, fin);
    pivot = partitionner(t, deb, fin, pivot);
    tri_rapide(t, deb, pivot-1);
    tri_rapide(t, pivot+1, fin);
  }
}

int recherche_dichotomique(unsigned int valeur, unsigned int *tab, int deb, int fin){
  if(deb==fin){
    if(tab[deb]==valeur)
      return 1; // oui
    else
      return 0; // non
  }
  else if(deb>fin)
    return 0;
  
  int m = (int) ((fin-deb)/2.0) + deb;
  if(tab[m]==valeur) 
    return 1; // oui
  if(tab[m] > valeur)
    return recherche_dichotomique(valeur, tab, deb, m-1) ;
  else
    return recherche_dichotomique(valeur, tab, m+1, fin) ;
}

int output_triangles(struct adj_arr graphe, unsigned int u, unsigned int v){
  unsigned int i;
  int triangles = 0;
  for(i=graphe.cd[u]; i<graphe.cd[u+1]; i++)
    if(recherche_dichotomique(graphe.adj[i], graphe.adj, graphe.cd[v], graphe.cd[v+1]-1)){
      //printf("%u %u %u\n", u, v, graphe.adj[i]);
      triangles++;
    }
  
  return triangles;
}

int trlist(struct adj_arr graphe){
  int triangles=0;
  int j;
  unsigned int u=0;
  for(u=0; u<graphe.n; u++){
    for(j=graphe.cd[u]; j<graphe.cd[u+1]; j++)
      triangles+=output_triangles(graphe, u, graphe.adj[j]);
  }

  return triangles;
}

int main(int argc, char** argv){
  srand(time(NULL));
  
  if(argc!=2){
    printf("commande : ./adjacency_array fichier\n");
    exit(-1);
  }
  
  FILE* f =fopen(argv[1], "r");
  char buf[LIGNE_MAX];
  int a, b, i=0;
  struct adj_arr graphe;

  time_t t1=time(NULL);
  
  struct graph_metadata metadata = noeud_max_et_nb_aretes(f);
  graphe.n=metadata.nombre_noeuds;
  graphe.m=metadata.nombre_aretes;  
  

  int taille_cd = graphe.n+1;
  int taille_adj = graphe.m; // car on ne met que les voisins plus petits
  
  graphe.cd = (unsigned*)malloc(taille_cd*sizeof(unsigned)); // nombre de noeuds +1
  graphe.adj= (unsigned*)malloc(taille_adj*sizeof(unsigned)); // somme des degrés = m, car on ne mets que les voisins plus petits    
  memset(graphe.adj, 0, taille_adj*sizeof(unsigned int));
  
  int* d = malloc(taille_cd*sizeof(int));
  memset(d, 0, taille_cd*sizeof(int));	

  remplir_cd(f, graphe.cd, taille_cd);

  // on remplit adj
  fseek(f, 0, SEEK_SET);
  while(fgets(buf, LIGNE_MAX, f)!=NULL){
    sscanf(buf, "%d %d\n", &a, &b); 	


    // pour compter les triangles, on ne mets que les voisins plus petits que le noeuds
    if(b<a){
      graphe.adj[graphe.cd[a]+d[a]]=b;
      d[a]++;
    }
    else if(a<b){
      graphe.adj[graphe.cd[b]+d[b]]=a;
      d[b]++;
    }
  }

  for(i=0; i<graphe.n; i++)
    tri_rapide(graphe.adj, graphe.cd[i], graphe.cd[i+1]-1); // pas sûre du -1
  
  // affichage de i et ses voisins
  //int j, k=0;
  /*for(i=0; i<graphe.n; i++){
    printf("\n%d : ", i);
    for(j=0; j<d[i]; j++)	
    printf("%d ", graphe.adj[graphe.cd[i]+j]); 
    k++;
    }*/

  printf("%d", trlist(graphe));

  char temps[200];
  sprintf(temps, "echo temps : %ld > temps_triangles", time(NULL)-t1);
  system(temps);
  
  free(graphe.adj);
  free(graphe.cd);
  free(d);
  fclose(f);
}

