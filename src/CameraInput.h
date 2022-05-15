#pragma once
#include <string>
#include <vector>
#include <random>
#include <functional>
#include <algorithm>
#include "Loader.h"
#include "Terrain.h"

//-----------------------------------------
//----       CAMERA INPUT CLASS        ----
//-----------------------------------------

class CameraInput
{
private:
	///		- Minimum elevation in radians
	static const float min_elevation;
	///		- Maximum elevation in radians
	static const float max_elevation; 
	///		- Minimum camera position
	static const float min_position;
	///		- Maximum camera position
	static const float max_position;
	///		- Sensitivity of the mouse when changing elevation or direction angles
	static const float angle_sensitivity;
	///		- Camera move speed
	static const float move_speed;

	Terrain* terrain;

	float angle_direction;
	float angle_elevation;

	/// Final position of the eye in world space coordinates
	glm::vec3 eye_position;
	/// Final position of the camera view orientation in world space coordinates
	glm::vec3 view_orientation;

	/// Recomputes 'view_orientation' from 'angle_direction', 'angle_elevation', and 'distance'
	void UpdateViewOrien();

	float GetHeight(float x, float z);

public:
	bool vel_w = false;
	bool vel_s = false;
	bool vel_a = false;
	bool vel_d = false;

	CameraInput() : terrain(nullptr), angle_direction(0.0f), angle_elevation(0.0f) { };

	CameraInput(Terrain* terrain, float x, float y);

	void Move();

	/// Call when the user moves with the mouse cursor
	void OnMouseMoved(int x, int y);

	/// Returns the position of the eye in world space coordinates
	glm::vec3 GetEyePosition() const;

	/// Returns the position for the view orientation in world space coordinates
	glm::vec3 GetViewOrientation() const;
};