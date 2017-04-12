#include "apriori.h"
#include "data.h"

#include <QMessageBox>

apriori::apriori() {
	inFileName="./database.txt";
	outFile.open( "apriori_result.txt" );
//	inFileName="./testdata/INTEGRATED-DATASET.txt";
//	outFile.open("./testdata/apriori_result42.txt");
}

// ���ɺ�ѡ��
void apriori::CandidateGen( const CFreqItemSet& freqItemSet,  const CTransactionSet& transSet, CItemSetCountTable& candidateSet )
{
	candidateSet.clear();

	// �ϲ�����ѡ�    
	for ( CFreqItemSet::const_iterator cIt1 = freqItemSet.begin(); cIt1 != freqItemSet.end(); ++cIt1 )
		for ( CFreqItemSet::const_iterator cIt2 = freqItemSet.begin(); cIt2 != freqItemSet.end(); ++cIt2 )	{
			size_t freqItemNum = cIt1->size();

			CItemSet candidate;
			set_union( cIt1->begin(), cIt1->end(), cIt2->begin(), cIt2->end(), std::inserter( candidate,  candidate.begin() ) );

			// �ϲ�����candidate����Ӧ�ñ�frequency������1.
			if ( candidate.size() == freqItemNum + 1 )	{
				// candidate �����е�k-1�Ӽ� �������� Ƶ����Ŀ�����С�
				// ����ɾ��һ��Ԫ�ؾ�������k-1�Ӽ�
				bool isSubset = true;
				CItemSet subSet = candidate;
				for ( CItemSet::iterator itDel = candidate.begin(); itDel != candidate.end(); ++itDel )	{
					subSet.erase( *itDel ); // ɾ��������Ӽ���
					if ( freqItemSet.find( subSet ) == freqItemSet.end() )	{	// û�ҵ��� ��֦��
						isSubset = false;
						break;
					}
					subSet.insert( *itDel );        // �������ӽ�����
				}

				if ( isSubset )	candidateSet[ candidate ] = 0;
			}
		}
}

// Candidate Itemset ת��Ϊ Frequency ItemSet, Ҫ����������С֧�ֶ� minSup��
void apriori::Candidate2Freq( const CItemSetCountTable& candidateTable, float minSup,  int transactionCount, CFreqItemSet& freqSet, CCandidateItemTableList& allCanList )
{
	freqSet.clear();
	CItemSetCountTable validCandi;

	for ( CItemSetCountTable::const_iterator itCan = candidateTable.begin(); itCan != candidateTable.end(); ++itCan )	{	// ����֧�֡�
		float sup = float(itCan->second) / transactionCount;
		if ( sup < minSup )	continue;
		else	{
			freqSet.insert( itCan->first );
			validCandi.insert( *itCan );
		}
	}

	allCanList.push_back( validCandi );
}

// Apriori �㷨 ��������
void apriori::Apriori( const CTransactionSet& transSet, float minSup, float minConf, CCandidateItemTableList& allCanList )
{
	// ���ݳ�ʼ�ĵ���Ƶ����Ŀ�����ɺ�ѡ����ϡ�
	CItemSetCountTable candidateSet;
	// ���г�ʼ��Candidate������
	for ( size_t i=0; i<transSet.size(); ++i )	{
		const CTransaction& tran = transSet[i];
		for ( CTransaction::const_iterator cIt = tran.begin(); cIt != tran.end(); ++cIt )	{
			CItemSet itemSet;
			itemSet.insert( *cIt );
			candidateSet[itemSet]++;
		}
	}

	// �����ݿ�transaction����ת��Ϊ��ʼ�ĵ���Ƶ����Ŀ����
	CFreqItemSet freqSet;
	Candidate2Freq( candidateSet, minSup, transSet.size(), freqSet, allCanList );

	for ( int i=2; !freqSet.empty(); ++i )	{       
		// F(k-1) -> Ck 
		// �ɵ�ǰ�����Ƶ����Ŀ����ȡ��һ���ĺ�ѡ���
		CandidateGen( freqSet, transSet, candidateSet );

		// �������е��������� ��Candidate���м�����
		for ( size_t transIndex=0; transIndex<transSet.size(); ++transIndex )	{
			const CTransaction& trans = transSet[ transIndex ];

			// ��ÿ����ѡ�����Ƿ��������У�����ھͼ�����
			for ( CItemSetCountTable::iterator itCan = candidateSet.begin(); itCan != candidateSet.end(); ++itCan )	{
				// ÿ��С���Ƿ��������С�
				bool isInclude = std::includes( trans.begin(), trans.end(), itCan->first.begin(), itCan->first.end() );
				if ( isInclude )	itCan->second++;
			}
		}
		// ������������Candidateת��ΪFrequence��
		// Ҫ����֧�ּ��������趨ֵminSup��
		Candidate2Freq( candidateSet, minSup, transSet.size(), freqSet, allCanList );
	}
}

