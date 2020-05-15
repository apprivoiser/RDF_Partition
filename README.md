RUN

	g++ main.cpp -std=c++11 -o main
	./main RDFData Data_name tag partition part
	(
		tag:split the triple
			1 " "
			2 "\t"

		partition:four methods
			1 PCP-Exact
			2 PCP-Basic
			3 PCP
			4 Subject Hash

		part:the number of partitons
	)
	Ex:	./main /data/RDFData/LUBM/LUBM10M.nt LUBM10M 1 1 4

	Results are in Data_nameInternalPoints.txt and Data_namecrossingEdges.txt(1,2,3)
				or Data_namesub_hash_InternalPoints.txt(4)
	
