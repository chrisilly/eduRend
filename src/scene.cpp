#include <string>
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
	SetSampler(D3D11_FILTER_MIN_MAG_MIP_POINT, D3D11_TEXTURE_ADDRESS_MIRROR);

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
	m_cube = new Cube(m_dxdevice, m_dxdevice_context, DefaultMaterial, Transform(mat4f::translation(0, 0, 0), mat4f::rotation(0, 0, 0), mat4f::scaling(1.5f)));
	m_orbiterCube = new Cube(m_dxdevice, m_dxdevice_context, DefaultMaterial, Transform(mat4f::translation(0, 0, 0), mat4f::rotation(0, 0, 0), mat4f::scaling(1.0f)));
	m_orbiterCube2 = new Cube(m_dxdevice, m_dxdevice_context, DefaultMaterial, Transform(mat4f::translation(0, 0, 0), mat4f::rotation(0, 0, 0), mat4f::scaling(0.5f)));

	// Sponza model-to-world transformation
	m_sponza = new OBJModel("assets/crytek-sponza/sponza.obj", m_dxdevice, m_dxdevice_context, DefaultMaterial, Transform(mat4f::translation(0, -5, 0), mat4f::rotation(fPI / 2, 0.0f, 1.0f, 0.0f), mat4f::scaling(0.05f)));

	const std::string spherePath = "assets/sphere/sphere.obj";
	m_sphere = new OBJModel(spherePath, m_dxdevice, m_dxdevice_context, DefaultMaterial, Transform());
	m_sphere2 = new OBJModel(spherePath, m_dxdevice, m_dxdevice_context, DefaultMaterial, Transform(mat4f::translation(0, 5, 0), mat4f::rotation(0, 0, 0), mat4f::scaling(0.5f)));

	m_lightCube = new Cube(m_dxdevice, m_dxdevice_context, DefaultMaterial, Transform(mat4f::translation(m_point_light.x,m_point_light.y,m_point_light.z), mat4f::rotation(0, 0, 0), mat4f::scaling(0.3f)));

	models = { m_sponza, m_sphere, m_sphere2, m_lightCube, m_cube, m_orbiterCube, m_orbiterCube2 };	//, m_lightCube, m_cube, m_orbiterCube, m_orbiterCube2
}

