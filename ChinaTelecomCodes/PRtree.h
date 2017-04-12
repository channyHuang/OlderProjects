#ifndef PRTREE_H
#define PRTREE_H

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <map>
#include <queue>
using namespace std;

struct PRNode
{
	string nodename;
	int count;
	PRNode *parentNode;
	vector<PRNode *> childNode;
	int num;

	bool operator < (const PRNode &other) const {
		return count>other.count;
	}
};

class prtree
{
private:
	char* filename;
	int num;
	vector<PRNode *>leaf;
	ofstream outfile;

public:
	prtree();
	bool create1set(char *filename,vector<vector<string > > &database,vector<PRNode> &largeItemset,double minsupprot);
	bool addNode(int i,vector<PRNode> largeItemset,PRNode *node);
	bool CreatePRTree(vector<PRNode> largeItemset,PRNode **treeRoot);
	void OutputPRTree(PRNode *root);
	void destroyTree(PRNode *root);
	void obtainSet(vector<vector<string > > database,vector<PRNode> largeItemset,PRNode *treeRoot);
	void updateTree(PRNode *treeRoot,int minsup,double conf);
	void outputRules();
	void saveRules();
	void action();
};

#endif