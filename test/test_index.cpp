#include "index.h"
int main(int argc,char* argv)
{
    NM::CIndex* pIndex = NM::CIndex::getInstance();
    pIndex->loadIndexMapFromIndexFile();
}
