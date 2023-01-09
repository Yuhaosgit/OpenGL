#pragma once
/*
Class:OGLRenderer
Author:Rich Davison	 <richard-gordon.davison@newcastle.ac.uk>
Description:Abstract base class for the graphics tutorials. Creates an OpenGL 
3.2+ CORE PROFILE rendering context. Each lesson will create a renderer that 
inherits from this class - so all context creation is handled automatically,
but students still get to see HOW such a context is created.

-_-_-_-_-_-_-_,------,   
_-_-_-_-_-_-_-|   /\_/\   NYANYANYAN
-_-_-_-_-_-_-~|__( ^ .^) /
_-_-_-_-_-_-_-""  ""   

*/
#include "Common.h"

#include <string>
#include <fstream>
#include <vector>

#include "KHR\khrplatform.h"
#include "glad\glad.h"

#include "GL/GL.h"
#include "KHR/WGLext.h"

#include "Vector4.h"
#include "Vector3.h"
#include "Vector2.h"
#include "Quaternion.h"
#include "Matrix4.h"
#include "Win32Window.h"
#include "Shader.h"
#include "Mesh.h"
#include "Camera.h"

using std::vector;

#define OPENGL_DEBUGGING _DEBUG

extern const Matrix4 biasMatrix;

class OGLRenderer	{
public:
	friend class Window;
	OGLRenderer(Win32Window &parent);
	virtual ~OGLRenderer(void);

	virtual void	RenderScene() = 0;
	virtual void	UpdateScene(const float& msec) = 0;
	void			SwapBuffers();

	bool			HasInitialised() const;

	void OnWindowResize(int w, int h);
	void OnWindowDetach() { }

	static int GetCurrentWidth() { return currentWidth; }
	static int GetCurrentHeight() { return currentHeight; }
protected:
	void StartDebugGroup(const std::string& s) {
		glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, (GLsizei)s.length(), s.c_str());
	}

	void EndDebugGroup() {
		glPopDebugGroup();
	}

	static int		currentWidth;			//Render area width (not quite the same as window width)
	static int		currentHeight;			//Render area height (not quite the same as window height)

	bool	init;			//Did the renderer initialise properly?
	Win32Window& hostWindow;
	void OGLRenderer::InitWithWin32(Window& w);
private:
	HDC		deviceContext;	//...Device context?
	HGLRC	renderContext;	//Permanent Rendering Context
#ifdef _DEBUG
	static void CALLBACK DebugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam);
#endif
};