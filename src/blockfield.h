#ifndef __BLOCKFIELD_H__
#define __BLOCKFIELD_H__
#include <list>
#include "block.h"
#include "bullet.h"
#include "grafix.h"
#include <string>
class BlockField
{
    public:
        BlockField();
        ~BlockField() {};
        void add(LegoBlock * block, bool useid=false, unsigned long int newid=0);
        void update();
        void tick();
        void del(LegoBlock * block);
        void moveByID(int x, int y, int z, int damage,unsigned long int id);
        void delByID(unsigned long int id);
        bool collideBullet(Bullet * b,int damage);
        void RenderTop();
        void RenderShadows();
        void RenderShadowsAside();
        void RenderLines();
        void RenderSidesBottom();
        std::list <LegoBlock*> * getList() {return &V;};
        const char * getChangeData() {return changedata.c_str();};
        void clearChangeData() {changedata="";};
    private:
        char blockMap[110][110][31]; // padded for safety
        std::list <LegoBlock *> colorBlocks[MANARANGE]; // hope this doesn't overrun!
        std::list <LegoBlock *> V;
        void fall(LegoBlock * block);
        std::string changedata;
};
#endif
