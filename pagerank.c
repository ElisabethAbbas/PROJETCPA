#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>

#define LIGNE_MAX 300
#define ALPHA 0.90

struct graphe {
  long nombre_noeuds;
  unsigned long nombre_aretes;
  long *noeuds;
};

struct topsflops {
  long tops[5];
  long flops[5];
};

struct noms_topsflops{
  char tops[5][LIGNE_MAX];
  char flops[5][LIGNE_MAX];
};

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

unsigned* alloc_degres(FILE* f, struct graphe graphe, unsigned* indegrees){  
  unsigned* degres = malloc(graphe.nombre_noeuds*sizeof(unsigned));
  memset(degres, 0, graphe.nombre_noeuds*sizeof(unsigned));
  long a, b;
  char buf[LIGNE_MAX];
  
  fseek(f, 0, SEEK_SET);
  
  while(fgets(buf, LIGNE_MAX, f)!=NULL){
    if(buf[0]<'0' || buf[0]>'9')
      continue;
    sscanf(buf, "%ld\t%ld\n", &a, &b);
    // graphe dirigé, que les arêtes sortantes comptées
    degres[graphe.noeuds[a]]++;
    indegrees[graphe.noeuds[b]]++;
  }

  return degres;
}

unsigned* free_degres(unsigned* degres){
  free(degres);
}

void alloc_noeuds(FILE* f, struct graphe *graphe){
  long a, b, i=0;
  char buf[LIGNE_MAX];  
  graphe->noeuds = (long*)malloc(sizeof(long)*graphe->nombre_noeuds);

  for(i=0; i<graphe->nombre_noeuds; i++)
    graphe->noeuds[i]=-1;

  i=0;
  fseek(f, 0, SEEK_SET);
  while(fgets(buf, LIGNE_MAX, f)!=NULL){
    if(buf[0]<'0' || buf[0]>'9')
      continue;
    sscanf(buf, "%ld\t%ld\n", &a, &b);
    if(graphe->noeuds[a]==-1)
      graphe->noeuds[a]=i++;
    if(graphe->noeuds[b]==-1)
      graphe->noeuds[b]=i++;
  }
  
  graphe->nombre_noeuds=i-1;
}

void free_noeuds(struct graphe graphe){
  free(graphe.noeuds);
}

struct graphe nombre_noeuds_aretes(FILE* f){
  long noeud_max=-1, a=-1, b=-1;
  struct graphe res={0,0, NULL};
  char buf[LIGNE_MAX];
  
  fseek(f, 0, SEEK_SET);
  while(fgets(buf, LIGNE_MAX, f)!=NULL){
    if(buf[0]<'0' || buf[0]>'9')
      continue;
    sscanf(buf, "%ld\t%ld \n", &a, &b); 		
    if(noeud_max<a) noeud_max=a;
    if(noeud_max<b) noeud_max=b;
    res.nombre_aretes++;
  }
  res.nombre_noeuds=noeud_max+1;
  return res;
}

void init_A(double *A, unsigned taille){
  long i=0;
  double d = (double)1/(double)taille; 
  for(i=0; i<taille; i++)
    A[i]=d;
}

double *alloc_vect(struct graphe graphe){
  double *A = (double*)malloc(graphe.nombre_noeuds*sizeof(double));
  memset(A, 0, graphe.nombre_noeuds*sizeof(double));  
  return A;
}

void free_vect(double *A){
  free(A);
}

int cmpdouble(double f1, double f2, long puissance){
  return ((long)(f1*puissance))==((long)(f2*puissance));
}

struct topsflops tops_et_flops(struct graphe graphe, double *A){
  struct topsflops res;
  int i;

  for(i=0; i<5; i++){
    res.tops[i]=-1;
    res.flops[i]=-1;
  }
  
