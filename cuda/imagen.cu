#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>       /* round() */

typedef struct bmpFileHeader
{
  /* 2 bytes de identificación */
  unsigned int size;        /* Tamaño del archivo */
  unsigned short resv1;       /* Reservado */
  unsigned short resv2;       /* Reservado */
  unsigned int offset;      /* Offset hasta hasta los datos de imagen */
} bmpFileHeader;

typedef struct bmpInfoHeader
{
  unsigned int headersize;  /* Tamaño de la cabecera */
  unsigned int width;       /* Ancho */
  unsigned int height;      /* Alto */
  unsigned short planes;      /* Planos de color (Siempre 1) */
  unsigned short bpp;         /* bits por pixel */
  unsigned int compress;    /* compresión */
  unsigned int imgsize;     /* tamaño de los datos de imagen */
  unsigned int bpmx;        /* Resolución X en bits por metro */
  unsigned int bpmy;        /* Resolución Y en bits por metro */
  unsigned int colors;      /* colors used en la paleta */
  unsigned int imxtcolors;  /* Colores importantes. 0 si son todos */
} bmpInfoHeader;

unsigned char *LoadBMP(char *filename, bmpInfoHeader *bInfoHeader);
void DisplayInfo(bmpInfoHeader *info);
void TextDisplay(bmpInfoHeader *info, unsigned char *img);

void SaveBMP(char *filename, bmpInfoHeader *info, unsigned char *imgdata);

__global__
void escalaGrises(unsigned char *img, unsigned char *grayimg, int numRows, int numCols)
{
  int Col = threadIdx.x + blockIdx.x*blockDim.x;
  int Row = threadIdx.y + blockIdx.y*blockDim.y;

  if(Col < numCols && Row < numRows)
  {
    int grayOffset = Col + Row*numCols;
    int rgbOffset = grayOffset*3;
    unsigned char r = img[rgbOffset + 2];
    unsigned char g = img[rgbOffset + 1];
    unsigned char b = img[rgbOffset    ];
    grayimg[grayOffset] = 0.21f*r + 0.71f*g + 0.07*b;
  }
}

int main()
{
  bmpInfoHeader info;
  unsigned char *img, *im2;
  int blockThreadSize, numberOfBlocks;
  int numRows, numCols;

  img = LoadBMP("magia_sonrisa.bmp", &info);
  DisplayInfo(&info);
  TextDisplay(&info, img);
  numRows = info.height;
  numCols = info.width;

  blockThreadSize = 32;
  numberOfBlocks = 1 + ((numRows - 1) / blockThreadSize);

  const int isize = (numRows*numCols)*sizeof(char);
  cudaMalloc((void**)&im2, isize);

  const dim3 blockSize(blockThreadSize, blockThreadSize, 1);
  const dim3 gridSize(numberOfBlocks , ceil(numCols/blockThreadSize), 1);
  escalaGrises<<<gridSize, blockSize>>>(img, im2, numRows, numCols);

  cudaMemcpy(img, im2, isize, cudaMemcpyDeviceToHost);
  cudaFree(im2);

  SaveBMP("resultado.bmp", &info, img);

  free(img);

  return 0;
}

void TextDisplay(bmpInfoHeader *info, unsigned char *img)
{
  int x, y;
  /* Reducimos la resolución vertical y horizontal para que la imagen entre en pantalla */
  static const int reduccionX=6, reduccionY=4;
  /* Si la componente supera el umbral, el color se marcará como 1. */
  static const int umbral=90;
  /* Asignamos caracteres a los colores en pantalla */
  static unsigned char colores[9]=" bgfrRGB";
  int r,g,b;

  /* Dibujamos la imagen */
  for (y=info->height; y>0; y-=reduccionY)
  {
    for (x=0; x<info->width; x+=reduccionX)
    {
      b=(img[3*(x+y*info->width)]>umbral);
      g=(img[3*(x+y*info->width)+1]>umbral);
      r=(img[3*(x+y*info->width)+2]>umbral);

      printf("%c", colores[b+g*2+r*4]);
    }
    printf("\n");
  }
}

unsigned char *LoadBMP(char *filename, bmpInfoHeader *bInfoHeader)
{

  FILE *f;
  bmpFileHeader header;     /* cabecera */
  unsigned char *imgdata;   /* datos de imagen */
  unsigned short type;            /* 2 bytes identificativos */

  f=fopen (filename, "r");
  if (!f)
    return NULL;            /* Si no podemos leer, no hay imagen*/

  /* Leemos los dos primeros bytes */
  fread(&type, sizeof(unsigned short), 1, f);
  if (type !=0x4D42)        /* Comprobamos el formato */
  {
    fclose(f);
    return NULL;
  }

  /* Leemos la cabecera de fichero completa */
  fread(&header, sizeof(bmpFileHeader), 1, f);

  /* Leemos la cabecera de información completa */
  fread(bInfoHeader, sizeof(bmpInfoHeader), 1, f);

  /* Reservamos memoria para la imagen, ¿cuánta? 
     Tanto como indique imgsize */
  imgdata=(unsigned char*)malloc(bInfoHeader->imgsize);

  /* Nos situamos en el sitio donde empiezan los datos de imagen,
   nos lo indica el offset de la cabecera de fichero*/
  fseek(f, header.offset, SEEK_SET);

  /* Leemos los datos de imagen, tantos bytes como imgsize */
  fread(imgdata, bInfoHeader->imgsize,1, f);

  /* Cerramos */
  fclose(f);

  /* Devolvemos la imagen */
  return imgdata;
}

void DisplayInfo(bmpInfoHeader *info)
{
  printf("Tamaño de la cabecera: %u\n", info->headersize);
  printf("Anchura: %d\n", info->width);
  printf("Altura: %d\n", info->height);
  printf("Planos (1): %d\n", info->planes);
  printf("Bits por pixel: %d\n", info->bpp);
  printf("Compresión: %d\n", info->compress);
  printf("Tamaño de datos de imagen: %u\n", info->imgsize);
  printf("Resolucón horizontal: %u\n", info->bpmx);
  printf("Resolucón vertical: %u\n", info->bpmy);
  printf("Colores en paleta: %d\n", info->colors);
  printf("Colores importantes: %d\n", info->imxtcolors);
}

void SaveBMP(char *filename, bmpInfoHeader *info, unsigned char *imgdata)
{
  bmpFileHeader header;
  FILE *f;
  unsigned short type;
  
  f=fopen(filename, "w+");
  header.size = info->imgsize + sizeof(bmpFileHeader) + sizeof(bmpInfoHeader);
  /* header.resv1=0; */
  /* header.resv2=1; */
  
  /* El offset será el tamaño de las dos cabeceras + 2 (información de fichero)*/
  header.offset = sizeof(bmpFileHeader) + sizeof(bmpInfoHeader) + 2;
  
  /* Escribimos la identificación del archivo */
  type = 0x4D42;
  fwrite(&type, sizeof(type),1,f);
  
  /* Escribimos la cabecera de fichero */
  fwrite(&header, sizeof(bmpFileHeader),1,f);
  
  /* Escribimos la información básica de la imagen */
  fwrite(info, sizeof(bmpInfoHeader),1,f);
  
  /* Escribimos la imagen */
  fwrite(imgdata, info->imgsize, 1, f);
  
  fclose(f);
}