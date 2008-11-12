#include "player.h"
#include "hash.h"
#include <math.h>

using namespace std;

void Player::cast(const char * spell)
{
    if (!strcmp(spell,"fnord"))
    {
        int i;
        for (i=0;i<100;i++)
        {
		long sh;
                sh=random();
                int hue=sh%MANARANGE;
                sh/=MANARANGE;
                int typ=sh%9;
                sh/=9;
                int rot=sh%2;
                sh/=2;
				int strength=sh%101;
				sh/=101;
                int x=sh%100;
                sh/=100;
                int y=sh%100;
                sh/=100;
                blocks->add(new LegoBlock(x,
                            y,hue,strength,
                            typ,rot));
                blocks->update();
        }
        return;
    }
    //I must be a server so I have to hash the spell.
    SpellHasher hash("MySaltIsSalty");
    unsigned long int sh=hash.Hash(spell);
    // determine spell type
    int st=sh%2;
    //0 is summon block
    //1 is ATTAK
    sh/=2;
    switch(st)
    {
        case 0:
            {
                int hue=sh%MANARANGE;
                sh/=MANARANGE;
                int typ=sh%9;
                sh/=9;
                int rot=sh%2;
                sh/=2;
				int strength=sh%101;
				sh/=101;
                blocks->add(new LegoBlock(mage->getX(),
                            mage->getY(),hue,strength,
                            typ,rot));
                blocks->update();
                int pc=mage->getC();
                int cdiff=hue-pc;
                if (abs(cdiff)>(MANARANGE/2))
                {
                    if (cdiff<0)
                        cdiff=MANARANGE-abs(cdiff);
                    else
                        cdiff=-(MANARANGE-cdiff);
                }
                int pd=cdiff/4;
                if (pd==0)
                    if (cdiff!=0)
                        pd=(cdiff/abs(cdiff));
                pc+=pd;
                if (pc>MANARANGE)
                    pc-=MANARANGE;
                if (pc<0)
                    pc+=MANARANGE;
                mage->setColor(pc);
            }
            break;
        case 1:
            {
                double zv=double(sh%10)/10.0;
                sh/=10;
                double za=-double(sh%10)/100.0-0.01;
                sh/=10;
                Bullet * BB=new Bullet(
                            mage->getX()+0.5,
                            mage->getY()+0.5,
                            mage->getZ()+0.5,
                            cos(mage->getA())*0.5,
                            sin(mage->getA())*0.5,
                            zv,
                            za);
                unsigned long int newid=0;
                bool done=false;
                while(!done)
                {
                    done=true;
                    for (list<Bullet *>::iterator i=B->begin();
                            i!=B->end();++i)
                    {
                        if ((*i)->id==newid)
                        {
                            newid++;
                            done=false;
                        }
                    }
                }
                BB->id=newid;
                B->push_back(BB);
                //code outside of here will take care of this.
            }
            break;
    }
}

void Player::tick()
{
    if (turnspeed)
    {
        mage->turnRel(turnspeed);
    }
    walkSkip++;
    if (walkSkip>2)
    {
        walkTap=false;
        if (!movespeed) walkTapReady=true;
        falling = mage->walkRel(movespeed,*(blocks->getList()),!jumping);
        walkSkip=0;
        if (jumping)
        {
            if (--jumpcount>=0)
            {
                if (!mage->tryMoveUp(*(blocks->getList())))
                    jumping=false;
            }
            else
                jumping=false;
        }
    }
}
Player::Player(BlockField * Blocks, int x, int y, int z, int hue, double ang,
                int vx, int vy, int vw, int vh, list<Bullet *> * BB)
    :blocks(Blocks),movespeed(0),turnspeed(0),keymask(0),jumping(false),jumpcount(0),
    falling(false),walkTap(false),walkTapReady(false),walkSkip(0),B(BB)
{
    mage = new Mage (x,y,z,hue,ang,vx,vy,vw,vh);
}

void Player::walkKeyUp()
{
    if (movespeed && walkTap)
    {
        walkTap=false;
        walkTapReady=false;
        walkSkip=0;
        falling = mage->walkRel(movespeed,*(blocks->getList()),!jumping);
    }
    if (movespeed) movespeed=0;
    mage->setMoved();
}

void Player::walkKey(int direction)
{
    if (!movespeed)
    {
        movespeed=direction;
        walkSkip=0;
        walkTap=walkTapReady;
        mage->setMoved();
    }
}

void Player::turnKey(int direction)
{
    if (!turnspeed)
    {
        if (direction < 0)
            turnspeed=-M_PI/160;
        else
            turnspeed=M_PI/160;
        mage->setMoved();
    }
}

void Player::jumpKey()
{
    if (!jumping && !falling)
    {
        jumping=true;
        jumpcount=2; // up at most 2 spaces
    }
}
