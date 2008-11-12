#include <iostream>
#include <GL/gl.h>
#include "block.h"
#include "grafix.h"
char * BrickTypes[]={
    "4x1","4x2","1x1",
    "2x1","2x2","6x1",
    "8x1","3x2","3x1"
};
int BrickSizes[][2]={
    {4,1},{4,2},{1,1},
    {2,1},{2,2},{6,1},
    {8,1},{3,2},{3,1}
};



bool LegoBlock::doDamage(int amount)
{
    if ((damage+=amount) > strength)
    {
        return true;
    }
    return false;
}
void LegoBlock::RenderShadowAside(LegoBlock & other)
{
    //if (z<=other.getZ())
    //    return;
    double x1,y1,x2,y2;
    int ox=other.getX();
    int oy=other.getY();
    int ot=other.getType();
    int orot=other.getRotation();
    int xmax=x+BrickSizes[type][rotation];
    int ymax=y+BrickSizes[type][1-rotation];
    int oxmax=ox+BrickSizes[ot][orot];
    int oymax=oy+BrickSizes[ot][1-orot];
    double zf=(z-other.getZ())/50.0;
    if (zf>0.5) zf=0.5;
    double zf2=(z-other.getZ()+1)/50.0;
    if (zf2>0.5) zf2=0.5;
    
    double sfx=(xmax-x)*zf;
    double zxadj=(xmax-x)*zf2-sfx;
    double sfy=(ymax-y)*zf;
    double zyadj=(ymax-y)*zf2-sfy;
    x1=x+sfx;
    y1=y+sfy;
    x2=xmax-sfx;
    y2=ymax-sfy;
    double df,r,g,b;
    df=(double(other.getDamage())/double(other.getStrength()))*(0.50);

    double ohue=other.getHue();
    r=Hue_2_RGB(0,0.5-df,ohue+0.3333);
    g=Hue_2_RGB(0,0.5-df,ohue);
    b=Hue_2_RGB(0,0.5-df,ohue-0.3333);

    glColor3d(r,g,b);
   
    double lx1=x1;
    double lx2=x2;
    double ly1=y1;
    double ly2=y2;
    if (lx1<ox)
        lx1=ox;
    if (lx2>oxmax)
        lx2=oxmax;
    if (ly1<oy)
        ly1=oy;
    if (ly2>oymax)
        ly2=oymax;
    double lx1a=x1-zxadj;
    double lx2a=x2+zxadj;
    double ly1a=y1-zyadj;
    double ly2a=y2+zyadj;
    if (lx1a<ox)
        lx1a=ox;
    if (lx2a>oxmax)
        lx2a=oxmax;
    if (ly1a<oy)
        ly1a=oy;
    if (ly2a>oymax)
        ly2a=oymax;

    int oz=other.getZ();
    if (x1<ox)
    {
        glVertex3d(ox,ly1a,oz+1);
        glVertex3d(ox,ly2a,oz+1);
        glVertex3d(ox,ly2,oz);
        glVertex3d(ox,ly1,oz);
    }
    //    x1=ox;
    if (x2>oxmax)
    {
        glVertex3d(oxmax,ly1,oz);
        glVertex3d(oxmax,ly2,oz);
        glVertex3d(oxmax,ly2a,oz+1);
        glVertex3d(oxmax,ly1a,oz+1);
    }
    //    x2=oxmax;
    if (y1<oy)
    {
        glVertex3d(lx1,oy,oz);
        glVertex3d(lx2,oy,oz);
        glVertex3d(lx2a,oy,oz+1);
        glVertex3d(lx1a,oy,oz+1);
    }
    //    y1=oy;
    if (y2>oymax)
    {
        glVertex3d(lx1a,oymax,oz+1);
        glVertex3d(lx2a,oymax,oz+1);
        glVertex3d(lx2,oymax,oz);
        glVertex3d(lx1,oymax,oz);
    }
    //    y2=oymax;

}

