#ifndef APRIORI_H
#define APRIORI_H

#include <iostream>
#include <fstream>
#include <set>
#include <vector>
#include <string>
#include <algorithm>
#include <iterator>
#include <map>

using namespace std;

// 元素。
typedef std::string CItem;

// 元素集合。
typedef std::set< CItem > CItemSet;

// 事务
typedef CItemSet CTransaction;

// 事务集
typedef std::vector< CTransaction > CTransactionSet;

// 候选项集合。
// key: 项集合， value: 计数
typedef std::map< CItemSet, int > CItemSetCountTable;
typedef std::vector< CItemSetCountTable > CCandidateItemTableList;

// 频繁项目集合。
typedef std::set<CItemSet> CFreqItemSet;

typedef std::pair< CItemSet, CItemSet > CRule;
struct CRuleInfo
{
	CRule rule;     // 关联规则。
	float conf;             // 置信度。
	CRuleInfo() : conf( 0 )
	{}

	bool operator<( const CRuleInfo& another ) const
	{
		return this->rule < another.rule;
	}
};
typedef std::set< CRuleInfo > CRuleInfoList;
/*
ostream& operator << ( ostream& oss, const CItemSet& itemset )
{
	for ( CItemSet::const_iterator cIt = itemset.begin(); cIt != itemset.end(); ++cIt )	{
		oss << *cIt << " ";
	}
	oss << "\t";
	return oss;
}
*/
class apriori 
{
private:
	string inFileName;
	ofstream outFile;
public:
	apriori();

	void CandidateGen(const CFreqItemSet& freqItemSet,  const CTransactionSet& transSet, CItemSetCountTable& candidateSet);
	void Candidate2Freq( const CItemSetCountTable& candidateTable, float minSup,  int transactionCount, CFreqItemSet& freqSet, CCandidateItemTableList& allCanList );
	void Apriori(const CTransactionSet& transSet, float minSup, float minConf, CCandidateItemTableList& allCanList);
	void GenRules( const CItemSetCountTable& allFrequencyTable, CRuleInfoList& ruleList, float minConf );

	void action();

};

#endif