#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <ctime>  
#include <cstring>
#include <queue>
#include <stack>
#include <vector>
#include <array>
#include <iostream>
#include <algorithm>
#include <iterator>

using namespace std;

int SIGMA = 30;
int SOURCE = -2;
int SINK = -1;
int TOTALSEED = 2;
int FORECODE = 255;
int BACKCODE = 0;
int CUTCOLOR [] = {0,0,0};
vector<array<int, 2>> cuts;

int main(){
   //convert image.ppm -scale 400x result.png
   unsigned char pix[]={0,255,255, 0,255,255, 255,255,255, 0,255,255,
                        255,255,255, 255,255,255, 0,255,255, 0,255,255,
                        255,255,255, 255,255,255, 0,255,255, 0,255,255,
                        255,255,255, 255,255,255, 0,255,255, 0,255,255};

   int height = 4;
   int width = sizeof(pix)/3/height;

   createImage(pix, height, width, "image.ppm");

   char pixSigned[width*height*3];
   for(int i = 0; i<width*height*3; i++){
      pixSigned[i] = (signed char) pix[i];
   }

   imageSegmentation(pixSigned, width, height);
   return 0;
}

void createImage(unsigned char pix[], int width, int height, const char* fileName){
   FILE *imageFile;
   int x,y,pixel;

   imageFile=fopen(fileName,"wb");
   if(imageFile==NULL){
      perror("ERROR: Cannot open output file");
      exit(EXIT_FAILURE);
   }

   fprintf(imageFile,"P6\n"); // P6 filetype
   fprintf(imageFile,"%d %d\n",width,height); // dimensions
   fprintf(imageFile,"255\n"); // Max pixel
   fwrite(pix,1,sizeof(pix),imageFile);
   fclose(imageFile);
}

class Graph {
   public:
      int totalNode;
      int** matrix;
      Graph(int totalNode){
         matrix = new int* [totalNode];
         for (int i = 0; i<totalNode; i++){
            matrix[i] = new int [totalNode];
            for (int j = 0; j<totalNode; j++){
               matrix[i][j] = 0;
            }
         }
      }
      Graph(const Graph& g){
         totalNode = g.totalNode;
         matrix = new int* [totalNode];
         for (int i = 0; i<totalNode; i++){
            matrix[i] = new int [totalNode];
            for (int j = 0; j<totalNode; j++){
               matrix[i][j] = g.matrix[i][j];
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
   Graph graph = buildGraph(pix, height, width);
   SOURCE += graph.totalNode;
   SINK += graph.totalNode;
   augmentingPath(graph, SOURCE, SINK);
   char pixCut [width*height*3];
   for (int i=0; i<width*height*3; i++){
      pixCut[i] = pix[i];
   }
   createCut(pixCut, width, height);
}

void createCut(char pixCut[], int width, int height){
   for (int i=0; i<cuts.size(); i++){
      int x = cuts[i][0];
      int y = cuts[i][1];
      if(x != SOURCE && x != SINK && y != SOURCE && y != SINK){
         pixCut[coordinatToPoint(x/width, x%width, width)*3] = CUTCOLOR[0];
         pixCut[coordinatToPoint(x/width, x%width, width)*3+1] = CUTCOLOR[1];
         pixCut[coordinatToPoint(x/width, x%width, width)*3+2] = CUTCOLOR[2];
         pixCut[coordinatToPoint(y/width, y%width, width)*3] = CUTCOLOR[0];
         pixCut[coordinatToPoint(y/width, y%width, width)*3+1] = CUTCOLOR[1];
         pixCut[coordinatToPoint(y/width, y%width, width)*3+2] = CUTCOLOR[2];
      }
   }

   unsigned char pixCutUnsigned[width*height*3];
   for(int i = 0; i<width*height*3; i++){
      pixCutUnsigned[i] = (unsigned char) pixCut[i];
   }
   createImage(pixCutUnsigned, width, height, "imagecut.ppm");
}

Graph buildGraph(char pix[], int height, int width){
   Graph graph(height*width + 2);
   int K = makeNLinks(pix, height, width, graph);
   plantSeed(pix, width, height, graph, K);
   return graph;
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
            bp = boundaryPenalty(pix[x*3], pix[y*3]);
            graph.matrix[x][y] = bp;
            graph.matrix[y][x] = bp;
            if(bp>K){
               K = bp;
            }
         }
         // pixel right
         if(j+1<width){
            y = coordinatToPoint(i,j+1,width);
            bp = boundaryPenalty(pix[x*3], pix[y*3]);
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

void plantSeed(char pix[], int width, int height, Graph& graph, int K){
   srand(time(0));
   int foreSeed = 0, backSeed = 0, r;
   while (foreSeed<TOTALSEED && backSeed<TOTALSEED){
      r = (rand() % width*height);
      if(pix[r*3] == FORECODE && foreSeed<TOTALSEED){
         foreSeed++;
         graph.matrix[graph.totalNode+SOURCE][r] = K;
      }
      if(pix[r*3] == BACKCODE && backSeed<TOTALSEED){
         backSeed++;
         graph.matrix[r][graph.totalNode+SINK] = K;
      }
   }
}

//from (row,col) to point in image
int coordinatToPoint(int x, int y, int width){
   return (x*width + y);
}

// large when ip-iq < sigma, small otherwise
int boundaryPenalty(char ip, char iq){
   int bp = 100 * exp(- pow(int(ip) - int(iq), 2) / (2 * pow(SIGMA, 2)));
   return bp;
}

void augmentingPath(Graph graph, int s, int t){
   int V = graph.totalNode;
   Graph rGraph = graph;
   int parent[V];
   memset(parent, 0, V);
   while(bfs(rGraph, V, s, t, parent)){
      int pathFlow = 9999;
      int v = t, u;
      while (v!=s){
         u = parent[v];
         if(pathFlow>rGraph.matrix[u][v]){
            pathFlow = rGraph.matrix[u][v];
         }
         v = parent[v];
      }
      v = t;
      while (v!=s){
         u = parent[v];
         rGraph.matrix[u][v] -= pathFlow;
         rGraph.matrix[v][u] += pathFlow;
         v = parent[v];
      }
   }
   bool visited[V];
   memset(visited, false, V);
   dfs(rGraph, V, s, visited);

   for(int i=0; i<V; i++){
      for(int j=0; j<V; j++){
         if(visited[i] && !visited[j] && graph.matrix[i][j]){
            cuts.push_back({i,j});
         }
      }
   }
}

bool bfs(Graph rGraph, int V, int s, int t, int parent[]){
   queue <int> q;
   bool visited[V];
   int i;
   memset(visited, false, V);
   q.push(s);
   visited[s] = true;
   parent[s] = -1;
   while(!q.empty()){
      int u = q.pop();
      for (i=0; i<V; i++){
         if(!visited[i] && rGraph.matrix[u][i]>0){
            q.push(i);
            parent[i] = u;
            visited[i]= true;
         }
      }
   }
   return visited[i];
}

void dfs(Graph rGraph, int V, int s, bool visited[]){
   stack <int> st;
   st.push(s);
   while (!st.empty()){
      int v = st.pop();
      if(!visited[v]){
         visited[v] = true;
         for(int i=0; i<V; i++){
            if(rGraph.matrix[v][i]){
               st.push(i);
            }
         }
      }
   }
}