#include "view.h"

void GLView::Push_Perspective_Projection()
{
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluPerspective(45.0, GLfloat(Width) / GLfloat(Height), 0.5f, 500.0);
}

void GLView::Pop_Perspective_Projection()
{
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
}

void GLView::Begin()
{
    // If the camera has been moved, we'll continue moving it
    if (Moved)
    {
        Moved--;

        Posx -= (Posx - nPosx) / 4.0;
        Posy -= (Posy - nPosy) / 4.0;
        Posz -= (Posz - nPosz) / 4.0;

        Dirx -= (Dirx - nDirx) / 4.0;
        Diry -= (Diry - nDiry) / 4.0;
        Dirz -= (Dirz - nDirz) / 4.0;

        Upx -= (Upx - nUpx) / 4.0;
        Upy -= (Upy - nUpy) / 4.0;
        Upz -= (Upz - nUpz) / 4.0;

        if (Moved==0)
        {
            Posx = nPosx;
            Posy = nPosy;
            Posz = nPosz;

            Dirx = nDirx;
            Diry = nDiry;
            Dirz = nDirz;

            Upx = Upx;
            Upy = Upy;
            Upz = Upz;
       }

    }
    
    glPushAttrib(GL_ALL_ATTRIB_BITS);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glViewport(Left, Top, Width, Height);
	gluLookAt(Posx, Posy, Posz, Dirx, Diry, Dirz, Upx, Upy, Upz);
}

void GLView::End()
{
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

    glPopAttrib();
}

void GLView::Translate(GLfloat dx, GLfloat dy, GLfloat dz)
{
    Posx += dx;
    Posy += dy;
    Posz += dz;

    Dirx += dx;
    Diry += dy;
    Dirz += dz;
}

void GLView::Follow(GLfloat posx, GLfloat posy, GLfloat posz)
{
    Follow(posx, posy, posz, Dirx, Diry, Dirz, Upx, Upy, Upz);
}

void GLView::Follow(GLfloat posx, GLfloat posy, GLfloat posz,
        GLfloat dirx, GLfloat diry, GLfloat dirz,
        GLfloat upx, GLfloat upy, GLfloat upz)
{
    Moved = 100;
    nPosx = posx;
    nPosy = posy;
    nPosz = posz;
    
    nDirx = dirx;
    nDiry = diry;
    nDirz = dirz;
    
    nUpx = upx;
    nUpy = upy;
    nUpz = upz;
}

