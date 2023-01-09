#include "Renderer.h"

int main(){
	Window* windows = Window::CreateGameWindow("Opengl", 1280, 720);

	if(!windows->HasInitialised()) {
		return -1;
	}
	
	if(!Renderer::instance((Win32Window*)windows)->HasInitialised()) {
		return -1;
	}

	windows->LockMouseToWindow(true);
	windows->ShowOSPointer(true);

	while(windows->UpdateWindow()  && !Window::GetKeyboard()->KeyPressed(KeyboardKeys::ESCAPE)){
		Renderer::instance()->UpdateScene(Window::GetTimer()->GetTimeDeltaSeconds());
		Renderer::instance()->RenderScene();
		Renderer::instance()->SwapBuffers();
	}
	return 0;
}