void LegoBlock::RenderShadowOnto(LegoBlock & other)
{
    //if ((z-1)<=other.getZ())
    //    return;
    //    do this outside
    double x1,y1,x2,y2;
    int ox=other.getX();
    int oy=other.getY();
    int ot=other.getType();
    int orot=other.getRotation();
    int xmax=x+BrickSizes[type][rotation];
    int ymax=y+BrickSizes[type][1-rotation];
    int oxmax=ox+BrickSizes[ot][orot];
    int oymax=oy+BrickSizes[ot][1-orot];
    double zf=(z-other.getZ()-1)/50.0;
    if (zf>0.5) zf=0.5;
    double sfx=(xmax-x)*zf;
    double sfy=(ymax-y)*zf;
    x1=x+sfx;
    y1=y+sfy;
    x2=xmax-sfx;
    y2=ymax-sfy;
    if (x1<ox)
        x1=ox;
    if (x2>oxmax)
        x2=oxmax;
    if (y1<oy)
        y1=oy;
    if (y2>oymax)
        y2=oymax;
    double df,r,g,b;
    df=(double(other.getDamage())/double(other.getStrength()))*(0.50);

    double ohue=other.getHue();
    r=Hue_2_RGB(0,0.5-df,ohue+0.3333);
    g=Hue_2_RGB(0,0.5-df,ohue);
    b=Hue_2_RGB(0,0.5-df,ohue-0.3333);

    int oz=other.getZ();
    glColor3d(r,g,b);
    glTexCoord2d(x1,-y1);
    glVertex3d(x1,y1,oz+1);
    glTexCoord2d(x2,-y1);
    glVertex3d(x2,y1,oz+1);
    glTexCoord2d(x2,-y2);
    glVertex3d(x2,y2,oz+1);
    glTexCoord2d(x1,-y2);
    glVertex3d(x1,y2,oz+1);

}
LegoBlock::LegoBlock(int X,int Y,int Color,int Strength,int Type,int Rotation)
    : x(X),y(Y),color(Color),strength(Strength),type(Type),damage(0),z(30),rotation(Rotation)
{
    hue=double(color)/double(MANARANGE);
    V.clear();
    //nothing to do here so far
}
void LegoBlock::RenderLines()
{
    double x1=x;
    double y1=y;
    double x2=x+BrickSizes[type][rotation];
    double y2=y+BrickSizes[type][1-rotation];

    double r,g,b;
    double df;
    df=(double(damage)/double(strength))*(0.50);

    r=Hue_2_RGB(0,1-df,hue+0.3333);
    g=Hue_2_RGB(0,1-df,hue);
    b=Hue_2_RGB(0,1-df,hue-0.3333);
    glColor3d(r,g,b);
    glVertex3d(x1,y1,z+1);
    glVertex3d(x2,y1,z+1);
    
    glVertex3d(x2,y1,z+1);
    glVertex3d(x2,y2,z+1);
    
    glVertex3d(x2,y2,z+1);
    glVertex3d(x1,y2,z+1);
    
    glVertex3d(x1,y2,z+1);
    glVertex3d(x1,y1,z+1);
    
    
    glVertex3d(x1,y1,z);
    glVertex3d(x2,y1,z);
    
    glVertex3d(x2,y1,z);
    glVertex3d(x2,y2,z);
    
    glVertex3d(x2,y2,z);
    glVertex3d(x1,y2,z);
    
    glVertex3d(x1,y2,z);
    glVertex3d(x1,y1,z);
    
    
    glVertex3d(x1,y1,z+1);
    glVertex3d(x1,y1,z);
    
    glVertex3d(x2,y1,z+1);
    glVertex3d(x2,y1,z);
    
    glVertex3d(x2,y2,z+1);
    glVertex3d(x2,y2,z);
    
    glVertex3d(x1,y2,z+1);
    glVertex3d(x1,y2,z);
}
void LegoBlock::RenderTop()
{
    double x1=x;
    double y1=y;
    double x2=x+BrickSizes[type][rotation];
    double y2=y+BrickSizes[type][1-rotation];

    double r,g,b;
    double df;
    df=(double(damage)/double(strength))*(0.50);
    r=Hue_2_RGB(0,0.95-df,hue+0.3333);
    g=Hue_2_RGB(0,0.95-df,hue);
    b=Hue_2_RGB(0,0.95-df,hue-0.3333);
    glColor3d(r,g,b);

    glTexCoord2d(x1,-y1);
    glVertex3d(x1,y1,z+1);
    glTexCoord2d(x2,-y1);
    glVertex3d(x2,y1,z+1);
    glTexCoord2d(x2,-y2);
    glVertex3d(x2,y2,z+1);
    glTexCoord2d(x1,-y2);
    glVertex3d(x1,y2,z+1);
}
void LegoBlock::RenderShadow()
{
    if (z>0)
    {
        double x1=x;
        double y1=y;
        double x2=x+BrickSizes[type][rotation];
        double y2=y+BrickSizes[type][1-rotation];

        double r,g,b;
        double df;
        df=(double(damage)/double(strength))*(0.50);
        r=Hue_2_RGB(0,0.95-df,hue+0.3333);
        g=Hue_2_RGB(0,0.95-df,hue);
        b=Hue_2_RGB(0,0.95-df,hue-0.3333);
        glColor3d(r,g,b);


        double zf=(z/50.0);
        if (zf>0.5) zf=0.5;
        double pcx,pcy;
        pcx=(x2-x1)*zf;
        pcy=(y2-y1)*zf;
        glColor3d(0.5,0.5,0.5);
        glTexCoord2d(x1+pcx,-(y1+pcy));
        glVertex3d(x1+pcx,y1+pcy,0);
        glTexCoord2d(x2-pcx,-(y1+pcy));
        glVertex3d(x2-pcx,y1+pcy,0);
        glTexCoord2d(x2-pcx,-(y2-pcy));
        glVertex3d(x2-pcx,y2-pcy,0);
        glTexCoord2d(x1+pcx,-(y2-pcy));
        glVertex3d(x1+pcx,y2-pcy,0);
    }
}
void LegoBlock::RenderSidesBottom()
{
    double x1=x;
    double y1=y;
    double x2=x+BrickSizes[type][rotation];
    double y2=y+BrickSizes[type][1-rotation];
    //std::cout<<"Rendering brick type "<<BrickTypes[type]<<" at ("<<x<<","<<y<<","<<z<<")"<<std::endl;
    //TODO - set color
    double df;
    df=(double(damage)/double(strength))*(0.50);
    double r,g,b;
    r=Hue_2_RGB(0,0.75-df,hue+0.3333);
    g=Hue_2_RGB(0,0.75-df,hue);
    b=Hue_2_RGB(0,0.75-df,hue-0.3333);
    glColor3d(r,g,b);
    //glBegin(GL_QUAD_STRIP);
    //{
    glVertex3d(x1,y1,z);
    glVertex3d(x2,y1,z);
    glVertex3d(x2,y1,z+1);
    glVertex3d(x1,y1,z+1);

    glVertex3d(x2,y1,z);
    glVertex3d(x2,y2,z);
    glVertex3d(x2,y2,z+1);
    glVertex3d(x2,y1,z+1);

    glVertex3d(x2,y2,z);
    glVertex3d(x1,y2,z);
    glVertex3d(x1,y2,z+1);
    glVertex3d(x2,y2,z+1);

    glVertex3d(x1,y2,z);
    glVertex3d(x1,y1,z);
    glVertex3d(x1,y1,z+1);
    glVertex3d(x1,y2,z+1);
    //}
    //glEnd(); //Quad Strip

    
    r=Hue_2_RGB(0,0.50-df,hue+0.3333);
    g=Hue_2_RGB(0,0.50-df,hue);
    b=Hue_2_RGB(0,0.50-df,hue-0.3333);
    glColor3d(r,g,b);

    glVertex3d(x1,y1,z);
    glVertex3d(x1,y2,z);
    glVertex3d(x2,y2,z);
    glVertex3d(x2,y1,z);
    
/*    double pcx,pcy;
    pcx=(x2-x1)*(z/50.0);
    pcy=(y2-y1)*(z/50.0);
    glColor3d(0.5,0.5,0.5);
    glVertex3d(x1+pcx,y1+pcy,0);
    glVertex3d(x2-pcx,y1+pcy,0);
    glVertex3d(x2-pcx,y2-pcy,0);
    glVertex3d(x1+pcx,y2-pcy,0);*/
}
bool LegoBlock::collideAcross(LegoBlock & other)
{
    int ox=other.getX();
    int oy=other.getY();
    int ot=other.getType();
    int orot=other.getRotation();
    int xmax=x+BrickSizes[type][rotation];
    int ymax=y+BrickSizes[type][1-rotation];
    int oxmax=ox+BrickSizes[ot][orot];
    int oymax=oy+BrickSizes[ot][1-orot];
    //collide the rects
    if (ymax <= oy || oymax <= y)
        return false;
    if (xmax <= ox || oxmax <= x)
        return false;
    return true;
}
