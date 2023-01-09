/*
Class:OGLRenderer
Author:Rich Davison	 <richard-gordon.davison@newcastle.ac.uk>
Description:Abstract base class for the graphics tutorials. Creates an OpenGL 
3.2 CORE PROFILE rendering context. Each lesson will create a renderer that 
inherits from this class - so all context creation is handled automatically,
but students still get to see HOW such a context is created.

-_-_-_-_-_-_-_,------,   
_-_-_-_-_-_-_-|   /\_/\   NYANYANYAN
-_-_-_-_-_-_-~|__( ^ .^) /
_-_-_-_-_-_-_-""  ""   

*/
#include "OGLRenderer.h"
#include "Shader.h"
#include <algorithm>

using std::string;

int		OGLRenderer::currentWidth;
int		OGLRenderer::currentHeight;

static const float biasValues[16] = {
	0.5, 0.0, 0.0, 0.0,
	0.0, 0.5, 0.0, 0.0,
	0.0, 0.0, 0.5, 0.0,
	0.5, 0.5, 0.5, 1.0
};
static const Matrix4 biasMatrix(biasValues);

PFNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT = NULL;

/*
Creates an OpenGL 3.2 CORE PROFILE rendering context. Sets itself
as the current renderer of the passed 'parent' Window. Not the best
way to do it - but it kept the Tutorial code down to a minimum!
*/
OGLRenderer::OGLRenderer(Win32Window& window):hostWindow(window)	{
	init					= false;
	InitWithWin32(window);
}

void OGLRenderer::InitWithWin32(Window& w) {
	Win32Window* realWindow = (Win32Window*)&w;

	if (!(deviceContext = GetDC(realWindow->GetHandle()))) {
		std::cout << __FUNCTION__ << " Failed to create window!" << std::endl;
		return;
	}

	//A pixel format descriptor is a struct that tells the Windows OS what type of front / back buffers we want to create etc
	PIXELFORMATDESCRIPTOR pfd;
	memset(&pfd, 0, sizeof(PIXELFORMATDESCRIPTOR));

	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DOUBLEBUFFER | PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW;   //It must be double buffered, it must support OGL(!), and it must allow us to draw to it...
	pfd.iPixelType = PFD_TYPE_RGBA;	//We want our front / back buffer to have 4 channels!
	pfd.cColorBits = 32;				//4 channels of 8 bits each!
	pfd.cDepthBits = 24;				//24 bit depth buffer
	pfd.cStencilBits = 8;				//plus an 8 bit stencil buffer
	pfd.iLayerType = PFD_MAIN_PLANE;

	GLuint		PixelFormat;
	if (!(PixelFormat = ChoosePixelFormat(deviceContext, &pfd))) {	// Did Windows Find A Matching Pixel Format for our PFD?
		std::cout << __FUNCTION__ << " Failed to choose a pixel format!" << std::endl;
		return;
	}

	if (!SetPixelFormat(deviceContext, PixelFormat, &pfd)) {		// Are We Able To Set The Pixel Format?
		std::cout << __FUNCTION__ << " Failed to set a pixel format!" << std::endl;
		return;
	}

	HGLRC		tempContext;		//We need a temporary OpenGL context to check for OpenGL 3.2 compatibility...stupid!!!
	if (!(tempContext = wglCreateContext(deviceContext))) {	// Are We Able To get the temporary context?
		std::cout << __FUNCTION__ << "  Cannot create a temporary context!" << std::endl;
		wglDeleteContext(tempContext);
		return;
	}

	if (!wglMakeCurrent(deviceContext, tempContext)) {	// Try To Activate The Rendering Context
		std::cout << __FUNCTION__ << " Cannot set temporary context!" << std::endl;
		wglDeleteContext(tempContext);
		return;
	}
	if (!gladLoadGL()) {
		std::cout << __FUNCTION__ << " Cannot initialise GLAD!" << std::endl;	//It's all gone wrong!
		return;
	}
	//Now we have a temporary context, we can find out if we support OGL 4.x
	char* ver = (char*)glGetString(GL_VERSION); // ver must equal "4.1.0" (or greater!)
	int major = ver[0] - '0';		//casts the 'correct' major version integer from our version string
	int minor = ver[2] - '0';		//casts the 'correct' minor version integer from our version string

	if (major < 3) {					//Graphics hardware does not support OGL 4! Erk...
		std::cout << __FUNCTION__ << " Device does not support OpenGL 4.x!" << std::endl;
		wglDeleteContext(tempContext);
		return;
	}

	if (major == 4 && minor < 1) {	//Graphics hardware does not support ENOUGH of OGL 4! Erk...
		std::cout << __FUNCTION__ << " Device does not support OpenGL 4.1!" << std::endl;
		wglDeleteContext(tempContext);
		return;
	}
	//We do support OGL 4! Let's set it up...

	int attribs[] = {
		WGL_CONTEXT_MAJOR_VERSION_ARB, major,
		WGL_CONTEXT_MINOR_VERSION_ARB, minor,
		WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB
#ifdef OPENGL_DEBUGGING 
		| WGL_CONTEXT_DEBUG_BIT_ARB
#endif		//No deprecated stuff!! DIE DIE DIE glBegin!!!!
		,WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
		0					//That's enough attributes...
	};

	//Everywhere else in the Renderers, we use function pointers provided by GLEW...but we can't initialise GLEW yet! So we have to use the 'Wiggle' API
	//to get a pointer to the function that will create our OpenGL 3.2 context...
	PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB = (PFNWGLCREATECONTEXTATTRIBSARBPROC)wglGetProcAddress("wglCreateContextAttribsARB");
	renderContext = wglCreateContextAttribsARB(deviceContext, 0, attribs);

	// Check for the context, and try to make it the current rendering context
	if (!renderContext || !wglMakeCurrent(deviceContext, renderContext)) {
		std::cout << __FUNCTION__ << " Cannot set OpenGL 3 context!" << std::endl;	//It's all gone wrong!
		wglDeleteContext(renderContext);
		wglDeleteContext(tempContext);
		return;
	}

	wglDeleteContext(tempContext);	//We don't need the temporary context any more!

	std::cout << __FUNCTION__ << " Initialised OpenGL " << major << "." << minor << " rendering context" << std::endl;	//It's all gone wrong!
#ifdef OPENGL_DEBUGGING
	glDebugMessageCallback(DebugCallback, NULL);
	glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, nullptr, GL_FALSE);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS_ARB);
