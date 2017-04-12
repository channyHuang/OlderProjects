#include "apriori.h"
#include "data.h"

#include <QMessageBox>

apriori::apriori() {
	inFileName="./database.txt";
	outFile.open( "apriori_result.txt" );
//	inFileName="./testdata/INTEGRATED-DATASET.txt";
//	outFile.open("./testdata/apriori_result42.txt");
}

// 生成候选集
void apriori::CandidateGen( const CFreqItemSet& freqItemSet,  const CTransactionSet& transSet, CItemSetCountTable& candidateSet )
{
	candidateSet.clear();

	// 合并出候选项。    
	for ( CFreqItemSet::const_iterator cIt1 = freqItemSet.begin(); cIt1 != freqItemSet.end(); ++cIt1 )
		for ( CFreqItemSet::const_iterator cIt2 = freqItemSet.begin(); cIt2 != freqItemSet.end(); ++cIt2 )	{
			size_t freqItemNum = cIt1->size();

			CItemSet candidate;
			set_union( cIt1->begin(), cIt1->end(), cIt2->begin(), cIt2->end(), std::inserter( candidate,  candidate.begin() ) );

			// 合并出的candidate项数应该比frequency项数多1.
			if ( candidate.size() == freqItemNum + 1 )	{
				// candidate 里所有的k-1子集 都必须在 频繁项目集合中。
				// 挨个删掉一个元素就是所有k-1子集
				bool isSubset = true;
				CItemSet subSet = candidate;
				for ( CItemSet::iterator itDel = candidate.begin(); itDel != candidate.end(); ++itDel )	{
					subSet.erase( *itDel ); // 删除，获得子集。
					if ( freqItemSet.find( subSet ) == freqItemSet.end() )	{	// 没找到， 剪枝。
						isSubset = false;
						break;
					}
					subSet.insert( *itDel );        // 重新添加进来。
				}

				if ( isSubset )	candidateSet[ candidate ] = 0;
			}
		}
}

// Candidate Itemset 转换为 Frequency ItemSet, 要求必须大于最小支持度 minSup。
void apriori::Candidate2Freq( const CItemSetCountTable& candidateTable, float minSup,  int transactionCount, CFreqItemSet& freqSet, CCandidateItemTableList& allCanList )
{
	freqSet.clear();
	CItemSetCountTable validCandi;

	for ( CItemSetCountTable::const_iterator itCan = candidateTable.begin(); itCan != candidateTable.end(); ++itCan )	{	// 计算支持。
		float sup = float(itCan->second) / transactionCount;
		if ( sup < minSup )	continue;
		else	{
			freqSet.insert( itCan->first );
			validCandi.insert( *itCan );
		}
	}

	allCanList.push_back( validCandi );
}

// Apriori 算法 主函数。
void apriori::Apriori( const CTransactionSet& transSet, float minSup, float minConf, CCandidateItemTableList& allCanList )
{
	// 根据初始的单项频繁项目集生成候选项集集合。
	CItemSetCountTable candidateSet;
	// 进行初始的Candidate计数。
	for ( size_t i=0; i<transSet.size(); ++i )	{
		const CTransaction& tran = transSet[i];
		for ( CTransaction::const_iterator cIt = tran.begin(); cIt != tran.end(); ++cIt )	{
			CItemSet itemSet;
			itemSet.insert( *cIt );
			candidateSet[itemSet]++;
		}
	}

	// 将数据库transaction集合转换为初始的单项频繁项目集。
	CFreqItemSet freqSet;
	Candidate2Freq( candidateSet, minSup, transSet.size(), freqSet, allCanList );

	for ( int i=2; !freqSet.empty(); ++i )	{       
		// F(k-1) -> Ck 
		// 由当前级别的频繁项目集获取下一级的候选项集。
		CandidateGen( freqSet, transSet, candidateSet );

		// 遍历所有的数据事务， 对Candidate进行计数。
		for ( size_t transIndex=0; transIndex<transSet.size(); ++transIndex )	{
			const CTransaction& trans = transSet[ transIndex ];

			// 看每个候选集项是否在数据中，如果在就计数。
			for ( CItemSetCountTable::iterator itCan = candidateSet.begin(); itCan != candidateSet.end(); ++itCan )	{
				// 每个小项是否在数据中。
				bool isInclude = std::includes( trans.begin(), trans.end(), itCan->first.begin(), itCan->first.end() );
				if ( isInclude )	itCan->second++;
			}
		}
		// 将满足条件的Candidate转换为Frequence。
		// 要满足支持计数大于设定值minSup。
		Candidate2Freq( candidateSet, minSup, transSet.size(), freqSet, allCanList );
	}
}

