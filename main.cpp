#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int SIGMA = 30;

int main(){
   FILE *imageFile;
   int x,y,pixel,height=4,width=4;

   imageFile=fopen("image.ppm","wb");
   if(imageFile==NULL){
      perror("ERROR: Cannot open output file");
      exit(EXIT_FAILURE);
   }

   fprintf(imageFile,"P6\n");               // P6 filetype
   fprintf(imageFile,"%d %d\n",width,height);   // dimensions
   fprintf(imageFile,"255\n");              // Max pixel

   unsigned char pix[]={0,255,255, 0,255,255, 255,255,255, 0,255,255,
                        255,255,255, 255,255,255, 0,255,255, 0,255,255,
                        255,255,255, 255,255,255, 0,255,255, 0,255,255,
                        255,255,255, 255,255,255, 0,255,255, 0,255,255};

   fwrite(pix,1,sizeof(pix),imageFile);

   fclose(imageFile);
   //convert image.ppm -scale 400x result.png
   return 0;
}

class Graph {
   public:
      int totalNode;
      int** matrix;
      Graph(int totalNode, int totalNode){
         matrix = new int* [totalNode];
         for (int i = 0; i<totalNode; i++){
            matrix[i] = new int [totalNode];
            for (int j = 0; j<totalNode; j++){
               matrix[i][j] = 0;
            }
         }
      }
      ~Graph(){
         for(int i=0; i<totalNode; i++){
            delete[] matrix[i];
         }
         delete[] matrix;
      }
};

void imageSegmentation(char pix[], int width, int height){
   // graph, seededImage = buildGraph(pix[]);
}

void buildGraph(char pix[], int height, int width, Graph& graph){
   int K = makeNLinks(pix, height, width, graph);

}

// return max boundary penalty
int makeNLinks(char pix[], int height, int width, Graph& graph){
   int x,y, bp;
   int K = -9999;
   for(int i=0; i<height; i++){
      for(int j=0; j<width; j++){
         x = coordinatToPoint(i,j,width);
         // pixel below
         if(i+1<height){
            y = coordinatToPoint(i+1,j,width);
            bp = boundaryPenalty(pix[x], pix[y]);
            graph.matrix[x][y] = bp;
            graph.matrix[y][x] = bp;
            if(bp>K){
               K = bp;
            }
         }
         // pixel right
         if(j+1<width){
            y = coordinatToPoint(i,j+1,width);
            bp = boundaryPenalty(pix[x], pix[y]);
            graph.matrix[x][y] = bp;
            graph.matrix[y][x] = bp;
            if(bp>K){
               K = bp;
            }
         }
      }
   }
   return K;
}

void plantSeed(char pix[]){
   
}

//from (row,col) to point in image
int coordinatToPoint(int x, int y, int width){
   return (x*width + y)*3;
}

// large when ip-iq < sigma, small otherwise
int boundaryPenalty(char ip, char iq){
   int bp = 100 * exp(- pow(int(ip) - int(iq), 2) / (2 * pow(SIGMA, 2)));
   return bp;
}