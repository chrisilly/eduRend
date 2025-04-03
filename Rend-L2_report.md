Chris Illy Culling

Lab report for DA307A Assignment 2

3 April 2025

---

# First-Person Camera

## Camera Mouse-controlled Rotation Rework

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
// OBS! The yaw and pitch values are negative here now because I am now adding instead of subtracting in my Rotate() method
return transpose(mat4f::rotation(0, -m_yaw, -m_pitch)) * mat4f::translation(-m_position);
```

## Limiting the up/down Camera Rotation

In order to prevent our poor camera from breaking its neck (looking too far up or down), we can simply put an if-clause in the `Rotate()` method to check if we're going too far before applying any changes. This trivial solution took absolutely no time at all and had no impact on my well-being.

```cpp
void Camera::Rotate(const float& yaw, const float& pitch) noexcept
{
	bool lookingTooFarDown = pitch < 0 && m_pitch <= (-3.1415 / 2);
	bool lookingTooFarUp = pitch > 0 && m_pitch >= (3.1415 / 2);
	bool lookingTooFar = lookingTooFarDown || lookingTooFarUp;

	m_yaw += yaw;

    // Only 
	if (!lookingTooFar)
		m_pitch += pitch;
}
```

## View-direction Dependent Movement

Making sense of the calculations here took me hours. I was conceptually grasping the need to add the multiplication of the `ViewToWorld` matrix and the `forward view` vector (the `forward world`) to the camera's `m_position`. The trouble is, I wasn't quite sure what type each variable was meant to be or how to make the `forward world` a 3-dimensional vector so as to be able to add it to the camera's `m_position` in the first place. I ended up by sheer luck and ages of scrambling finding the `.xyz()` method which turns a `vec4f` into a 3-dimensional vector.

```cpp
void Camera::MoveForward() noexcept
{
	mat4f viewToWorld = mat4f::translation(m_position) * m_rotation;

	vec4f viewForward = { 0, 0, -1, 0 };

	vec4f worldSpaceForward = viewToWorld * viewForward;

	Move(worldSpaceForward.xyz());
}
```

After successfully making the `MoveForward()` method, I was able to make respective methods for the other three directions.

# Constant Buffers

# Phong Shading