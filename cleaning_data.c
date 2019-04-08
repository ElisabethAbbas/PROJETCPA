#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define LIGNE_MAX 300
#define SBUF 60

int main(int argc, char** argv){
  if(argc != 2){
    printf("commande : ./cleaning_data fichier\n");
    exit(-1);
  }

  // modification de chaque ligne :
  // a b avec a<b
  // sinon b a
  FILE * f = fopen(argv[1], "r");
  FILE * r = fopen("data.tmp", "w");
  char buf[SBUF];
  int a,b;
  char c;
  while(fgets(buf, SBUF,  f)!=NULL){
    if(!(buf[0]>='0' && buf[0]<='9'))
      continue;
    sscanf(buf, "%d%c%d\n", &a, &c, &b);
    if(a>b){
      fprintf(r, "%d %d\n", b, a);
    }
    else if (a<b){
      fprintf(r, "%d %d\n", a, b);
    }
  }
  fclose(r);
  fclose(f);
  
  // tri numérique
  char sort[LIGNE_MAX]="sort -n -t' ' -k 1 -k 2 data.tmp -o data2.tmp";
  system(sort);
  char uniq[LIGNE_MAX]="uniq data2.tmp > "; 
  strcat(uniq, argv[1]);
  uniq[strlen(uniq)-4]='\0'; // on retire le ".txt"
  strcat(uniq, "-cleaned.txt");
  system(uniq);

  system("rm -f data.tmp");
  system("rm -f data2.tmp");
  
  return 0;
}
