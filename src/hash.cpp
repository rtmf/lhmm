#include <string>
#include "hash.h"
SpellHasher::SpellHasher (char * Salt)
{
    hashedsalt = 0;
    hashedsalt = Hash(Salt);
}
unsigned long int SpellHasher::Hash(const char * spell)
{
    unsigned long int temphash = hashedsalt;
    for (unsigned long int i=0;i<strlen(spell);i++)
    {
        temphash^=spell[i];
        temphash*=(i+1);
        temphash^=spell[i]*(i+1);
        temphash*=spell[i];
        temphash+=spell[i];
        temphash+=(i+1)*(i+1);
        temphash^=spell[i];
        temphash = ((temphash<<5)^(temphash>>27))^spell[i];
    }
    return temphash;
}
