#ifndef __MAGE_H__
#define __MAGE_H__
#include "view.h"
#include "grafix.h"
#include <list>
#include "math.h"
#include "block.h"
class Mage
{
    public:
        Mage(int X, int Y, int Z, int Col, double Ang, int Vx=0, int Vy=0, int Vw=WIDTH, int Vh=HEIGHT );
        int getX() {return x;};
        int getY() {return y;};
        int getZ() {return z;};
        int getC() {return col;};
        int getFX() {return fx;};
        int getFY() {return fy;};
        void setColor(int Col);
        double getHue() {return hue;};
        double getA() {return ang;};
        bool walkRel(int direction,std::list <LegoBlock *> & V,bool doFall);
//      bool walk(int direction, std::list<LegoBlock*> &V, bool doFall);
        void turnRel(double ad); // + is right
        void beginView() {view.Begin();};
        void endView() {view.End();};
        void Render();
        void RenderShadow();
        void movefh(double offset) {fh+=offset;updateView();};
        void movefd(double offset) {fd+=offset;updateView();};
        void setMoved() {moved=true;};
        bool getMoved() {return moved;};
        void clearMoved() {moved=false;};

        void moveabs(double X, double Y, double Z)
            {fpos.x=X; fpos.y=Y; fpos.z=Z; 
             x=int(X); y=int(Y); z=int(Z); updateView(); };

        // two versions of the same function cause i'm a cock
        void getFPos(float *X, float *Y, float *Z)
            {*X=fpos.x; *Y=fpos.y; *Z=fpos.z;};

        void getFPos(double *X, double *Y, double *Z)
            {*X=fpos.x; *Y=fpos.y; *Z=fpos.z;};

        void turnabs(double angg) {ang=angg;updateView();};

        bool tryMoveUp(std::list <LegoBlock *> & V);

        LegoBlock * getBlockOn() {return blockOn;};
        LegoBlock * getBlockAside() {return blockAside;};

        ~Mage() {};
    private:
        struct { double x, y, z; } fpos; // floaty position 
        int x,y,z,col,vx,vy,vw,vh;
            //x, y, z are block-grid-coordinate location of mage
            //col is mage's color in 0-MANARANGE
            //vx, vy are x&y of viewport
            //vw, vh are width, height of viewport
        double fh, fd;
            //fh is camera-follow-height
            //fd is camera-follow-distance
        double hue;
            //precomputed hue (0-1) based on col
        double ang;
            //angle, duh
        double sr,sg,sb;
            //current shadow red, green, blue
        int sz;
            //current shadow zpos
        int fx,fy;
            //current facing direction (computed from ang)
        double mr,mg,mb;
            //current r,g,b of mage itself
        GLView view;
            //the view
        void updateOrtho();
        void updateView();

        double speed;

        bool moved;

        LegoBlock * blockOn;
            //BLOCK ON, DUDE!
        LegoBlock * blockAside;

		int ticker;
};
#endif
