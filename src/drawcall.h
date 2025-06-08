/**
 * @file drawcall.h
 * @brief Contains Vertex, Material, and Drawcall definitions.
 * @author Carl Johan Gribel 2016-2021, cjgribel@gmail.com
*/

#pragma once
#ifndef MATERIAL_H
#define MATERIAL_H

#include <iostream>
#include <vector>
#include <unordered_map>
#include "stdafx.h"
#include "vec/vec.h"
#include "vec/mat.h"

#include "Texture.h"

using namespace linalg;

/**
 * @brief Structure defining a vertex
*/
struct Vertex
{
	vec3f Position; //!< 3D coordinate of the vertex
	vec3f Normal; //!< Normal of the vertex
	vec3f Tangent; //!< Tangent of the vertex
	vec3f Binormal; //!< Binormal of the vertex
	vec2f TexCoord; //!< 2D texture coordiante of the vertex
};

struct Transform
{
	mat4f TRS;
	mat4f translation;
	mat4f rotation;
	mat4f scaling;

public:
	Transform(mat4f translation = mat4f::translation(0, 0, 0), mat4f rotation = mat4f::rotation(0, 0, 0), mat4f scaling = mat4f::scaling(1.0f)) 
		: translation(translation), rotation(rotation), scaling(scaling), TRS(translation * rotation * scaling) {}
};

/**
 * @brief Phong-esque material
*/
struct Material
{
	vec3f AmbientColour; //!< Ambient colour component
	vec3f DiffuseColour; //!< Diffuse colour component
	vec3f SpecularColour; //!< Specular colour component
	float shininess;
    
	std::string Name; //!< Name of the Material

	// File paths to textures
	std::string DiffuseTextureFilename; //!< Diffuse texture path
	std::string NormalTextureFilename; //!< Normal texture path

	// + more texture types (extend OBJLoader::LoadMaterials if needed)

	// Device textures
	Texture DiffuseTexture; //!< Diffuse Texture
	// + other texture types

public:
	Material(vec3f ambientColour = { 0.0f, 0.5f, 0.0f }, vec3f diffuseColour = { 0.0f, 0.5f, 0.0f }, vec3f specularColour = { 1.0f, 1.0f, 1.0f }, float shininess = 10.0f) 
		: AmbientColour(ambientColour), DiffuseColour(diffuseColour), SpecularColour(specularColour), shininess(shininess) {}
};

/**
 * @brief Default material
*/
static Material DefaultMaterial = Material();
static Material PastelRedMaterial = Material({ 1.0f, 0.3921568627f, 0.3921568627f }, { 1.0f, 0.3921568627f, 0.3921568627f });
//static Material PinkMaterial = Material();
/**
 * @brief Map between Material using name as the key.
*/
typedef std::unordered_map<std::string, Material> MaterialHash;

/**
 * @brief Indices representing a triangle
*/
struct Triangle 
{ 
	unsigned VertexIndices[3]; //!< Indices of the triangle
};

/**
 * @brief Indices representing a quad
*/
struct Quad
{ 
	unsigned VertexIndices[4]; //!< Indices of the quad
};

/**
 * @brief Contains the data specifying a drawcall
*/
struct Drawcall
{
    std::string GroupName; //!< Name of the drawcall group
    int MaterialIndex = -1; //!< Index of the material used in the drawcall
    std::vector<Triangle> Triangles; //!< List of the Triangles in the drawcall
    std::vector<Quad> Quads; //!< List of the Quads in the drawcall
    
    /**
     * @brief Used for sorting Drawcalls based on material
    */
    bool operator < (const Drawcall& other) const
    {
        return MaterialIndex < other.MaterialIndex;
    }
};

#endif
