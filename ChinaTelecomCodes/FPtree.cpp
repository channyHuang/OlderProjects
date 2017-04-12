#include "FPtree.h"
#include "data.h"
#include <QMessageBox>

fptree::fptree() {
	fileName="./database.txt";
	resultFile="./fptree_result.txt";
//	fileName="./testdata/INTEGRATED-DATASET.txt";
//	resultFile="./testdata/FPtree_result42.txt";
	outfile.open(resultFile);
}

//��ȡ�ļ���ȡ�������ݿ�洢��database�У�fileName����Ϊchar*�ͣ�Ҫ����string�ᱨ��in()����
bool fptree
	::ObtainDatabase(vector<vector<string> > &database,char *fileName)
{
	ifstream in(fileName);
	if(!in)	{
		QMessageBox::about(NULL,"Error:","Cound not open the source file.");
		return false;
	}

	string s="";
	int i=0;
	while(getline(in,s))	{//��ȡһ�м�¼
		i++;
		vector<string> transaction;	
		int len=s.length();
		string str="";
		for(int i=0;i<len;i++)	{//����¼�е�����ȡ����
			if(s[i]!=' ')	{
				str+=s[i];
			}
			else if(s[i]==' '||i==len-1)	{		
				transaction.push_back(str);
				str="";
			}
		}
		database.push_back(transaction);
		s="";
	}
	outfile<<"����������"<<i<<endl;
	return true;
}

//����һ�����ݿ⣬����1-����
void fptree::CreateItemset(vector<vector<string> > &database,vector<FPNode> &largeItemset,unsigned int minSupport)
{
	map<string,int> dir;
	map<string,int>::iterator dirIt;

	vector<vector<string> >::iterator databaseIt;

	vector<string> temp;
	vector<string>::iterator tempIt;

	//�������ݿⴴ���ֵ䣬�ֵ���ʽΪ<item,count>
	for(databaseIt=database.begin();databaseIt!=database.end();databaseIt++)	{
		temp=*databaseIt;
		for(tempIt=temp.begin();tempIt!=temp.end();tempIt++)	{
			string item=*tempIt;
			dirIt=dir.find(item);
			if(dirIt==dir.end())	{//item�����ֵ�dir��
				dir.insert(pair<string,int>(item,1));
			}
			else	{//item���ֵ�dir�У�����countֵ��1
				(dirIt->second)++;
			}
		}
	}

	//���ֵ���ѡ��֧�ֶȳ���minSopport��item
	for(dirIt=dir.begin();dirIt!=dir.end();dirIt++)	{
		if(dirIt->second>=minSupport)	{
			FPNode large;
			large.nodeName=dirIt->first;
			large.count=dirIt->second;
			large.parentNode=NULL;
			(large.childNode).clear();
			large.siblingNode=NULL;
			largeItemset.push_back(large);
		}
	}
	//����ÿ���ڵ��countֵ�Դ������
	sort(largeItemset.begin(),largeItemset.end());
}

//���һ����
void fptree::OutputLargeItemset(vector<FPNode> &largeItemset,unsigned int i)
{
	outfile<<"���� "<<largeItemset.size()<<" ��� "<<i<<"-������"<<endl;	
	vector<FPNode>::iterator largeItemsetIt;
	for(largeItemsetIt=largeItemset.begin();largeItemsetIt!=largeItemset.end();largeItemsetIt++)	{
		FPNode temp=*largeItemsetIt;
		outfile<<"{ "<<temp.nodeName<<" : "<<temp.count<<" }"<<endl;
	}
	outfile<<endl<<endl;
}

