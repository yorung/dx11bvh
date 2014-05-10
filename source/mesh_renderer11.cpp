#include "stdafx.h"

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
	pIndexBuffer = nullptr;
	pConstantBuffer = nullptr;
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
	SAFE_RELEASE(pConstantBuffer);
	SAFE_RELEASE(pSamplerState);
	SAFE_RELEASE(pDSState);
}

void MeshRenderer11::Init(int sizeVertices, int sizeIndices, void* vertices, void* indices)
{
	Destroy();

	D3D11_INPUT_ELEMENT_DESC layout[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R8G8B8A8_UNORM, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 28, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BLENDWEIGHTS", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 36, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BLENDINDICES", 0, DXGI_FORMAT_R8G8B8A8_UINT, 0, 48, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	shaderId = shaderMan.Create("fx\\dx11mesh.fx", layout, dimof(layout));

	D3D11_SUBRESOURCE_DATA subresData = { vertices, 0, 0 };
	deviceMan11.GetDevice()->CreateBuffer(&CD3D11_BUFFER_DESC(sizeVertices, D3D11_BIND_VERTEX_BUFFER), &subresData, &pVertexBuffer);
	subresData.pSysMem = indices;
	deviceMan11.GetDevice()->CreateBuffer(&CD3D11_BUFFER_DESC(sizeIndices, D3D11_BIND_INDEX_BUFFER), &subresData, &pIndexBuffer);
	deviceMan11.GetDevice()->CreateBuffer(&CD3D11_BUFFER_DESC(sizeof(MeshConstantBuffer), D3D11_BIND_CONSTANT_BUFFER, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE), nullptr, &pConstantBuffer);

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

	UINT strides[] = { sizeof(MeshVertex) };
	UINT offsets[] = { 0 };
	deviceMan11.GetContext()->IASetIndexBuffer(pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
	deviceMan11.GetContext()->IASetVertexBuffers(0, 1, &pVertexBuffer, strides, offsets);

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
		D3D11_MAPPED_SUBRESOURCE mappedResource;
		HRESULT hr = deviceMan11.GetContext()->Map(pConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		CopyMemory(mappedResource.pData, &cBuf, sizeof(cBuf));
		deviceMan11.GetContext()->Unmap(pConstantBuffer, 0);
		deviceMan11.GetContext()->VSSetConstantBuffers(0, 1, &pConstantBuffer);
		deviceMan11.GetContext()->PSSetConstantBuffers(0, 1, &pConstantBuffer);

		deviceMan11.GetContext()->DrawIndexed(matMap.faces * 3, matMap.faceStartIndex * 3, 0);
	}
}