//
// Called every frame
// dt (seconds) is time elapsed since the previous frame
//
void OurTestScene::Update(
	float dt,
	const InputHandler& input_handler)
{
	UpdateInput(input_handler, dt);

	std::cout << "light position: " << m_point_light << std::endl;

	// I think the X and Y are supposed to be swapped here? (yaw = Y and pitch = X). I think maybe the camera WorldToMatrix transform calculation is wrong, or my rotate methods are wrong
	// NO! BECAUSE IT'S ABOUT *ROTATION* AROUND THE AXES, NOT MOVEMENT *IN* THE AXES. (Rotation around X-axis = movement on the Y axis, i.e. looking up and down)
	m_camera->Rotate(input_handler.GetMouseInputX(), input_handler.GetMouseInputY());

	// Now set/update object transformations
	// This can be done using any sequence of transformation matrices,
	// but the T*R*S order is most common; i.e. scale, then rotate, and then translate.
	// If no transformation is desired, an identity matrix can be obtained 
	// via e.g. Mquad = linalg::mat4f_identity; 

	m_lightCube->transform = mat4f::translation(m_point_light.x, m_point_light.y, m_point_light.z) * mat4f::scaling(0.3f);

	// Quad model-to-world transformation
	m_quad_transform = mat4f::translation(0, 0, 0) *			// No translation
		mat4f::rotation(-m_angle, 0.0f, 1.0f, 0.0f) *	// Rotate continuously around the y-axis
		mat4f::scaling(1.5f);				// Scale uniformly to 150%

	m_cube->transform = mat4f::translation(0, 0, 0) *			// No translation
		mat4f::rotation(-m_angle, 0.0f, 1.0f, 0.0f) *	// Rotate continuously around the y-axis
		mat4f::scaling(1.5f);				// Scale uniformly to 150%
	
	m_orbiterCube->transform = mat4f::translation(4, 0, 0) *			// No translation
		mat4f::rotation(-m_angle, 0.0f, 1.0f, 0.0f) *	// Rotate continuously around the y-axis
		mat4f::scaling(1.0f);				// Scale uniformly to 150%
	
	m_orbiterCube->transform = m_cube->transform.TRS * m_orbiterCube->transform.TRS;
	
	m_orbiterCube2->transform = mat4f::translation(-1.5, 0, 0) *			// No translation
		mat4f::rotation(-m_angle, 0.0f, 1.0f, 0.0f) *	// Rotate continuously around the y-axis
		mat4f::scaling(0.5f);				// Scale uniformly to 150%
	
	m_orbiterCube2->transform = m_orbiterCube->transform.TRS * m_orbiterCube2->transform.TRS;

	// nvm, the camera doesn't have a transform, wtf??
	// I was gonna make the camera a child of m_orbiterCube2 and make it go weeeeee

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
	m_dxdevice_context->PSSetSamplers(0, 1, &sampler);

	// Obtain the matrices needed for rendering from the camera
	m_view_matrix = m_camera->WorldToViewMatrix();
	m_projection_matrix = m_camera->ProjectionMatrix();
	m_viewToWorld_matrix = m_camera->ViewToWorldMatrix();

	UpdateLightCameraBuffer(m_point_light, m_camera->GetPosition().xyz0());
	UpdateMaterialBuffer(PastelRedMaterial);

	for (auto& model : models)
	{
		vec4f modelAmbient =	{ model->material.AmbientColour.x, model->material.AmbientColour.y, model->material.AmbientColour.z, 0 };
		vec4f modelDiffuse =	{ model->material.DiffuseColour.x, model->material.DiffuseColour.y, model->material.DiffuseColour.z, 0 };
		vec4f modelSpecular =	{ model->material.SpecularColour.x, model->material.SpecularColour.y, model->material.SpecularColour.z, 0.05 };
		UpdateTransformationBuffer(model->transform.TRS, m_view_matrix, m_projection_matrix);
		UpdateMaterialBuffer(model->material);
		model->Render();
	}
}

