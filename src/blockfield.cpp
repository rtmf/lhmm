#include "blockfield.h"
using namespace std;

BlockField::BlockField()
{
    for (int x=0;x<110;x++)
        for (int y=0;y<110;y++)
            for (int z=0;z<31;z++)
                blockMap[x][y][z]=-1;
    changedata="";
    update();
}

void BlockField::update()
{
    for (list<LegoBlock *>::const_iterator 
            i = V.begin();i!=V.end();++i)
    {
        (*i)->getVector()->clear();
        for (list<LegoBlock *>::const_iterator 
                j = V.begin();j!=V.end();++j)
        {
            if ((*i)->collideAcross(*(*j)))
                (*i)->getVector()->push_back(*j);
        }
    }
}

void BlockField::add(LegoBlock * block, bool useid, unsigned long int newid)
{
    //update the bitmask array with the block
    int width=BrickSizes[block->getType()][block->getRotation()];
    int length=BrickSizes[block->getType()][1-block->getRotation()];
    for (int x=0;x<width;x++)
        for (int y=0;y<length;y++)
            blockMap[x+block->getX()][y+block->getY()][30]=block->getColor();
    colorBlocks[block->getColor()].push_back(block);
    if (useid)
        block->id=newid;
    else
    {
        newid=0;
        bool done=false;
        while(!done)
        {
            done=true;
            for (list <LegoBlock *>::iterator i=V.begin();
                    i!=V.end();++i)
                if ((*i)->id==newid)
                {
                    done=false;
                    newid++;
                    break;
                }
           
        }
        block->id=newid;
        //only really need to update the moving string if useid==false
        //add both a create(n) and move(l)
        char senStuff[256];
        snprintf(senStuff,256,"n%d,%d,%d,%ld|",block->getColor(),block->getType(),
                block->getRotation(),block->id);
        senStuff[255]='\0';
        changedata+=senStuff;
        snprintf(senStuff,256,"l%d,%d,%d,%d,%ld|",block->getX(),block->getY(),
                block->getZ(),block->getDamage(),block->id);
        senStuff[255]='\0';
        changedata+=senStuff;

    }
    V.push_back(block);
}

void BlockField::fall(LegoBlock * block)
{
    //call AFTER its fallen
    int width=BrickSizes[block->getType()][block->getRotation()];
    int length=BrickSizes[block->getType()][1-block->getRotation()];
    for (int x=0;x<width;x++)
        for (int y=0;y<length;y++)
        {
            blockMap[x+block->getX()][y+block->getY()][block->getZ()+1]=-1;
            blockMap[x+block->getX()][y+block->getY()][block->getZ()]=block->getColor();
        }
    char senStuff[256];
    snprintf(senStuff,256,"l%d,%d,%d,%d,%ld|",block->getX(),block->getY(),
            block->getZ(),block->getDamage(),block->id);
    senStuff[255]='\0';
    changedata+=senStuff;

}

void BlockField::del(LegoBlock * block)
{
    char senStuff[256];
    snprintf(senStuff,256,"L%ld|",block->id);
    senStuff[255]='\0';
    changedata+=senStuff;

    //call AFTER its fallen
    int width=BrickSizes[block->getType()][block->getRotation()];
    int length=BrickSizes[block->getType()][1-block->getRotation()];
    for (int x=0;x<width;x++)
        for (int y=0;y<length;y++)
        {
            blockMap[x+block->getX()][y+block->getY()][block->getZ()]=-1;
        }
    colorBlocks[block->getColor()].remove(block);
    V.remove(block);
    delete block;
    update();
}

void BlockField::tick()
{
    for (list<LegoBlock *>::const_iterator
            i = V.begin();
            i != V.end();
            ++i)
    {
        if ((*i)->getZ()>0)
        {
            //try to collide first
            bool doStep=true;
            for (vector<LegoBlock *>::const_iterator 
                    j = (*i)->getVector()->begin();
                    j!= (*i)->getVector()->end(); ++j)
            {
                if ((*i)->getZ()-1==(*j)->getZ())
                {
                    doStep=false;
                    break;
                }
            }
            if (doStep)
            {
                (*i)->stepDown();
                fall(*i);
            }
        }
    }
}

bool BlockField::collideBullet(Bullet * b,int damage)
{
    int x = int(b->getX());
    int y = int(b->getY());
    int z = int(b->getZ());
    if (x<0 || x >100 || y < 0 || y > 100)
        return true;
    if (blockMap[x][y][z]!=-1)
    {
        for (list<LegoBlock *>::iterator j 
                = colorBlocks[int(blockMap[x][y][z])].begin();
                j!=colorBlocks[int(blockMap[x][y][z])].end();++j)
        {
            if (b->Collide(*(*j)))
            {
                if ((*j)->doDamage(random()%100))
                {
                    list<LegoBlock *>::iterator k = j;
                    k--;

                    del(*j);
                    j = k;
                }
                else
                {
                    char senStuff[256];
                    snprintf(senStuff,256,"l%d,%d,%d,%d,%ld|",(*j)->getX(),(*j)->getY(),
                            (*j)->getZ(),(*j)->getDamage(),(*j)->id);
                    senStuff[255]='\0';
                    changedata+=senStuff;
                }
                return true;
            }
        }
    }
    return false;
}

void BlockField::RenderTop()
{
    for (list<LegoBlock *>::const_iterator i = V.begin();i!=V.end();++i)
    {
        (*i)->RenderTop();
    }
}

void BlockField::RenderShadows()
{
    for (list<LegoBlock *>::const_iterator i = V.begin();i!=V.end();++i)
    {
        (*i)->RenderShadow();
        for (vector<LegoBlock *>::const_iterator 
                j = (*i)->getVector()->begin();
                j!= (*i)->getVector()->end(); ++j)
        {
            if ((*i)->getZ()-1>(*j)->getZ())
                (*i)->RenderShadowOnto(*(*j));
        }
    }
}

void BlockField::RenderShadowsAside()
{
    for (list<LegoBlock *>::const_iterator i = V.begin();i!=V.end();++i)
    {
        if ((*i)->getZ()>0)
        {
            for (vector<LegoBlock *>::const_iterator 
                    j = (*i)->getVector()->begin();
                    j!= (*i)->getVector()->end(); ++j)
            {
                if ((*i)->getZ()>(*j)->getZ())
                    (*i)->RenderShadowAside(*(*j));
            }
        }
    }
}

void BlockField::RenderLines()
{
    for (list<LegoBlock *>::const_iterator i = V.begin();i!=V.end();++i)
    {
        (*i)->RenderLines();
    }
}

void BlockField::RenderSidesBottom()
{
    for (list<LegoBlock *>::const_iterator i = V.begin();i!=V.end();++i)
    {
        {
            (*i)->RenderSidesBottom();
        }
    }
}

void BlockField::moveByID(int x, int y, int z, int damage, unsigned long int id)
{
    for (list <LegoBlock *>::iterator i=V.begin();i!=V.end();++i)
    {
        if ((*i)->id==id)
        {
            (*i)->setPosition(x,y);
            (*i)->setZ(z);
            (*i)->setDamage(damage);
            update();
            return;
        }
    }
}

void BlockField::delByID(unsigned long int id)
{
    for (list <LegoBlock *>::iterator i=V.begin();i!=V.end();++i)
    {
        if ((*i)->id==id)
        {
            del(*i);
            return;
        }
    }

}
