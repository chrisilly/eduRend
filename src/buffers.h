/**
 * @file buffers.h
 * @brief Contains constant buffers
*/

#pragma once

#include "vec/mat.h"

/**
 * @brief Contains transformation matrices.
*/
struct TransformationBuffer
{
	linalg::mat4f ModelToWorldMatrix; //!< Matrix for converting from object space to world space.
	linalg::mat4f WorldToViewMatrix; //!< Matrix for converting from world space to view space.
	linalg::mat4f ProjectionMatrix; //!< Matrix for converting from view space to clip cpace.
};

struct /*alignas(16)*/ LightCamBuffer
{
	//alignas(16) vec3 v1;

	vec4f light_position;
	vec4f camera_position;
};

struct MaterialBuffer
{
	vec4f ambient;
	vec4f diffuse;
	vec4f specular;
	float shininess;
	vec3f padding;
};