#include"graph.h"
#include<fstream>

graph::graph(){}

graph::~graph(){}

void 
graph::init()
{
	vector<pair<int,int> >tmp;
	edge.push_back(tmp);
	entityTriples.push_back(0);
	IDToEntity.push_back("");
	IDToPredicate.push_back("");
	preType=entityCnt=triples=invalidEdgeCnt=0;
}
void 
graph::loadGraph(string txt_name,string tag)
{
	ifstream in(txt_name.data());
	vector<pair<int,int> >tmp;
	string line;
	while(getline(in,line))
	{
		triples++;
		line.resize(line.length()-2);
		vector<string> s;
		s=split(line,tag);
		predicate.insert(s[1]);
		for(int i=0;i<3;i+=2)if((s[i][0]=='<'||s[i][0]=='_')&&entityToID.count(s[i])==0)
		{
			entityToID[s[i]]=++entityCnt;
			IDToEntity.push_back(s[i]);
			entityTriples.push_back(0);
		}
		edge_cnt[s[1]]++;
		int a=entityToID[s[0]];
		entityTriples[a]++;
		if((s[0][0]=='<'||s[0][0]=='_')&&(s[2][0]=='<'||s[2][0]=='_'))
		{
			if(predicateToID.count(s[1])==0)
			{
				predicateToID[s[1]]=++preType;
				IDToPredicate.push_back(s[1]);
				edge.push_back(tmp);
			}		
			int b=entityToID[s[2]];
			edge[predicateToID[s[1]]].push_back(make_pair(a,b));	
			entityTriples[b]++;
		}
	}
	in.close();


	limit=entityCnt/part/2;
	printf("limit: %d\n",limit);
	printf("triples: %d\n", triples);
	printf("entityCnt: %d\n",entityCnt);
	printf("predicate: %d\n", predicate.size());

	printf("entity->preType: %d\n",preType);
}

vector<string> 
graph::split(string textline,string tag)
{
	vector<string> res;
	std::size_t pre_pos = 0;
	std::size_t pos = textline.find(tag);
	while (pos != std::string::npos)
	{
		string curStr = textline.substr(pre_pos, pos - pre_pos);
		curStr.erase(0, curStr.find_first_not_of("\r\t\n "));
		curStr.erase(curStr.find_last_not_of("\r\t\n ") + 1);
		if(strcmp(curStr.c_str(), "") != 0)
			res.push_back(curStr);
		pre_pos = pos + tag.size();
		pos = textline.find(tag, pre_pos);
	}

	string curStr = textline.substr(pre_pos, pos - pre_pos);
	curStr.erase(0, curStr.find_first_not_of("\r\t\n "));
	curStr.erase(curStr.find_last_not_of("\r\t\n ") + 1);
	if(strcmp(curStr.c_str(), "") != 0)
		res.push_back(curStr);

	return res;
}

int 
graph::getParent(int son,vector<int> &fa)
{
	// return fa[son]==son?son:fa[son]=getParent(fa[son],fa);
	int i,j,k;
	k=son;
	while(k!=fa[k])k=fa[k];
	i=son;
	while(i!=k)
	{
		j=fa[i];
		fa[i]=k;
		i=j;
	}
	return k;
}

int 
graph::getParentMap(int son,unordered_map<int,int> &fa)
{
	int i,j,k;
	k=son;
	while(k!=fa[k])k=fa[k];
	i=son;
	while(i!=k)
	{
		j=fa[i];
		fa[i]=k;
		i=j;
	}
	return k;
}