//�����ݿ�ĳ������transɸѡ����1-����large1�г��ֵ��������1-����large1���ֵ�˳�����򣬷ŵ�tempTrans��
void fptree::SortItem(vector<string> &trans,vector<string> &tempTrans,vector<FPNode> &large1)
{
	unsigned int sizeLarge=large1.size();
	unsigned int sizeTrans=trans.size();	
	for(int i=0;i<sizeLarge;i++)	{
		FPNode tempNode=large1[i];
		for(int j=0;j<sizeTrans;j++)	{
			if(tempNode.nodeName==trans[j])	{
				tempTrans.push_back(tempNode.nodeName);
			}
		}
	}
}

//���ð���transTemp[k]���ڼ����Ժ��ֵΪroot�ڵ㴴������
int fptree::AddNode(FPNode *parentNode,vector<string> &transTemp,unsigned int k,vector<FPNode> &large1)
{
	unsigned int size=transTemp.size();
	if(size>0&&(k>=0&&k<size))	{
		FPNode *nodeTemp=new FPNode;//�����½ڵ�
		if(nodeTemp==NULL)	return 1;
		nodeTemp->nodeName=transTemp[k];
		nodeTemp->count=1;
		nodeTemp->parentNode=parentNode;
		nodeTemp->siblingNode=NULL;

		(parentNode->childNode).push_back(nodeTemp);

		//��nodeTemp��ӵ������ֵܽڵ��ϡ�
		unsigned int sizeLarge=large1.size();
		for(int i=0;i<sizeLarge;i++)	{
			if((large1[i]).nodeName==nodeTemp->nodeName)	{
				FPNode *temp=&(large1[i]);
				while(temp->siblingNode!=NULL)	{
					temp=temp->siblingNode;
				}
				temp->siblingNode=nodeTemp;
				break;
			}
		}

		k++;
		if(k<size)	{//�˽����л���������
			AddNode(nodeTemp,transTemp,k,large1);
		}
	}
	return 0;
}

//����FP-Tree
int fptree::CreateFPTree(vector<vector<string> > &database,vector<FPNode> &large1,FPNode **treeRoot)
{
	*treeRoot=new FPNode;
	FPNode *root=*treeRoot;
	if(root==NULL)	return 1;
	root->nodeName="-1";//-1��ʾ�սڵ�
	root->count=0;
	root->parentNode=NULL;
	(root->childNode).clear();
	root->siblingNode=NULL;

	unsigned int sizeDatabase=database.size();
	for(int i=0;i<sizeDatabase;i++)	{
		vector<string> trans=database[i];//ȡ�����ݿ�һ������
		vector<string> transTemp;
		SortItem(trans,transTemp,large1);//�Ѵ˽��װ�����large1�е���ɸѡ���������򣬷ŵ�transTemp��
		vector<FPNode *> childNode=root->childNode;//��ȡ���Ķ��ӽڵ㼯��
		if(childNode.size()==0)	{//��������Ķ��ӽڵ㼯��Ϊ�գ�˵�������Ϊ�ա���Ѵ˽���ת��Ϊ����root��������
			AddNode(root,transTemp,0,large1);
		}
		else	{//����Ϊ��
			unsigned int sizeTrans=transTemp.size();
			FPNode *pt=NULL;
			for(int i=0;i<sizeTrans;i++)	{//�Ȳ���transTemp[i]�Ƿ������г��ֹ�
				string temp=transTemp[i];				
				unsigned int sizeChild=childNode.size();
				int j=0;
				for(j=0;j<sizeChild;j++)	{
					if(temp==(childNode[j])->nodeName)	{
						pt=childNode[j];
						(pt->count)++;
						//transTemp[i]�Ѿ������г��ֹ���,������ļ�������1
						childNode=pt->childNode;
						break;
					}
				}				
				if(j==sizeChild)	{
					if(pt==NULL)	{//transTemp[i]δ�����У�����Ӱ���i�ڵ�֮������нڵ㣬��ӵ�������
						AddNode(root,transTemp,i,large1);
					}
					else	{//transTemp[i]�������г��ֹ�����������û���ӽڵ㣬���԰�������Ľڵ����������
						AddNode(pt,transTemp,i,large1);//����ط�����i+1
					}
					break;
				}
			}
		}
	}
	return 0;
}

