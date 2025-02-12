#include "Cube.h"

Cube::Cube(
	ID3D11Device* dxdevice, 
	ID3D11DeviceContext* dxdevice_context) 
	: Model(dxdevice, dxdevice_context)
{
	std::vector<Vertex> vertices;
	std::vector<unsigned> indices;

	Vertex vert0, vert1, vert2, vert3,
		vert4, vert5, vert6, vert7,
		vert8, vert9, vert10, vert11;

	vert0.Position = { -0.5, -0.5f, 0.0f };
	vert0.Normal = { 0, 0, 1 };
	vert0.TexCoord = { 0, 0 };

	vert1.Position = { 0.5, -0.5f, 0.0f };
	vert1.Normal = { 0, 0, 1 };
	vert1.TexCoord = { 0, 1 };

	vert2.Position = { 0.5, 0.5f, 0.0f };
	vert2.Normal = { 0, 0, 1 };
	vert2.TexCoord = { 1, 1 };

	vert3.Position = { -0.5, 0.5f, 0.0f };
	vert3.Normal = { 0, 0, 1 };
	vert3.TexCoord = { 1, 0 };

	vert4.Position = { -0.5, -0.5f, 1 };
	vert4.Normal = { 0, 0, 1 };
	vert4.TexCoord = { 0, 0 };

	vert5.Position = { 0.5, -0.5f, 1 };
	vert5.Normal = { 0, 0, 1 };
	vert5.TexCoord = { 0, 1 };

	vert6.Position = { 0.5, 0.5f, 1 };
	vert6.Normal = { 0, 0, 1 };
	vert6.TexCoord = { 1, 1 };

	vert7.Position = { -0.5, 0.5f, 1 };
	vert7.Normal = { 0, 0, 1 };
	vert7.TexCoord = { 1, 0 };

	vert8.Position = { -0.5, 0.5f, 1 };
	vert8.Normal = { 0, 0, 1 };
	vert8.TexCoord = { 0, 0 };

	vert9.Position = { 0.5, 0.5f, 1 };
	vert9.Normal = { 0, 0, 1 };
	vert9.TexCoord = { 0, 1 };

	vert10.Position = { 0.5, 0.5f, 1 };
	vert10.Normal = { 0, 0, 1 };
	vert10.TexCoord = { 1, 1 };

	vert11.Position = { -0.5, 0.5f, 1 };
	vert11.Normal = { 0, 0, 1 };
	vert11.TexCoord = { 1, 0 };

	vertices.push_back(vert0);
	vertices.push_back(vert1);
	vertices.push_back(vert2);
	vertices.push_back(vert3);

	vertices.push_back(vert4);
	vertices.push_back(vert5);
	vertices.push_back(vert6);
	vertices.push_back(vert7);

	vertices.push_back(vert8);
	vertices.push_back(vert9);
	vertices.push_back(vert10);
	vertices.push_back(vert11);

	//Back triangles
	indices.push_back(0);
	indices.push_back(3);
	indices.push_back(1);

	indices.push_back(1);
	indices.push_back(3);
	indices.push_back(2);

	//Bottom Triangles
	indices.push_back(0);
	indices.push_back(1);
	indices.push_back(5);

	indices.push_back(0);
	indices.push_back(5);
	indices.push_back(4);

	//Top Triangles
	indices.push_back(3);
	indices.push_back(11);
	indices.push_back(10);

	indices.push_back(3);
	indices.push_back(10);
	indices.push_back(2);

	//Left triangles
	indices.push_back(0);
	indices.push_back(11);
	indices.push_back(3);

	indices.push_back(0);
	indices.push_back(4);
	indices.push_back(11);

	//Right triangles
	indices.push_back(2);
	indices.push_back(10);
	indices.push_back(1);

	indices.push_back(1);
	indices.push_back(10);
	indices.push_back(5);

	//Front triangles (based on starting position)
	indices.push_back(4);
	indices.push_back(5);
	indices.push_back(11);

	indices.push_back(11);
	indices.push_back(5);
	indices.push_back(10);

	// Vertex array descriptor
	D3D11_BUFFER_DESC vertexbufferDesc{ 0 };
	vertexbufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexbufferDesc.CPUAccessFlags = 0;
	vertexbufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexbufferDesc.MiscFlags = 0;
	vertexbufferDesc.ByteWidth = (UINT)(vertices.size() * sizeof(Vertex));
	// Data resource
	D3D11_SUBRESOURCE_DATA vertexData = { 0 };
	vertexData.pSysMem = &vertices[0];
	// Create vertex buffer on device using descriptor & data
	dxdevice->CreateBuffer(&vertexbufferDesc, &vertexData, &m_vertex_buffer);
	SETNAME(m_vertex_buffer, "VertexBuffer");

	//  Index array descriptor
	D3D11_BUFFER_DESC indexbufferDesc = { 0 };
	indexbufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexbufferDesc.CPUAccessFlags = 0;
	indexbufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexbufferDesc.MiscFlags = 0;
	indexbufferDesc.ByteWidth = (UINT)(indices.size() * sizeof(unsigned));
	// Data resource
	D3D11_SUBRESOURCE_DATA indexData{ 0 };
	indexData.pSysMem = &indices[0];
	// Create index buffer on device using descriptor & data
	dxdevice->CreateBuffer(&indexbufferDesc, &indexData, &m_index_buffer);
	SETNAME(m_index_buffer, "IndexBuffer");

	m_number_of_indices = (unsigned int)indices.size();
}


void Cube::Render() const
{
	// Bind our vertex buffer
	const UINT32 stride = sizeof(Vertex); //  sizeof(float) * 8;
	const UINT32 offset = 0;
	m_dxdevice_context->IASetVertexBuffers(0, 1, &m_vertex_buffer, &stride, &offset);

	// Bind our index buffer
	m_dxdevice_context->IASetIndexBuffer(m_index_buffer, DXGI_FORMAT_R32_UINT, 0);

	// Make the drawcall
	m_dxdevice_context->DrawIndexed(m_number_of_indices, 0, 0);
}