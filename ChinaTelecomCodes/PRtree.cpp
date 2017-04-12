#include "PRtree.h"
#include "data.h"
#include <QMessageBox>

prtree::prtree() {
	filename="./database.txt";
	num=0;
	outfile.open("./prtree_result.txt");
//	filename="./testdata/INTEGRATED-DATASET.txt";
//	outfile.open("./testdata/PRtree_result42.txt");
}

//创建数据库，计数，计算1-项集
bool prtree::create1set(char *filename,vector<vector<string > > &database,vector<PRNode> &largeItemset,double minsupprot) {

	ifstream in(filename);
	if(!in) {
		QMessageBox::about(NULL,"Error:","Cound not open the source file.");
		return false;
	}

	map<string,int> dir;
	map<string,int>::iterator dirIt;

	string line="";
	while(getline(in,line)) {
		vector<string> temp;
		string str="";
		int len=line.length();
		for(int i=0;i<len;i++) {
			if(line[i]!=' ') str+=line[i];
			else if(line[i]==' '||i==len-1) {
				dirIt=dir.find(str);
				if(dirIt==dir.end()) dir.insert(pair<string,int>(str,1));
				else (dirIt->second)++;

				temp.push_back(str);
				str="";
			}
		}
		database.push_back(temp);
	}

	minsupprot=(int)(database.size()*minsupprot/100.0);
	for(dirIt=dir.begin();dirIt!=dir.end();dirIt++) {
		if(dirIt->second>=minsupprot) {
			PRNode large;
			large.nodename=dirIt->first;
			large.count=dirIt->second;
			large.parentNode=NULL;
			large.childNode.clear();
			largeItemset.push_back(large);
		}
	}
	sort(largeItemset.begin(),largeItemset.end());

	outfile<<"size of database:"<<database.size()<<endl;
	outfile<<"size of largeItemset:"<<largeItemset.size()<<endl;
	return true;
}

bool prtree::addNode(int i,vector<PRNode> largeItemset,PRNode *node) {
	if(i>=largeItemset.size())	{
		leaf.push_back(node);
		return true;
	}
	for(int j=i;j<largeItemset.size();j++) {
		PRNode *temp=new PRNode;
		temp->nodename=largeItemset.at(j).nodename;
		temp->count=0;
		temp->parentNode=node;
		num++;
		temp->num=num;
		node->childNode.push_back(temp);
		addNode(j+1,largeItemset,temp);
	}
	return true;
}

bool prtree::CreatePRTree(vector<PRNode> largeItemset,PRNode **treeRoot) {
	*treeRoot=new PRNode;
	PRNode *root=*treeRoot;
	root->nodename="-1";
	root->count=999999;
	root->parentNode=NULL;
	root->childNode.clear();
	root->num=0;

	addNode(0,largeItemset,root);

	return true;
}

/*
void OutputPRTree(PRNode *root) {
	if(root==NULL) return;
	vector<PRNode*> childNode=root->childNode;
	cout<<"{"<<root->nodename<<","<<root->count<<","<<childNode.size()<<" }"<<endl;
	if(childNode.size()!=0) {
		for(int i=0;i<childNode.size();i++) OutputPRTree(childNode[i]);
	}
	else return ;
}
*/
void prtree::OutputPRTree(PRNode *root) {
	if(root==NULL) return;
	queue<PRNode*> child;
	child.push(root);
	while(!child.empty()) {
		PRNode *tmp=child.front();
		child.pop();
		outfile<<"{"<<tmp->nodename<<","<<tmp->count<<","<<tmp->childNode.size()<<" }"<<endl;
		for(int i=0;i<tmp->childNode.size();i++)
			child.push(tmp->childNode[i]);
	}
	return ;
}

void prtree::destroyTree(PRNode *root) {
	if(root!=NULL) {
		vector<PRNode *> child=root->childNode;
		int len=child.size();
		for(int i=0;i<len;i++) {
			destroyTree(child[i]);
		}
		delete root;
	}
}

void prtree::obtainSet(vector<vector<string > > database,vector<PRNode> largeItemset,PRNode *treeRoot) {
	vector<string> temp;
	string temp2;
	vector<vector<string> >::iterator itr;
	vector<string>::iterator itr2;
	for(itr=database.begin();itr!=database.end();itr++) {
		temp=*itr;
		PRNode *node=treeRoot;
		while(node->childNode.size()!=0) {
			bool flag=false;
			int i;
			for(i=0;i<node->childNode.size();i++) {
				for(itr2=temp.begin();itr2!=temp.end();itr2++) {
					temp2=*itr2;
					if(strcmp(node->childNode[i]->nodename.c_str(),temp2.c_str())==0) {
						flag=true;
						break;
					}
				}
				if(flag) break;
			}
			if(i==node->childNode.size()) break;
			node=node->childNode[i];
			node->count++;
		}
	}
}

void prtree::updateTree(PRNode *treeRoot,int minSup,double conf) {
	bool flag;
	vector<PRNode *>::iterator itr,itr2;
	for(itr=leaf.begin();itr!=leaf.end();) {
		PRNode *tmp=*itr;
		flag=true;

		if(tmp->count<minSup/*||(parent->count*conf<tmp->count)*/) {
			PRNode *mytmp=tmp;
			tmp=tmp->parentNode;
			leaf.erase(itr);

			for(itr2=tmp->childNode.begin();itr2!=tmp->childNode.end();itr2++) {
				PRNode *tmp2=*itr2;
				if(tmp2->num==mytmp->num) {tmp->childNode.erase(itr2);break;}
			}

			if(tmp->nodename=="-1") {
				itr=leaf.begin();
				delete mytmp;
				continue;
			}
			for(itr=leaf.begin();itr!=leaf.end();itr++) {
				PRNode *tmp2=*itr;
				if(tmp2->num==tmp->num) {
					flag=false;
					itr=leaf.begin();
					break;
				}
			}
			if(flag) {
				leaf.push_back(tmp);
				itr=leaf.begin();
			}
			delete mytmp;
		}
		else itr++;
	}
}

void prtree::outputRules() {
	vector<PRNode *>::iterator itr;
	for(itr=leaf.begin();itr!=leaf.end();itr++) {
		PRNode *tmp=*itr;
		while(tmp->nodename!="-1") {
			outfile<<tmp->nodename<<" ";
			tmp=tmp->parentNode;
		}
		cout<<endl;
	}
}

void prtree::saveRules() {
	outfile<<endl<<"Rules:"<<endl;
	vector<PRNode *>::iterator itr;
	for(itr=leaf.begin();itr!=leaf.end();itr++) {
		PRNode *tmp=*itr;
		while(tmp->nodename!="-1") {
			outfile<<tmp->nodename<<" ";
			tmp=tmp->parentNode;
		}
		outfile<<endl;
	}
}

void prtree::action() {
	vector<vector<string > > database;
	vector<PRNode> largeItemset;
	create1set(filename,database,largeItemset,minSup);

	PRNode *treeRoot=NULL;
	CreatePRTree(largeItemset,&treeRoot);
	obtainSet(database,largeItemset,treeRoot);

	outfile<<"Now print the result tree:"<<endl;
	OutputPRTree(treeRoot);

	outfile<<"the minsup:"<<minSup*database.size()/100.0<<endl;
	updateTree(treeRoot,(int)(minSup*database.size()/100.0),minConf/100.0);
	OutputPRTree(treeRoot);
	outputRules();
	saveRules();

	destroyTree(treeRoot);
	outfile.close();
}