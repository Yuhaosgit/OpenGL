#include "../nclgl/window.h"
#include "Renderer.h"

int main(){
	//Window windows("OpenGL", 1920, 1080, false);
	Window windows("OpenGL", 1280, 760, false);


	if(!windows.HasInitialised()) {
		return -1;
	}
	
	if(!Renderer::instance(&windows)->HasInitialised()) {
		return -1;
	}

	windows.LockMouseToWindow(true);
	windows.ShowOSPointer(true);

	while(windows.UpdateWindow()  && !Window::GetKeyboard()->KeyDown(KEYBOARD_ESCAPE)){
		Renderer::instance()->UpdateScene(Window::GetTimer()->GetTimeDeltaSeconds());
		Renderer::instance()->RenderScene();
		Renderer::instance()->SwapBuffers();
	}
	return 0;
}