//��ȡlarge1����һ��֮��ĸ����conditional pattern base����ģʽ�����ù��̼��ǽ���ÿһ���Ӧ�������ݿ⡣
void fptree::CreateConditionalPatterBase(vector<FPNode> &large1,map<string,vector<vector<string> > > &cpbMap)
{
	vector<string> condition;
	int sizeLarge=large1.size();
	//large1�г��˵�һ����(��Ϊֱ�ӹ��������ϵĽڵ�϶�û��ǰ׺�����Բ������ˣ�����˷�ʱ��)��ȫ��������ģʽ��
	for(int i=sizeLarge-1;i>0;i--)	{
		FPNode *sibling=(large1[i]).siblingNode;
		vector<vector<string> > pathSet;
		pathSet.clear();
		while(sibling!=NULL)
		{
			unsigned int count=sibling->count;
			FPNode *parent=sibling->parentNode;
			vector<string> path;
			path.clear();
			while(parent!=NULL)	{//�������ϣ��ѳ�����֮��ĸ��ڵ���ӵ�·��path��
				if(parent->parentNode!=NULL)	{//parent�ĸ��ڵ�Ϊ�գ�˵��parent�������������ﲻ����
					path.push_back(parent->nodeName);
					parent=parent->parentNode;
				}
				else	{
					break;
				}
			}

			if(!(path.empty()))	{
				for(int j=0;j<count;j++)	{
					pathSet.push_back(path);	
				}
			}
			sibling=sibling->siblingNode;
		}
		if(pathSet.size()!=0)	{
			cpbMap.insert(pair<string,vector<vector<string> > >((large1[i]).nodeName,pathSet));
		}
	}
}

void fptree::DestroyTree(FPNode *root)
{
	if(root!=NULL)	{
		vector<FPNode *> childNode=root->childNode;
		unsigned int size=childNode.size();
		for(int i=0;i<size;i++)	{
			DestroyTree(childNode[i]);
		}
		delete root;
	}
	root=NULL;
}

//���FP��
void fptree::OutputFPTree(FPNode *root)
{
	if(root==NULL)	return;
	vector<FPNode*> childNode=root->childNode;
	outfile<<"{"<<root->nodeName<<","<<root->count<<","<<childNode.size();
	if(childNode.size()!=0)	{
		outfile<<"}��������:"<<endl;
		for(int i=0;i<childNode.size();i++)	{
			OutputFPTree(childNode[i]);
		}
	}
	else 	{
		outfile<<"}����"<<endl;
	}	
}

//��ÿһ�������ģʽ���õ���Ӧ������ģʽFP�������յõ�����ģʽFP���ֵ�
void fptree::CreateCpbFPtree(map<string,vector<vector<string> > > &cpbMap,map<string,FPNode *> &cpbFPTreeMap,unsigned int minSupport)
{
	map<string,vector<vector<string> > >::iterator cpbMapIt;
	for(cpbMapIt=cpbMap.begin();cpbMapIt!=cpbMap.end();cpbMapIt++)	{
		string nodeName=cpbMapIt->first;
		vector<vector<string> > pathSet=cpbMapIt->second;
		vector<FPNode> cpbLarge;
		//����nodeName���Ӧ������ģʽ������pathSet��������Ƶ��һ�cpbLarge
		CreateItemset(pathSet,cpbLarge,minSupport);
		//����ոմ�����Ƶ��һ�

		FPNode *cpbRoot=NULL;
		if(!(pathSet.empty())&&!(cpbLarge.empty()))	{
			CreateFPTree(pathSet,cpbLarge,&cpbRoot);//����nodeName��Ӧ������ģʽFP��
		}

		if(cpbRoot!=NULL)	{//�����ɹ�
			vector<FPNode *> *childNode=&(cpbRoot->childNode);
			vector<FPNode *>::iterator childIt=(*childNode).begin();
			unsigned int size=(*childNode).size();
			for(int i=0;i<size;i++)	{
				if((*childIt)->count< minSupport)	{
					//ɾ����С֧�ֶȲ�����ķ�֧				
					DestroyTree(*childIt);
					childIt=(*childNode).erase(childIt);
				}
				else	{
					childIt++;
				}
			}			
			if((*childNode).size()!=0)	{//����������ģʽ�������Ķ��ӽڵ������Ϊ0��������ӵ�����ģʽFP���ֵ���
				cpbFPTreeMap.insert(pair<string,FPNode*>(nodeName,cpbRoot));
			}
		}
	}
}

