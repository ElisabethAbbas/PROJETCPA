#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <assert.h>
#define LIGNE_MAX 60

struct fifo{
  unsigned int *pile;
  int taille;
  int taille_max;
};

int full_fifo(struct fifo f){
  return f.taille == f.taille_max;
}

int empty_fifo(struct fifo f){
  return f.taille==0;
}

int put_fifo(struct fifo *f, unsigned int v){
  if(!full_fifo(*f)){
    f->pile[f->taille++]=v;
    return 0;
  }
  else
    return -1;
}

int pop_fifo(struct fifo *f){
  if(!empty_fifo(*f))
    return f->pile[--(f->taille)];
  return -1;
}

void init_fifo(struct fifo *f, int taille_max){
  f->pile=(unsigned int*)malloc(taille_max*sizeof(unsigned int));
  f->taille=0;
  f->taille_max=taille_max;
}

void kill_fifo(struct fifo f){
  free(f.pile);
}

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

void bfs(struct adj_arr g, unsigned int s, unsigned int *noeuds, int* taille, int affiche){
  int *marques = (unsigned int*)malloc(g.n*sizeof(unsigned int));
  memset(marques, 0, g.n*sizeof(unsigned int));
  struct fifo fifo;
  unsigned int u;
  int j;
  
  init_fifo(&fifo, g.n);
  put_fifo(&fifo, s);
  marques[s]=1;

  assert(!empty_fifo(fifo));
  
  while(!empty_fifo(fifo)){
    u=pop_fifo(&fifo);
    if(affiche)
      printf("%u ", u);
    for(j=g.cd[u]; j<g.cd[u+1]; j++)
      if(!marques[g.adj[j]]){
	put_fifo(&fifo, g.adj[j]);
	marques[g.adj[j]]=1;
      }
  }

  int i;
  for(i=0; i<g.n; i++)
    if(marques[i]){
      noeuds[i]=1;
      (*taille)++;
    }
  
  kill_fifo(fifo);
  free(marques);
}

int noeud_with_min_D(int *D, int *Gw, int taille){
  int i;
  int noeud_min_Du=-1;
  int min_Du=-1;
  for(i=0; i<taille; i++){
    if((D[i]<min_Du || min_Du==-1) && Gw[i]==0){
      min_Du=D[i];
      noeud_min_Du=i;
    }    
  }
  return noeud_min_Du;
}

int supprimer(int *Gw, unsigned u, int *taille_Gw){
  assert(Gw[u]==0);
  Gw[u]=1;
  taille_Gw--;
}

struct diametre {
  unsigned int noeud1;
  unsigned int noeud2;
  int distance;
};

struct diametre Dijkstra(struct adj_arr graphe, int s){
  int u, i, tmp;
  int *D = malloc(graphe.n*sizeof(int));
  int *Gw = malloc(graphe.n*sizeof(int));
  int taille_Gw=graphe.n;
  
  memset(Gw, 0, graphe.n*sizeof(int));
  
  for(u=0; u<graphe.n; u++)
    D[u] = INT_MAX;

  D[s]=0;

  while(taille_Gw>0){
    u = noeud_with_min_D(D, Gw, graphe.n);

    if(u==-1)
      break;
    if(D[u]==INT_MAX)
      break;
    
    assert(D[u]<INT_MAX);
    
    for(i=graphe.cd[u]; i<graphe.cd[u+1]; i++){
      if(Gw[graphe.adj[i]]==1)
	continue;
      tmp=D[u]+1;
      assert(tmp>=0);
      if(tmp<D[graphe.adj[i]])
	D[graphe.adj[i]]=tmp;
    }
    
    supprimer(Gw, u, &taille_Gw);
  }

  int max_Du=0;
  int noeud_max_Du=0;
  for(i=0; i<graphe.n; i++){
    if(D[i]>max_Du && D[i]!=INT_MAX){
      max_Du=D[i];
      noeud_max_Du=i;
    }
  }

  free(Gw);
  free(D);

  struct diametre res;
  res.noeud1=s;
  res.noeud2=noeud_max_Du;
  res.distance=max_Du;
  
  return res;
}

int diametre(struct adj_arr graphe){
  int i;
  int noeud=1;
  struct diametre d;
  for(i=0; i<10; i++){
    d=Dijkstra(graphe, noeud);
    noeud=d.noeud2;
  }

  return d.distance;
}

int pop_min_fifo(struct fifo *fifo, int *D, int taille){
  assert(!empty_fifo(*fifo));
  
  int i;
  int indice_pile_min_Du=-1;
  int min_Du=-1;
  for(i=0; i<fifo->taille; i++){
    if(D[fifo->pile[i]]<min_Du || min_Du==-1){
      min_Du=D[fifo->pile[i]];
      indice_pile_min_Du=fifo->pile[i];
    }    
  }
  
  unsigned int tmp = fifo->pile[taille-1];
  fifo->pile[taille-1] = fifo->pile[indice_pile_min_Du];
  fifo->pile[indice_pile_min_Du] = tmp;
  
  return pop_fifo(fifo);
}

int main(int argc, char** argv){ 
  if(argc!=2){
    printf("commande : ./bfs fichier\n");
    exit(-1);
  }
  
  FILE* f =fopen(argv[1], "r");
  char buf[LIGNE_MAX];
  int a, b, i=0;
  struct adj_arr graphe;
  
  struct graph_metadata metadata = noeud_max_et_nb_aretes(f);
  graphe.n=metadata.nombre_noeuds;
  graphe.m=metadata.nombre_aretes;  
  

  int taille_cd = graphe.n+1;
  int taille_adj = 2*graphe.m;
  
  graphe.cd = (unsigned*)malloc(taille_cd*sizeof(unsigned)); // nombre de noeuds +1
  graphe.adj= (unsigned*)malloc(taille_adj*sizeof(unsigned)); // somme des degrés = 2*m    
  memset(graphe.adj, 0, taille_adj);
  
  int* d = malloc(taille_cd*sizeof(int));
  memset(d, 0, taille_cd);	

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
  /*int j;
    for(i=0; i<graphe.n; i++){
    if(d[i]){
    printf("\n%d : ", i);
    for(j=0; j<d[i]; j++)	
    printf("%d ", graphe.adj[graphe.cd[i]+j]);
    }
    }*/


  // bfs
  //bfs(graphe, 1);

  // diametre
  //printf("Diametre: %d\n", diametre(graphe));

  unsigned int *noeuds = malloc(sizeof(unsigned int)*graphe.n);
  memset(noeuds, 0, sizeof(unsigned int)*graphe.n);
  
  int noeuds_restants=1;
  int taille;
  int taille_max=-1;
  unsigned int noeud_taille_max;
  while(noeuds_restants){
    noeuds_restants=0;
    for(i=0; i<graphe.n; i++)
      if(noeuds[i]==0){
	noeuds_restants=1;
	bfs(graphe, noeuds[i], noeuds, &taille, 0);
	if(taille_max==-1 || taille>taille_max){
	  noeud_taille_max=i;
	  taille_max=taille;
	}
      }
  }

  bfs(graphe, noeud_taille_max, noeuds, &taille, 1);
  printf("noeud : %u", noeud_taille_max);
  printf("taille : %d", taille_max);
  
  free(graphe.adj);
  free(graphe.cd);
  free(noeuds);
  free(d);
  fclose(f);
}
