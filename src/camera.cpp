#include "Camera.h"

using namespace linalg;

void Camera::MoveTo(const vec3f& position) noexcept
{
	m_position = position;
}

void Camera::Move(const vec3f& direction) noexcept
{
	m_position += direction;
}

void Camera::MoveForward() noexcept
{
	vec4f viewForward = { 0, 0, -1, 0 };
	vec4f worldSpaceForward = ViewToWorldMatrix() * viewForward;

	Move(worldSpaceForward.xyz());
}

void Camera::MoveBack() noexcept 
{ 
	vec4f viewBack = { 0, 0, 1, 0 };
	vec4f worldSpaceBack = ViewToWorldMatrix() * viewBack;

	Move(worldSpaceBack.xyz());
}

void Camera::MoveLeft() noexcept 
{
	vec4f viewLeft = { -1, 0, 0, 0 };
	vec4f worldSpaceLeft = ViewToWorldMatrix() * viewLeft;

	Move(worldSpaceLeft.xyz());
}

void Camera::MoveRight() noexcept 
{
	vec4f viewRight = { 1, 0, 0, 0 };
	vec4f worldSpaceRight = ViewToWorldMatrix() * viewRight;

	Move(worldSpaceRight.xyz());
}

#pragma region LAB 1 CAMERA SOLUTION DEPRECATED
//void Camera::RotateX(const float mousedx)
//{
//	m_rotation.x += mousedx;
//}
//
//void Camera::RotateY(const float mousedy)
//{
//	m_rotation.y += mousedy;
//}
#pragma endregion

// LAB 2 SOLUTION
void Camera::RotateTo(const float& yaw, const float& pitch) noexcept
{
	m_yaw = yaw;
	m_pitch = pitch;

	m_rotation = mat4f::rotation(0, -m_yaw, -m_pitch);
}

void Camera::Rotate(const float& yaw, const float& pitch) noexcept
{
	bool lookingTooFarDown = pitch < 0 && m_pitch <= (-3.1415 / 2);
	bool lookingTooFarUp = pitch > 0 && m_pitch >= (3.1415 / 2);
	bool lookingTooFar = lookingTooFarDown || lookingTooFarUp;

	m_yaw += yaw;

	if (!lookingTooFar)
		m_pitch += pitch;

	m_rotation = mat4f::rotation(0, -m_yaw, -m_pitch);
}

mat4f Camera::WorldToViewMatrix() const noexcept
{
	// Assuming a camera's position and rotation is defined by matrices T(p) and R,
	// the View-to-World transform is T(p)*R (for a first-person style camera).
	//
	// World-to-View then is the inverse of T(p)*R;
	//		inverse(T(p)*R) = inverse(R)*inverse(T(p)) = transpose(R)*T(-p)
	// Since now there is no rotation, this matrix is simply T(-p)

	//return mat4f::rotation(0, m_rotation.x, m_rotation.y) * mat4f::translation(-m_position);
	
	return transpose(m_rotation) * mat4f::translation(-m_position);
}

mat4f Camera::ViewToWorldMatrix() const noexcept
{
	return mat4f::translation(m_position) * m_rotation;
}

mat4f Camera::ProjectionMatrix() const noexcept
{
	return mat4f::projection(m_vertical_fov, m_aspect_ratio, m_near_plane, m_far_plane);
}

vec3f Camera::GetPosition()
{
	return m_position;
}