#endif

	//If we get this far, everything's going well!
	init = true;

	wglSwapIntervalEXT = (PFNWGLSWAPINTERVALEXTPROC)wglGetProcAddress("wglSwapIntervalEXT");
	w.SetRenderer(this);
}

/*
Destructor. Deletes the default shader, and the OpenGL rendering context.
*/
OGLRenderer::~OGLRenderer(void)	{
	wglDeleteContext(renderContext);
}

/*
Returns TRUE if everything in the constructor has gone to plan.
Check this to end the application if necessary...
*/
bool OGLRenderer::HasInitialised() const{
	return init;
}

/*
Swaps the buffers, ready for the next frame's rendering. Should be called
every frame, at the end of RenderScene(), or whereever appropriate for
your application.
*/
void OGLRenderer::SwapBuffers() {
	//We call the windows OS SwapBuffers on win32. Wrapping it in this 
	//function keeps all the tutorial code 100% cross-platform (kinda).
	::SwapBuffers(deviceContext);
}

void OGLRenderer::OnWindowResize(int w, int h) {
	currentWidth = w;
	currentHeight = h;
	glViewport(0, 0, currentWidth, currentHeight);
}

/*
Used by some later tutorials when we want to have framerate-independent
updates on certain datatypes. Really, OGLRenderer should have its own
timer, so it can just sit and look after itself (potentially even in
another thread...), but it's fine for the tutorials.

STUDENTS: Don't put your entity update routine in this, or anything like
that - it's just asking for trouble! Strictly speaking, even the camera
shouldn't be in here...(I'm lazy)
*/
#ifdef _DEBUG
void OGLRenderer::DebugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)	{
		string sourceName;
		string typeName;
		string severityName;

		if (type == GL_DEBUG_TYPE_OTHER_ARB) {
			return;
		}

		switch(source) {
			case GL_DEBUG_SOURCE_API_ARB			: sourceName = "Source(OpenGL)"			;break;
			case GL_DEBUG_SOURCE_WINDOW_SYSTEM_ARB	: sourceName = "Source(Window System)"	;break;
			case GL_DEBUG_SOURCE_SHADER_COMPILER_ARB: sourceName = "Source(Shader Compiler)";break;
			case GL_DEBUG_SOURCE_THIRD_PARTY_ARB	: sourceName = "Source(Third Party)"	;break;
			case GL_DEBUG_SOURCE_APPLICATION_ARB	: sourceName = "Source(Application)"	;break;
			case GL_DEBUG_SOURCE_OTHER_ARB			: sourceName = "Source(Other)"			;break;
		}

		switch(type) {
			case GL_DEBUG_TYPE_ERROR_ARB				: typeName = "Type(Error)"					;break;
			case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR_ARB	: typeName = "Type(Deprecated Behaviour)"	;break;
			case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR_ARB	: typeName = "Type(Undefined Behaviour)"	;break;
			case GL_DEBUG_TYPE_PORTABILITY_ARB			: typeName = "Type(Portability)"			;break;
			case GL_DEBUG_TYPE_PERFORMANCE_ARB			: typeName = "Type(Performance)"			;break;
			case GL_DEBUG_TYPE_OTHER_ARB				: typeName = "Type(Other)"					;break;
			default: {
				return;
			}
		}

		switch(severity) {
			case GL_DEBUG_SEVERITY_HIGH_ARB		: severityName = "Priority(High)"		;break;
			case GL_DEBUG_SEVERITY_MEDIUM_ARB	: severityName = "Priority(Medium)"		;break;
			case GL_DEBUG_SEVERITY_LOW_ARB		: severityName = "Priority(Low)"		;break;
		}

		std::cout << "OpenGL Debug Output: " + sourceName + ", " + typeName + ", " + severityName + ", " + string(message) << "\n";
}
#endif




