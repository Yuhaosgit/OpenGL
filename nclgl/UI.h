#pragma once
#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_win32.h"

class UI {
public:
	static void initialization();
	static void terminate();
	static void Render();
};