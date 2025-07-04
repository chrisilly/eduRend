/**
 * @file scene.h
 * @brief Contains scene related classes
*/

#pragma once
#ifndef SCENE_H
#define SCENE_H

#include "stdafx.h"
#include "InputHandler.h"
#include "Camera.h"
#include "Model.h"
#include "Texture.h"
#include "buffers.h"

/**
 * @brief Abstract class defining scene rendering and updating.
*/
class Scene
{
public:
	/**
	 * @brief Setup for member variables, no initialization is done here.
	 * @note These params are saved in the scene so they must be valid for as long as the scene is.
	 * @param[in] dxdevice ID3D11Device that will be used in the scene.
	 * @param[in] dxdevice_context ID3D11DeviceContext that will be used in the scene.
	 * @param[in] window_width Window hight for the scene.
	 * @param[in] window_height Window width for the scene.
	*/
	Scene(ID3D11Device* dxdevice, ID3D11DeviceContext* dxdevice_context, int window_width, int window_height);

	/**
	 * @brief Initialize all scene data.
	*/
	virtual void Init() = 0;

	/**
	 * @brief Relese all scene data created in Init()
	*/
	virtual void Release() = 0;

	/**
	 * @brief Update any relevant scene data.
	 * @param[in] delta_time Seconds since the last call.
	 * @param[in] input_handler Reference to the current InputHandler.
	*/
	virtual void Update(float delta_time, const InputHandler& input_handler) = 0;
	
	/**
	 * @brief Render the scene.
	*/
	virtual void Render() = 0;
	

	/**
	 * @brief Method called whenever the Window has changed size.
	 * @param[in] window_width New window width.
	 * @param[in] window_height New window height.
	*/
	virtual void OnWindowResized(int window_width,	int window_height);

protected:
	ID3D11Device*			m_dxdevice; //!< Graphics device, use for creating resources.
	ID3D11DeviceContext*	m_dxdevice_context; //!< Graphics context, use for binding resources and draw commands.
	int						m_window_width; //!< Current width of the window.
	int						m_window_height; //!< Current height of the window.
	std::list<Model*>		models;
};

/**
 * @brief Test scene used in the project.
*/
class OurTestScene : public Scene
{
	bool displayNormalMaps = false;

	//
	// Constant buffers (CBuffers) for data that is sent to shaders
	//

	// CBuffer for transformation matrices
	ID3D11Buffer* m_transformation_buffer = nullptr;
	// + other CBuffers
	ID3D11Buffer* m_light_camera_buffer = nullptr;
	ID3D11Buffer* m_material_buffer = nullptr;
	// Sampler state
	ID3D11SamplerState* sampler;

	//
	// Scene content
	//
	Camera* m_camera;
	vec4f m_camera_position;

	vec4f m_point_light;

	Model* m_lightCube;
	Model* m_quad;
	Model* m_cube;
	Model* m_orbiterCube;
	Model* m_orbiterCube2;
	Model* m_sponza;
	Model* m_sphere;
	Model* m_sphere2;

	mat4f m_quad_transform;
	mat4f m_cube_transform;
	mat4f m_orbiterCube_transform;
	mat4f m_orbiterCube2_transform;
	mat4f m_lightCube_transform;

	mat4f m_view_matrix;
	mat4f m_projection_matrix;
	mat4f m_viewToWorld_matrix;

	// Misc
	float m_angle = 0;			// A per-frame updated rotation angle (radians)...
	float m_angular_velocity = fPI;	// ...and its velocity (radians/sec)
	float m_camera_velocity = 5.0f;	// Camera movement velocity in units/s
	float m_fps_cooldown = 0;

	void InitTransformationBuffer();
	void InitLightCameraBuffer();
	void InitMaterialBuffer();
	void SetSampler(D3D11_FILTER filter, D3D11_TEXTURE_ADDRESS_MODE textureAddressMode);

	void UpdateTransformationBuffer(mat4f model_to_world_matrix, mat4f world_to_view_matrix, mat4f projection_matrix);
	void UpdateLightCameraBuffer(vec4f light_position, vec4f camera_position);
	void UpdateMaterialBuffer(Material material);

public:
	/**
	 * @brief Constructor
	 * @param dxdevice Valid ID3D11Device.
	 * @param dxdevice_context Valid ID3D11DeviceContext.
	 * @param window_width Current window width.
	 * @param window_height Current window height.
	*/
	OurTestScene(ID3D11Device* dxdevice, ID3D11DeviceContext* dxdevice_context,	int window_width, int window_height);

	/**
	 * @brief Initializes all resources held by the scene.
	*/
	void Init() override;

	/**
	 * @brief Updates all ojects in the scene
	 * @param dt Time in seconds since last iteration
	 * @param input_handler Current InputHandler
	*/
	void Update(float dt, const InputHandler& input_handler) override;

	/**
	 * @brief Renders all objects in the scene
	*/
	void Render() override;

	/**
	 * @brief Releases all resources created by the scene.
	*/
	void Release() override;

	/**
	 * @brief Updates all scene data that relates to Window size
	 * @param window_width New width
	 * @param window_height New height
	*/
	void OnWindowResized(int window_width, int window_height) override;

	void MoveLight(const vec4f& direction) noexcept;

	void UpdateCameraPosition(const InputHandler& input);
	void UpdateLightPosition(const InputHandler& input, float dt);
	void UpdateInput(const InputHandler& input, float dt);
	void UpdateBrightness(const InputHandler& input);
	void UpdateColour(const InputHandler& input);
};

#endif