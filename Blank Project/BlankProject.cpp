#include "../NCLGL/window.h"
#include "Renderer.h"
#include <iostream>
#include <fstream>


int main()	{
	int counterFrameRate = 0;

	Window w("Make your own project!", 1280, 720, false);

	if(!w.HasInitialised()) {
		return -1;
	}
	
	Renderer renderer(w);
	if(!renderer.HasInitialised()) {
		return -1;
	}

	w.LockMouseToWindow(true);
	w.ShowOSPointer(false);

	std::ofstream file_results("CurrentResults.txt");
	if (file_results.is_open())
	{
		file_results << "collison detection 600000 4 object" << std::endl;

		while (w.UpdateWindow() && !Window::GetKeyboard()->KeyDown(KEYBOARD_ESCAPE)) {
			renderer.UpdateScene(w.GetTimer()->GetTimeDeltaSeconds());
			renderer.RenderScene();
			renderer.SwapBuffers();
			if (Window::GetKeyboard()->KeyDown(KEYBOARD_F5)) {
				Shader::ReloadAllShaders();
			}

			counterFrameRate++;
			file_results << std::to_string(1.0f / w.GetTimer()->GetTimeDeltaSeconds()) << std::endl;
		}
	}
	file_results.close();

	return 0;
}