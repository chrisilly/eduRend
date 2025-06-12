/**
 * @file quadmodel.h
 * @brief Contains the Cube
 * @author Carl Johan Gribel 2016, cjgribel@gmail.com
*/

#pragma once
#include "Model.h"

/**
 * @brief Model reprsenting a Quad.
*/
class Cube : public Model
{
	unsigned m_number_of_indices = 0;

	void InitializeCube();

public:
	/**
	 * @brief Create a model of a quad.
	 * @param dxdevice Valid ID3D11Device.
	 * @param dxdevice_context Valid ID3D11DeviceContext.
	*/
	//Cube(ID3D11Device* dxdevice, ID3D11DeviceContext* dxdevice_context);

	Cube(ID3D11Device* dxdevice, ID3D11DeviceContext* dxdevice_context, Material material = DefaultMaterial, Transform transform = Transform());

	/**
	 * @brief Render the model.
	*/
	virtual void Render() const;

	/**
	 *@brief Destructor.
	*/
	~Cube() {}
};

