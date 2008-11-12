#include "dict.h"
#include <vector>
#include <iostream>
//std::getline(std::istream,std::string);
SpellDict::SpellDict(std::istream & DictSource)
{
    std::string S;
    while (!DictSource.eof())
    {
        std::getline(DictSource,S);
        if (S != "")
        {
            D.push_back(S);
        }
    }
}
bool SpellDict::Contains(char * Spell)
{
    //I know this is slow.
    for (std::vector<std::string>::const_iterator i = D.begin();i < D.end();i++)
    {
        if (*i == Spell)
            return true;
    }
    return false;
}