void apriori::GenRules( const CItemSetCountTable& allFrequencyTable, CRuleInfoList& ruleList, float minConf )
{
	// ��ʼ���� ����Ϊ0�Ĺ�������
	CRuleInfoList curRuleList;
	for ( CItemSetCountTable::const_iterator itCan = allFrequencyTable.begin(); itCan!=allFrequencyTable.end(); ++itCan )	{
		// ��ӡ�����
		CRuleInfo ruleInfo;
		ruleInfo.rule = CRule( itCan->first, CItemSet() );
		curRuleList.insert( ruleInfo );
	}

	while ( !curRuleList.empty() )
	{
		// ��ǰ�Ĺ����������Ϊ k ��Ԫ�أ� ����������ȥѰ�Һ���Ϊ k+1 ��Ԫ�ء�
		// ���ݹ�����������ʣ� ����������������k��Ԫ�صļ��ϣ� ��ô��k��Ԫ�ص��Ӽ���Ϊ�µĺ���Ĺ�������һ�����������ŶȺϸ񣩡�
		// ��֮�� ����ֻ��Ҫ�ںϸ�� ������ k ��Ԫ�ص����й���������ȥ���ɺ����� k+1��Ԫ�ص��¹�������
		// ��һ�ֵ�Rule List��
		CRuleInfoList nextCurRuleList;

		for ( CRuleInfoList::const_iterator cItCurRuleList = curRuleList.begin(); cItCurRuleList != curRuleList.end(); ++cItCurRuleList )	{
			// ����������������ɶ���� �ֱ�ǰ�漯�ϵ�һ��Ԫ�طֱ��Ƶ����档
			// ǰ�����������Ϊ�ա���
			const CRuleInfo& curRule = *cItCurRuleList;
			const CItemSet& firstItemSet = curRule.rule.first;
			for ( CItemSet::const_iterator cItItem = firstItemSet.begin(); cItItem != firstItemSet.end(); ++cItItem )	{
				CRule newRule = curRule.rule;
				newRule.first.erase( *cItItem );
				newRule.second.insert( *cItItem );

				// �¹����Ƿ�ϸ� ǰ��������Ϊ�ա� ���Ŷȷ���Ҫ��
				if ( newRule.first.empty() )	continue;
				
				// ���Ŷȼ��㣬 ���ںϼ���count ���� ǰ��������count��                             
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
		// ��һ�֡�
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
	// ��ȡ���񼯡�
	// �򻯣� �����ݽṹ�������ݿ⣬ �� STL ��set�� ���� Apriori �㷨���ֵ����Ҫ�󡣡�
	CTransactionSet transSet;
/*	while( inFile )	{
		CTransaction tran;
		// ��ȡһ������
		bool tranBegin = false; // �Ƿ�ʼ������
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
			// ��ȡһ������
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

	// �����㷨, �õ����е�Ƶ����Ŀ����

	CCandidateItemTableList allCanTableList;
	Apriori( transSet, float(minSup)/100, float(minConf)/100, allCanTableList );

	outFile << "֧�ֶȴ��� " << minSup << "% ��Ƶ����Ŀ���Լ�֧�ֶ���Ϣ��" << endl << endl;
	for ( size_t i=0; i<allCanTableList.size(); ++i )
		for ( CItemSetCountTable::iterator itCan = allCanTableList[i].begin(); itCan!=allCanTableList[i].end(); ++itCan )	{
			// ��ӡ�����
//			outFile << itCan->first << " cout: " << itCan->second << "\t Sup: " << float(itCan->second)/transSet.size() << endl;
			for ( CItemSet::const_iterator cIt = itCan->first.begin(); cIt != itCan->first.end(); ++cIt )	{
				outFile << *cIt << " ";
			}
			outFile << "\t";
			outFile << " cout: " << itCan->second << "\t Sup: " << float(itCan->second)/transSet.size() << endl;
		}

	// ���ɹ�������
	CItemSetCountTable allCanTalbe;
	for ( size_t i=0; i<allCanTableList.size(); ++i )	{
		allCanTalbe.insert(allCanTableList[i].begin(), allCanTableList[i].end() );
	}
	CRuleInfoList allRules;
	GenRules( allCanTalbe, allRules, float( minConf ) / 100 );

	outFile << std::endl << std::endl << "��ʼ�ھ����е����Ŷȴ��� " << minConf << "% �Ĺ�������" << std::endl << std::endl;
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