//����ÿ���ؼ��ֵ�����ģʽFP�������������е�·�����ŵ�·������pathSet��
void fptree::CreatePath(FPNode *root,vector<list<FPNode> > &pathSet)
{
	if(root!=NULL)	{
		if((root->childNode).empty())	{//rootû�з�֧
			list<FPNode> path;
			if(root->parentNode!=NULL)	{//���ܰ�������ӵ�·����
				path.push_back(*root);
			}
			if(path.size()!=0)	{
				pathSet.push_back(path);
			}
		}
		else	{//root�з�֧
			vector<FPNode *> childNode=root->childNode;
			unsigned int size=childNode.size();
			if(size>1)	{//root�ж����֧
				for(int i=0;i<size;i++)	{
					unsigned int count=childNode[i]->count;
					CreatePath(childNode[i],pathSet);
					unsigned int cnt=(childNode[i]->childNode).size();
					if(cnt==0)	{
						cnt=1;
					}
					if(root->parentNode!=NULL)	{
						unsigned int sizePathSet=pathSet.size();
						for(int j=sizePathSet-cnt;j<sizePathSet;j++)	{
							list<FPNode> *lis=&(pathSet[j]);
							FPNode *node=new FPNode;
							node->nodeName=root->nodeName;
							node->count=count;
							node->parentNode=NULL;
							lis->push_front(*node);
						}
					}
				}
			}
			else	{//rootֻ��һ����֧
				for(int i=0;i<size;i++)	{
					unsigned int count=childNode[i]->count;
					CreatePath(childNode[i],pathSet);
					if(root->parentNode!=NULL)		{
						unsigned int sizePathSet=pathSet.size();
						for(int j=sizePathSet-1;j<sizePathSet;j++)		{
							list<FPNode> *lis=&(pathSet[j]);
							lis->push_front(*root);
						}
					}
				}
			}
		}
	}
}

//���ÿ�����Ӧ��ȫ��ǰ׺·��
void fptree::OutputPathSet(vector<list<FPNode> > &pathSet)
{
	vector<list<FPNode> >::iterator pathSetIt;
	for(pathSetIt=pathSet.begin();pathSetIt!=pathSet.end();pathSetIt++)	{
		list<FPNode> path=*pathSetIt;
		list<FPNode>::iterator pathIt;
		for(pathIt=path.begin();pathIt!=path.end();pathIt++)	{
			outfile<<(*pathIt).nodeName<<" ";
		}
		cout<<endl<<endl;
	}
}

//����path����fp��k��ʼֵΪpath.size()-1
map<list<string>,int>* fptree::CreateFP(list<FPNode> &path)
{
	if(path.size()>0)	{
		//��·��path�ĵ�һ���ڵ����
		FPNode *temp=&(path.front());
		if(!temp)	{
			return NULL;
		}
		list<string> lis;
		string start=temp->nodeName;
		lis.push_back(start);
		map<list<string>,int> *fp=new map<list<string>,int>;
		if(!fp)	{
			return NULL;
		}
		fp->insert(pair<list<string>,int>(lis,temp->count));
		path.pop_front();

		map<list<string>,int> *others=CreateFP(path);
		if(others)	{
			map<list<string>,int>::iterator othersIt=others->begin();
			for(;othersIt!=others->end();othersIt++)	{
				fp->insert(pair<list<string>,int>(othersIt->first,othersIt->second));
				const list<string> *li=&(othersIt->first);
				list<string> tmp;

				list<string>::const_iterator liIt=li->begin();
				while(liIt!=li->end())	{
					tmp.push_back(*liIt);
					liIt++;
				}
				tmp.push_front(start);
				fp->insert(pair<list<string>,int>(tmp,othersIt->second));
			}
		}
		return fp;
	}
	return NULL;
}

