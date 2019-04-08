#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#define LIGNE_MAX 200

struct adj_arr {
  unsigned n;
  unsigned m;
  unsigned *cd;
  unsigned *adj;
};

struct core_decomposition {
  int *eta;
  int *c;
};

struct graph_metadata{
  unsigned nombre_noeuds;
  unsigned nombre_aretes;
};

int indice_min_degre(int *sortedt_to_t, int taille){
  return sortedt_to_t[taille-1];
}

int max(int a, int b){
  return (a>b)?a:b;
}

int dG(int v, int *d, int *t_to_sortedt){
  return d[t_to_sortedt[v]];
}

void echanger(int *t, int i1, int i2){
  int d = t[i1];
  t[i1]=t[i2];
  t[i2]=d;
}

int partitionner(int *t, int deb, int fin, int pivot, int *t_correspondance){
  echanger(t, pivot, fin);
  int i, j= deb;

  for(i=deb; i<=fin; i++){
    if(t_correspondance[t[i]]>t_correspondance[t[fin]]){
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

void _tri_rapide(int *t, int deb, int fin, int *t_correspondance){
  int pivot;
  if(deb<fin){
    pivot = choix_pivot(deb, fin);
    pivot = partitionner(t, deb, fin, pivot, t_correspondance);
    _tri_rapide(t, deb, pivot-1, t_correspondance);
    _tri_rapide(t, pivot+1, fin, t_correspondance);
  }
}

void tri_rapide(int *t, int deb, int fin, int *sortedt_to_t, int *t_to_sortedt, int taille){
  _tri_rapide(t, deb, fin, sortedt_to_t);

  int i;
  for(i=0; i<taille; i++)
    t_to_sortedt[sortedt_to_t[i]]=i;
}

void supprimer_sommet(struct adj_arr graphe,
		      int v,
		      int *sommets_supprimes,
		      int *d,
		      int *t_to_sortedt,
		      int *sortedt_to_t,
		      int *nombre_noeuds){
  int i;
  
  sommets_supprimes[v]=1;
  d[t_to_sortedt[v]]=0;
  
  // baisser le degre de 1 des voisins, le degre du noeud supprimé tombe à 0
  // retrier les degres
  printf("%d: ", v);
  for(i=graphe.cd[v]; i<graphe.cd[v+1]; i++){
    if(!sommets_supprimes[graphe.adj[i]]){
      d[t_to_sortedt[graphe.adj[i]]]--;
    }
    printf("%d ", graphe.adj[i]);
  }
  printf("\n");
  
  (*nombre_noeuds)--;
  
  // tri rapide
  tri_rapide(d, 0, *nombre_noeuds-1, sortedt_to_t, t_to_sortedt, *nombre_noeuds);
}

void echangerkco(int *t, int i1, int i2){
  int d = t[i1];
  t[i1]=t[i2];
  t[i2]=d;
}

int partitionnerkco(int *t, int deb, int fin, int pivot, int *tind){
  echangerkco(t, pivot, fin);
  int i, j= deb;

  for(i=deb; i<=fin; i++){
    if(tind[t[i]]<tind[t[fin]]){
      echangerkco(t, i, j);
      j++;
    }      
  }
  
  echangerkco(t, fin, j);
  return j;
}

void _tri_rapidekco(int *t, int deb, int fin, int *tind){
  int pivot;
  if(deb<fin){
    pivot = choix_pivot(deb, fin);
    pivot = partitionnerkco(t, deb, fin, pivot, tind);
    _tri_rapidekco(t, deb, pivot-1, tind);
    _tri_rapidekco(t, pivot+1, fin, tind);
  }
}

void tri_rapidekco(int *t, int deb, int fin, int *sortedt_to_t, int *t_to_sortedt, int taille){
  _tri_rapidekco(t, deb, fin, sortedt_to_t);

  int i;
  for(i=0; i<taille; i++)
    t_to_sortedt[t[i]]=i;
}

struct core_decomposition
kcore(int *t_to_sortedt,
      int *sortedt_to_t,
      int *d,
      struct adj_arr graphe){

  int nombre_noeuds=graphe.n;
  int *d_res = malloc(graphe.n*sizeof(int));
  memcpy(d_res, d, (graphe.n)*sizeof(int));
  int *sommets_supprimes = malloc(graphe.n*sizeof(int));
  memset(sommets_supprimes, 0, graphe.n*sizeof(int));
  struct core_decomposition res;
  res.eta=malloc(graphe.n*sizeof(int));
  res.c=malloc(graphe.n*sizeof(int));
  
  int i = nombre_noeuds; int c=0;
  int v;

  int iii;
  for(iii=0; iii<nombre_noeuds; iii++)
    printf("d_res %d : %d\n", iii, d_res[t_to_sortedt[iii]]);
  
  while(nombre_noeuds!=0){
    v = indice_min_degre(sortedt_to_t, nombre_noeuds);
    c = max(c, dG(v, d_res, t_to_sortedt));
    res.c[v] = c;
    
    supprimer_sommet(graphe, v, sommets_supprimes, d_res, t_to_sortedt, sortedt_to_t, &nombre_noeuds);
    res.eta[v] = i;
    i--;
  }

  free(d_res);
  free(sommets_supprimes);
  return res;
}

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
  memset(cd, 0, taille);
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

float *mkscore(struct adj_arr graphe, int t){
  int i, j, it;
  float *r = malloc(sizeof(int)*graphe.n);
  memset(r, 0, sizeof(int)*graphe.n);

  for(it=0; it<t; it++)
    for(i=0; i<graphe.n; i++)
      for(j=graphe.cd[i]; j<graphe.cd[i+1]; j++)
	if(r[i]<=r[graphe.adj[j]])
	  r[i]++;
	else
	  r[graphe.adj[j]]++;

  for(i=0; i<graphe.n; i++)
    r[i]/=t;
  
  return r; 
}

int main(int argc, char** argv){ 
  if(argc!=3){
    printf("commande : ./kcore fichier [1 for kcore; 2 for density score]");
    exit(-1);
  }
  
  FILE* f =fopen(argv[1], "r");
  char buf[LIGNE_MAX];
  int a, b, i=0;
  srand(time(NULL));
  
  struct adj_arr graphe;
  
  struct graph_metadata metadata = noeud_max_et_nb_aretes(f);
  graphe.n=metadata.nombre_noeuds;
  graphe.m=metadata.nombre_aretes;  

  int taille_cd = graphe.n+1; 
  int taille_adj = 2*graphe.m;
  
  graphe.cd = (unsigned*)malloc(taille_cd*sizeof(unsigned)); // nombre de noeuds +1
  graphe.adj= (unsigned*)malloc(taille_adj*sizeof(unsigned)); // somme des degrés = 2*m    
  memset(graphe.adj, 0, taille_adj * sizeof(unsigned int));
  
  int* d = malloc(taille_cd*sizeof(int));
  memset(d, 0, taille_cd*sizeof(int));	

  remplir_cd(f, graphe.cd, taille_cd);

  // on remplit adj
  fseek(f, 0, SEEK_SET);
  while(fgets(buf, LIGNE_MAX, f)!=NULL){
    sscanf(buf, "%d %d\n", &a, &b); 	
 
    graphe.adj[graphe.cd[a]+d[a]]=b;
    d[a]++;
   
    graphe.adj[graphe.cd[b]+d[b]]=a;
    d[b]++;
  }

  
  // affichage de i et ses voisins
  int k=0;
  /* for(i=0; i<graphe.n; i++){
     printf("\n%d : ", i);
     for(j=0; j<d[i]; j++)	
     printf("%d ", graphe.adj[graphe.cd[i]+j]); 
     }*/

  if(!strcmp(argv[2], "1")){
    // tableau de correspondance entre degre et numero de noeud
    int *sortedt_to_t = malloc(sizeof(int)*metadata.nombre_noeuds);
    int *t_to_sortedt = malloc(sizeof(int)*metadata.nombre_noeuds);
  
    for(i=0; i<graphe.n; i++)
      sortedt_to_t[i]=i;
  
    // tri rapide
    tri_rapide(d, 0, metadata.nombre_noeuds-1, sortedt_to_t, t_to_sortedt, graphe.n);
  
    // affichage de i et ses voisins
    /*for(i=0; i<graphe.n; i++){
      printf("noeud %d a un degre %d\n",  i, d[t_to_sortedt[i]]); 
      k++;
      }*/

    /*printf("indice min degre : %d\n", indice_min_degre(sortedt_to_t, metadata.nombre_noeuds));*/

    struct core_decomposition core_d = kcore(t_to_sortedt, sortedt_to_t, d, graphe);


    for(int i=0; i<graphe.n; i++)
      printf("i: %d, eta: %d, c: %d\n", i, core_d.eta[i], core_d.c[i]);
    
    FILE* f_corevalues = fopen("corevalues.txt", "w");
    
    // average degree density
    printf("average degree density : %f\n", (((double)graphe.m)/((double)graphe.n)));
    printf("edge density : %f\n", (2.0*((double)graphe.m)/((double)(graphe.n*graphe.n-1))));

    int *k_core_ordering = malloc(sizeof(unsigned int)*graphe.n);
    int *k_core_ordering_to_node = malloc(sizeof(unsigned int)*graphe.n);
    
    for(i=0; i<graphe.n; i++)
      k_core_ordering[i]=i;
    
    tri_rapidekco(k_core_ordering, 0, graphe.n-1, core_d.eta, k_core_ordering_to_node, graphe.n);
    
    for(i=0; i<graphe.n; i++)
      fprintf(f_corevalues, "i: %d, eta: %d, c: %d\n", k_core_ordering[i], core_d.eta[k_core_ordering[i]], core_d.c[k_core_ordering[i]]);
    
    free(sortedt_to_t);
    free(t_to_sortedt);
    free(graphe.adj);
    free(graphe.cd);
    free(d);
    free(k_core_ordering);
    free(k_core_ordering_to_node);
    fclose(f);
    fclose(f_corevalues);
  }
  else if(!strcmp(argv[2], "2")){
    float *r;
    r=mkscore(graphe,10);
    for(i=0;i<graphe.n;i++)
      printf("r[%d]=%f\n", i, r[i]);

    free(r);
    free(graphe.adj);
    free(graphe.cd);
    free(d);
    fclose(f);
  }
}