  for(i=0; i<graphe.nombre_noeuds; i++){
    if(res.tops[0]==-1 || A[res.tops[0]]<A[i]){
      res.tops[4]=res.tops[3];
      res.tops[3]=res.tops[2];
      res.tops[2]=res.tops[1];
      res.tops[1]=res.tops[0];
      res.tops[0]=i;
    }
    else if(res.tops[1]==-1 || A[res.tops[1]]<A[i]){
      res.tops[4]=res.tops[3];
      res.tops[3]=res.tops[2];
      res.tops[2]=res.tops[1];
      res.tops[1]=i;
    }
    else if(res.tops[2]==-1 || A[res.tops[2]]<A[i]){
      res.tops[4]=res.tops[3];
      res.tops[3]=res.tops[2];
      res.tops[2]=i;
    }
    else if(res.tops[3]==-1 || A[res.tops[3]]<A[i]){
      res.tops[4]=res.tops[3];
      res.tops[3]=i;
    }
    else if(res.tops[4]==-1 || A[res.tops[4]]<A[i]){
      res.tops[4]=i;
    }

    if(res.flops[0]==-1 || A[res.flops[0]]>A[i]){
      res.flops[4]=res.flops[3];
      res.flops[3]=res.flops[2];
      res.flops[2]=res.flops[1];
      res.flops[1]=res.flops[0];
      res.flops[0]=i;
    }
    else if(res.flops[1]==-1 || A[res.flops[1]]>A[i]){
      res.flops[4]=res.flops[3];
      res.flops[3]=res.flops[2];
      res.flops[2]=res.flops[1];
      res.flops[1]=i;
    }
    else if(res.flops[2]==-1 || A[res.flops[2]]>A[i]){
      res.flops[4]=res.flops[3];
      res.flops[3]=res.flops[2];
      res.flops[2]=i;
    }
    else if(res.flops[3]==-1 || A[res.flops[3]]>A[i]){
      res.flops[4]=res.flops[3];
      res.flops[3]=i;
    }
    else if(res.flops[4]==-1 || A[res.flops[4]]>A[i]){
      res.flops[4]=i;
    }
  }

  return res;
}

struct noms_topsflops noms_tops_et_flops(FILE* fname, struct graphe graphe, struct topsflops tops_flops){
  char buf[LIGNE_MAX], nom[LIGNE_MAX], *_nom;
  int t;
  long a;
  
  struct noms_topsflops res;
  
  while(fgets(buf, LIGNE_MAX, fname)!=NULL){
    if(buf[0]<'0' || buf[0]>'9')
      continue;
    
    sscanf(buf, "%ld %s\n", &a, nom);

    _nom=strchr(buf, '\t');
    strncpy(nom, _nom+1, (t=strchr(buf, '\n')-_nom-1));
    nom[t]='\0';
    
    if(graphe.noeuds[a]==tops_flops.tops[0])
      strcpy(res.tops[0], nom);
    if(graphe.noeuds[a]==tops_flops.tops[1])
      strcpy(res.tops[1], nom);
    if(graphe.noeuds[a]==tops_flops.tops[2])
      strcpy(res.tops[2], nom);
    if(graphe.noeuds[a]==tops_flops.tops[3])
      strcpy(res.tops[3], nom);
    if(graphe.noeuds[a]==tops_flops.tops[4])
      strcpy(res.tops[4], nom);
    
    if(graphe.noeuds[a]==tops_flops.flops[0])
      strcpy(res.flops[0], nom);
    if(graphe.noeuds[a]==tops_flops.flops[1])
      strcpy(res.flops[1], nom);
    if(graphe.noeuds[a]==tops_flops.flops[2])
      strcpy(res.flops[2], nom);
    if(graphe.noeuds[a]==tops_flops.flops[3])
      strcpy(res.flops[3], nom);
    if(graphe.noeuds[a]==tops_flops.flops[4])
      strcpy(res.flops[4], nom);
  }

  return res;
}

void remplir_cd(FILE* f, unsigned* cd, int taille, struct graphe graphe){
  char buf[LIGNE_MAX];
  memset(cd, 0, taille * sizeof(unsigned));
  int a, b, i;
  fseek(f, 0, SEEK_SET);
  while(fgets(buf, LIGNE_MAX, f)!=NULL){
    if(buf[0]<'0' || buf[0]>'9')
      continue;
    sscanf(buf, "%d %d\n", &a, &b); 		
    cd[graphe.noeuds[a]+1]++;
  }
  
  for(i=1; i<taille; i++)
    cd[i]=cd[i]+cd[i-1];
}

