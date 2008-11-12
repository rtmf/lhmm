#ifndef __BULLET_H__
#define __BULLET_H__
#include "block.h"
class Bullet
{
    public:
        Bullet (double X, double Y, double Z, double XV, double YV, double ZV, double ZA);
        ~Bullet () {};
        void Render ();
        bool Progress (); // returns false if <= 0
        bool Collide (LegoBlock & Block); // returns true on collision
        double getX() {return x;};
        double getY() {return y;};
        double getZ() {return z;};
        double getXV() {return xv;};
        double getYV() {return yv;};
        double getZV() {return zv;};
        double getZA() {return za;};
        unsigned long int id;
    private:
        double x,y,z,xv,yv,zv,za, rotation;
};
#endif
