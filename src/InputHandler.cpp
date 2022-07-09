#include "InputHandler.h"
const float InputHandler::SPEED_STEP = 0.005f;

InputHandler::InputHandler(CameraInput* camera_input)
	: camera_input(camera_input) {}

// Called when the user presses a key
void InputHandler::OnKeyDown(unsigned char key, int mouseX, int mouseY, float* animation_speed)
{
	switch (key)
	{
	case 27: // Escape
		exit(0);
		break;
	case 'g':
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glutPostRedisplay();
		break;
	case 'h':
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glutPostRedisplay();
		break;
	case 'f':
		glutFullScreenToggle();
		break;
	case '+':
		*animation_speed += SPEED_STEP;
		break;
	case '-':
		*animation_speed -= *animation_speed > InputHandler::SPEED_STEP ? InputHandler::SPEED_STEP : 0.0f;
		break;
	case 'w':
		camera_input->vel_w = true;
		break;
	case 's':
		camera_input->vel_s = true;
		break;
	case 'a':
		camera_input->vel_a = true;
		break;
	case 'd':
		camera_input->vel_d = true;
		break;
	}
}

void InputHandler::OnKeyUp(unsigned char key, int mouseX, int mouseY)
{
	switch (key)
	{
	case 'w':
		camera_input->vel_w = false;
		break;
	case 's':
		camera_input->vel_s = false;
		break;
	case 'a':
		camera_input->vel_a = false;
		break;
	case 'd':
		camera_input->vel_d = false;
		break;
	}
}

// Called when the user moves with the mouse (when some mouse button is pressed)
void InputHandler::OnMouseMove(int x, int y, int win_width, int win_height)
{
	static bool just_warped = false;

	if (just_warped) {
		just_warped = false;
		return;
	}

	int cx = win_width / 2;
	int cy = win_height / 2;
	camera_input->OnMouseMoved(x - cx, y - cy);

	glutWarpPointer(cx, cy);
	just_warped = true;
}
