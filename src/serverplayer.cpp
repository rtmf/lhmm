#include "serverplayer.h"
void ServerPlayer::recvData(const char * data)
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
        parseClientString(buffer);
        memmove(buffer,&(i[1]),1024-idx);
    }
    bufpos=strlen(buffer);
}
void ServerPlayer::tellClientPosition()
{
    if (Player::getMage()->getMoved())
    {
        char toClient[256];
        double x, y, z;
        Player::getMage()->getFPos(&x, &y, &z);
        double a = Player::getMage()->getA();
        int h = Player::getMage()->getC();

        snprintf(toClient,256,"m%lf,%lf,%lf,%lf,%d,%lf,%d|",x,y,z,a,h,turnspeed,movespeed);
        toClient[255]='\0';
        tellClient(toClient);
        lastX=x;
        lastY=y;
        lastZ=z;
        lastA=a;
    }
}

void ServerPlayer::tellClient(const char * stuff)
{
    puts(stuff);
    if (sock != NULL)
        outDataBuf += stuff;
}

void ServerPlayer::outFlush()
{
    if (outDataBuf.size())
    {
        puts("Flush");
        SDLNet_TCP_Send(
                sock,
                (void *) outDataBuf.c_str(),
                outDataBuf.size());
        outDataBuf.erase();
    }
    //else printf("Nothing to flush for player %p\n", this);
}

void ServerPlayer::parseClientString(const char * stuff)
{
    //t is a turn key (rlu)
    //w is a walk key (fbu)
    //j is a jump key
    //h is a hop key
    //c is a cast
    switch(stuff[0])
    {
        case 't':
            switch(stuff[1])
            {
                case 'l':
                    Player::turnKey(1);
                    break;
                case 'r':
                    Player::turnKey(-1);
                    break;
                case 'u':
                    Player::turnKeyUp();
                    break;
            }
            break;
        case 'w':
            switch(stuff[1])
            {
                case 'f':
                    Player::walkKey(1);
                    break;
                case 'b':
                    Player::walkKey(-1);
                    break;
                case 'u':
                    Player::walkKeyUp();
                    break;
            }
            break;
        case 'j':
            Player::jumpKey();
            break;
        case 'c':
            Player::cast(&(stuff[1]));
            break;
    }
}
