#include "mage.h"
#include <iostream>
#define FALL_COUNT 30
Mage::Mage(int X, int Y, int Z, int Col, double Ang, int Vx, 
        int Vy, int Vw, int Vh )
    :x(X),y(Y),z(Z),col(Col),vx(Vx),vy(Vy),vw(Vw),vh(Vh),fh(1),fd(4),ang(Ang),
    sr(0.5),sg(0.5),sb(0.5),sz(0),fx(0),fy(0),
    view(
        x-cos(ang)*fd,y-sin(ang)*fd,z+fh,
        x,y,z,
        0,0,1,
        vx,vy,
        vw,vh), speed(0.1), blockOn(NULL), blockAside(NULL), ticker(0)
{
    fpos.x=x;
    fpos.y=y;
    fpos.z=z;
    updateOrtho();
    setColor(col);
    moved=false;
}

void Mage::setColor(int Col)
{
    col=Col;
    hue=(double(col)/double(MANARANGE));
    mr=Hue_2_RGB(0,1,hue+0.3333);
    mg=Hue_2_RGB(0,1,hue);
    mb=Hue_2_RGB(0,1,hue-0.3333);
    //std::cout << "Color: " << col << " Hue: " << hue << " r: " << mr << " g: " << mg << " b: " << mb << std::endl;
}
bool Mage::tryMoveUp(std::list <LegoBlock *> & V)
{
    for (std::list<LegoBlock *>::const_iterator 
            i = V.begin();i!=V.end();++i)
    {
        int bx=(*i)->getX();
        int by=(*i)->getY();
        int br=(*i)->getRotation();
        int bt=(*i)->getType();
        int maxx=bx+BrickSizes[bt][br];
        int maxy=by+BrickSizes[bt][1-br];
        if (fpos.x >= bx && fpos.x < maxx &&
                fpos.y >= by && fpos.y < maxy)
        {
            int zp=(*i)->getZ();
            if (zp<fpos.z && (zp+1)>fpos.z)
            {
                //can't walk here.
                return false;
            }
        }
    }
	ticker=0;
    z++;
    fpos.z++;
    updateView();
    setMoved();
    blockOn=NULL;
    return true;
}

bool Mage::walkRel(int direction,std::list <LegoBlock *> & V,bool doFall)
{
    // This is the position we're attempting to move to
    double attempt_fx = fpos.x + cos(ang) * direction * speed; 
    double attempt_fy = fpos.y + sin(ang) * direction * speed;

    // Clip against the outer world boundary
    if (attempt_fx > 99.0) attempt_fx = 99.0;
    else if (attempt_fx < 0.0) attempt_fx = 0.0;
    if (attempt_fy > 99.0) attempt_fy = 99.0;
    else if (attempt_fy < 0.0) attempt_fy = 0.0;

    // Can't 'continue' out of nested loops :|
    int tryIndividualPoles=-1;
TRY_ANOTHER_MOVEMENT_POLE_COMBO:
    tryIndividualPoles++;
    double future_fx = tryIndividualPoles&1? fpos.x:attempt_fx;
    double future_fy = tryIndividualPoles>1? fpos.y:attempt_fy;

    // Integral representation of our future grid position
    int nxp = int(future_fx);
    int nyp = int(future_fy);

    // is this a step into a new gridpsace?
    if ((int(future_fx) != x) || (int(future_fy) != y) || 1)
    {
        // TODO: this is where you can set some sort of "yes i've moved" flag
    }

    // We will search for blocks who share our column position on the field
    // which are also below us, and the altitude of the highest of these
    // blocks will determine the lowest point we can fall to in our column
    // given our current altitude. If we find no such blocks, we use the
    // ground, which we acknowledge here
    int maxz=-1;

    // These values will determine the color of the shadow to draw. If no
    // block as the one described above is found, then these settings will
    // reflect the color and position of the ground plane
    sr=0.5;
    sg=0.5;
    sb=0.5;
    sz=0;

    LegoBlock * temBlockOn = NULL;
    if (direction != 0) blockAside = NULL;

    // Now we iterate through each block searching for ones sharing our
    // column which we might land on
    for (std::list<LegoBlock *>::const_iterator 
            i = V.begin();i!=V.end();++i)
    {
        int bx=(*i)->getX();
        int by=(*i)->getY();
        int br=(*i)->getRotation();
        int bt=(*i)->getType();
        int maxx=bx+BrickSizes[bt][br];
        int maxy=by+BrickSizes[bt][1-br];

        // Collide with block?
        if (    nxp >= (bx-1) &&
                nxp < maxx &&
                nyp >= (by-1) &&
                nyp < maxy)
        {
            int zp=(*i)->getZ(); // elevation of this block
            if (zp==z) // block is in our plane -- an in our way
            {
                //can't walk here.
                blockAside=(*i);
                if (tryIndividualPoles == 3) // tried all poles, return not falling
                    return direction ? walkRel(0, V, doFall) : false;
                //printf("Trying to walk along (%i,%i)\n", !tryIndividualPoles&1, !tryIndividualPoles>1);
                goto TRY_ANOTHER_MOVEMENT_POLE_COMBO;

            } else if (zp>z) // block is above us
            {
                //ignore these
            }
            else if (zp>maxz) // block is below us
            {
                double bhue=(*i)->getHue();
                double df=(double((*i)->getDamage())/
                        double((*i)->getStrength()))*(0.50);
                sr=Hue_2_RGB(0,0.5-df,bhue+0.3333);
                sg=Hue_2_RGB(0,0.5-df,bhue);
                sb=Hue_2_RGB(0,0.5-df,bhue-0.3333);

                maxz=(*i)->getZ();
                sz=(maxz+1);
                temBlockOn=(*i);
            }
        }
        //only need to check X and Y
        //then fetch Z and add if higher
    }

    //zp=maxz+1.5;

    // We've determined maxz to be the highest block lower than our mage who
    // shares our column on the game field. If we are still higher than this
    // position, then we can descend but only if timeout has happened
    if (doFall)
    {
        if (z>(maxz+1))
        {
			ticker++;
			if (ticker>FALL_COUNT)
			{
				ticker=0;
	            z--;
	            fpos.z = z;
	            setMoved();
			}
        }
    }

    // Update our column position on the field
    x=nxp;
    y=nyp;

    // Update our floaty position on the field
    fpos.x = future_fx;
    fpos.y = future_fy;

    updateView();

    if (!(z>(maxz+1)))
        blockOn=temBlockOn;
    else
        blockOn=NULL;
    
    // Return whether or not we're falling
    return z>(maxz+1);
}

