#ifndef __CLIENTPLAYER_H__
#define __CLIENTPLAYER_H__
#include "block.h"
#include "blockfield.h"
#include "mage.h"
#include "bullet.h"
#include "player.h"
#include <SDL_net.h>
#include <list>
class ClientPlayer : public Player
{
    public:
        ClientPlayer(BlockField * Blocks, int x, int y, int z, int hue, double ang,
                int vx, int vy, int vw, int vh, std::list<Bullet *> * B = NULL)
            :Player(Blocks,x,y,z,hue,ang,vx,vy,vw,vh,B)
            ,sock(NULL),bufpos(0)
        {memset(buffer,0,1024);};
        virtual ~ClientPlayer() {};
        virtual void cast(const char * spell);
        virtual void jumpKey() {tellServer("j|");setKey(KEY_JUMP);};
        virtual void turnKey(int direction) {tellServer(direction>0?"tl|":"tr|");setKey(direction>0?KEY_TURN_LEFT:KEY_TURN_RIGHT);};
        virtual void walkKey(int direction) {tellServer(direction>0?"wf|":"wb|");setKey(direction>0?KEY_MOVE_FORWARD:KEY_MOVE_BACK);};
        virtual void walkKeyUp() {tellServer("wu|");clearKey(KEY_MOVE_FORWARD|KEY_MOVE_BACK);};
        virtual void turnKeyUp() {tellServer("tu|");clearKey(KEY_TURN_LEFT|KEY_TURN_RIGHT);};
        virtual void tellServer(const char * stuff);
        void parseServerString(const char * stuff);
        void setSock(TCPsocket Sock) {sock=Sock;};
        TCPsocket getSock() {return sock;};
        void recvData(const char * data);
        void tryRecv();
        std::list<Player *>vplayers;
    private:
        TCPsocket sock;
        char buffer[1024];
        int bufpos;
};
#endif
