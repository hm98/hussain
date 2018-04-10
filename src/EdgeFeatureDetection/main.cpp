#include <stdio.h>
#include <stdlib.h>
#include <fstream>

#include "Timer.h"
#include "EdgeMap.h"
#include "EDLib.h"

int ReadImagePGM(char *filename, char **pBuffer, int *pWidth, int *pHeight);
void SaveImagePGM(char *filename, char *buffer, int width, int height);

int main(){

  FILE *fp = fopen("../../outputs/data/edgeSegments.data", "w");

  int width, height;
  unsigned char *srcImg; 
  char *str = (char *)"../../outputs/images/grayscale.pgm";

  if (ReadImagePGM(str, (char **)&srcImg, &width, &height) == 0){
    printf("Failed opening <%s>\n", str);
    return 1;
  } 

  printf("Working on %dx%d image\n", width, height);

  fprintf(fp, "%d %d\n", width, height);

  //-------------------------------- ED Test ------------------------------------
  Timer timer;

  timer.Start();

  EdgeMap *map = DetectEdgesByED(srcImg, width, height, SOBEL_OPERATOR, 36, 8, 1.0);

  timer.Stop();

  printf("ED detects <%d> edge segments in <%4.2lf> ms\n\n", map->noSegments, timer.ElapsedTime());

  // This is how you access the pixels of the edge segments returned by ED
  memset(map->edgeImg, 0, width*height);
  fprintf(fp, "%d\n", map->noSegments);
  for (int i=0; i<map->noSegments; i++){
    fprintf(fp, "%d\n", map->segments[i].noPixels);
    for (int j=0; j<map->segments[i].noPixels; j++){
      int r = map->segments[i].pixels[j].r;
      int c = map->segments[i].pixels[j].c;
      
      fprintf(fp, "%d %d\n", r, c);
      map->edgeImg[r*width+c] = 255;
    } //end-for
  } //end-for

  SaveImagePGM((char *)"../../outputs/images/EdgeFeature.pgm", (char *)map->edgeImg, width, height);
  delete map;

  delete srcImg;

  return 0;
} 

int ReadImagePGM(char *filename, char **pBuffer, int *pWidth, int *pHeight){
   FILE *fp;
   char buf[71];
   int width, height;

   if ((fp = fopen(filename, "rb")) == NULL){
     fprintf(stderr, "Error reading the file %s in ReadImagePGM().\n", filename);
     return(0);
   } 

   fgets(buf, 70, fp);
   bool P2 = false;
   bool P5 = false;

   if      (strncmp(buf, "P2", 2) == 0) P2 = true;
   else if (strncmp(buf, "P5", 2) == 0) P5 = true;

   if (P2 == false && P5 == false){
      fprintf(stderr, "The file %s is not in PGM format in ", filename);
      fprintf(stderr, "ReadImagePGM().\n");
      fclose(fp);
      return 0;
   } //end-if

   do {fgets(buf, 70, fp);} while (buf[0] == '#'); 
   sscanf(buf, "%d %d", &width, &height);
   fgets(buf, 70, fp); 

   *pWidth = width;
   *pHeight = height;

   if (((*pBuffer) = (char *) malloc((*pWidth)*(*pHeight))) == NULL){
      fprintf(stderr, "Memory allocation failure in ReadImagePGM().\n");
      fclose(fp);
      return(0);
   } 

   if (P2){
      int index=0;
      char *p = *pBuffer;
      int col = 0;
      int read = 0;

      while (1){
        int c;
        if (fscanf(fp, "%d", &c) < 1) break;
        read++;

        if (col < *pWidth) p[index++] = (unsigned char)c;

        col++;
        if (col == width) col = 0;
      } 

      if (read != width*height){
        fprintf(stderr, "Error reading the image data in ReadImagePGM().\n");
        fclose(fp);
        free((*pBuffer));
        return(0);
      } 

   } else if (P5){
      int index=0;
      char *p = *pBuffer;
      int col = 0;
      int read = 0;

      while (1){
        unsigned char c;
        if (fread(&c, 1, 1, fp) < 1) break;
        read++;

        if (col < *pWidth) p[index++] = c;

        col++;
        if (col == width) col = 0;
      } //end-while

     if (read != width*height){
        fprintf(stderr, "Error reading the image data in ReadImagePGM().\n");
        fclose(fp);
        free((*pBuffer));
        return(0);
     } 
   } 

   fclose(fp);
   return 1;
} 

void SaveImagePGM(char *filename, char *buffer, int width, int height){
  FILE *fp = fopen(filename, "wb");

  fprintf(fp, "P5\n");
  fprintf(fp, "# Some comment here!\n");
  fprintf(fp, "%d %d\n", width, height);
  fprintf(fp, "255\n");

  // Grayscale image
  fwrite(buffer, 1, width*height, fp);

  fclose( fp );
} 
