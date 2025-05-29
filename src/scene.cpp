
#include "Scene.h"
#include "QuadModel.h"
#include "cube.h"
#include "OBJModel.h"

Scene::Scene(
	ID3D11Device* dxdevice,
	ID3D11DeviceContext* dxdevice_context,
	int window_width,
	int window_height) :
	m_dxdevice(dxdevice),
	m_dxdevice_context(dxdevice_context),
	m_window_width(window_width),
	m_window_height(window_height)
{ }

void Scene::OnWindowResized(
	int new_width,
	int new_height)
{
	m_window_width = new_width;
	m_window_height = new_height;
}

OurTestScene::OurTestScene(
	ID3D11Device* dxdevice,
	ID3D11DeviceContext* dxdevice_context,
	int window_width,
	int window_height) :
	Scene(dxdevice, dxdevice_context, window_width, window_height)
{ 
	InitTransformationBuffer();
	// + init other CBuffers
	InitLightCameraBuffer();
	InitMaterialBuffer();

	// Initialize point light
	m_point_light = { 0, 3, -3, 0 };
}

//
// Called once at initialization
//
void OurTestScene::Init()
{
	m_camera = new Camera(
		45.0f * fTO_RAD,		// field-of-view (radians)
		(float)m_window_width / m_window_height,	// aspect ratio
		1.0f,					// z-near plane (everything closer will be clipped/removed)
		500.0f);				// z-far plane (everything further will be clipped/removed)

	// Move camera to (0,0,5)
	m_camera->MoveTo({ 0, 0, 5 });

	// Create objects
	m_quad = new QuadModel(m_dxdevice, m_dxdevice_context);
	m_cube = new Cube(m_dxdevice, m_dxdevice_context);
	m_orbiterCube = new Cube(m_dxdevice, m_dxdevice_context);
	m_orbiterCube2 = new Cube(m_dxdevice, m_dxdevice_context);
	m_sponza = new OBJModel("assets/crytek-sponza/sponza.obj", m_dxdevice, m_dxdevice_context);
	m_sphere = new OBJModel("assets/sphere/sphere.obj", m_dxdevice, m_dxdevice_context);
	m_lightCube = new Cube(m_dxdevice, m_dxdevice_context);
}

