#include"graph.cpp"
#include<ctime>
clock_t st,ed;
double endtime;
int
main(int argc, char* argv[])
{	
	string txt_name=argv[1];
	string name=argv[2];
	string sign=(string(argv[3])=="1")?" ":"\t";
	string op=argv[4];
	int part=atoi(argv[5]);

	st=clock();
		graph* test=new graph();
		test->init();
		test->RDF=name;
		test->part=part;
		// if(op=="5")
		// {
		// 	test->metis(txt_name,sign);
		// }
		// else 
		if(op=="4")
		{
			test->randEntity(txt_name,sign);
		}
		else 
		{
			test->loadGraph(txt_name,sign);
			if(op=="1")
				test->unionEdgeForEnum();
			else if(op=="2")
				test->unionEdgeForGreed();
			else
				test->greed2();
		}
	ed=clock();
	endtime=(double)(ed-st)/CLOCKS_PER_SEC;
	cout<<"partition : "<<endtime<<" s"<<endl;

	delete test;

	return 0;
}
