#include "CameraInput.h"

const float CameraInput::min_elevation = -1.5f;
const float CameraInput::max_elevation = 1.5f;
const float CameraInput::min_position = -50.0f;
const float CameraInput::max_position = 49.0f;

const float CameraInput::angle_sensitivity = 0.008f;
const float CameraInput::move_speed = 0.2f;

CameraInput::CameraInput(Terrain* terrain, float x, float z)
	: terrain(terrain), angle_direction(0.0f), angle_elevation(0.0f)
{
	eye_position.x = x;
	eye_position.z = z;
	eye_position.y = GetHeight(x, z);
	UpdateViewOrien();
}

void CameraInput::UpdateViewOrien() {
	view_orientation.x = eye_position.x + cosf(angle_elevation) * cosf(angle_direction);
	view_orientation.y = eye_position.y + -sinf(angle_elevation);
	view_orientation.z = eye_position.z + cosf(angle_elevation) * sinf(angle_direction);
}

float CameraInput::GetHeight(float x, float z) {
	int xi = static_cast<int>(round((eye_position.x / 50) * 128)) + 128;
	int zi = static_cast<int>(round((eye_position.z / 50) * 128)) + 128;
	xi = std::max(std::min(xi, 255), 0);
	zi = std::max(std::min(zi, 255), 0);
	return terrain->height[xi][zi] * TERRAIN_HEIGHT;
}

void CameraInput::OnMouseMoved(int dx, int dy)
{
	angle_direction += dx * angle_sensitivity;
	angle_elevation += dy * angle_sensitivity;

	// Clamp the results
	if (angle_elevation > max_elevation)    angle_elevation = max_elevation;
	if (angle_elevation < min_elevation)    angle_elevation = min_elevation;

	UpdateViewOrien();
}

void CameraInput::Move()
{
	float vv = vel_w ? 1.0 : 0.0 + vel_s ? -1.0 : 0.0;
	float vu = vel_d ? 1.0 : 0.0 + vel_a ? -1.0 : 0.0;

	if (vv + vu == 0)
		return;

	float inAngle = atan2(vu, vv);

	eye_position.x += move_speed * cosf(angle_direction + inAngle);
	if (eye_position.x > max_position) eye_position.x = max_position;
	if (eye_position.x < min_position) eye_position.x = min_position;

	eye_position.z += move_speed * sinf(angle_direction + inAngle);
	if (eye_position.z > max_position) eye_position.z = max_position;
	if (eye_position.z < min_position) eye_position.z = min_position;

	eye_position.y = GetHeight(eye_position.x, eye_position.z);

	UpdateViewOrien();
}

glm::vec3 CameraInput::GetEyePosition() const
{
	return eye_position;
}

glm::vec3 CameraInput::GetViewOrientation() const
{
	return view_orientation;
}