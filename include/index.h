#ifndef __INDEX_H__
#define __INDEX_H__
#include <vector>
#include <ext/hash_map>
#include <set>
#include <string>
#include <common.h>
namespace NM
{
    class CIndex
    {
        public:
        typedef std::vector<std::pair<std::string,int> > IndexVecType;//词典，存储词和频率
        typedef __gnu_cxx::hash_map<std::string,std::set<IndexVecType::size_type>,NM::CMyHash > IndexMapType;  //每个单词的字母对应一个vector的下标
        typedef std::vector<IndexVecType::size_type> IndexQueryVecType;
            CIndex()
            {
#ifndef BUILD_INDEX
                loadIndexVecFromDiction();
                loadIndexMapFromIndexFile();
#endif
            }
            static CIndex* getInstance();
            void loadIndexVecFromDiction();
            void buildIndexFromDiction();
            void saveIndexToFile();
            void loadIndexMapFromIndexFile();
            void getQueryIndex(const std::string& word,std::set<IndexQueryVecType::size_type>& queryIndex);
            IndexVecType& getIndexVec()
            {
                return m_index_vec;
            }
        private:
            IndexVecType  m_index_vec;
            IndexMapType  m_index_map;
            void splitWordToLetter(const std::string& word,std::vector<std::string>& letterVec);
            

    };
}
#endif
