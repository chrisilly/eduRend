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

# Constant Buffers and Phong Shading

While revisiting lectures and slides on phong shading and buffers, and using the existing `TransformationBuffer` as a reference, I added in a ton of codeblocks for my new buffers:

- `LightCameraBuffer`, to keep track of light and camera positions
    - `vec4f position`
- `MaterialBuffer`, to keep track of material (phong) components needed to make calculations using the Phong Illumination Model
    - `vec4f ambient`
    - `vec4f diffuse`
    - `vec4f specular`

$$ I = k_a + \sum^{lights}{( k_d(L \cdot N) + k_s(R\cdot V)^\alpha )} $$

Where $k_a$ is the **ambient** color component, $k_d(L\cdot N)$ is the **diffuse** color component, and $k_s(R\cdot V)^\alpha$ is the **specular** color component. $N$ is the surface **normal**, $L$ is a **light vector**, $R$ is the light vector ($L$) reflected in the normal ($N$), $V$ is the view vector, and $\alpha$ is the shininess.

I did my best to copy the structure of the existing `TransformationBuffer` for my new buffers. I also defined a `vec4f m_point_light` in `scene.h`.

**Defined** in `scene.h`

```cpp
// CBuffer for transformation matrices
ID3D11Buffer* m_transformation_buffer = nullptr;
// + other CBuffers
ID3D11Buffer* m_light_camera_buffer = nullptr;
ID3D11Buffer* m_material_buffer = nullptr;
```

**Bound** in Render()

```cpp
void OurTestScene::Render()
{
    // Bind transformation_buffer to slot b0 of the VS
    m_dxdevice_context->VSSetConstantBuffers(0, 1, &m_transformation_buffer);
    m_dxdevice_context->PSSetConstantBuffers(0, 1, &m_light_camera_buffer);
    m_dxdevice_context->PSSetConstantBuffers(1, 1, &m_material_buffer);

//...
```

And of course **released** in `OurTestScene::Release()`

```cpp
SAFE_RELEASE(m_transformation_buffer);
// + release other CBuffers
SAFE_RELEASE(m_light_camera_buffer);
SAFE_RELEASE(m_material_buffer);
```

### Structs in `buffers.h`

```cpp
/**
 * @brief Contains transformation matrices.
*/
struct TransformationBuffer
{
	linalg::mat4f ModelToWorldMatrix; //!< Matrix for converting from object space to world space.
	linalg::mat4f WorldToViewMatrix; //!< Matrix for converting from world space to view space.
	linalg::mat4f ProjectionMatrix; //!< Matrix for converting from view space to clip cpace.
};

struct LightCamBuffer
{
	vec4f light_position;
	vec4f camera_position;
};

struct MaterialBuffer
{
	vec4f ambient;
	vec4f diffuse;
	vec4f specular;
};
```

### Matching `cbuffer`s in `pixel_shader.hlsl`

Not sure how correct this is 🤷

```cpp
cbuffer LightCamBuffer : register(b0)
{
    float4 lightPosition;
    float4 cameraPosition;
};

cbuffer MaterialBuffer : register(b1)
{
    float4 ambient;
    float4 diffuse;
    float4 specular;
};
```

### Initialization Methods

I don't actually know what these do or how correct these are. I essentially copied the code from `TransfromationBuffer` completely.

```cpp
void OurTestScene::InitTransformationBuffer()
{
	HRESULT hr;
	D3D11_BUFFER_DESC matrixBufferDesc = { 0 };
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(TransformationBuffer);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;
	ASSERT(hr = m_dxdevice->CreateBuffer(&matrixBufferDesc, nullptr, &m_transformation_buffer));
}

void OurTestScene::InitLightCameraBuffer()
{
	HRESULT hr;
	D3D11_BUFFER_DESC lightCameraBuffer = { 0 };
	lightCameraBuffer.Usage = D3D11_USAGE_DYNAMIC;
	lightCameraBuffer.ByteWidth = sizeof(LightCamBuffer);
	lightCameraBuffer.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightCameraBuffer.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	lightCameraBuffer.MiscFlags = 0;
	lightCameraBuffer.StructureByteStride = 0;
	ASSERT(hr = m_dxdevice->CreateBuffer(&lightCameraBuffer, nullptr, &m_light_camera_buffer));
}

void OurTestScene::InitMaterialBuffer()
{
	HRESULT hr;
	D3D11_BUFFER_DESC materialBuffer = { 0 };
	materialBuffer.Usage = D3D11_USAGE_DYNAMIC;
	materialBuffer.ByteWidth = sizeof(MaterialBuffer);
	materialBuffer.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	materialBuffer.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	materialBuffer.MiscFlags = 0;
	materialBuffer.StructureByteStride = 0;
	ASSERT(hr = m_dxdevice->CreateBuffer(&materialBuffer, nullptr, &m_material_buffer));
}
```

### Update Methods

```cpp
void OurTestScene::UpdateTransformationBuffer(
	mat4f ModelToWorldMatrix,
	mat4f WorldToViewMatrix,
	mat4f ProjectionMatrix)
{
	// Map the resource buffer, obtain a pointer and then write our matrices to it
	D3D11_MAPPED_SUBRESOURCE resource;
	m_dxdevice_context->Map(m_transformation_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
	TransformationBuffer* matrixBuffer = (TransformationBuffer*)resource.pData;
	matrixBuffer->ModelToWorldMatrix = ModelToWorldMatrix;
	matrixBuffer->WorldToViewMatrix = WorldToViewMatrix;
	matrixBuffer->ProjectionMatrix = ProjectionMatrix;
	m_dxdevice_context->Unmap(m_transformation_buffer, 0);
}

void OurTestScene::UpdateLightCameraBuffer(
	vec4f lightPosition, 
	vec4f cameraPosition)
{
	D3D11_MAPPED_SUBRESOURCE resource;
	m_dxdevice_context->Map(m_light_camera_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
	LightCamBuffer* lightCameraBuffer = (LightCamBuffer*)resource.pData;
	lightCameraBuffer->light_position = lightPosition;
	lightCameraBuffer->camera_position = cameraPosition;
	m_dxdevice_context->Unmap(m_light_camera_buffer, 0);
}

void OurTestScene::UpdateMaterialBuffer(
	vec4f ambient,
	vec4f diffuse,
	vec4f specular)
{
	D3D11_MAPPED_SUBRESOURCE resource;
	m_dxdevice_context->Map(m_material_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
	MaterialBuffer* materialBuffer = (MaterialBuffer*)resource.pData;
	materialBuffer->ambient = ambient;
	materialBuffer->diffuse = diffuse;
	materialBuffer->specular = specular;
	m_dxdevice_context->Unmap(m_material_buffer, 0);
}
```

...
