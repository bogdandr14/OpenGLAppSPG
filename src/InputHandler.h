#include "CameraInput.h"
//-----------------------------------------
//----       INPUT HANDLER CLASS       ----
//-----------------------------------------
class InputHandler
{
private:
	CameraInput* camera_input;
	static const float SPEED_STEP;

public:
	InputHandler() : camera_input(nullptr){ };

	InputHandler(CameraInput* camera_input);
	void OnKeyDown(unsigned char key, int mouseX, int mouseY, float* animation_speed);
	void OnKeyUp(unsigned char key, int mouseX, int mouseY);
	void OnMouseMove(int x, int y, int win_width, int win_height);
};