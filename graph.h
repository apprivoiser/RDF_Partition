#include<iostream>
#include<stdio.h>
#include<vector>
#include<unordered_map>
#include<set>
#include<string.h>
#include<string>
#include<queue>
#include<algorithm>
#include<stdlib.h> 
#include<sys/types.h>
#include<sys/stat.h>
#include<unistd.h>
using namespace std;

class graph
{
public:
	graph();
	~graph();
	void init();
	void loadGraph(string txt_name,string tag);
	vector<string> split(string textline,string tag);
	int getParent(int son,vector<int>& fa);
	int getParentMap(int son,unordered_map<int,int>& fa);
	void coarsening();
	void unionEdgeForEnum();
	void enumPre(int preID,long long choice,vector<int> &curParent,vector<int> &curSoncnt,vector<int> &curRank,bool *inValid);
	void unionEdgeForGreed();
	void greed1(vector<int> &choice,vector<int> &curParent,vector<int> &curSonCnt,vector<int> &curRank,vector<bool> &invalid);
	int cal(long long cur);
	void compareCrossingEdgeCnt(long long cur);
	void greed2();
	void FS(string op);
	int DFS(int e,int pre,int c,vector<int> &color);
	int BFS(int e,int pre,int c,vector<int> &color);
	int DFS_Union(int e,vector<int> &pre_set,vector<int> &color);
	int BFS_Union(int e,vector<int> &pre_set,vector<int> &color);
	void unionBlock(vector<int> &choice,int goal);
	void randEntity(string txt_name,string tag);
	void metis(string txt_name,string tag);
	// void update();
	string RDF;	
	int part;

private:
	set<string> predicate;
	unordered_map<string,int> entityToID;
	vector<string> IDToEntity;
	unordered_map<string,int> predicateToID;
	vector<string> IDToPredicate;
	vector<vector<pair<int,int> > >edge;
	vector<pair<int,string> >otherEdge;
	vector<unordered_map<int,int> >coarseningPoint;
	vector<int> entityTriples;
	set<pair<string,int> >finalResult;
	unordered_map<string,int> edge_cnt;
	vector<bool> invalid;
	vector<vector<pair<int, int> > > id_list;
	int triples;
	int entityCnt;
	int preType;
	int limit;	
	long long ans;	
	int crossEgdeCnt;
	int invalidEdgeCnt;																																		
};