//
// Called every frame
// dt (seconds) is time elapsed since the previous frame
//
void OurTestScene::Update(
	float dt,
	const InputHandler& input_handler)
{
	// Basic camera control
	if (input_handler.IsKeyPressed(Keys::W))
		//m_camera->Move({ 0.0f, 0.0f, -m_camera_velocity * dt });
		m_camera->MoveForward();
	if (input_handler.IsKeyPressed(Keys::S))
		//m_camera->Move({ 0.0f, 0.0f, m_camera_velocity * dt });
		m_camera->MoveBack();
	if (input_handler.IsKeyPressed(Keys::D))
		//m_camera->Move({ m_camera_velocity * dt, 0.0f, 0.0f });
		m_camera->MoveRight();
	if (input_handler.IsKeyPressed(Keys::A))
		//m_camera->Move({ -m_camera_velocity * dt, 0.0f, 0.0f });
		m_camera->MoveLeft();
	if (input_handler.IsKeyPressed(Keys::Up))
		MoveLight({ 0.0f, m_camera_velocity * dt, 0.0f, 0 });
	if(input_handler.IsKeyPressed(Keys::Down))
		MoveLight({ 0.0f, -m_camera_velocity * dt, 0.0f, 0 });
	if(input_handler.IsKeyPressed(Keys::Right))
		MoveLight({ 0.0f, 0.0f, -m_camera_velocity * dt, 0 });
	if(input_handler.IsKeyPressed(Keys::Left))
		MoveLight({ 0.0f, 0.0f, m_camera_velocity * dt, 0 });

	std::cout << "light position: " << m_point_light << std::endl;

	#pragma region LAB 1 CAMERA SOLUTION DEPRECATED
	//long mousedx = input_handler.GetMouseDeltaX();
	//long mousedy = input_handler.GetMouseDeltaY();
	//float sensitivity = (1 / 300.0f);
	//
	//m_camera->RotateX(mousedx * sensitivity);
	//m_camera->RotateY(mousedy * sensitivity);
	#pragma endregion

	// I think the X and Y are supposed to be swapped here? (yaw = Y and pitch = X). I think maybe the camera WorldToMatrix transform calculation is wrong, or my rotate methods are wrong
	// NO! BECAUSE IT'S ABOUT *ROTATION* AROUND THE AXES, NOT MOVEMENT *IN* THE AXES. (Rotation around X-axis = movement on the Y axis, i.e. looking up and down)
	m_camera->Rotate(input_handler.GetMouseInputX(), input_handler.GetMouseInputY());

	// Now set/update object transformations
	// This can be done using any sequence of transformation matrices,
	// but the T*R*S order is most common; i.e. scale, then rotate, and then translate.
	// If no transformation is desired, an identity matrix can be obtained 
	// via e.g. Mquad = linalg::mat4f_identity; 

	m_lightCube_transform = mat4f::translation(m_point_light.x, m_point_light.y, m_point_light.z) *
		mat4f::scaling(0.3, 0.3, 0.3);

	// Quad model-to-world transformation
	m_quad_transform = mat4f::translation(0, 0, 0) *			// No translation
		mat4f::rotation(-m_angle, 0.0f, 1.0f, 0.0f) *	// Rotate continuously around the y-axis
		mat4f::scaling(1.5, 1.5, 1.5);				// Scale uniformly to 150%

	m_cube_transform = mat4f::translation(0, 0, 0) *			// No translation
		mat4f::rotation(-m_angle, 0.0f, 1.0f, 0.0f) *	// Rotate continuously around the y-axis
		mat4f::scaling(1.5, 1.5, 1.5);				// Scale uniformly to 150%

	m_orbiterCube_transform = mat4f::translation(4, 0, 0) *			// No translation
		mat4f::rotation(-m_angle, 0.0f, 1.0f, 0.0f) *	// Rotate continuously around the y-axis
		mat4f::scaling(1, 1, 1);				// Scale uniformly to 150%

	m_orbiterCube_transform = m_cube_transform * m_orbiterCube_transform;

	m_orbiterCube2_transform = mat4f::translation(-1.5, 0, 0) *			// No translation
		mat4f::rotation(-m_angle, 0.0f, 1.0f, 0.0f) *	// Rotate continuously around the y-axis
		mat4f::scaling(0.5, 0.5, 0.5);				// Scale uniformly to 150%

	m_orbiterCube2_transform = m_orbiterCube_transform * m_orbiterCube2_transform;

	// nvm, the camera doesn't have a transform, wtf??
	// I was gonna make the camera a child of m_orbiterCube2 and make it go weeeeee

	// Sponza model-to-world transformation
	m_sponza_transform = mat4f::translation(0, -5, 0) *		 // Move down 5 units
		mat4f::rotation(fPI / 2, 0.0f, 1.0f, 0.0f) * // Rotate pi/2 radians (90 degrees) around y
		mat4f::scaling(0.05f);						 // The scene is quite large so scale it down to 5%

	m_sphere_transform = mat4f::translation(0, 5, 0) *
		mat4f::scaling(3.0f);

	// Increment the rotation angle.
	m_angle += m_angular_velocity * dt;

	// Print fps
	m_fps_cooldown -= dt;
	if (m_fps_cooldown < 0.0)
	{
		std::cout << "fps " << (int)(1.0f / dt) << std::endl;
//		printf("fps %i\n", (int)(1.0f / dt));
		m_fps_cooldown = 2.0;
	}
}

//
// Called every frame, after update
//
void OurTestScene::Render()
{
	// Bind transformation_buffer to slot b0 of the VS
	m_dxdevice_context->VSSetConstantBuffers(0, 1, &m_transformation_buffer);
	m_dxdevice_context->PSSetConstantBuffers(0, 1, &m_light_camera_buffer);
	m_dxdevice_context->PSSetConstantBuffers(1, 1, &m_material_buffer);

	// Obtain the matrices needed for rendering from the camera
	m_view_matrix = m_camera->WorldToViewMatrix();
	m_projection_matrix = m_camera->ProjectionMatrix();
	m_viewToWorld_matrix = m_camera->ViewToWorldMatrix();

	UpdateLightCameraBuffer(m_point_light, m_camera->GetPosition().xyz0());
	//UpdateMaterialBuffer();

	m_lightCube_transform.translation(m_point_light.x, m_point_light.y, m_point_light.z);
	UpdateTransformationBuffer(m_lightCube_transform, m_view_matrix, m_projection_matrix);
	m_lightCube->Render();

	// Load matrices + the Quad's transformation to the device and render it
	UpdateTransformationBuffer(m_quad_transform, m_view_matrix, m_projection_matrix);
	//m_quad->Render();

	UpdateTransformationBuffer(m_cube_transform, m_view_matrix, m_projection_matrix);
	m_cube->Render();

	UpdateTransformationBuffer(m_orbiterCube_transform, m_view_matrix, m_projection_matrix);
	m_orbiterCube->Render();

	UpdateTransformationBuffer(m_orbiterCube2_transform, m_view_matrix, m_projection_matrix);
	m_orbiterCube2->Render();

	// Load matrices + Sponza's transformation to the device and render it
	UpdateTransformationBuffer(m_sponza_transform, m_view_matrix, m_projection_matrix);
	m_sponza->Render();

	UpdateTransformationBuffer(m_sphere_transform, m_view_matrix, m_projection_matrix);
	UpdateTransformationBuffer(m_sphere->transform.TRS, m_view_matrix, m_projection_matrix);
	m_sphere->Render();
}

