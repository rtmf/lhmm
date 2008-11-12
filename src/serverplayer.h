#ifndef __SERVERPLAYER_H__
#define __SERVERPLAYER_H__
#include "block.h"
#include "blockfield.h"
#include "mage.h"
#include "bullet.h"
#include "player.h"
#include <SDL_net.h>
#include <list>
#include <string.h>
class ServerPlayer : public Player
{
    public:
        ServerPlayer(BlockField * Blocks, int x, int y, int z, int hue, double ang,
                int vx, int vy, int vw, int vh, std::list<Bullet *> * B, TCPsocket Sock=NULL)
            : Player(Blocks,x,y,z,hue,ang,vx,vy,vw,vh,B),
            lastX(-1),lastY(-1),lastZ(-1),lastA(100),lastC(-1),sock(Sock),bufpos(0)
        {memset(buffer,0,1024);};
        virtual ~ServerPlayer() {};
        virtual void tick() {Player::tick(); tellClientPosition();};
        virtual void jumpKey() {Player::jumpKey();};
        virtual void turnKey(int direction) {Player::turnKey(direction);};
        virtual void walkKey(int direction) {Player::walkKey(direction);};
        virtual void walkKeyUp() {Player::walkKeyUp();};
        virtual void turnKeyUp() {Player::turnKeyUp();};
        virtual void cast(const char * spell) {Player::cast(spell);};
        void parseClientString(const char * stuff);
        void recvData(const char * data);
        TCPsocket getSock() {return sock;};
        bool getChanged() {return Player::getMage()->getMoved();};
        double getTurnSpeed() {return Player::turnspeed;};
        int getMoveSpeed() {return Player::movespeed;};

        // Outgoing data
    private:
        std :: string outDataBuf;
    public:
        virtual void tellClient(const char * stuff);
        virtual void outFlush();

    private:
        void tellClientPosition();
        double lastX,lastY,lastZ;
        double lastA;
        int lastC;
        TCPsocket sock;
        char buffer[1024];
        int bufpos;
};
#endif
