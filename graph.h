#ifndef GRAPH_H_INCLUDED
#define GRAPH_H_INCLUDED
#include<vector>
#include<map>
#include<stack>
#include<pthread.h>
#include<time.h>
#include<algorithm>
#include<fstream>

using namespace std;


//sort the corepair according to core number by ascent
bool cmp_by_core(const pair<int,int>& v1, const pair<int,int>& v2) {
    if(v1.second == v2.second)
        return v1.first < v2.first;
    else
        return v1.second > v2.second;
}

struct vertex{
    bool visited;
    bool removed;
    int core;
    int mcd;
    int pcd;
    int cd;
    vertex(){
        visited=removed=false;
        core=mcd=pcd=cd=0;
    }
};

class Graph
{

//--------------------------------------------------------------------------
//Variables
//--------------------------------------------------------------------------

/** Number of vertexes and edges */
public: int vertexNum;
        int edgeNum;
		vector<int> allcores;

vector<vertex> vertices;
/** Edges associated with the vertexes stored at this node
edges[u][i] means the i-th neighbor of vertex u*/
vector<vector<int> > edges;

//--------------------------------------------------------------------------
//Constructor
//--------------------------------------------------------------------------
Graph()
{
	vertexNum = 0;
	edgeNum = 0;
}

void init(int vNum, int eNum){
    vertexNum=vNum;
    edgeNum=eNum;
    for(int i=0;i<vertexNum;i++){
        vertex temp=vertex();
        vertices.push_back(temp);
        edges.push_back(vector<int>(0));
    }
}

void clear(){
    vertices.clear();
    edges.clear();
    vertexNum = 0;
	edgeNum = 0;
}

// --------------------------------------------------------------------------
// Methods
// --------------------------------------------------------------------------
/**
 * Adds an edge to the graph.
 *
 * @param from
 *          the starting node of an edge
 * @param to
 *          the final node of an edge
 */

 bool addEdge(int from, int to)
{
    if(from == to)
        return false;
	// Update size,add another  vertex
	while(from+1 > vertexNum) {
		vertexNum ++;
		vertex temp=vertex();
        vertices.push_back(temp);
        edges.push_back(vector<int>(0));
    }
        // Search if the edge is already present; if so, stop here and return
	for (int i = 0; i < edges[from].size(); i++) {
		if (edges[from][i] == to)
			return false;
	}
	// Finally store the edge
	edges[from].push_back(to);
	edgeNum++;
	return true;
}


 bool deleteEdge(int from, int to){
    if(from > vertexNum){
        return false;
    }
    vector <int>::iterator Iter;
    for(Iter=edges[from].begin();Iter!=edges[from].end();Iter++){
      if (*Iter== to){
       edges[from].erase(Iter);
       edgeNum--;
       Iter = edges[from].begin();
       return true;
      }
    }
    //if there is no edge <from, to>
    if(Iter == edges[from].end()){
        return false;
    }
}

/**
*set and get core number of a vertex
**/
 void SetCore(int a, int b){
    vertices[a].core = b;
}

 int GetCore(int u){
   return vertices[u].core;
}

/**
 * Returns the number of nodes in the graph.
 */
 int GetvertexNum()
{
	return vertexNum;
}

