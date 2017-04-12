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

//读取文件获取整个数据库存储在database中，fileName必须为char*型，要是用string会报错，in()不认
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
	while(getline(in,s))	{//读取一行记录
		i++;
		vector<string> transaction;	
		int len=s.length();
		string str="";
		for(int i=0;i<len;i++)	{//将记录中的数提取出来
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
	outfile<<"总事务数："<<i<<endl;
	return true;
}

//遍历一遍数据库，创建1-项大项集
void fptree::CreateItemset(vector<vector<string> > &database,vector<FPNode> &largeItemset,unsigned int minSupport)
{
	map<string,int> dir;
	map<string,int>::iterator dirIt;

	vector<vector<string> >::iterator databaseIt;

	vector<string> temp;
	vector<string>::iterator tempIt;

	//根据数据库创建字典，字典形式为<item,count>
	for(databaseIt=database.begin();databaseIt!=database.end();databaseIt++)	{
		temp=*databaseIt;
		for(tempIt=temp.begin();tempIt!=temp.end();tempIt++)	{
			string item=*tempIt;
			dirIt=dir.find(item);
			if(dirIt==dir.end())	{//item不在字典dir中
				dir.insert(pair<string,int>(item,1));
			}
			else	{//item在字典dir中，则将其count值加1
				(dirIt->second)++;
			}
		}
	}

	//从字典中选出支持度超过minSopport的item
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
	//根据每个节点的count值对大项集排序
	sort(largeItemset.begin(),largeItemset.end());
}

//输出一项大项集
void fptree::OutputLargeItemset(vector<FPNode> &largeItemset,unsigned int i)
{
	outfile<<"包含 "<<largeItemset.size()<<" 项的 "<<i<<"-项大项集："<<endl;	
	vector<FPNode>::iterator largeItemsetIt;
	for(largeItemsetIt=largeItemset.begin();largeItemsetIt!=largeItemset.end();largeItemsetIt++)	{
		FPNode temp=*largeItemsetIt;
		outfile<<"{ "<<temp.nodeName<<" : "<<temp.count<<" }"<<endl;
	}
	outfile<<endl<<endl;
}

//对数据库某条交易trans筛选出在1-项大项集large1中出现的项，并按在1-项大项集large1出现的顺序排序，放到tempTrans中
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

//利用包括transTemp[k]在内及其以后的值为root节点创建子树
int fptree::AddNode(FPNode *parentNode,vector<string> &transTemp,unsigned int k,vector<FPNode> &large1)
{
	unsigned int size=transTemp.size();
	if(size>0&&(k>=0&&k<size))	{
		FPNode *nodeTemp=new FPNode;//创建新节点
		if(nodeTemp==NULL)	return 1;
		nodeTemp->nodeName=transTemp[k];
		nodeTemp->count=1;
		nodeTemp->parentNode=parentNode;
		nodeTemp->siblingNode=NULL;

		(parentNode->childNode).push_back(nodeTemp);

		//将nodeTemp添加到它的兄弟节点上。
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
		if(k<size)	{//此交易中还有其它项
			AddNode(nodeTemp,transTemp,k,large1);
		}
	}
	return 0;
}

//创建FP-Tree
int fptree::CreateFPTree(vector<vector<string> > &database,vector<FPNode> &large1,FPNode **treeRoot)
{
	*treeRoot=new FPNode;
	FPNode *root=*treeRoot;
	if(root==NULL)	return 1;
	root->nodeName="-1";//-1表示空节点
	root->count=0;
	root->parentNode=NULL;
	(root->childNode).clear();
	root->siblingNode=NULL;

	unsigned int sizeDatabase=database.size();
	for(int i=0;i<sizeDatabase;i++)	{
		vector<string> trans=database[i];//取出数据库一条交易
		vector<string> transTemp;
		SortItem(trans,transTemp,large1);//把此交易包含于large1中的项筛选出来并排序，放到transTemp中
		vector<FPNode *> childNode=root->childNode;//获取根的儿子节点集合
		if(childNode.size()==0)	{//如果树根的儿子节点集合为空，说明这个树为空。则把此交易转换为树根root的子树。
			AddNode(root,transTemp,0,large1);
		}
		else	{//树不为空
			unsigned int sizeTrans=transTemp.size();
			FPNode *pt=NULL;
			for(int i=0;i<sizeTrans;i++)	{//先查找transTemp[i]是否在树中出现过
				string temp=transTemp[i];				
				unsigned int sizeChild=childNode.size();
				int j=0;
				for(j=0;j<sizeChild;j++)	{
					if(temp==(childNode[j])->nodeName)	{
						pt=childNode[j];
						(pt->count)++;
						//transTemp[i]已经在树中出现过了,则把它的计数器加1
						childNode=pt->childNode;
						break;
					}
				}				
				if(j==sizeChild)	{
					if(pt==NULL)	{//transTemp[i]未在树中，则添加包括i节点之后的所有节点，添加到树根上
						AddNode(root,transTemp,i,large1);
					}
					else	{//transTemp[i]已在树中出现过，但是它还没儿子节点，所以把它后面的节点挂在它后面
						AddNode(pt,transTemp,i,large1);//这个地方不是i+1
					}
					break;
				}
			}
		}
	}
	return 0;
}

//获取large1除第一项之外的各项的conditional pattern base条件模式基。该过程即是建立每一项对应的子数据库。
void fptree::CreateConditionalPatterBase(vector<FPNode> &large1,map<string,vector<vector<string> > > &cpbMap)
{
	vector<string> condition;
	int sizeLarge=large1.size();
	//large1中除了第一项外(因为直接挂在树根上的节点肯定没有前缀，所以不用求了，免得浪费时间)，全部求条件模式基
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
			while(parent!=NULL)	{//逆向向上，把除树根之外的父节点添加到路径path中
				if(parent->parentNode!=NULL)	{//parent的父节点为空，说明parent是树根，树根里不含项
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

//输出FP树
void fptree::OutputFPTree(FPNode *root)
{
	if(root==NULL)	return;
	vector<FPNode*> childNode=root->childNode;
	outfile<<"{"<<root->nodeName<<","<<root->count<<","<<childNode.size();
	if(childNode.size()!=0)	{
		outfile<<"}儿子如下:"<<endl;
		for(int i=0;i<childNode.size();i++)	{
			OutputFPTree(childNode[i]);
		}
	}
	else 	{
		outfile<<"}无子"<<endl;
	}	
}

//由每一项的条件模式基得到对应的条件模式FP树，最终得到条件模式FP树字典
void fptree::CreateCpbFPtree(map<string,vector<vector<string> > > &cpbMap,map<string,FPNode *> &cpbFPTreeMap,unsigned int minSupport)
{
	map<string,vector<vector<string> > >::iterator cpbMapIt;
	for(cpbMapIt=cpbMap.begin();cpbMapIt!=cpbMap.end();cpbMapIt++)	{
		string nodeName=cpbMapIt->first;
		vector<vector<string> > pathSet=cpbMapIt->second;
		vector<FPNode> cpbLarge;
		//根据nodeName项对应的条件模式基集合pathSet创建它的频繁一项集cpbLarge
		CreateItemset(pathSet,cpbLarge,minSupport);
		//输出刚刚创建的频繁一项集

		FPNode *cpbRoot=NULL;
		if(!(pathSet.empty())&&!(cpbLarge.empty()))	{
			CreateFPTree(pathSet,cpbLarge,&cpbRoot);//创建nodeName对应的条件模式FP树
		}

		if(cpbRoot!=NULL)	{//创建成功
			vector<FPNode *> *childNode=&(cpbRoot->childNode);
			vector<FPNode *>::iterator childIt=(*childNode).begin();
			unsigned int size=(*childNode).size();
			for(int i=0;i<size;i++)	{
				if((*childIt)->count< minSupport)	{
					//删掉最小支持度不满足的分支				
					DestroyTree(*childIt);
					childIt=(*childNode).erase(childIt);
				}
				else	{
					childIt++;
				}
			}			
			if((*childNode).size()!=0)	{//如果这个条件模式树树根的儿子节点个数不为0，则把它加到条件模式FP树字典中
				cpbFPTreeMap.insert(pair<string,FPNode*>(nodeName,cpbRoot));
			}
		}
	}
}

//根据每个关键字的条件模式FP树，创建出所有的路径并放到路径集合pathSet中
void fptree::CreatePath(FPNode *root,vector<list<FPNode> > &pathSet)
{
	if(root!=NULL)	{
		if((root->childNode).empty())	{//root没有分支
			list<FPNode> path;
			if(root->parentNode!=NULL)	{//不能把树根添加到路径中
				path.push_back(*root);
			}
			if(path.size()!=0)	{
				pathSet.push_back(path);
			}
		}
		else	{//root有分支
			vector<FPNode *> childNode=root->childNode;
			unsigned int size=childNode.size();
			if(size>1)	{//root有多个分支
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
			else	{//root只有一个分支
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

//输出每个项对应的全部前缀路径
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

//根据path创建fp，k初始值为path.size()-1
map<list<string>,int>* fptree::CreateFP(list<FPNode> &path)
{
	if(path.size()>0)	{
		//把路径path的第一个节点加入
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

//根据条件模式FP树字典，生成每一个关键字对应的所有频繁模式
void fptree::ObtainFrequenPattern(map<string,FPNode *> &cpbFPTreeMap,map<string,map<list<string>,int>* > &frequentPatternMap)
{//if(cpbFPTreeMap.empty()){cout<<"cpbFPTreeMap is empty"<<endl;}
	map<string,FPNode *>::iterator cpbTreeIt;
	for(cpbTreeIt=cpbFPTreeMap.begin();cpbTreeIt!=cpbFPTreeMap.end();cpbTreeIt++)	{
		string nodeName=cpbTreeIt->first;
		FPNode *cpbRoot=cpbTreeIt->second;
		//根据关键字nodeName对应的条件模式树把所有路径择出来放到pathSet中
		vector<list<FPNode> > pathSet;
		list<FPNode> path;

		CreatePath(cpbRoot,pathSet);
		//输出每个项对应的全部前缀路径

		vector<list<FPNode> >::iterator pathSetIt;
		for(pathSetIt=pathSet.begin();pathSetIt!=pathSet.end();pathSetIt++)
		{
			list<FPNode> pathTemp=*pathSetIt;

			//根据每条路径创建其对应的频繁模式集合，放到fp中
			map<list<string>,int>* fp=NULL;
			fp=CreateFP(pathTemp);
			if(fp==NULL)	continue;
			map<string,map<list<string>,int>* >::iterator fpmIt;
			fpmIt=frequentPatternMap.find(nodeName);
			if(fpmIt==frequentPatternMap.end())	{//关键字nodeName未在频繁模式字典中出现过,就把它和相应的频繁模式子集集合fp加入进来
				frequentPatternMap.insert(pair<string,map<list<string>,int>* >(nodeName,fp));
			}
			else	{
				map<list<string>,int>::iterator fpIt=fp->begin();
				unsigned int sizeFp=fp->size();
				map<list<string>,int>* pt=fpmIt->second;
				map<list<string>,int>::iterator ptIt=pt->begin();
				unsigned int sizePt=pt->size();
				for(;fpIt!=fp->end();++fpIt)	{//查找fp中的list是否在pt中list表中出现过
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

//输出所有频繁模式
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

//输出每一项的全部条件模式基
void fptree::OutputConditionalPatterBase(map<string,vector<vector<string> > > &cpbMap)
{
	map<string,vector<vector<string> > >::iterator cpbMapIt;
	for(cpbMapIt=cpbMap.begin();cpbMapIt!=cpbMap.end();cpbMapIt++)	{
		string nodeName=cpbMapIt->first;
		vector<vector<string> > cpbSet=cpbMapIt->second;
		vector<vector<string> >::iterator cpbSetIt;
		outfile<<nodeName<<"有 "<<cpbSet.size()<<" 个条件模式基:"<<endl;
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
//输出每一项的条件模式FP树
void fptree::OutputCpbFPTree(map<string,FPNode *> &cpbFPTreeMap)
{
	map<string,FPNode *>::iterator cpbFPTreeMapIt;
	for(cpbFPTreeMapIt=cpbFPTreeMap.begin();cpbFPTreeMapIt!=cpbFPTreeMap.end();cpbFPTreeMapIt++)	{
		string nodeName=cpbFPTreeMapIt->first;
		FPNode *root=cpbFPTreeMapIt->second;
		outfile<<nodeName<<" 的条件模式FP树为:"<<endl;
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