//��������ģʽFP���ֵ䣬����ÿһ���ؼ��ֶ�Ӧ������Ƶ��ģʽ
void fptree::ObtainFrequenPattern(map<string,FPNode *> &cpbFPTreeMap,map<string,map<list<string>,int>* > &frequentPatternMap)
{//if(cpbFPTreeMap.empty()){cout<<"cpbFPTreeMap is empty"<<endl;}
	map<string,FPNode *>::iterator cpbTreeIt;
	for(cpbTreeIt=cpbFPTreeMap.begin();cpbTreeIt!=cpbFPTreeMap.end();cpbTreeIt++)	{
		string nodeName=cpbTreeIt->first;
		FPNode *cpbRoot=cpbTreeIt->second;
		//���ݹؼ���nodeName��Ӧ������ģʽ��������·��������ŵ�pathSet��
		vector<list<FPNode> > pathSet;
		list<FPNode> path;

		CreatePath(cpbRoot,pathSet);
		//���ÿ�����Ӧ��ȫ��ǰ׺·��

		vector<list<FPNode> >::iterator pathSetIt;
		for(pathSetIt=pathSet.begin();pathSetIt!=pathSet.end();pathSetIt++)
		{
			list<FPNode> pathTemp=*pathSetIt;

			//����ÿ��·���������Ӧ��Ƶ��ģʽ���ϣ��ŵ�fp��
			map<list<string>,int>* fp=NULL;
			fp=CreateFP(pathTemp);
			if(fp==NULL)	continue;
			map<string,map<list<string>,int>* >::iterator fpmIt;
			fpmIt=frequentPatternMap.find(nodeName);
			if(fpmIt==frequentPatternMap.end())	{//�ؼ���nodeNameδ��Ƶ��ģʽ�ֵ��г��ֹ�,�Ͱ�������Ӧ��Ƶ��ģʽ�Ӽ�����fp�������
				frequentPatternMap.insert(pair<string,map<list<string>,int>* >(nodeName,fp));
			}
			else	{
				map<list<string>,int>::iterator fpIt=fp->begin();
				unsigned int sizeFp=fp->size();
				map<list<string>,int>* pt=fpmIt->second;
				map<list<string>,int>::iterator ptIt=pt->begin();
				unsigned int sizePt=pt->size();
				for(;fpIt!=fp->end();++fpIt)	{//����fp�е�list�Ƿ���pt��list���г��ֹ�
					bool flag=true;
					int j=0;
					for(ptIt=pt->begin(),j=0;j<sizePt&&ptIt!=pt->end();++j,++ptIt)	{
						list<string>::const_iterator t1It=(fpIt->first).begin();
						unsigned int sizeT1=(fpIt->first).size();
						list<string>::const_iterator t2It=(ptIt->first).begin();
						unsigned int sizeT2=(ptIt->first).size();
						if(sizeT1!=sizeT2)	{
							flag=false;
							continue;
						}
						for(int k=0;k<sizeT1;k++)	{
							if(*t1It!=*t2It)	{
								flag=false;
								break;
							}
							t1It++;
							t2It++;
							if(k==sizeT1-1)	{
								flag=true;
							}
						}
						if(flag==true)	{
							break;
						}
					}
					if(flag==true)	{
						ptIt->second+=fpIt->second;
					}
					else	{
						pt->insert(pair<list<string>,int>(fpIt->first,fpIt->second));
					}
				}
			}
		}
	}
}