void OurTestScene::Release()
{
	SAFE_DELETE(m_quad);
	SAFE_DELETE(m_cube);
	SAFE_DELETE(m_orbiterCube);
	SAFE_DELETE(m_orbiterCube2);
	SAFE_DELETE(m_sponza);
	SAFE_DELETE(m_camera);

	SAFE_RELEASE(m_transformation_buffer);
	// + release other CBuffers
	SAFE_RELEASE(m_light_camera_buffer);
	SAFE_RELEASE(m_material_buffer);
}

void OurTestScene::OnWindowResized(
	int new_width,
	int new_height)
{
	if (m_camera)
		m_camera->SetAspect(float(new_width) / new_height);

	Scene::OnWindowResized(new_width, new_height);
}

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

	LightCamBuffer lightCameraBufferData;
	lightCameraBufferData.camera_position = 0.0f;
	lightCameraBufferData.light_position = m_point_light;

	D3D11_BUFFER_DESC lightCameraBuffer = { 0 };
	lightCameraBuffer.Usage = D3D11_USAGE_DYNAMIC;
	lightCameraBuffer.ByteWidth = sizeof(LightCamBuffer);
	lightCameraBuffer.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightCameraBuffer.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	lightCameraBuffer.MiscFlags = 0;
	lightCameraBuffer.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA InitData;
	InitData.pSysMem = &lightCameraBufferData;
	InitData.SysMemPitch = 0;
	InitData.SysMemSlicePitch = 0;

	ASSERT(hr = m_dxdevice->CreateBuffer(&lightCameraBuffer, &InitData, &m_light_camera_buffer));
}

void OurTestScene::InitMaterialBuffer()
{
	HRESULT hr;

	// Let's set our default data to start
	MaterialBuffer materialBufferData;
	//materialBufferData.ambient = { 0, 0, 0, 1 }; //black
	materialBufferData.ambient = { 0.1960784314, 0.0, 0.0, 0 }; // dark red
	//materialBufferData.ambient = { 0.05, 0.168, 0.082, 1 }; //green
	//materialBufferData.diffuse = { 0.168, 0.050, 0.050, 0 }; // red
	materialBufferData.diffuse = { 1, 0, 0, 0 }; // red
	//materialBufferData.diffuse = {0.05, 0.168, 0.082, 1}; // green
	//materialBufferData.diffuse = {0.05, 0.05, 0.168, 1}; // blue
	//materialBufferData.specular = 0.05f;
	//materialBufferData.specular = { 1, 0, 0, 0 }; // red
	materialBufferData.specular = {1, 0.3921568627, 0.3921568627, 0.05}; // pastel red

	D3D11_BUFFER_DESC materialBuffer = { 0 };
	materialBuffer.ByteWidth = sizeof(MaterialBuffer);
	materialBuffer.Usage = D3D11_USAGE_DYNAMIC;
	materialBuffer.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	materialBuffer.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	materialBuffer.MiscFlags = 0;
	materialBuffer.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA InitData;
	InitData.pSysMem = &materialBufferData;
	InitData.SysMemPitch = 0;
	InitData.SysMemSlicePitch = 0;

	ASSERT(hr = m_dxdevice->CreateBuffer(&materialBuffer, &InitData, &m_material_buffer));
}

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

void OurTestScene::MoveLight(const vec4f& direction) noexcept
{
	m_point_light += direction;
}
