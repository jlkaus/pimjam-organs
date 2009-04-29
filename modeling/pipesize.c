#include <stdio.h>
#include <math.h>
#include <stdlib.h>

int main(int argc, char* argv[]) {

  
  double f_8 = 65.415;
  double l_k = argc>=2?atof(argv[1]):512.0;
  double f_k = f_8/(l_k/8.0);
  int max_k = floor(22050.0/f_k);
  int max_d = argc>=3?atoi(argv[2]):10;
  int all_d = argc>=4?atoi(argv[3]):0;
  float scale_set[] = {1.0, 1.5, 2.0, 2.5, 3.0, 4.0, 4.5, 5.0, 5.5, 6.0, 6.5, 7.0};
  char* family_set[] = {"I","ii","II","iii","III","IV","v","V","vi","VI","vii","VII"};
  char* note_set[] = {"C","Db","D","Eb","E","F","Gb","G","Ab","A","Bb","B"};
  char buffer[1024];

  printf("k\tf\t\tk8\t\ths8\t\tshs8\t\thsd8\t\tfhs8\tscale\t\tfamily\tnote\tlength\n");

  for(int k = 1; k < max_k; ++k) {
    double f = f_k * (double)k;
    double k8 = (8.0L/l_k) * (double)k;
    double length =  (1/(double)k) * l_k;
    float hs8 = 12.0L*log(f/f_8)/log(2.0L);
    float shs8 = rint(hs8);
    float hsd8 = hs8-shs8;
    int fhs8 = 0;
    if(shs8<0.0L) {
      fhs8 = (int)(floor(shs8/12.0L)*-12.0L + shs8);
    } else {
      fhs8 = (int)fmod(shs8,12.0L);
    }
    float scale = scale_set[fhs8];
    char* family = family_set[fhs8];
    char mod = ' ';
    if(hsd8< -0.2L) {
      mod = '-';
    } else if(hsd8 < -0.001L) {
      mod = ',';
    } else if(hsd8 > 0.2L) {
      mod ='+';
    } else if(hsd8 > 0.001L) {
      mod = '.';
    }
    char* note = note_set[fhs8];
    int octave = (int)(2.0L + floor(shs8/12.0L));
    
    sprintf(buffer, "%d\t%f\t%f\t%f\t%f\t%f\t%d\t%f\t%s%c\t%s%c%d\t%f",k,f,k8,hs8,shs8,hsd8,fhs8,scale,family,mod,note,mod, octave,length);

    // find string length
    int fa = (int)floor(length);
    float ff = length - (double)fa;
    int fb = 0;
    int fc = 0;
    int i = 1;
    for(i = 1; i < max_d; ++i) {
      if(ff*(double)i - floor(ff*(double)i) < 0.00001L) {
	fb = (int)floor(ff*(double)i);
	fc = i;
	break;
      }
    }
    
    if(i == 1) {
      printf("%s\t%d\n",buffer,fa);
    } else if(i == max_d) {
      if(all_d) {
	printf("%s\t\t\t\t\t\t%f20\n",buffer,length);
      }
    } else if(fa == 0) {
      printf("%s",buffer);
      if(fc > 17) {
	printf("\t\t");
      }
      if(fc > 9) {
	printf("\t\t");
      }
      printf("\t%d/%d\n",fb,fc);
    } else {
      printf("%s",buffer);
      if(fc > 17) {
	printf("\t\t");
      }
      if(fc > 9) {
	printf("\t\t");
      }
      printf("\t%d-%d/%d\n",fa,fb,fc);
    }
  }

  return 0;
}