//�������Ƶ��ģʽ
void fptree::OutputFrequentPattern(map<string,map<list<string>,int>* > &frequentPatternMap,unsigned int minSupport)
{
	map<string,map<list<string>,int>* >::iterator it;
	int geshu=0;
	for(it=frequentPatternMap.begin();it!=frequentPatternMap.end();it++)	{
		geshu++;
		string nodeName=it->first;
		map<list<string>,int>* fpSet=it->second;
		unsigned int size=fpSet->size();

		map<list<string>,int>::iterator fpSetIt=fpSet->begin();
		cout<<nodeName<<":"<<endl;
		for(int i=0;i<size;++i,++fpSetIt)	{
			const list<string> *fp=&(fpSetIt->first);
			unsigned int cnt=fpSetIt->second;
			if(cnt>=minSupport)	{
				unsigned int len=fp->size();
				list<string>::const_iterator fpIt=fp->begin();
				outfile<<"{ ";
				for(int j=0;j<len;j++,++fpIt)	{
					outfile<<*fpIt<<" ";
				}
				outfile<<nodeName<<" }"<<endl;
			}
		}
	}
	outfile<<geshu<<"---";
}

//���ÿһ���ȫ������ģʽ��
void fptree::OutputConditionalPatterBase(map<string,vector<vector<string> > > &cpbMap)
{
	map<string,vector<vector<string> > >::iterator cpbMapIt;
	for(cpbMapIt=cpbMap.begin();cpbMapIt!=cpbMap.end();cpbMapIt++)	{
		string nodeName=cpbMapIt->first;
		vector<vector<string> > cpbSet=cpbMapIt->second;
		vector<vector<string> >::iterator cpbSetIt;
		outfile<<nodeName<<"�� "<<cpbSet.size()<<" ������ģʽ��:"<<endl;
		for(cpbSetIt=cpbSet.begin();cpbSetIt!=cpbSet.end();cpbSetIt++)	{
			vector<string> cpb=*cpbSetIt;
			vector<string>::iterator cpbIt;
			for(cpbIt=cpb.begin();cpbIt!=cpb.end();cpbIt++)	{
				outfile<<*cpbIt<<" ";
			}
			outfile<<endl;
		}
	}
}
//���ÿһ�������ģʽFP��
void fptree::OutputCpbFPTree(map<string,FPNode *> &cpbFPTreeMap)
{
	map<string,FPNode *>::iterator cpbFPTreeMapIt;
	for(cpbFPTreeMapIt=cpbFPTreeMap.begin();cpbFPTreeMapIt!=cpbFPTreeMap.end();cpbFPTreeMapIt++)	{
		string nodeName=cpbFPTreeMapIt->first;
		FPNode *root=cpbFPTreeMapIt->second;
		outfile<<nodeName<<" ������ģʽFP��Ϊ:"<<endl;
		OutputFPTree(root);
	}
}

void fptree::action()
{
//	int minSupport=minSup*totalrecord/100;
	int minSupport=minSup*100000/100;

	vector<vector<string> >database;
	ObtainDatabase(database,fileName);
	vector<FPNode> large1;
	CreateItemset(database,large1,minSupport);
	int k=1;
	vector<FPNode> largeTemp=large1;
	OutputLargeItemset(largeTemp,k);
	FPNode *root=NULL;
	CreateFPTree(database,large1,&root);
	map<string,vector<vector<string> > >cpbMap;
	CreateConditionalPatterBase(large1,cpbMap);
	map<string,FPNode *>cpbFPTreeMap;
	CreateCpbFPtree(cpbMap,cpbFPTreeMap,minSupport);
	map<string,map<list<string>,int>*>frequentPatternMap;
	ObtainFrequenPattern(cpbFPTreeMap,frequentPatternMap);
	OutputFrequentPattern(frequentPatternMap,minSupport);

	outfile.close();
}