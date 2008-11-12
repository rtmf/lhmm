#include <iostream>
#include "block.h"
#include <SDL.h>
#ifdef __MACH__
#  include <OpenGL/gl.h>
#  include <OpenGL/glu.h>
#else
#  include <GL/gl.h>
#  include <GL/glu.h>
#endif

#include <math.h>
#include <stdlib.h>
#include <time.h>
#include "bullet.h"
#include "texture.h"
#include "texture_list.h"
#include "oglconsole.h"
#include "grafix.h"
#include "mage.h"
#include "hash.h"
#include "dict.h"
#include <fstream>
#include <list>
#include "player.h"
#include "clientplayer.h"
#include "serverplayer.h"
#include "blockfield.h"
#include <SDL_net.h>
#define COLORS 32

using namespace std;
list <Bullet *> B;
BlockField blocks;

list<ServerPlayer *> clients;
#define FRAMERATE 30

char * textures[]=
{
    "data/textures/pip.png",
    "we_have_no_texture_for_this_yet",
};

TextureManager * TexMgr;

int perframe=0;

ClientPlayer * player;

SpellHasher hash("MySaltIsSalty");
ifstream dfile("/usr/share/dict/words"); // TODO - configuralize
SpellDict dict(dfile);

bool texture=true;
bool lines=true;
bool solid=true;
bool shadow=true;

void conCB(OGLCONSOLE_Console console, char *cmd)
{
    if (!strncmp("cast", cmd, 4))
    {
        if (!cmd[4])
        {
            OGLCONSOLE_Output(console, "usage: cast spell\n");
            OGLCONSOLE_SetInputLine("cast ");
        }
        if (dict.Contains(cmd+5))
        {
            OGLCONSOLE_Output(console,"Casting %s.\n",cmd+5);
            player->cast(cmd+5);
        }
        else
        {
            OGLCONSOLE_Output(console, "SPELL MUST BE IN DICTIONARY!\n");
            OGLCONSOLE_SetInputLine("cast ");
        }
        OGLCONSOLE_SetVisibility(0); // TODO non-temporary solution
        return;
    }

    // Unrecognized command
    OGLCONSOLE_Output(console, "Unrecognized command\n");
}


void tick()
{
    static int n=0;
    n++;
    if (n>3)
    {
        n=0;
        blocks.tick();
    }
    const char * s=blocks.getChangeData();
    if (strlen(s))
        for (list<ServerPlayer *>::iterator i=clients.begin();
                i!=clients.end();i++)
        {
            (*i)->tellClient(s);
        }
    blocks.clearChangeData();
}

void BulletProgress(bool handle)
{
    for (list<Bullet *>::iterator i
            =B.begin();i!=B.end();++i)
    {
        bool doKeep=(*i)->Progress();
        if (handle)
        {
            //check the array
            if (doKeep)
                doKeep=!(blocks.collideBullet(*i,random()%100));
            if (!doKeep)
            {
                list<Bullet *>::iterator j = i;
                j--;
                char senStuff[256];
                snprintf(senStuff,256,"B%ld|",(*i)->id);
                senStuff[255]='\0';
                //tell all clients i vanished
                for (list<ServerPlayer *>::iterator k=clients.begin();
                        k!=clients.end();k++)
                {
                     (*k)->tellClient(senStuff);
                }
                delete (*i);
                B.remove(*i);
                i = j;
            }
        }
    }
}

