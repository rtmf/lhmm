#ifndef __DICT_H__
#define __DICT_H__
#include <string>
#include <iostream>
#include <vector>
class SpellDict
{
    public:
        SpellDict(std::istream & DictSource);
        ~SpellDict() {}; // I think the vector destructor makes this safe
        bool Contains(char * Spell);
    private:
        std::vector <std::string> D;
};
#endif
