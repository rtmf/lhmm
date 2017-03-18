#ifndef __HASH_H__
#define __HASH_H__
#include <string.h>
class SpellHasher
{
    public:
        SpellHasher (char * Salt = "");
        ~SpellHasher () {};
        unsigned long int Hash (const char * spell);
    private:
        unsigned long int hashedsalt;
};
#endif