void Render()
{
    // -- BEGIN TEXTURE SECTION --
    {
        if (texture)
        {
            glEnable(GL_TEXTURE_2D);
            TexMgr->Bind(TEX_PIP);
        }

        if (solid)
        {
            glBegin(GL_QUADS);

            glColor3d(0.75,0.75,0.75);

            glTexCoord2d(0,0);
            glVertex3d(0,0,0);
            glTexCoord2d(100,0);
            glVertex3d(100,0,0);
            glTexCoord2d(100,-100);
            glVertex3d(100,100,0);
            glTexCoord2d(0,-100);
            glVertex3d(0,100,0);

            blocks.RenderTop();
            
            glEnd();
            TexMgr->Bind(TEX_INVALID_TEX);
            glBegin(GL_QUADS);
            {
                glColor3d(1,1,1);

                glTexCoord2d(0,0);
                glVertex3d(0,0,0);
                glTexCoord2d(0,-30);
                glVertex3d(0,0,30);
                glTexCoord2d(100,-30);
                glVertex3d(100,0,30);
                glTexCoord2d(100,0);
                glVertex3d(100,0,0);

                glTexCoord2d(0,0);
                glVertex3d(0,100,0);
                glTexCoord2d(100,0);
                glVertex3d(100,100,0);
                glTexCoord2d(100,-30);
                glVertex3d(100,100,30);
                glTexCoord2d(0,-30);
                glVertex3d(0,100,30);

                glTexCoord2d(0,0);
                glVertex3d(0,0,0);
                glTexCoord2d(100,0);
                glVertex3d(0,100,0);
                glTexCoord2d(100,-30);
                glVertex3d(0,100,30);
                glTexCoord2d(0,-30);
                glVertex3d(0,0,30);

                glTexCoord2d(0,0);
                glVertex3d(100,0,0);
                glTexCoord2d(0,-30);
                glVertex3d(100,0,30);
                glTexCoord2d(100,-30);
                glVertex3d(100,100,30);
                glTexCoord2d(100,0);
                glVertex3d(100,100,0);


            }
            glEnd();
            TexMgr->Bind(TEX_PIP);
        }

        if (shadow)
        {
            glEnable(GL_POLYGON_OFFSET_FILL);
            glPolygonOffset(-1,-1);
            glBegin(GL_QUADS);

            player->RenderShadow();
            for (list<Player *>::iterator i=player->vplayers.begin();
                    i!=player->vplayers.end();++i)
            {
                (*i)->RenderShadow();
            }

            blocks.RenderShadows();

            glEnd();
            glDisable(GL_POLYGON_OFFSET_FILL);
            glPolygonOffset(0,0);
        }
        if (texture) glDisable(GL_TEXTURE_2D);
    }
    // - END TEXTURE SECTION -
    
    if (shadow)
    {
        glEnable(GL_POLYGON_OFFSET_FILL);
        glPolygonOffset(-1,-1);
        glBegin(GL_QUADS);
        blocks.RenderShadowsAside();
        glEnd();
        glDisable(GL_POLYGON_OFFSET_FILL);
        glPolygonOffset(0,0);
    }

    // - SIMPLE LINES, NO TEX
    if (lines)
    {
        glLineWidth(3.5);
        glBegin(GL_LINES);
        blocks.RenderLines();
        glEnd();
    }

    //WARNING - test stuff
    glLineWidth(3.5);
    glBegin(GL_LINES);
    if (player->getMage()->getBlockOn())
        player->getMage()->getBlockOn()->RenderLines();
    if (player->getMage()->getBlockAside())
        player->getMage()->getBlockAside()->RenderLines();
    glEnd();


    // - SIMPLE QUADS, SOLID, NO TEX
    if (solid)
    {
        glBegin(GL_QUADS);
        player->Render();
        for (list<Player *>::iterator i=player->vplayers.begin();
                i!=player->vplayers.end();++i)
        {
            (*i)->Render();
        }

        blocks.RenderSidesBottom();
        glEnd();
    }
    
    for (list<Bullet *>::iterator i = B.begin();i!=B.end();++i)
    {
        (*i)->Render();
    }
}

