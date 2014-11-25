#include "stdafx.h"

BufferMan::BMID MeshRenderer11::constantBufferId = -1;

struct MeshConstantBuffer
{
	Mat matW;
	Mat matV;
	Mat matP;
	XMFLOAT4 faceColor;
	XMFLOAT4 emissive;
	Vec3 camPos;
	float padding1;
	XMFLOAT4 padding2;
	Mat bone[BONE_MAX];
};

MeshRenderer11::MeshRenderer11()
{
	pVertexBuffer = nullptr;
	skinBuffer = nullptr;
	pIndexBuffer = nullptr;
	pSamplerState = nullptr;
	pDSState = nullptr;
}

MeshRenderer11::~MeshRenderer11()
{
	Destroy();
}

void MeshRenderer11::Destroy()
{
	SAFE_RELEASE(pIndexBuffer);
	SAFE_RELEASE(pVertexBuffer);
	SAFE_RELEASE(skinBuffer);
	SAFE_RELEASE(pSamplerState);
	SAFE_RELEASE(pDSState);
}

void MeshRenderer11::Init(const Block& block)
{
	block.Verify();
	if (!block.vertices.empty() && !block.indices.empty()) {
		Init(block.vertices.size(), &block.vertices[0], &block.skin[0], block.indices.size(), &block.indices[0]);
	}
}

void MeshRenderer11::Init(int numVertices, const MeshVertex* vertices, const MeshSkin* skin, int numIndices, const unsigned* indices)
{
	Destroy();

	static D3D11_INPUT_ELEMENT_DESC layout[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R8G8B8A8_UNORM, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 28, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BLENDWEIGHTS", 0, DXGI_FORMAT_R32G32B32_FLOAT, 1, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BLENDINDICES", 0, DXGI_FORMAT_R8G8B8A8_UINT, 1, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	shaderId = shaderMan.Create("fx\\dx11mesh.fx", layout, dimof(layout));

	D3D11_SUBRESOURCE_DATA subresData = { vertices, 0, 0 };
	deviceMan11.GetDevice()->CreateBuffer(&CD3D11_BUFFER_DESC(numVertices * sizeof(MeshVertex), D3D11_BIND_VERTEX_BUFFER), &subresData, &pVertexBuffer);
	subresData.pSysMem = skin;
	deviceMan11.GetDevice()->CreateBuffer(&CD3D11_BUFFER_DESC(numVertices * sizeof(MeshSkin), D3D11_BIND_VERTEX_BUFFER), &subresData, &skinBuffer);
	subresData.pSysMem = indices;
	deviceMan11.GetDevice()->CreateBuffer(&CD3D11_BUFFER_DESC(numIndices * sizeof(unsigned), D3D11_BIND_INDEX_BUFFER), &subresData, &pIndexBuffer);
	if (constantBufferId < 0) {
		constantBufferId = bufferMan.Create(sizeof(MeshConstantBuffer));
	}

	CD3D11_SAMPLER_DESC descSamp(D3D11_DEFAULT);
	descSamp.AddressU = descSamp.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	deviceMan11.GetDevice()->CreateSamplerState(&descSamp, &pSamplerState);
	deviceMan11.GetDevice()->CreateDepthStencilState(&CD3D11_DEPTH_STENCIL_DESC(D3D11_DEFAULT), &pDSState);
}

void MeshRenderer11::Draw(const Mat BoneMatrices[BONE_MAX], int nBones, const Block& block) const
{
	deviceMan11.GetContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	shaderMan.Apply(shaderId);

	Mat matWorld, matView, matProj;
	matrixMan.Get(MatrixMan::WORLD, matWorld);
	matrixMan.Get(MatrixMan::VIEW, matView);
	matrixMan.Get(MatrixMan::PROJ, matProj);

	deviceMan11.GetContext()->OMSetDepthStencilState(pDSState, 1);
	deviceMan11.GetContext()->PSSetSamplers(0, 1, &pSamplerState);

	deviceMan11.GetContext()->IASetIndexBuffer(pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
	UINT strides[] = { sizeof(MeshVertex), sizeof(MeshSkin) };
	UINT offsets[] = { 0, 0 };
	ID3D11Buffer* vertexBuffers[] = { pVertexBuffer, skinBuffer };
	deviceMan11.GetContext()->IASetVertexBuffers(0, dimof(vertexBuffers), vertexBuffers, strides, offsets);

	for (int j = 0; (unsigned)j < block.materialMaps.size(); j++) {
		const MaterialMap& matMap = block.materialMaps[j];
		const Material* mat = matMan.Get(matMap.materialId);
		assert(mat);
		ID3D11ShaderResourceView* tx = texMan.Get(mat->tmid);
		deviceMan11.GetContext()->PSSetShaderResources(0, 1, &tx);

		MeshConstantBuffer cBuf;
		cBuf.matW = matWorld;
		cBuf.matV = matView;
		cBuf.matP = matProj;
		cBuf.faceColor = mat->faceColor;
		cBuf.emissive = mat->emissive;
		cBuf.camPos = fastInv(matView).GetRow(3);
		CopyMemory(cBuf.bone, BoneMatrices, BONE_MAX * sizeof(Mat));
		bufferMan.Write(constantBufferId, &cBuf);
		const auto buf = bufferMan.Get(constantBufferId);
		deviceMan11.GetContext()->VSSetConstantBuffers(0, 1, &buf);
		deviceMan11.GetContext()->PSSetConstantBuffers(0, 1, &buf);

		deviceMan11.GetContext()->DrawIndexed(matMap.faces * 3, matMap.faceStartIndex * 3, 0);
	}
}