void apriori::GenRules( const CItemSetCountTable& allFrequencyTable, CRuleInfoList& ruleList, float minConf )
{
	// 初始化， 后项为0的关联规则。
	CRuleInfoList curRuleList;
	for ( CItemSetCountTable::const_iterator itCan = allFrequencyTable.begin(); itCan!=allFrequencyTable.end(); ++itCan )	{
		// 打印输出。
		CRuleInfo ruleInfo;
		ruleInfo.rule = CRule( itCan->first, CItemSet() );
		curRuleList.insert( ruleInfo );
	}

	while ( !curRuleList.empty() )
	{
		// 当前的关联规则后项为 k 个元素， 我们依靠它去寻找后项为 k+1 的元素。
		// 根据关联规则的性质， 如果关联规则后项是k个元素的集合， 那么这k个元素的子集作为新的后项的关联规则一定成立（置信度合格）。
		// 反之， 我们只需要在合格的 后项是 k 个元素的所有关联规则中去生成后项是 k+1个元素的新关联规则。
		// 下一轮的Rule List。
		CRuleInfoList nextCurRuleList;

		for ( CRuleInfoList::const_iterator cItCurRuleList = curRuleList.begin(); cItCurRuleList != curRuleList.end(); ++cItCurRuleList )	{
			// 将这个关联规则项拆成多个， 分别将前面集合的一个元素分别移到后面。
			// 前面的条件不能为空。。
			const CRuleInfo& curRule = *cItCurRuleList;
			const CItemSet& firstItemSet = curRule.rule.first;
			for ( CItemSet::const_iterator cItItem = firstItemSet.begin(); cItItem != firstItemSet.end(); ++cItItem )	{
				CRule newRule = curRule.rule;
				newRule.first.erase( *cItItem );
				newRule.second.insert( *cItItem );

				// 新规则是否合格。 前项条件不为空。 置信度符合要求。
				if ( newRule.first.empty() )	continue;
				
				// 置信度计算， 等于合集的count 除以 前项条件的count。                             
				CItemSet unionSet;
				set_union( newRule.first.begin(), newRule.first.end(), newRule.second.begin(), newRule.second.end(), inserter( unionSet, unionSet.begin() ) );
				int unionCount = 0;
				CItemSetCountTable::const_iterator cItCountTable = allFrequencyTable.find( unionSet );
				if ( cItCountTable != allFrequencyTable.end() )	{
					unionCount = cItCountTable->second;
				}

				int firstCount = 0;
				cItCountTable = allFrequencyTable.find( newRule.first );
				if ( cItCountTable != allFrequencyTable.end() )	{
					firstCount = cItCountTable->second;
				}
				if ( firstCount == 0 )		continue;
				
				float conf =   float(unionCount) / firstCount;
				if ( conf < minConf )	continue;
				
				CRuleInfo newRuleInfo;
				newRuleInfo.rule = newRule;
				newRuleInfo.conf = conf;

				nextCurRuleList.insert( newRuleInfo );

				ruleList.insert( newRuleInfo );
			}
		}
		// 下一轮。
		curRuleList = nextCurRuleList;
	}
}

void apriori::action()
{
	ifstream inFile( inFileName );
	if ( !inFile ) {
		QMessageBox::about(NULL,"File not found!","Sorry, the data file could not be opened.");
		return ;
	}
	// 读取事务集。
	// 简化， 用数据结构代表数据库， 用 STL 的set， 满足 Apriori 算法对字典序的要求。。
	CTransactionSet transSet;
/*	while( inFile )	{
		CTransaction tran;
		// 读取一个事务。
		bool tranBegin = false; // 是否开始了事务。
		while( inFile )	{
			string item;
			inFile >> item;
			if ( tranBegin )	{
				if ( item == "}" ) break;
				else	tran.insert( item );
			}
			else	{
				if ( item == "{" )		tranBegin = true;
			}
		}	           
	}*/
		string s="";
		while(getline(inFile,s)) {
			CTransaction tran;
			// 读取一个事务。
			int len=s.length();
			string str="";
			for(int i=0;i<len;i++) {
				if(s[i]!=' ') str+=s[i];
				else if(s[i]==' '||i==len-1) {
					tran.insert(str);
					str="";
				}
			}
			if ( !tran.empty() )	{
				transSet.push_back( tran );
			}    
		}

	// 调用算法, 得到所有的频繁项目集。

	CCandidateItemTableList allCanTableList;
	Apriori( transSet, float(minSup)/100, float(minConf)/100, allCanTableList );

	outFile << "支持度大于 " << minSup << "% 的频繁项目集以及支持度信息：" << endl << endl;
	for ( size_t i=0; i<allCanTableList.size(); ++i )
		for ( CItemSetCountTable::iterator itCan = allCanTableList[i].begin(); itCan!=allCanTableList[i].end(); ++itCan )	{
			// 打印输出。
//			outFile << itCan->first << " cout: " << itCan->second << "\t Sup: " << float(itCan->second)/transSet.size() << endl;
			for ( CItemSet::const_iterator cIt = itCan->first.begin(); cIt != itCan->first.end(); ++cIt )	{
				outFile << *cIt << " ";
			}
			outFile << "\t";
			outFile << " cout: " << itCan->second << "\t Sup: " << float(itCan->second)/transSet.size() << endl;
		}

	// 生成关联规则。
	CItemSetCountTable allCanTalbe;
	for ( size_t i=0; i<allCanTableList.size(); ++i )	{
		allCanTalbe.insert(allCanTableList[i].begin(), allCanTableList[i].end() );
	}
	CRuleInfoList allRules;
	GenRules( allCanTalbe, allRules, float( minConf ) / 100 );

	outFile << std::endl << std::endl << "开始挖掘其中的置信度大于 " << minConf << "% 的关联规则" << std::endl << std::endl;
	for ( CRuleInfoList::const_iterator cItRule = allRules.begin(); cItRule != allRules.end(); ++cItRule)	{
		const CRuleInfo& ruleInfo = *cItRule;
//		outFile << "{ " << ruleInfo.rule.first << " } --> { " << ruleInfo.rule.second << " } \t conf: " << ruleInfo.conf << std::endl;
		outFile << "{ " ;
		for ( CItemSet::const_iterator cIt = ruleInfo.rule.first.begin(); cIt != ruleInfo.rule.first.end(); ++cIt )	{
			outFile << *cIt << " ";
		}
		outFile << "\t";
		outFile << " } --> { " ;
		for ( CItemSet::const_iterator cIt = ruleInfo.rule.second.begin(); cIt != ruleInfo.rule.second.end(); ++cIt )	{
			outFile << *cIt << " ";
		}
		outFile << "\t";
		outFile << " } \t conf: " << ruleInfo.conf << std::endl;
	}

	outFile.close();
}