int clientMain(int argc, char * argv[])
{
    srandom(time(NULL));
    puts("Initializing SDL");
    if ( SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK | SDL_INIT_AUDIO ) < 0)
    {
        puts("SDL_Init Failure");
        exit(1);
    }
    puts("Initializing the display");
    const SDL_VideoInfo *Info = SDL_GetVideoInfo();


    if (SDL_SetVideoMode
        (WIDTH, HEIGHT, COLORS, 
         SDL_OPENGL | SDL_HWPALETTE 
         | (Info -> hw_available ? SDL_HWSURFACE :0)
         | (Info -> blit_hw ? SDL_HWACCEL :0)) == NULL)
    {
        // We could not set this video mode
        puts("Could not initialize display");
        SDL_Quit();
        return 0;
    }

    SDL_WM_SetCaption("\"LegoHashMageMan\"", "\"LegoHashMageMan\"");

    SDL_ShowCursor(SDL_DISABLE);
    // Initialize OpenGL
    puts("Initializing OpenGL");
    SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );
    SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, COLORS); // arbitrary?
    SDL_GL_SetAttribute( SDL_GL_STENCIL_SIZE, 0);
    SDL_GL_SetAttribute( SDL_GL_ACCUM_RED_SIZE, 0);
    SDL_GL_SetAttribute( SDL_GL_ACCUM_GREEN_SIZE, 0);
    SDL_GL_SetAttribute( SDL_GL_ACCUM_BLUE_SIZE, 0);
    SDL_GL_SetAttribute( SDL_GL_ACCUM_ALPHA_SIZE, 0);

    // Set up our window
    glViewport(0, 0, WIDTH, HEIGHT);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, GLfloat(WIDTH) / GLfloat(HEIGHT), 0.5, 500.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glEnable(GL_DEPTH_TEST);
    glDisable(GL_LINE_SMOOTH);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    GLint T0     = 0;
    GLint Frames = 0;
    GLint FrameTime = 0;

    player=new ClientPlayer(&blocks,
            0,0,0,
            0,0,
            0,0,
            WIDTH,HEIGHT,
            &B);
    SDLNet_Init();
    IPaddress ip;
    SDLNet_ResolveHost(&ip,argv[1],atoi(argv[2]));
    TCPsocket sock=SDLNet_TCP_Open(&ip);
    player->setSock(sock);

    bool Finish=false;
    SDL_EnableKeyRepeat(250,30);
    TexMgr = new TextureManager(TEX_COUNT,textures);
    OGLCONSOLE_Create();
    OGLCONSOLE_EnterKey(conCB);

    OGLCONSOLE_Print("CONTROLS\n");
    OGLCONSOLE_Print("w/s - move forward, backward\n");
    OGLCONSOLE_Print("a/d - turn left, right\n");
    OGLCONSOLE_Print("r/f - move camera up, down\n");
    OGLCONSOLE_Print("t/g - move camera in, out\n");
    OGLCONSOLE_Print("j - jump up up to 2 blocks\n");
    OGLCONSOLE_Print("h - hop forward 1, up infinite\n");
    OGLCONSOLE_Print("q - quit\n");
    OGLCONSOLE_Print("1234 - toggle various parameters\n");
    OGLCONSOLE_Print("THERE MAY BE OTHER KEYS I FORGOT\n");
    int tOld=SDL_GetTicks();
    while (! Finish)
    {
        while ((SDL_GetTicks()-tOld)>5)
        {
            player->tick();
            for (list<Player *>::iterator i=player->vplayers.begin();
                    i!=player->vplayers.end();++i)
            {
                (*i)->tick();
            }
            tOld+=5;
        }
        if (player->getSock()==NULL)
            break;
        else
            player->tryRecv();
        int TS=SDL_GetTicks();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        player->beginView();
        Render();
        player->endView();

        OGLCONSOLE_Draw();
        glEnable(GL_DEPTH_TEST);

        SDL_GL_SwapBuffers();

        BulletProgress(false);

        Frames++;
        {
            GLint t = SDL_GetTicks();
            FrameTime+=(t-TS);
            if (t - T0 >= 5000) {
                GLfloat seconds = (t - T0) / 1000.0;
                GLfloat fps = Frames / seconds;
                GLfloat rps = Frames / (FrameTime / 1000.0);
                printf("%d blocks, %d frames in %g seconds = %g FPS, rendertime = %g FPS\n", (int)blocks.getList()->size(), Frames, seconds, fps, rps);
                T0 = t;
                Frames = 0;
                FrameTime = 0;
            }
        }


        bool notNextFrame=true;
        while(notNextFrame)
        {
            SDL_Event event;
            while (SDL_PollEvent(&event))if(!OGLCONSOLE_SDLEvent(&event))
            {
                switch(event.type)
                {
                    case SDL_QUIT:
                        Finish=true;
                        break;
                    case SDL_KEYDOWN:
                        switch(event.key.keysym.sym)
                        {
                            case SDLK_1:
                                texture=!texture;
                                break;
                            case SDLK_2:
                                lines=!lines;
                                break;
                            case SDLK_3:
                                solid=!solid;
                                break;
                            case SDLK_4:
                                shadow=!shadow;
                                break;
                            case SDLK_5:
                                SDL_EnableKeyRepeat(250,30);
                                break;
                            case SDLK_6:
                                SDL_EnableKeyRepeat(0,0);
                                break;
                            case SDLK_q:
                                Finish=true;
                                break;
                            case SDLK_w:
                                player->walkKey(1);
                                break;
                            case SDLK_s:
                                player->walkKey(-1);
                                break;
                            case SDLK_a:
                                player->turnKey(1);
                                break;
                            case SDLK_d:
                                player->turnKey(-1);
                                break;
                            case SDLK_j:
                                player->jumpKey();
                                break;
                            case SDLK_SPACE:
                                OGLCONSOLE_SetVisibility(1);
                                OGLCONSOLE_SetInputLine("cast ");
                                break;
                            case SDLK_r:
                                player->adjustFollowHeight(0.5);
                                break;
                            case SDLK_f:
                                player->adjustFollowHeight(-0.5);
                                break;
                            case SDLK_t:
                                player->adjustFollowDistance(-0.5);
                                break;
                            case SDLK_g:
                                player->adjustFollowDistance(0.5);
                                break;
                            default:
                                cout<<"unhandled key or missing break."<<endl;
                                break;
                        }
                        break;
                    case SDL_KEYUP:
                        switch(event.key.keysym.sym)
                        {
                            case SDLK_w:
                            case SDLK_s:
                                player->walkKeyUp();
                                break;
                            case SDLK_a:
                            case SDLK_d:
                                player->turnKeyUp();
                                break;
                            default:
                                break;
                        }
                        break;
                    default:
                        break;
                }
            }

            if ((SDL_GetTicks()-TS)>=(1000/FRAMERATE))
            {
                notNextFrame=false;
            }
            else
            {
                SDL_Delay(10);
            }
        }

    }
    OGLCONSOLE_Quit();
    SDLNet_Quit();
    SDL_Quit();
    return 0;
}