void 
graph::coarsening()
{
	invalid=vector<bool>(preType+1,0);
	coarseningPoint=vector<unordered_map<int,int> >(preType+1,unordered_map<int,int>());
	for(int preID=1;preID<=preType;preID++)
	{
		vector<int> sonCnt=vector<int>(entityCnt+1,1);
		vector<int> rank=vector<int>(entityCnt+1,0);
		for(int p=0;p<edge[preID].size();p++)
		{
			int A=edge[preID][p].first,B=edge[preID][p].second;
			if(coarseningPoint[preID].count(A)==0)
				coarseningPoint[preID].insert(make_pair(A,A));
			if(coarseningPoint[preID].count(B)==0)
				coarseningPoint[preID].insert(make_pair(B,B));

			int parentA=getParentMap(A,coarseningPoint[preID]),parentB=getParentMap(B,coarseningPoint[preID]);

			if(rank[parentA]<rank[parentB])swap(parentA,parentB);
			if(parentA!=parentB)
			{
				coarseningPoint[preID][parentB]=parentA;
				sonCnt[parentA]+=sonCnt[parentB];
				rank[parentA]=max(rank[parentA],rank[parentB]+1);
				if(sonCnt[parentA]>limit)
				{
					invalid[preID]=1;
					invalidEdgeCnt++;
					printf("invalid: %d %s\n",preID,IDToPredicate[preID].data());
					break;
				}
			}
		}
	}	
}
void 
graph::unionEdgeForEnum()
{
	coarsening();
	bool *invalidST=new bool[1<<preType]();
	for(int i=1;i<=preType;i++)if(invalid[i])invalidST[1<<(i-1)]=1;

	ans=0;
	vector<int> choice(preType+1,0);
	vector<int> parent=vector<int>(entityCnt+1);
	for(int i=1;i<=entityCnt;i++)parent[i]=i;
	vector<int> sonCnt=vector<int>(entityCnt+1,1);
	vector<int> rank=vector<int>(entityCnt+1,0);

	crossEgdeCnt=preType;
	printf("enumPre\n");
	enumPre(0,0,parent,sonCnt,rank,invalidST);

	printf("crossEgdeCnt: %d\n", preType-cal(ans));
	for(int i=1;i<=preType;i++)
	{
		choice[i]=((1<<(i-1))&ans)?1:0;
		if(choice[i]==0)cout<<i<<"	"<<IDToPredicate[i]<<endl;
	}
	printf("\n");
	unionBlock(choice,part);
	delete[] invalidST;
}

void 
graph::enumPre(int preID,long long choice,vector<int> &curParent,vector<int> &curSonCnt,vector<int> &curRank,bool *invalidST)
{
	if(cal(ans)>=cal(choice)+preType-preID-invalidEdgeCnt)return;
	if(preID==preType)
	{
		printf("%lld\n",choice);
		compareCrossingEdgeCnt(choice);
		return;
	}
	preID++;

	long long nextchoice=choice|(1LL<<(preID-1));
	bool flag=invalidST[nextchoice]|invalidST[1LL<<(preID-1)];
	if(!flag)
	{
		vector<int> nextFa(curParent);
		vector<int> nextSonCnt(curSonCnt);
		vector<int> nextRank(curRank);
		unordered_map<int,int>::iterator it;
		for(it=coarseningPoint[preID].begin();it!=coarseningPoint[preID].end();it++)
		{
			int point=it->first;
			int parentA=getParent(point,nextFa),parentB=getParent(getParentMap(point,coarseningPoint[preID]),nextFa);
			if(nextRank[parentA]<nextRank[parentB])swap(parentA,parentB);
			if(parentA!=parentB)
			{
				nextFa[parentB]=parentA;
				nextSonCnt[parentA]+=nextSonCnt[parentB];
				nextRank[parentA]=max(nextRank[parentA],nextRank[parentB]+1);
				if(nextSonCnt[parentA]>limit)
				{
					flag=1;
					break;
				}
			}
		}
		if(!flag)enumPre(preID,nextchoice,nextFa,nextSonCnt,nextRank,invalidST);
	}
	if(flag)
	{
		for(int i=1;i<=preType;i++)invalidST[(1LL<<(i-1))|nextchoice]=1;
	}
	enumPre(preID,choice,curParent,curSonCnt,curRank,invalidST);
}

