#include <iostream>
#include <fstream>
#include "hash.h"
#include "dict.h"
using namespace std;
void parsehash(unsigned long int hash)
{
    unsigned int spelltype=hash%4;
    hash/=4;
    unsigned int spellcolor=hash%360;
    hash/=360;
    unsigned int strength=hash%256;
    hash/=256;
    cout << "\tColor : "<<spellcolor<<endl;
    cout << "\tStrength : "<<strength<<endl;
    switch (spelltype)
    {
        case 0:
            cout << "\tSummon Block" << endl;
            unsigned int blocktype=hash%9;
            hash/=9;
            cout << "\tType : ";
            switch (blocktype)
            {
                case 0:
                    cout<<"4x1";
                    break;
                case 1:
                    cout<<"4x2";
                    break;
                case 2:
                    cout<<"1x1";
                    break;
                case 3:
                    cout<<"2x1";
                    break;
                case 4:
                    cout<<"2x2";
                    break;
                case 5:
                    cout<<"6x1";
                    break;
                case 6:
                    cout<<"8x1";
                    break;
                case 7:
                    cout<<"3x2";
                    break;
                case 8:
                    cout<<"3x1";
                    break;
            }
            cout << endl;
            break;
        case 1:
            cout << "\tAttack" << endl;
        default:
            break;
    }
};
int main (int argc, char ** argv)
{
    if (argc<4)
    {
        cout<<"COWARD"<<endl;
        return 1;
    }
    SpellHasher h (argv[2]);
    ifstream f (argv[1]);
    SpellDict d (f);
    for (int i = 3;i < argc;i++)
    {
        if (d.Contains(argv[i]))
        {
            cout<<argv[i]<<" : "<<endl;
            parsehash(h.Hash(argv[i]));
        }
        else
        {
            cout<<argv[i]<<" : not in dictionary"<<endl;
        }
    }
}