int serverMain(int argc, char * argv[])
{
    puts("Initializing SDL");
    if ( SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        puts("SDL_Init Failure");
        exit(1);
    }

/*  SDL_WM_SetCaption("\"LHMMServer\"", "\"LHMMServer\"");*/

    SDLNet_Init();
    //clients will start out empty.
    TCPsocket listener;
    IPaddress ip;
    printf("ResolveHost returned %d.\n",SDLNet_ResolveHost(&ip,NULL,atoi(argv[1])));
    listener=SDLNet_TCP_Open(&ip);
    if (listener==NULL)
        puts("Socket Open Failed.");
    bool done=false;
    int tOld=SDL_GetTicks();
    int tOld2=tOld;
    while(!done)
    {
        if ((SDL_GetTicks()-tOld2)>(1000/30))
        {
            tOld2=SDL_GetTicks();
            BulletProgress(true);
        }
        while ((SDL_GetTicks()-tOld)>5)
        {
            tOld+=5;
            tick();
            for ( list<ServerPlayer *>::iterator i=clients.begin();i!=clients.end();++i)
            {
                (*i)->tick();
                if ((*i)->getChanged())
                {
                    char senStuff[256];
                    int sx,sy,sz;
                    double sa;
                    int sc;
                    unsigned long int newid=(*i)->id;
                    sx=(*i)->getMage()->getX();
                    sy=(*i)->getMage()->getY();
                    sz=(*i)->getMage()->getZ();
                    sa=(*i)->getMage()->getA();
                    sc=(*i)->getMage()->getC();
                    double ts;
                    int ms;
                    ts=(*i)->getTurnSpeed();
                    ms=(*i)->getMoveSpeed();
                    
                    snprintf(senStuff,256,"o%d,%d,%d,%lf,%d,%lf,%d,%ld|",
                            sx,sy,sz,sa,sc,ts,ms,newid);
                    senStuff[255]='\0';
                    for ( list <ServerPlayer *>::iterator j=clients.begin();
                            j!=clients.end();++j)
                    {
                        if ((*j)->id != (*i)->id)
                            (*j)->tellClient(senStuff);
                    }
                }
                (*i)->getMage()->clearMoved();
            }
        }
        SDL_Event event;
        while (SDL_PollEvent(&event))//if(!OGLCONSOLE_SDLEvent(&event))
        {
            // Several signals yield this SDL event
            if (event.type == SDL_QUIT)
            {
                puts("Quitting");
                done = true;
            }
        }
        list <Bullet*> :: iterator curbullen = B.end();
        curbullen--;
        SDLNet_SocketSet set;
        set=SDLNet_AllocSocketSet(1+clients.size());
        SDLNet_TCP_AddSocket(set,listener);
        for ( list<ServerPlayer *>::iterator i=clients.begin();i!=clients.end();++i)
        {
            SDLNet_TCP_AddSocket(set,(*i)->getSock());
        }
        SDLNet_CheckSockets(set,10);
        for ( list<ServerPlayer *>::iterator i=clients.begin();i!=clients.end();++i)
        {
            if (SDLNet_SocketReady((*i)->getSock()))
            {
                char temBuffer[256];
                int ret=SDLNet_TCP_Recv((*i)->getSock(),temBuffer,255);
                if (ret<=0)
                {
                    //error / closed
                    SDLNet_TCP_Close((*i)->getSock());
                    list<ServerPlayer *>::iterator j=i;
                    j--;
                    unsigned long int delid=(*i)->id;
                    char senStuff[256];
                    delete (*i);
                    clients.remove(*i);
                    i=j;
                    temBuffer[0]='\0';
                    puts("Killed Player.");
                    snprintf(senStuff,256,"P%ld|",delid);
                    senStuff[255]='\0';
                    for (list <ServerPlayer *>::iterator j=clients.begin();j!=clients.end();++j)
                        (*j)->tellClient(senStuff);
                }
                else
                {
                    temBuffer[ret]='\0';
                    (*i)->recvData(temBuffer);
                }
            }
        }
        curbullen++;
        for (; curbullen != B.end(); curbullen++)
        {
            //notify all connected players about this
            char senStuff[256];
            Bullet * BB = *curbullen;
            snprintf(senStuff,256,"b%lf,%lf,%lf,%lf,%lf,%lf,%lf,%ld|",
                    (BB)->getX(),(BB)->getY(),(BB)->getZ(),
                    (BB)->getXV(),(BB)->getYV(),(BB)->getZV(),
                    (BB)->getZA(),(BB)->id);
            senStuff[255]='\0';
            for (list<ServerPlayer *>::iterator j=clients.begin();
                    j!=clients.end();++j)
            {
                (*j)->tellClient(senStuff);
            }
        }
        if (SDLNet_SocketReady(listener)) // do this last
        {
            TCPsocket sock;
            puts("New Player.");
            //tell other players about it
            char senStuff[256];
            unsigned long int newid=0;
            bool sdone=false;
            while(!sdone)
            {
                sdone=true;
                for (list<ServerPlayer*>::iterator i=clients.begin();i!=clients.end();++i)
                    if ((*i)->id==newid)
                    {
                        sdone=false;
                        newid++;
                    }
            }
            snprintf(senStuff,256,"p%ld|",newid);
            senStuff[255]='\0';
            for ( list<ServerPlayer *>::iterator i=clients.begin();i!=clients.end();++i)
            {
                (*i)->tellClient(senStuff);
            }
            sock=SDLNet_TCP_Accept(listener);
            ServerPlayer * p = new ServerPlayer(
                        &blocks,
                        random()%100,random()%100,5,
                        random()%MANARANGE,random()%4*(M_PI/2),
                        0,0,WIDTH,HEIGHT,
                        &B,sock);
            p->id=newid;
            clients.push_back(p);
            int sx,sy,sz;
            double sa;
            int sc;
            sx=p->getMage()->getX();
            sy=p->getMage()->getY();
            sz=p->getMage()->getZ();
            sa=p->getMage()->getA();
            sc=p->getMage()->getC();
            double ts;
            int ms;
            ts=p->getTurnSpeed();
            ms=p->getMoveSpeed();

            snprintf(senStuff,256,"o%d,%d,%d,%lf,%d,%lf,%d,%ld|",
                    sx,sy,sz,sa,sc,ts,ms,newid);
            senStuff[255]='\0';
            //tell all clients the position of the new player
            for ( list<ServerPlayer *>::iterator i=clients.begin();i!=clients.end();++i)
            {
                if ((*i)->id != newid)
                    (*i)->tellClient(senStuff);
            }
            for ( list <ServerPlayer *>::iterator i=clients.begin();i!=clients.end();++i)
            {
                if ((*i)->id != newid)
                {
                    snprintf(senStuff,256,"p%ld|",(*i)->id);
                    senStuff[255]='\0';
                    p->tellClient(senStuff);
                    sx=(*i)->getMage()->getX();
                    sy=(*i)->getMage()->getY();
                    sz=(*i)->getMage()->getZ();
                    sa=(*i)->getMage()->getA();
                    sc=(*i)->getMage()->getC();
                    double ts;
                    int ms;
                    ts=(*i)->getTurnSpeed();
                    ms=(*i)->getMoveSpeed();
                    
                    snprintf(senStuff,256,"o%d,%d,%d,%lf,%d,%lf,%d,%ld|",
                            sx,sy,sz,sa,sc,ts,ms,newid);
                    senStuff[255]='\0';
                    p->tellClient(senStuff);
                }
            }
            for (list<LegoBlock *>::iterator i=blocks.getList()->begin();
                    i!=blocks.getList()->end();++i)
            {
                char senStuff[256];
                snprintf(senStuff,256,"n%d,%d,%d,%ld|",(*i)->getColor(),(*i)->getType(),
                        (*i)->getRotation(),(*i)->id);
                senStuff[255]='\0';
                p->tellClient(senStuff);
                snprintf(senStuff,256,"l%d,%d,%d,%d,%ld|",(*i)->getX(),(*i)->getY(),
                        (*i)->getZ(),(*i)->getDamage(),(*i)->id);
                senStuff[255]='\0';
                p->tellClient(senStuff);
            }
            for (list<Bullet *>::iterator i=B.begin(); i != B.end(); i++)
            {
                //tell p about all bullets
                char senStuff[256];
                Bullet * BB = *i;
                snprintf(senStuff,256,"b%lf,%lf,%lf,%lf,%lf,%lf,%lf,%ld|",
                        (BB)->getX(),(BB)->getY(),(BB)->getZ(),
                        (BB)->getXV(),(BB)->getYV(),(BB)->getZV(),
                        (BB)->getZA(),(BB)->id);
                senStuff[255]='\0';
                p->tellClient(senStuff);
            }
        }
        SDLNet_FreeSocketSet(set);

        // flush all outgoing network data
        for ( list<ServerPlayer *>::iterator i=clients.begin();i!=clients.end();++i)
            (*i)->outFlush();
    }

    SDLNet_Quit();
    SDL_Quit();
    return 0;
}

int main(int argc, char * argv[])
{
    if (argc==2)
        return serverMain(argc,argv);
    if (argc==3)
        return clientMain(argc,argv);
    puts("either lhmm host port, or lhmm port (for server).  all else is dead.");
    return 1;
}
