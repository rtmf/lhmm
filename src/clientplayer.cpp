#include "clientplayer.h"
using namespace std;

void ClientPlayer::cast(const char * spell)
{
    // tell the server to cast
    char castHolder[256];
    snprintf(castHolder,256,"c%s|",spell);
    castHolder[255]='\0';
    tellServer(castHolder);
}
void ClientPlayer::parseServerString(const char * stuff)
{
    switch(stuff[0])
    {
        case 'm':
            //moveto x,y,z,ang
            double x,y,z;
            double a;
            int h;
            double ts;
            int ms;
            sscanf(stuff,"m%lf,%lf,%lf,%lf,%d,%lf,%d",&x,&y,&z,&a,&h,&ts,&ms);
            Player::moveAbs(x,y,z);
            Player::turnAbs(a);
            Player::setColor(h);
            Player::setMoveSpeed(ms);
            Player::setTurnSpeed(ts);
            break;
        case 'p':
            {
                unsigned long int thisid;
                sscanf(stuff,"p%ld",&thisid);
                //add a player to the vplayers list
                Player * p = new Player
                    (blocks,
                     0,0,0,
                     0,0.0,
                     0,0,
                     10,10,
                     NULL
                    );
                p->id=thisid;
                vplayers.push_back(p);
            }
            break;
        case 'o':
            {
                unsigned long int thisid;
                int sx,sy,sz;
                double sang;
                int shue;
                double ts;
                int ms;
                sscanf(stuff,"o%d,%d,%d,%lf,%d,%lf,%d,%ld",
                        &sx,&sy,&sz,&sang,&shue,&ts,&ms,&thisid);
                for(list<Player *>::iterator i=vplayers.begin();
                        i!=vplayers.end();++i)
                {
                    if ((*i)->id==thisid)
                    {
                        puts("moved.");
                        (*i)->moveAbs(sx,sy,sz);
                        (*i)->turnAbs(sang);
                        (*i)->setColor(shue);
                        (*i)->setTurnSpeed(ts);
                        (*i)->setMoveSpeed(ms);
                        break;
                    }
                }
            }
            break;
        case 'P':
            unsigned long int thisid;
            sscanf(stuff,"P%ld",&thisid);
            for(list<Player *>::iterator i=vplayers.begin();
                    i!=vplayers.end();++i)
            {
                if ((*i)->id==thisid)
                {
                    vplayers.remove(*i);
                    delete (*i);
                    break;
                }
            }
            break;
        case 'n':
            {
                unsigned long int thisid;
                int shue,styp,srot;
                sscanf(stuff,"n%d,%d,%d,%ld",&shue,&styp,&srot,&thisid);
                blocks->add(new LegoBlock(0,0,
                           shue,100,
                           styp,srot),true,thisid);
            }
            break;
        case 'l':
             {
                unsigned long int thisid;
                int sx,sy,sz,sd;
                sscanf(stuff,"l%d,%d,%d,%d,%ld",&sx,&sy,&sz,&sd,&thisid);
                blocks->moveByID(sx,sy,sz,sd,thisid);
            }
            break;
        case 'L':
            {
                unsigned long int thisid;
                sscanf(stuff,"L%ld",&thisid);
                blocks->delByID(thisid);
            }
            break;
        case 'b':
            {
                unsigned long int thisid;
                double sx,sy,sz,sxv,syv,szv,sza;
                sscanf(stuff,"b%lf,%lf,%lf,%lf,%lf,%lf,%lf,%ld",
                        &sx,&sy,&sz,
                        &sxv,&syv,&szv,
                        &sza,&thisid);
                Bullet * BB = new Bullet(sx,sy,sz,sxv,syv,szv,sza);
                BB->id=thisid;
                getBulletList()->push_back(BB);
            }
            break;
        case'B':
            {
                unsigned long int thisid;
                sscanf(stuff,"B%ld",&thisid);
                for (list<Bullet *>::iterator i=getBulletList()->begin();
                        i!=getBulletList()->end();++i)
                {
                    if ((*i)->id==thisid)
                    {
                        list<Bullet *>::iterator j=i;
                        j--;
                        delete (*i);
                        getBulletList()->remove(*i);
                        i=j;
                    }
                }
            }
            break;
    }
}
void ClientPlayer::tellServer(const char * stuff)
{
    puts(stuff);
    if (sock!=NULL)
    {
        SDLNet_TCP_Send(sock,(void *)stuff,strlen(stuff));
    }
}
void ClientPlayer::recvData(const char * data)
{
    //try to stuff $data into buffer
    //buffer overflow prevention:
    if (bufpos>=512)
    {
        bufpos=0; // yeah yeah but nothing should be > 256
        buffer[0]='\0';
    }
    strncpy(&(buffer[bufpos]),data,512);
    char * i;
    while ((i=strchr(buffer,'|'))!=NULL)
    {
        //find index
        int idx=(i-buffer);
        buffer[idx]='\0';
        parseServerString(buffer);
        memmove(buffer,&(i[1]),1024-idx);
    }
    bufpos=strlen(buffer);
}
void ClientPlayer::tryRecv()
{
    if (sock!=NULL)
    {
        SDLNet_SocketSet set;
        set=SDLNet_AllocSocketSet(1);
        SDLNet_TCP_AddSocket(set,sock);
        if (SDLNet_CheckSockets(set,0))
        {
            char temBuffer[256];
            int ret=SDLNet_TCP_Recv(sock,temBuffer,255);
            if (ret<=0)
            {
                SDLNet_TCP_Close(sock);
                sock=NULL;
            }
            else
            {
                temBuffer[ret]='\0';
                recvData(temBuffer);
            }
        }
    }
}