void 
graph::unionEdgeForGreed()
{
	coarsening();
	ans=0;
	vector<int> choice(preType+1,0);
	vector<int> parent=vector<int>(entityCnt+1);
	for(int i=1;i<=entityCnt;i++)parent[i]=i;
	vector<int> sonCnt=vector<int>(entityCnt+1,1);
	vector<int> rank=vector<int>(entityCnt+1,1);
	printf("greed1\n");

	invalid=vector<bool>(preType+1,0);
	int threshold=entityCnt*0.0001;

	int optim=0;
    for(int preID=1;preID<=preType;preID++)
	    if(edge_cnt[IDToPredicate[preID]]<threshold)
	    {
	    	for(int p=0;p<edge[preID].size();p++)
	        {

	        	int A=edge[preID][p].first,B=edge[preID][p].second;
	            int parentA=getParent(A,parent),parentB=getParent(B,parent);

	            if(rank[parentA]<rank[parentB])swap(parentA,parentB);
	            if(parentA!=parentB)
	            {
                    parent[parentB]=parentA;
                    sonCnt[parentA]+=sonCnt[parentB];
                    rank[parentA]=max(rank[parentA],rank[parentB]+1);
	            }
	        }
	        choice[preID]=1;optim++;
	    }
	printf("opt: %d\n", optim);
	greed1(choice,parent,sonCnt,rank,invalid);

	int crossEdge=0;
	for(int preID=1;preID<=preType;preID++)
		if(choice[preID]==0)cout<<preID<<"	"<<IDToPredicate[preID]<<endl,crossEdge++;
	printf("crossEdge: %d\n",crossEdge);
	printf("\n");
	unionBlock(choice,part);
}
void graph::greed1(vector<int> &choice,vector<int> &curParent,vector<int> &curSonCnt,vector<int> &curRank,vector<bool> &invalid)
{
	int nextBestBlockNum=0;
	if(true)
	{
		vector<int> nextBestParent;
		vector<int> nextBestSonCnt;
		vector<int> nextBestRank;
		vector<int> nextBestChoice(choice);
		for(int preID=1;preID<=preType;preID++)
		if(choice[preID]==0&&!invalid[preID])
		{
			vector<int> nextParent(curParent);
			vector<int> nextSonCnt(curSonCnt);
			vector<int> nextRank(curRank);
			int nextBlockNum=0;
			bool flag=0;
			unordered_map<int,int>::iterator it;
			for(it=coarseningPoint[preID].begin();it!=coarseningPoint[preID].end();it++)
			{
				int point=it->first;
				int parentA=getParent(point,nextParent),parentB=getParent(getParentMap(point,coarseningPoint[preID]),nextParent);
				if(nextRank[parentA]<nextRank[parentB])swap(parentA,parentB);
				if(parentA!=parentB)
				{
					nextParent[parentB]=parentA;
					nextSonCnt[parentA]+=nextSonCnt[parentB];
					nextRank[parentA]=max(nextRank[parentA],nextRank[parentB]+1);
					if(nextSonCnt[parentA]>limit)
					{
						flag=1;
						break;
					}
				}
			}
			if(flag)continue;
			for(int p=1;p<=entityCnt;p++)if(getParent(p,nextParent)==p)nextBlockNum++;
			if(!nextBestBlockNum||nextBestBlockNum<nextBlockNum)
			{
				nextBestBlockNum=nextBlockNum;	
				nextBestParent.assign(nextParent.begin(),nextParent.end());
				nextBestSonCnt.assign(nextSonCnt.begin(),nextSonCnt.end());
				nextBestRank.assign(nextRank.begin(),nextRank.end());
				nextBestChoice.assign(choice.begin(),choice.end());
				nextBestChoice[preID]=1;
			}
		}
		choice.assign(nextBestChoice.begin(),nextBestChoice.end());
		curParent.assign(nextBestParent.begin(),nextBestParent.end());
		curSonCnt.assign(nextBestSonCnt.begin(),nextBestSonCnt.end());
		curRank.assign(nextBestRank.begin(),nextBestRank.end());
	}
	if(nextBestBlockNum)greed1(choice,curParent,curSonCnt,curRank,invalid);
}

int 
graph::cal(long long cur)
{
	int ret=0;
	while(cur)ret+=(cur&1),cur>>=1;
	return ret;
}

void 
graph::compareCrossingEdgeCnt(long long cur)
{
	int c=0,cnt=0;
	for(int i=0;i<preType;i++)
	{
		if(cur&(1LL<<i))c++;
		else cnt+=edge[i+1].size();
	}
	if(cal(ans)<c)ans=cur,crossEgdeCnt=cnt;
	else if(cal(ans)==c&&crossEgdeCnt>cnt)ans=cur,crossEgdeCnt=cnt;
}

