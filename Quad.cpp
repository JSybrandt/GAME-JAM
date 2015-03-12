
#include "Quad.h"
#include "Vertex.h"
#include "constants.h"

Quad::Quad()
: mNumVertices(0), mNumFaces(0), md3dDevice(0), mVB(0), mIB(0)
{
	rotX = 0;
	rotY = 0;
	rotZ = 0;
	Identity(&world);
}
 
Quad::~Quad()
{
	ReleaseCOM(mVB);
	ReleaseCOM(mIB);
}
void Quad::init(ID3D10Device* device, float scale, D3DXCOLOR c)
{
	md3dDevice = device;
 
	mNumVertices = 4; //2 triangles per quad
	mNumFaces    = 2; 

	// Create vertex buffer

	 Vertex vertices[] =
    {
		{D3DXVECTOR3(-1.0f, 0.0f, 0.0f), c},
		{D3DXVECTOR3(1.0f, 0.0f, 0.0f), c},
		{D3DXVECTOR3(0.0f, 0.0f, -1.0f), c},
		{D3DXVECTOR3(0.0f, 0.0f, 1.0f), c}
    };

	// Scale the Quad.
	for(DWORD i = 0; i < mNumVertices; ++i)
		vertices[i].pos *= scale;
    D3D10_BUFFER_DESC vbd;
    vbd.Usage = D3D10_USAGE_IMMUTABLE;
    vbd.ByteWidth = sizeof(Vertex) * mNumVertices;
    vbd.BindFlags = D3D10_BIND_VERTEX_BUFFER;
    vbd.CPUAccessFlags = 0;
    vbd.MiscFlags = 0;
    D3D10_SUBRESOURCE_DATA vinitData;
    vinitData.pSysMem = vertices;
    HR(md3dDevice->CreateBuffer(&vbd, &vinitData, &mVB));

	//Index buffer
	DWORD indices[] = {
		// front face
		0, 1, 3,
		0, 2, 1

	};

	D3D10_BUFFER_DESC ibd;
    ibd.Usage = D3D10_USAGE_IMMUTABLE;
    ibd.ByteWidth = sizeof(DWORD) * mNumFaces*3;
    ibd.BindFlags = D3D10_BIND_INDEX_BUFFER;
    ibd.CPUAccessFlags = 0;
    ibd.MiscFlags = 0;
    D3D10_SUBRESOURCE_DATA iinitData;
    iinitData.pSysMem = indices;
    HR(md3dDevice->CreateBuffer(&ibd, &iinitData, &mIB));
}

void Quad::update(float dt)
{
	Matrix rotXM, rotYM, rotZM, transM;
	RotateX(&rotXM, rotX);
	RotateY(&rotYM, rotY);
	RotateZ(&rotZM, rotZ); 
	Translate(&transM, position.x, position.y, position.z);
	world = rotXM * rotYM * rotZM * transM;
}

void Quad::draw()
{
UINT stride = sizeof(Vertex);
    UINT offset = 0;
	md3dDevice->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
    md3dDevice->IASetVertexBuffers(0, 1, &mVB, &stride, &offset);
	md3dDevice->IASetIndexBuffer(mIB, DXGI_FORMAT_R32_UINT, 0);
	md3dDevice->DrawIndexed(mNumFaces*3, 0, 0);
}