// This function may be removed, it was used to determine which cardinal
// direction you were facing for orthogonal grid movements and for bullets
void Mage::updateOrtho()
{
    double xang=ang+(M_PI/4);
    if (xang>(M_PI*2)) xang-=M_PI*2;
    if (xang<(M_PI/2))
    {
        fx=1;
        fy=0;
    } else if (xang<(M_PI))
    {
        fy=1;
        fx=0;
    } else if (xang<(M_PI*3/2))
    {
        fx=-1;
        fy=0;
    }
    else
    {
        fx=0;
        fy=-1;
    }
}

void Mage::updateView()
{
    double Rx = fpos.x + 0.5;
    double Ry = fpos.y + 0.5;
    double Rz = fpos.z;

/*  static double zfag = 0.0;
    if (zfag != fpos.z)
    {zfag = fpos.z; std::cout<<"z: " << fpos.z << std::endl;}*/

    view.Follow(Rx-cos(ang)*fd,Ry-sin(ang)*fd,Rz+fh,
            Rx,Ry,Rz,
            0,0,1);
}
void Mage::turnRel(double ad)
{
    ang+=ad;
    if (ang>M_PI*2)
        ang-=M_PI*2;
    if (ang<0)
        ang+=M_PI*2;
    updateOrtho();
    updateView();
}
void Mage::RenderShadow()
{
    double a, cx, cy, x1, x2, x3, x4, y1, y2, y3, y4;

    cx = fpos.x + 0.5;
    cy = fpos.y + 0.5;

    a = ang + M_PI_4;
    x1 = cos(a) * 0.36;
    y1 = sin(a) * 0.36;

    x3 = -x1;
    y3 = -y1;

    x2 = -y1;
    y2 =  x1;

    x4 = -x2;
    y4 = -y2;

    x1 += cx;
    x2 += cx;
    x3 += cx;
    x4 += cx;
    y1 += cy;
    y2 += cy;
    y3 += cy;
    y4 += cy;

    glColor3d(sr,sg,sb);

    glTexCoord2d(x1, y1);
    glVertex3d  (x1, y1, sz);

    glTexCoord2d(x2, y2);
    glVertex3d  (x2, y2, sz);

    glTexCoord2d(x3, y3);
    glVertex3d  (x3, y3, sz);

    glTexCoord2d(x4, y4);
    glVertex3d  (x4, y4, sz);

/*  glTexCoord2d(Rx-0.18,-(Ry-0.18));
    glVertex3d(Rx-0.18,Ry-0.18,sz);

    glTexCoord2d(Rx+0.18,-(Ry-0.18));
    glVertex3d(Rx+0.18,Ry-0.18,sz);

    glTexCoord2d(Rx+0.18,-(Ry+0.18));
    glVertex3d(Rx+0.18,Ry+0.18,sz);

    glTexCoord2d(Rx-0.18,-(Ry+0.18));
    glVertex3d(Rx-0.18,Ry+0.18,sz);*/
}

