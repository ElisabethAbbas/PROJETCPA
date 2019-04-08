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

struct label_frequency {
  unsigned int label;
  unsigned int frequency;
};
  

void remplir_cd(FILE* f, unsigned* cd, int taille){
  char buf[LIGNE_MAX];
  memset(cd, 0, taille*sizeof(unsigned));
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


void shuffle(unsigned int* liste, unsigned int taille, int *random){
  int i, j;
  unsigned int tmp;
  for(i=taille-1; i>=0; i--){
    if(i!=0)
      j=rand()%i;
    else
      j=0;
    *random+=1;
    tmp=liste[i];
    liste[i]=liste[j];
    liste[j]=tmp;
  }
}

int get_frequency(unsigned int l, struct label_frequency *labels_freq, int taille_lf){
  int i;
  for(i=0; i<taille_lf; i++)
    if(labels_freq[i].label==l)
      return labels_freq[i].frequency;

  return -1;
}

void ajout_label_freq(struct label_frequency *labels_freq, int *taille_label_freq, unsigned int label){
  int i;
  for(i=0; i<*taille_label_freq; i++)
    if(labels_freq[i].label==label){
      labels_freq[i].frequency++;
      return;
    }
  labels_freq[(*taille_label_freq)].label=label;
  labels_freq[(*taille_label_freq)].frequency++;
  (*taille_label_freq)++;
}

int main(int argc, char** argv){
  if(argc!=2){
    printf("commande : ./label fichier");
    exit(-1);
  }

  int a, b, i=0, random;
  FILE* f =fopen(argv[1], "r");
  char buf[LIGNE_MAX];
  int degre_max=0;
  struct adj_arr arr;
  
  time_t t, t1=time(NULL);
  srand((unsigned) time(&t)*time(NULL)+random);
  
  struct graph_metadata metadata = noeud_max_et_nb_aretes(f);
  arr.n=metadata.nombre_noeuds;
  arr.m=metadata.nombre_aretes;  

  int taille_cd = arr.n+1;
  int taille_adj = arr.m*2; 
  
  arr.cd = (unsigned*)malloc(taille_cd*sizeof(unsigned)); // nombre de noeuds +1
  arr.adj= (unsigned*)malloc(taille_adj*sizeof(unsigned)); // somme des degrés = 2*m    
  memset(arr.adj, 0, taille_adj*sizeof(unsigned));
  
  int* d = malloc(taille_cd*sizeof(unsigned));
  memset(d, 0, taille_cd*sizeof(unsigned));	

  remplir_cd(f, arr.cd, taille_cd);

  // on remplit adj
  fseek(f, 0, SEEK_SET);
  while(fgets(buf, LIGNE_MAX, f)!=NULL){
    sscanf(buf, "%d %d\n", &a, &b); 	
    
    arr.adj[arr.cd[a]+d[a]]=b;
    arr.adj[arr.cd[b]+d[b]]=a;
    d[a]++;
    d[b]++;
    degre_max=(d[a]>degre_max)?d[a]:degre_max;
    degre_max=(d[b]>degre_max)?d[b]:degre_max;
  }
  
  // affichage de i et ses voisins
  int j, k=0;
  /*for(i=0; i<arr.n; i++){
    if(d[i]>0){
    printf("\n%d : ", i);
    for(j=0; j<d[i]; j++)	
    printf("%d ", arr.adj[arr.cd[i]+j]); 
    k++;
    }
    }*/

  unsigned int *noeuds = (unsigned int*)malloc(arr.n*sizeof(unsigned int));
  int *labels = (int*)malloc(arr.n*sizeof(int));

  for(i=0; i<arr.n; i++){
    labels[i]=i;
    noeuds[i]=i;
  }

  struct label_frequency *labels_freq = (struct label_frequency *)malloc(degre_max*sizeof(struct label_frequency));

  int taille_label_freq = 0;

  int modifie = 1;

  while(modifie){
    shuffle(noeuds, arr.n, &random);
    modifie=0;
    for(i=0; i<arr.n; i++){
      memset(labels_freq, 0, degre_max*sizeof(struct label_frequency));
      taille_label_freq=0;
      
      for(j=arr.cd[noeuds[i]]; j<arr.cd[noeuds[i]+1]; j++)
	ajout_label_freq(labels_freq, &taille_label_freq, labels[arr.adj[j]]);
      
      struct label_frequency label_frq_max=labels_freq[0];
      for(k=0; k<taille_label_freq; k++){
	if(labels_freq[k].frequency>label_frq_max.frequency)
	  label_frq_max=labels_freq[k];
      }
  
      if(label_frq_max.label!=labels[noeuds[i]]){
	modifie=1;
	labels[noeuds[i]]=label_frq_max.label;
      }
    }
  }

  /*char temps[200];
  sprintf(temps, "echo temps : %ld > temps_label", time(NULL)-t1);
  system(temps);*/

  //FILE * f_labels = fopen("labels.txt", "a+"); 

  /*  printf("\n\n");
  for(i=0; i<arr.n; i++)
    fprintf(f_labels, "%d %u\n", i, labels[i]);
  */
  unsigned int *labels_uniq = malloc(arr.n*sizeof(unsigned int));
  int nb_labels_uniq = 0;
  unsigned int *nb_labels = malloc(arr.n*sizeof(unsigned int));

  int label_trouve=0;
  
  for(i=0; i<arr.n; i++){
    label_trouve=0;
    for(j=0; j<nb_labels_uniq; j++)
      if(labels_uniq[j]==labels[i]){
	nb_labels[j]++;
	label_trouve=1;
	break;
      }
    if(!label_trouve){
      labels_uniq[nb_labels_uniq]=labels[i];
      nb_labels[nb_labels_uniq]=1;
      nb_labels_uniq++;
    }
  }
  
  FILE * f_nblabels = fopen("nblabels.txt", "a+"); 

  fprintf(f_nblabels, "%d\n", nb_labels_uniq);
  
  free(labels_freq);
  free(noeuds);
  free(labels);
  free(arr.adj);
  free(arr.cd);
  free(d);
  free(nb_labels);
  free(labels_uniq);
  fclose(f);
  //fclose(f_labels);
  fclose(f_nblabels);
}
