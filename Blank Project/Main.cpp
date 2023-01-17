#include "Engine.h"

int main(){
	Window* windows = Window::CreateGameWindow("Opengl", 1280, 720);

	if(!windows->HasInitialised()) {
		return -1;
	}
	
	if(!Engine::instance((Win32Window*)windows)->HasInitialised()) {
		return -1;
	}

	windows->LockMouseToWindow(true);
	windows->ShowOSPointer(true);

	while(windows->UpdateWindow()  && !Window::GetKeyboard()->KeyPressed(KeyboardKeys::ESCAPE)){
		Engine::instance()->UpdateScene(Window::GetTimer()->GetTimeDeltaSeconds());
		Engine::instance()->RenderScene();
		Engine::instance()->SwapBuffers();
	}
	return 0;
}