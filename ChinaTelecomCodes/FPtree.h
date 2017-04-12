#ifndef FPTREE_H
#define FPTREE_H

#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <map>
#include <list>
#include <string>
#include <algorithm>
#include <ctime>
using namespace std;

struct FPNode
{
	string nodeName;
	unsigned int count;

	FPNode *parentNode;
	vector<FPNode *> childNode;
	FPNode *siblingNode;

	bool operator < (const FPNode &other) const	{
		return count>other.count;
	}
};

class fptree 
{
private:
	char* fileName;
	char* resultFile;

	ofstream outfile;
public:
	fptree();
	bool ObtainDatabase(vector<vector<string> > &database,char *fileName);
	void CreateItemset(vector<vector<string> > &database,vector<FPNode> &largeItemset,unsigned int minSupport);
	void OutputLargeItemset(vector<FPNode> &largeItemset,unsigned int i);
	void SortItem(vector<string> &trans,vector<string> &tempTrans,vector<FPNode> &large1);
	int AddNode(FPNode *parentNode,vector<string> &transTemp,unsigned int k,vector<FPNode> &large1);
	int CreateFPTree(vector<vector<string> > &database,vector<FPNode> &large1,FPNode **treeRoot);
	void CreateConditionalPatterBase(vector<FPNode> &large1,map<string,vector<vector<string> > > &cpbMap);
	void DestroyTree(FPNode *root);
	void OutputFPTree(FPNode *root);
	void CreateCpbFPtree(map<string,vector<vector<string> > > &cpbMap,map<string,FPNode *> &cpbFPTreeMap,unsigned int minSupport);
	void CreatePath(FPNode *root,vector<list<FPNode> > &pathSet);
	void OutputPathSet(vector<list<FPNode> > &pathSet);
	map<list<string>,int>* CreateFP(list<FPNode> &path);
	void ObtainFrequenPattern(map<string,FPNode *> &cpbFPTreeMap,map<string,map<list<string>,int>* > &frequentPatternMap);
	void OutputFrequentPattern(map<string,map<list<string>,int>* > &frequentPatternMap,unsigned int minSupport);
	void OutputConditionalPatterBase(map<string,vector<vector<string> > > &cpbMap);
	void OutputCpbFPTree(map<string,FPNode *> &cpbFPTreeMap);
	void action();
};

#endif