int main(int argc, char** argv){
  FILE* f =fopen("alr21--dirLinks--enwiki-20071018.txt", "r");
  FILE* fname = fopen("alr21--pageNum2Name--enwiki-20071018.txt", "r");

  if(f==NULL || fname==NULL){
    printf("Erreur ouverture fichiers.\n");
    exit(-1);
  }
  
  char buf[LIGNE_MAX];
  long a, b, i, j;
  int continuer;
  double *A, *B, *tmp;
  unsigned *degres;
  unsigned *indegrees;
  
  struct graphe graphe = nombre_noeuds_aretes(f);
  alloc_noeuds(f, &graphe);

  indegrees=(unsigned*)malloc(sizeof(unsigned int)*graphe.nombre_noeuds);
  memset(indegrees, 0, sizeof(unsigned int)*graphe.nombre_noeuds);
  degres = alloc_degres(f, graphe, indegrees);

  A=alloc_vect(graphe);
  init_A(A, graphe.nombre_noeuds);

  B=alloc_vect(graphe);

  fseek(f, 0, SEEK_SET);
  i=0;
  
  struct adj_arr arr;
  arr.n=graphe.nombre_noeuds;
  arr.m=graphe.nombre_aretes;  
  
  int taille_cd = arr.n+1;
  int taille_adj = arr.m;
  
  arr.cd = (unsigned*)malloc(taille_cd*sizeof(unsigned)); // nombre de noeuds +1
  arr.adj= (unsigned*)malloc(taille_adj*sizeof(unsigned)); // somme des degrés = m    
  memset(arr.adj, 0, taille_adj*sizeof(unsigned));
  
  int* d = malloc(taille_cd*sizeof(int));
  memset(d, 0, taille_cd*sizeof(int));	

  remplir_cd(f, arr.cd, taille_cd, graphe);

  // on remplit adj
  fseek(f, 0, SEEK_SET);
  while(fgets(buf, LIGNE_MAX, f)!=NULL){
    if(buf[0]<'0' || buf[0]>'9')
      continue;
    sscanf(buf, "%ld %ld\n", &a, &b); 	
 
    arr.adj[arr.cd[graphe.noeuds[a]]+d[graphe.noeuds[a]]]=graphe.noeuds[b];
    d[graphe.noeuds[a]]++;
  }
    
  int t;
  long ajs;
  int modifie=1;
  for(t=0; /*t<300*/ modifie==1; t++){
    modifie=0;
    
    for(i=0; i<graphe.nombre_noeuds; i++)
      for(j=arr.cd[i] ; j<arr.cd[i+1] ; j++){
	if(degres[i]!=0)
	  B[arr.adj[j]]+=(A[i]/(double)degres[i]);
	else
	  B[arr.adj[j]]+=(A[i]/(double)graphe.nombre_noeuds);
      }	
    
    long norme=0;
    for(i=0; i<graphe.nombre_noeuds; i++){
      B[i]= (1.0-ALPHA)*B[i] + ALPHA/((double)graphe.nombre_noeuds);
      norme += B[i]; 
    }
    
    // normalisation
    for(i=0; i<graphe.nombre_noeuds; i++){
      B[i]+=(1.0-norme)/((double)graphe.nombre_noeuds);
      //if(((int)(A[i] * 1000000.))!=((int)(B[i] * 1000000.)))
      if(A[i]!=B[i])
	modifie=1;
    }
    
    tmp=A;
    A=B;
    B=tmp;
    memset(B, 0, graphe.nombre_noeuds*sizeof(double));
  }

  // parcours de A pour connaître les 5 pagerank les plus élevées
  // et les moins élevées
  struct topsflops tops_flops = tops_et_flops(graphe, A);
  
  // on cherche les noms des tops et flops 5
  struct noms_topsflops noms_tops_flops = noms_tops_et_flops(fname, graphe, tops_flops);
  
  printf("TOP 1 : %s \nTOP 2 : %s \nTOP 3 : %s \nTOP 4 : %s \nTOP 5 : %s \n\nFLOP 1 : %s \nFLOP 2 : %s \nFLOP 3 : %s \nFLOP 4 : %s \nFLOP 5 : %s \n", noms_tops_flops.tops[0], noms_tops_flops.tops[1], noms_tops_flops.tops[2], noms_tops_flops.tops[3], noms_tops_flops.tops[4],
	 noms_tops_flops.flops[0], noms_tops_flops.flops[1], noms_tops_flops.flops[2], noms_tops_flops.flops[3], noms_tops_flops.flops[4]);

  FILE* f1 = fopen("graphe090.txt", "w");
  //FILE* f2 = fopen("graphe2.txt", "w");

  for(i=0; i<arr.n; i++)
    fprintf(f1, "%f\n", A[i]);
    
  /*for(i=0; i<arr.n; i++)
    fprintf(f2, "%f %u\n", A[i], degres[i]);*/

  printf("t=%d\n", t);
  
  fclose(f1);
  //fclose(f2);
 
  free_vect(A);
  free_vect(B);
  free_noeuds(graphe);
  free_degres(degres);
  free(arr.adj);
  free(arr.cd);
  free(d);

  fclose(f);
  fclose(fname);
}
