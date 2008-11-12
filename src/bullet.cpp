#include "bullet.h"
#include <GL/glut.h>
#include <GL/gl.h>
#include <stdlib.h>

Bullet::Bullet(double X, double Y, double Z, double XV, double YV, double ZV, double ZA)
    : x(X),y(Y),z(Z),xv(XV),yv(YV),zv(ZV),za(ZA),rotation(0.0)
{
    //not much to do here
}
bool Bullet::Progress()
{
    x+=xv;
    y+=yv;
    z+=zv;
    zv+=za;
    return z>0;
}
bool Bullet::Collide(LegoBlock & Block)
{
    if (int(z)==Block.getZ())
    {
        int bx=Block.getX();
        int by=Block.getY();
        int rot=Block.getRotation();
        int typ=Block.getType();
        int xmax=BrickSizes[typ][rot]+bx;
        int ymax=BrickSizes[typ][1-rot]+by;
        if (int(x) >= bx && int(x) < xmax &&
                int(y) >= by && int(y) < ymax)
        {
            return true;
        }
    }
    return false;
}
void Bullet::Render()
{
    static float LightPos[4]={-5.0f,5.0f,10.0f,0.0f};
    static float Ambient[4]={1.0f,0.5f,0.5f,1.0f};
    glPushAttrib(GL_ALL_ATTRIB_BITS);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glLightfv(GL_LIGHT0, GL_POSITION, LightPos);
    glLightfv(GL_LIGHT0, GL_AMBIENT, Ambient);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glTranslatef(x, y, z);
    glRotated(0,1,0, rotation);
    glScaled(0.3, 0.3, 0.3);
    if ((rotation+=8.0)>=360.0) rotation -= 360.0;

    glutSolidIcosahedron();

    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
    glPopAttrib();
}