void 
graph::greed2()
{
	printf("greed2\n");
	invalid=vector<bool>(preType+1,0);
	int threshold=entityCnt*0.0001;
	vector<int> fa(entityCnt+1);
	vector<int> FA(entityCnt+1);
    for(int i=1;i<=entityCnt;i++)fa[i]=FA[i]=i;
    vector<int> RANK(entityCnt+1,0);
	vector<int> SONCNT(entityCnt+1,1);
    vector<int> choice(preType+1,0);
    vector<pair<int,int> >arr;

    for(int preID=1;preID<=preType;preID++)
	    if(edge_cnt[IDToPredicate[preID]]<threshold)
	    {
	    	for(int p=0;p<edge[preID].size();p++)
	        {

	        	int A=edge[preID][p].first,B=edge[preID][p].second;
	            int parentA=getParent(A,FA),parentB=getParent(B,FA);

	            if(RANK[parentA]<RANK[parentB])swap(parentA,parentB);
	            if(parentA!=parentB)
	            {
                    FA[parentB]=parentA;
                    SONCNT[parentA]+=SONCNT[parentB];
                    RANK[parentA]=max(RANK[parentA],RANK[parentB]+1);
	            }
	        }
	        choice[preID]=1;
	    }
	    else
	    {
	        vector<int> parent(fa);
	        vector<int> sonCnt=vector<int>(entityCnt+1,1);

	        for(int p=0;p<edge[preID].size();p++)
	        {

	        	int A=edge[preID][p].first,B=edge[preID][p].second;
	            int parentA=getParent(A,parent),parentB=getParent(B,parent);

	            if(parentA!=parentB)
	            {
                    parent[parentB]=parentA;
                    sonCnt[parentA]+=sonCnt[parentB];
                    if(sonCnt[parentA]>limit)
                    {
                        invalid[preID]=1;
                        printf("invalid: %d\n",preID);
                        break;
                    }
	            }
	        }
	        if(invalid[preID])continue;
	        int SonCntNum=0;
	        for(int p=1;p<=entityCnt;p++)if(getParent(p,parent)==p)SonCntNum++;
	        arr.push_back(make_pair(SonCntNum,preID));
	    }

	sort(arr.begin(),arr.end());
    for(int i=arr.size()-1;i>=0;i--)
    {
        int preID=arr[i].second;
        // cout<<preID<<" "<<arr[i].first<<endl;
        for(int p=0;p<edge[preID].size();p++)
        {

        	int A=edge[preID][p].first,B=edge[preID][p].second;
            int parentA=getParent(A,FA),parentB=getParent(B,FA);

            if(RANK[parentA]<RANK[parentB])swap(parentA,parentB);
            if(parentA!=parentB)
            {
                FA[parentB]=parentA;
                SONCNT[parentA]+=SONCNT[parentB];
                RANK[parentA]=max(RANK[parentA],RANK[parentB]+1);
                if(SONCNT[parentA]>limit)
                {
                    invalid[preID]=1;
                    break;
                }
            }
        }
        if(invalid[preID])break;
        choice[preID]=1;
    }


	int crossEdge=0;
	for(int preID=1;preID<=preType;preID++)
		if(choice[preID]==0)cout<<preID<<"	"<<IDToPredicate[preID]<<endl,crossEdge++;
	printf("crossEdge: %d\n",crossEdge);
	printf("\n");
	unionBlock(choice,part);
}

void 
graph::unionBlock(vector<int> &choice,int goal)
{
	cout<<"unionBlock: "<<endl;
	vector<int>parent(entityCnt+1);
	for(int p=1;p<=entityCnt;p++)parent[p]=p;
	vector<int> rank=vector<int>(entityCnt+1,0);
	for(int preID=1;preID<=preType;preID++)if(choice[preID]==1)
	{
		for(int p=0;p<edge[preID].size();p++)
		{
			int parentA=getParent(edge[preID][p].first,parent),parentB=getParent(edge[preID][p].second,parent);
			if(rank[parentA]<rank[parentB])swap(parentA,parentB);
			if(parentA!=parentB)
			{
				rank[parentA]=max(rank[parentA],rank[parentB]+1);
				parent[parentB]=parentA;
				entityTriples[parentA]+=entityTriples[parentB];
			}
		}
	}
	vector<pair<int,int> >block;
	int blockNum=0;
	for(int p=1;p<=entityCnt;p++)if(p==getParent(p,parent))block.push_back(make_pair(entityTriples[p],p)),++blockNum;
	printf("blockNum: %d\n", blockNum);
	
	sort(block.begin(),block.end());
	priority_queue<pair<int,int>, vector<pair<int,int> >, greater<pair<int,int> > > Q;
	for(int i=1;i<=goal;i++)Q.push(make_pair(0,i));
	vector<int> blockTogoal(entityCnt+1,0);
	for(int i=block.size()-1;i>=0;i--) 
	{
		pair<int,int> tmp=Q.top();
		Q.pop();
		tmp.first+=block[i].first;
		blockTogoal[block[i].second]=tmp.second;
		Q.push(tmp);
	}
	while(!Q.empty())
	{
		printf("%d %d\n",Q.top().first,Q.top().second);
		Q.pop();
	}

	vector<int>CNT(goal+1,0);

	// if(access(RDF.c_str(), 0)==-1)
	// 	mkdir(RDF.c_str(),0777);
	// ofstream outFile("/opt/workspace/PCP/"+RDF+"InternalPoints.txt"); 
	ofstream outFile(RDF+"InternalPoints.txt");   
	for(int pos,p=1;p<=entityCnt;p++)
	{
		pos=blockTogoal[getParent(p,parent)];
		outFile<<IDToEntity[p]<<"	"<<pos-1<<"\n";
		CNT[pos]++;
	}
	outFile.close();
	printf("\n");
	for(int i=1;i<=goal;i++)printf("%d %d\n",i,CNT[i]);

	// ofstream File("/opt/workspace/PCP/"+RDF+"crossingEdges.txt"); 
	ofstream File(RDF+"crossingEdges.txt"); 
	for(unordered_map<string,int>::iterator it=edge_cnt.begin();it!=edge_cnt.end();it++)
	{
		File<<it->first<<"\t"<<it->second<<"\t";
		if(predicateToID.count(it->first))
			File<<(!choice[predicateToID[it->first]]);
		else File<<"0";
		File<<endl;
	}
	File.close();
	// update();
}

