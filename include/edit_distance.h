#ifndef __EDIT_DISTANCE_H__
#define __EDIT_DISTANCE_H__
#include <string>
namespace NM
{
    struct Editdistance
    {
        int operator()(const std::string& src,const std::string& des);
    };
}
#endif
