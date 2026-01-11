#include "globals.h"
#include "window.h"
#include "scenes/test.h"

int main() {
	window = new Window(800, 600, "Brick Battle", FLAG_WINDOW_RESIZABLE);
}

void Window::Init() {;
	sceneManager->LoadScene(new Test());
}