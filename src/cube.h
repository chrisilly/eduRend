#pragma once
#include "Model.h"

// Needs 24 vertices, 4 per face (3 per corner)
// Define positions, normals, and texture coordinates~
// Make proper use of vertex and index arrays
class Cube : public Model
{
	unsigned m_number_of_indices = 0;

public:
	Cube(ID3D11Device* dxdevice, ID3D11DeviceContext* dxdevice_context);

	/**
	 *	@brief Render the model
	*/
	virtual void Render() const;

	~Cube() { }
};