#ifndef _GLWINDOW_H
#define _GLWINDOW_H
#ifdef __MACH__
#  include <OpenGL/gl.h>
#  include <OpenGL/glu.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#endif

class GLView
{
	private:
        // Camera position in the scene
		GLfloat Posx, Posy, Posz;
		GLfloat Dirx, Diry, Dirz;
		GLfloat Upx,  Upy,  Upz;

        // Rendering window (think split screen)
		GLint Top, Left;
		GLsizei Width, Height;

        // Camera's future position in the scene
        int Moved;
		GLfloat nPosx, nPosy, nPosz;
		GLfloat nDirx, nDiry, nDirz;
		GLfloat nUpx,  nUpy,  nUpz;

	public:
		GLView(GLfloat posx, GLfloat posy, GLfloat posz,
			GLfloat dirx, GLfloat diry, GLfloat dirz,
			GLfloat upx, GLfloat upy, GLfloat upz,
			GLint left, GLint top, GLsizei width, GLsizei height)
				: Posx(posx), Posy(posy), Posz(posz),
				Dirx(dirx), Diry(diry), Dirz(dirz),
				Upx(upx), Upy(upy), Upz(upz),
				Top(top), Left(left), Width(width), Height(height),
                Moved(0)
					{};

		void Begin();
		void End();
		
		void Push_Perspective_Projection();
		void Pop_Perspective_Projection();
		
        void Translate(GLfloat dx, GLfloat dy, GLfloat dz);

        void Follow(GLfloat posx, GLfloat posy, GLfloat posz);

        void Follow(GLfloat posx, GLfloat posy, GLfloat posz,
			GLfloat dirx, GLfloat diry, GLfloat dirz,
			GLfloat upx, GLfloat upy, GLfloat upz);
};

#endif