void Mage::Render()
{
    double a, cx, cy, x1, x2, x3, x4, y1, y2, y3, y4, z1, z2;

    cx = fpos.x + 0.5;
    cy = fpos.y + 0.5;

    a = ang + M_PI_4;
    x1 = cos(a) * 0.36;
    y1 = sin(a) * 0.36;

    x3 = -x1;
    y3 = -y1;

    x2 = -y1;
    y2 =  x1;

    x4 = -x2;
    y4 = -y2;

    x1 += cx;
    x2 += cx;
    x3 += cx;
    x4 += cx;
    y1 += cy;
    y2 += cy;
    y3 += cy;
    y4 += cy;

    z1 = fpos.z + 0.2;
    z2 = z1 + 2*sqrt(0.36*0.36/2);

    // Back face
    glColor3d(1,1,1);
    glVertex3d(x2, y2, z1);
    glVertex3d(x3, y3, z1);
    glColor3d(mr,mg,mb);
    glVertex3d(x3, y3, z2);
    glVertex3d(x2, y2, z2);

    // Top face
    glColor3d(mr,mg,mb);
    glVertex3d(x1, y1, z2);
    glVertex3d(x2, y2, z2);
    glVertex3d(x3, y3, z2);
    glVertex3d(x4, y4, z2);

    // Left face
    glColor3d(1,1,1);
    glVertex3d(x1,y1,z1);
    glVertex3d(x2,y2,z1);
    glColor3d(mr,mg,mb);
    glVertex3d(x2,y2,z2);
    glVertex3d(x1,y1,z2);

    // Right face
    glColor3d(1,1,1);
    glVertex3d(x3,y3,z1);
    glVertex3d(x4,y4,z1);
    glColor3d(mr,mg,mb);
    glVertex3d(x4,y4,z2);
    glVertex3d(x3,y3,z2);

    // Back face
    glColor3d(1,1,1);
    glVertex3d(x4,y4,z1);
    glVertex3d(x1,y1,z1);
    glColor3d(mr,mg,mb);
    glVertex3d(x1,y1,z2);
    glVertex3d(x4,y4,z2);

    // Bottom face
    glColor3d(1,1,1);
    glVertex3d(x4,y4,z1);
    glVertex3d(x3,y3,z1);
    glVertex3d(x2,y2,z1);
    glVertex3d(x1,y1,z1);

    // tom you do the rest, and obviously fix the colors...

    return;

    /*
    glColor3d(1,1,1);
    glVertex3d(Rx-0.2,Ry+0.2,Rz-0.2);
    glVertex3d(Rx-0.2,Ry-0.2,Rz-0.2);

    glColor3d(mr,mg,mb);
    glVertex3d(Rx-0.2,Ry-0.2,Rz+0.2);
    glVertex3d(Rx-0.2,Ry+0.2,Rz+0.2);
    
    glColor3d(1,1,1);
    glVertex3d(Rx+0.2,Ry+0.2,Rz-0.2);
    glVertex3d(Rx-0.2,Ry+0.2,Rz-0.2);

    glColor3d(mr,mg,mb);
    glVertex3d(Rx-0.2,Ry+0.2,Rz+0.2);
    glVertex3d(Rx+0.2,Ry+0.2,Rz+0.2);

    glColor3d(1,1,1);
    glVertex3d(Rx+0.2,Ry-0.2,Rz-0.2);
    glVertex3d(Rx+0.2,Ry+0.2,Rz-0.2);

    glColor3d(mr,mg,mb);
    glVertex3d(Rx+0.2,Ry+0.2,Rz+0.2);
    glVertex3d(Rx+0.2,Ry-0.2,Rz+0.2);

    glColor3d(1,1,1);
    glVertex3d(Rx-0.2,Ry-0.2,Rz-0.2);
    glVertex3d(Rx+0.2,Ry-0.2,Rz-0.2);

    glColor3d(mr,mg,mb);
    glVertex3d(Rx+0.2,Ry-0.2,Rz+0.2);
    glVertex3d(Rx-0.2,Ry-0.2,Rz+0.2);

    glColor3d(1,1,1);
    glVertex3d(Rx-0.2,Ry-0.2,Rz-0.2);
    glVertex3d(Rx-0.2,Ry+0.2,Rz-0.2);
    glVertex3d(Rx+0.2,Ry+0.2,Rz-0.2);
    glVertex3d(Rx+0.2,Ry-0.2,Rz-0.2);

    glColor3d(mr,mg,mb);
    glVertex3d(Rx-0.2,Ry-0.2,Rz+0.2);
    glVertex3d(Rx+0.2,Ry-0.2,Rz+0.2);
    glVertex3d(Rx+0.2,Ry+0.2,Rz+0.2);
    glVertex3d(Rx-0.2,Ry+0.2,Rz+0.2);
*/
}

