Chris Illy Culling

Lab report for DA307A Assignment 2

3 April 2025

---

# First-Person Camera

In preparation of this task, I decided to rework my camera solution from **LAB 1**. Instead of using a `vec3f m_rotation`, I now make use of simple `float m_yaw` and `float m_pitch` variables instead.

My new rotate methods now also resemble the pre-existing structure of the engine:

```cpp
#pragma region LAB 1 CAMERA SOLUTION DEPRECATED
void Camera::RotateX(const float mousedx)
{
	m_rotation.x += mousedx;
}

void Camera::RotateY(const float mousedy)
{
	m_rotation.y += mousedy;
}
#pragma endregion

// LAB 2 SOLUTION
void Camera::RotateTo(const float& yaw, const float& pitch) noexcept
{
	m_yaw = yaw;
	m_pitch = pitch;
}

void Camera::Rotate(const float& yaw, const float& pitch) noexcept
{
	m_yaw -= yaw;
	m_pitch -= pitch;
}
```

The mouse sensitivity (`m_mouse_sensitivity`) is now handled inside of `input_handler` instead, and `Rotate()` parameter values are now gotten from `input_handler`'s `GetMouseInputX()` and  `GetMouseInputY()` methods which adjust for the `m_mouse_sensitivity`.

```cpp
#pragma region LAB 1 CAMERA SOLUTION DEPRECATED
    // WHAT A TERRIBLE SOLUTION AND CODE-UNHYGENIC WAY OF DOING THIS *INSIDE* THE SCENE UPDATE() METHOD 😱
	//long mousedx = input_handler.GetMouseDeltaX();
	//long mousedy = input_handler.GetMouseDeltaY();
	//float sensitivity = (1 / 300.0f);
	//
	//m_camera->RotateX(mousedx * sensitivity);
	//m_camera->RotateY(mousedy * sensitivity);
	#pragma endregion

    // LAB 2 SOLUTION; The rotation values are now handled inside input_handler! 
	m_camera->Rotate(input_handler.GetMouseInputX(), input_handler.GetMouseInputY());
```

The camera now uses the `m_yaw` and `m_pitch` values directly when getting the WorldToViewMatrix. FIX: With this improved and corrected solution, the camera now pitches down and up (when moving the mouse down and up) regardless of orientation.

```cpp
return transpose(mat4f::rotation(0, m_yaw, m_pitch)) * mat4f::translation(-m_position);
```

# Constant Buffers

# Phong Shading