void 
graph::randEntity(string txt_name,string tag)
{
	ifstream in(txt_name.data());
	string line;
	while(getline(in,line))
	{
		triples++;
		line.resize(line.length()-2);
		vector<string> s;
		s=split(line,tag);
		predicate.insert(s[1]);
		for(int i=0;i<3;i+=2)if(entityToID.count(s[i])==0)
        {
            entityToID[s[i]]=++entityCnt;
            IDToEntity.push_back(s[i]);
        }
	}
	in.close();
	// ofstream outFile("/opt/workspace/sub_hash/"+RDF+"InternalPoints.txt");
	ofstream outFile(RDF+"sub_hash_InternalPoints.txt");
	for(int i=1;i<=entityCnt;i++)
	{
		outFile<<IDToEntity[i]<<"\t"<<rand()%part<<"\n";
	}
	outFile.close();
	// update();
}

// void 
// graph::metis(string txt_name,string tag)
// {
//    ifstream in(txt_name.data());
//    string str;
//    entityCnt=0;
//    vector<vector<int> > EDGE;
//    EDGE.push_back(vector<int>());
//    triples=0;
//    while(getline(in,str))
//    {
//         triples++;
//         str.resize(str.length()-2);
//         vector<string> s;
//         s=split(str,tag);
//         predicate.insert(s[1]);
//         for(int i=0;i<3;i+=2)if(entityToID.count(s[i])==0)
//         {
//                 entityToID[s[i]]=++entityCnt;
//                 IDToEntity.push_back(s[i]);
//                 EDGE.push_back(vector<int>());
//         }
//         EDGE[entityToID[s[0]]].push_back(entityToID[s[2]]);
//         EDGE[entityToID[s[2]]].push_back(entityToID[s[0]]);
//    }
//    ofstream out(("/opt/workspace/metis/"+RDF+".tmp").data());
//    out<<entityCnt<<" "<<triples<<endl;
//    for(int i=1;i<EDGE.size();i++)
//    {
//         for(int j=0;j<EDGE[i].size();j++)
//                 out<<EDGE[i][j]<<" ";
//         out<<endl;
//     }

//    system(("/opt/workspace/metis-5.1.0/build/Linux-x86_64/programs/gpmetis /opt/workspace/metis/"+RDF+".tmp 4").data());
//    ifstream In(("/opt/workspace/metis/"+RDF+".tmp.part.4").data());
//    ofstream Out(("/opt/workspace/metis/"+RDF+"InternalPoints.txt").data());
//    int idx=1;
//    while(getline(In,str))
//    {
//         Out<<IDToEntity[idx++]<<"\t"<<atoi(str.c_str())<<endl;
//    }
//    update();
// }

// void 
// graph::update()
// {
// 	string info="insert data{ <"+RDF+"> <triple> \""+to_string(triples)+"\" .<"+RDF+"> <entity> \""+to_string(entityCnt)+"\" .<"+RDF+"> <label> \""+to_string(predicate.size())+"\" .}";
// 	ofstream out("/opt/workspace/gStoreD/insert.q");
// 	out<<info;
// 	out.close();
// }