 int GetedgeNum()
{
	return edgeNum;
}

/**
*compute core number for the graph, static method
*/
void ComputeCores(){
    int a,b;
    vector<int> bin,pos,vert,deg;
    a = b = 0;
    int n = vertexNum;
    int maxdegree = 0;
    for(int u = 0;u < n;u++){
        pos.push_back(0);
        vert.push_back(0);
        int usize = edges[u].size();
        deg.push_back(usize);
        if(usize > maxdegree){
            maxdegree = usize;
        }
    }
    //cout<<"maxdegree: "<<maxdegree<<endl;
    for(int k = 0;k <= maxdegree;k ++){
        bin.push_back(0);
    }
    for(int u = 0;u < n;u ++){
        bin[deg[u]]++;
    }
    int start  = 0;
    for(int k = 0;k <= maxdegree;k ++){
        int num = bin[k];
        bin[k] = start;
        start += num;
    }
    for(int u = 0;u < n ;u ++){
        pos[u] = bin[deg[u]];
        vert[pos[u]] = u;
        bin[deg[u]]++;
    }
    for(int d = maxdegree;d>0;d--){
        bin[d]=bin[d-1];
    }
    bin[0] = 0;
    int du,pu,pw,w;
    for(int i = 0;i < n;i++){
        int v = vert[i];
        for(int j=0;j<edges[v].size();j++){
            int u = edges[v][j];
            if(deg[u] > deg[v]){
                du = deg[u];
                pu = pos[u];
                pw = bin[du];
                w = vert[pw];
                if(u != w){
                    pos[u] = pw;
                    vert[pu] = w;
                    pos[w] = pu;
                    vert[pw] = u;
                }
                bin[du]++;
                deg[u]--;
            }
        }
    }
    vector<pair<int,int> > corepair;
    for(int i=0;i < n;i++){
        vertices[i].core = deg[i];
    }
	allcores = deg;
}

vector<int> GetAllcores(){
	return allcores;
}

/**
*decrease core numbers
*/
void delCores(){
	for(int i=0;i<vertexNum;i++){
		if((vertices[i].visited)&&(vertices[i].removed)){
			vertices[i].core--;
			allcores[i]--;
		}
	}
}
/**
*increase core numbers
*/
void insCores(){
	for(int i=0;i<vertexNum;i++){
		if((vertices[i].visited)&&(!vertices[i].removed)){
			vertices[i].core++;
			allcores[i]--;
		}
	}
}


/**
*in insertion case compute MCD for vertices whose core number is k
*/
 void computeInsertMcd(int k){
    for(int v = 0;v < vertexNum;v++){
        int corev = vertices[v].core;
        if(corev == k){
            computeMcd(v);
        }
    }
}

/**
*given a superior edge tree, compute the MCD for vertices in the EXPTree
*/
 void computeInsertMcd(vector<pair<int,int> > superioredges){
	int edgenums = superioredges.size();
    map<int,bool> visited;	
    for(int i = 0;i < edgenums;i++){
        stack<int> s;
        int a = superioredges[i].first;
        int b = superioredges[i].second;
        int sizea = edges[a].size();
        int sizeb = edges[b].size();
        int corea = vertices[a].core;
        int coreb = vertices[b].core;
        int r = a;
        int corer = corea;
        int sizer = sizea;
        if(corea > coreb){
            r = b;
            corer = coreb;
            sizer = sizeb;
        }
		if(!visited[r]){
			s.push(r);
			//vertices[r].visited = true;
			visited[r] = true;
			for(int j=0;j<sizer;j++)
			{
				int w = edges[r][j];
				int corew = vertices[w].core;
				if(corew >= corer){
					vertices[r].mcd++;
				}
			}
		   while(!s.empty()){
            int v=s.top();
            s.pop();
            int sizev = edges[v].size();
            int corev = vertices[v].core;
            for(int j=0;j<sizev;j++){
                int p = edges[v][j];
                if(vertices[p].core == corev && !visited[p]){
                    s.push(p);
                    //vertices[p].visited = true;
                    visited[p] = true;
                    if(!vertices[p].mcd){
                        int sizep = edges[p].size();
                        int corep = vertices[p].core;
						for(int k = 0;k < sizep;k ++){
                            int w = edges[p][k];
                            int corew = vertices[w].core;
                            if(corew >= corep){
                                vertices[p].mcd++;
                            }
                        }
                    }
                }
            }
        }
		}
	}

}

/**
*compute MCD for a vertex v
*/
 void computeMcd(int v){
    int corev = vertices[v].core;
    int sizev = edges[v].size();
    for(int j=0;j<sizev;j++)
    {
        int w = edges[v][j];
        int corew = vertices[w].core;
        if(corew >= corev){
            vertices[v].mcd++;
        }
    }
}

/**
*compute PCD for a vertex v, all MCDs are already known
*/
 void computePcd(int v){
    int corev = vertices[v].core;
    int sizev = edges[v].size();
    for(int j=0;j< sizev;j++){
        int w = edges[v][j];
        int corew = vertices[w].core;
        int mcdw = vertices[w].mcd;
        if(corew > corev || (corew == corev && mcdw > corev)){
            vertices[v].pcd++;
        }
    }
}


 void resetVertex(){
    for(int v = 0;v < vertexNum;v++){
        vertices[v].visited=false;
        vertices[v].removed=false;
        vertices[v].mcd=0;
        vertices[v].pcd=0;
        vertices[v].cd=0;
    }
}

void WriteCores(string corefile){
	ofstream fcore(corefile.data());
	vector<pair<int,int> > corepair;
	for(int i=0;i<vertexNum;i++){
		corepair.push_back(make_pair(i,vertices[i].core));
	}
	sort(corepair.begin(),corepair.end(),cmp_by_core);
	for(int i=0;i<corepair.size();i++){
		fcore<<corepair[i].first<<","<<corepair[i].second<<endl;
	}
	fcore.close();
}

};


#endif // GRAPH_H_INCLUDED