void OurTestScene::Release()
{
	SAFE_DELETE(m_quad);
	SAFE_DELETE(m_camera);

	for (auto& model : models)
	{
		SAFE_DELETE(model);
	}

	SAFE_RELEASE(m_transformation_buffer);
	// + release other CBuffers
	SAFE_RELEASE(m_light_camera_buffer);
	SAFE_RELEASE(m_material_buffer);
	SAFE_RELEASE(sampler);
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
	materialBufferData.ambient = toVec4f(PastelRedMaterial.AmbientColour); // dark red
	materialBufferData.diffuse = toVec4f(PastelRedMaterial.DiffuseColour); // red
	materialBufferData.specular = toVec4f(PastelRedMaterial.SpecularColour); // pastel red
	materialBufferData.shininess = DefaultMaterial.shininess;

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

void OurTestScene::SetSampler(D3D11_FILTER filter, D3D11_TEXTURE_ADDRESS_MODE textureAddressMode)
{
	D3D11_SAMPLER_DESC samplerDesc =
	{
		filter,
		textureAddressMode,
		textureAddressMode,
		textureAddressMode,
		0.0f,
		16,
		D3D11_COMPARISON_NEVER,
		{1.0f, 1.0f, 1.0f, 1.0f},
		-FLT_MAX,
		FLT_MAX,
	};
	m_dxdevice->CreateSamplerState(&samplerDesc, &sampler);
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

void OurTestScene::UpdateMaterialBuffer(Material material)
{
	D3D11_MAPPED_SUBRESOURCE resource;
	m_dxdevice_context->Map(m_material_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
	MaterialBuffer* materialBuffer = (MaterialBuffer*)resource.pData;
	materialBuffer->ambient = toVec4f(material.AmbientColour);
	materialBuffer->diffuse = toVec4f(material.DiffuseColour);
	materialBuffer->specular = toVec4f(material.SpecularColour);
	materialBuffer->shininess = material.shininess;
	m_dxdevice_context->Unmap(m_material_buffer, 0);
}

void OurTestScene::UpdateInput(const InputHandler& input, float dt)
{
	if (input.IsKeyPressed(Keys::Up))
		UpdateLightPosition(input, dt);
	else if (input.IsKeyPressed(Keys::Down))
		UpdateColour(input);
	else if (input.IsKeyPressed(Keys::Left))
		UpdateBrightness(input);
	else
		UpdateCameraPosition(input);
}

void OurTestScene::UpdateCameraPosition(const InputHandler& input)
{
	// Basic camera control
	if (input.IsKeyPressed(Keys::W))
		//m_camera->Move({ 0.0f, 0.0f, -m_camera_velocity * dt });
		m_camera->MoveForward();
	if (input.IsKeyPressed(Keys::S))
		//m_camera->Move({ 0.0f, 0.0f, m_camera_velocity * dt });
		m_camera->MoveBack();
	if (input.IsKeyPressed(Keys::D))
		//m_camera->Move({ m_camera_velocity * dt, 0.0f, 0.0f });
		m_camera->MoveRight();
	if (input.IsKeyPressed(Keys::A))
		//m_camera->Move({ -m_camera_velocity * dt, 0.0f, 0.0f });
		m_camera->MoveLeft();
}

void OurTestScene::UpdateLightPosition(const InputHandler& input, float dt)
{
	if (input.IsKeyPressed(Keys::W))
		MoveLight({ 0.0f, m_camera_velocity * dt, 0.0f, 0 });
	if (input.IsKeyPressed(Keys::S))
		MoveLight({ 0.0f, -m_camera_velocity * dt, 0.0f, 0 });
	if (input.IsKeyPressed(Keys::D))
		MoveLight({ 0.0f, 0.0f, -m_camera_velocity * dt, 0 });
	if (input.IsKeyPressed(Keys::A))
		MoveLight({ 0.0f, 0.0f, m_camera_velocity * dt, 0 });
	if (input.IsKeyPressed(Keys::Left))
		MoveLight({ -m_camera_velocity * dt, 0.0f, 0.0f, 0 });
	if (input.IsKeyPressed(Keys::Right))
		MoveLight({ +m_camera_velocity * dt, 0.0f, 0.0f, 0 });
}

void OurTestScene::UpdateBrightness(const InputHandler& input)
{
	vec3f increment = vec3f(0.01f, 0.01f, 0.01f);
	for (auto& model : models)
	{
		// SHININESS
		if(input.IsKeyPressed(Keys::A))
			model->material.shininess -= 0.1f;
		if(input.IsKeyPressed(Keys::D))
			model->material.shininess += 0.1f;

		// MATERIAL BRIGHTNESS
		if (input.IsKeyPressed(Keys::W))
		{
			model->material.AmbientColour += increment;
			model->material.DiffuseColour += increment;
			model->material.SpecularColour += increment;
		}
		if (input.IsKeyPressed(Keys::S))
		{
			model->material.AmbientColour -= increment;
			model->material.DiffuseColour -= increment;
			model->material.SpecularColour -= increment;
		}
	}
}

void OurTestScene::UpdateColour(const InputHandler& input)
{
	float increment = 0.01f;

	for (auto& model : models)
	{
		// RED
		if (input.IsKeyPressed(Keys::W))
		{
			model->material.AmbientColour.x += increment;
			model->material.DiffuseColour.x += increment;
		}
		if (input.IsKeyPressed(Keys::S))
		{
			model->material.AmbientColour.x -= increment;
			model->material.DiffuseColour.x -= increment;
		}

		// GREEN
		if (input.IsKeyPressed(Keys::D))
		{
			model->material.AmbientColour.y += increment;
			model->material.DiffuseColour.y += increment;
		}
		if (input.IsKeyPressed(Keys::A))
		{
			model->material.AmbientColour.y -= increment;
			model->material.DiffuseColour.y -= increment;
		}

		// BLUE
		if (input.IsKeyPressed(Keys::Right))
		{
			model->material.AmbientColour.z += increment;
			model->material.DiffuseColour.z += increment;
		}
		if (input.IsKeyPressed(Keys::Left))
		{
			model->material.AmbientColour.z -= increment;
			model->material.DiffuseColour.z -= increment;
		}
	}
}

void OurTestScene::MoveLight(const vec4f& direction) noexcept
{
	m_point_light += direction;
}

vec4f OurTestScene::toVec4f(vec3f value)
{
	return { value.x, value.y, value.z, 0.0f };
}
