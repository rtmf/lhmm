#ifndef __PLAYER_H__
#define __PLAYER_H__
#include "block.h"
#include "blockfield.h"
#include "mage.h"
#include "bullet.h"
#define KEY_TURN_LEFT 1
#define KEY_TURN_RIGHT 2
#define KEY_MOVE_FORWARD 4
#define KEY_MOVE_BACK 8
#define KEY_JUMP 16
class Player
{
    public:
        Player(BlockField * Blocks, int x, int y, int z, int hue, double ang,
                int vx, int vy, int vw, int vh, std::list<Bullet *> * BB = NULL);
        virtual ~Player() {delete mage;};
        virtual void tick();
        virtual void cast(const char * spell);
        virtual void jumpKey();
        virtual void turnKey(int direction);
        virtual void walkKey(int direction);
        virtual void walkKeyUp();
        virtual void turnKeyUp() {turnspeed=0;mage->setMoved();};
        virtual void setTurnSpeed(double Turnspeed) {turnspeed=Turnspeed;};
        virtual void setMoveSpeed(int Movespeed) {movespeed=Movespeed;};
        
        virtual void moveAbs(double x, double y, double z) {mage->moveabs(x,y,z);};
        virtual void turnAbs(double ang) {mage->turnabs(ang);};
        virtual void setColor(int hue) {mage->setColor(hue);};
        virtual void RenderShadow() {mage->RenderShadow();};
        virtual void Render() {mage->Render();};
        virtual void beginView() {mage->beginView();};
        virtual void endView() {mage->endView();};
        virtual void adjustFollowHeight(double adj) {mage->movefh(adj);};
        virtual void adjustFollowDistance(double adj) {mage->movefd(adj);};
        void setKey(int key) {keymask|=key;};
        void clearKey(int key) {keymask&=(~key);};
        int getKeys() {return keymask;};
        Mage * getMage(){return mage;};
        std::list<Bullet *> * getBulletList() {return B;};
        unsigned long int id;
    protected:
        BlockField * blocks;
        int movespeed;
        double turnspeed;
    private:
        int keymask;
        bool jumping;
        int jumpcount;
        bool falling;
        bool walkTap;
        bool walkTapReady;
        int walkSkip;
        Mage * mage;
        std::list<Bullet *> * B;
};
#endif
