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
};

MeshRenderer11::MeshRenderer11()
{
	posBuffer = nullptr;
	colorBuffer = nullptr;
	skinBuffer = nullptr;
	skinnedPosBuffer = nullptr;
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
	SAFE_RELEASE(posBuffer);
	SAFE_RELEASE(colorBuffer);
	SAFE_RELEASE(skinBuffer);
	SAFE_RELEASE(skinnedPosBuffer);
	SAFE_RELEASE(pSamplerState);
	SAFE_RELEASE(pDSState);
}

void MeshRenderer11::Init(const Block& block)
{
	block.Verify();
	if (!block.vertices.empty() && !block.indices.empty() && !block.color.empty()) {
		Init(block.vertices.size(), &block.vertices[0], &block.color[0], &block.skin[0], block.indices.size(), &block.indices[0]);
	}
}

void MeshRenderer11::Init(int numVertices, const MeshVertex* vertices, const MeshColor* color, const MeshSkin* skin, int numIndices, const unsigned* indices)
{
	Destroy();

	static D3D11_INPUT_ELEMENT_DESC layout[] = {
		{ "COLOR", 0, DXGI_FORMAT_R8G8B8A8_UNORM, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 4, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	shaderId = shaderMan.Create("fx\\dx11mesh.fx", layout, dimof(layout));

	deviceMan11.GetDevice()->CreateBuffer(&CD3D11_BUFFER_DESC(numVertices * sizeof(MeshVertex), D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS, D3D11_USAGE_DEFAULT, 0, D3D11_RESOURCE_MISC_BUFFER_STRUCTURED, sizeof(MeshVertex)), nullptr, &skinnedPosBuffer);
	D3D11_SUBRESOURCE_DATA subresData = { vertices, 0, 0 };
	deviceMan11.GetDevice()->CreateBuffer(&CD3D11_BUFFER_DESC(numVertices * sizeof(MeshVertex), D3D11_BIND_SHADER_RESOURCE, D3D11_USAGE_DEFAULT, 0, D3D11_RESOURCE_MISC_BUFFER_STRUCTURED, sizeof(MeshVertex)), &subresData, &posBuffer);
//	deviceMan11.GetDevice()->CreateBuffer(&CD3D11_BUFFER_DESC(numVertices * sizeof(MeshVertex), D3D11_BIND_VERTEX_BUFFER | D3D11_BIND_SHADER_RESOURCE, D3D11_USAGE_DEFAULT, 0, D3D11_RESOURCE_MISC_BUFFER_STRUCTURED, sizeof(MeshVertex)), &subresData, &posBuffer);
	subresData.pSysMem = skin;
//	deviceMan11.GetDevice()->CreateBuffer(&CD3D11_BUFFER_DESC(numVertices * sizeof(MeshSkin), D3D11_BIND_VERTEX_BUFFER), &subresData, &skinBuffer);
	HRESULT hr = deviceMan11.GetDevice()->CreateBuffer(&CD3D11_BUFFER_DESC(numVertices * sizeof(MeshSkin), D3D11_BIND_SHADER_RESOURCE, D3D11_USAGE_DEFAULT, 0, D3D11_RESOURCE_MISC_BUFFER_STRUCTURED, sizeof(MeshSkin)), &subresData, &skinBuffer);
	subresData.pSysMem = color;
	deviceMan11.GetDevice()->CreateBuffer(&CD3D11_BUFFER_DESC(numVertices * sizeof(MeshColor), D3D11_BIND_VERTEX_BUFFER), &subresData, &colorBuffer);
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

void MeshRenderer11::Calc(const Mat BoneMatrices[BONE_MAX], const Block& block) const
{
	ID3D11ShaderResourceView* srvPos;
	ID3D11ShaderResourceView* srvSkin;
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC desc;
		memset(&desc, 0, sizeof(desc));
		desc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
		desc.Format = DXGI_FORMAT_UNKNOWN;
		desc.BufferEx.FirstElement = 0;
		desc.BufferEx.NumElements = block.vertices.size();
		HRESULT hr = deviceMan11.GetDevice()->CreateShaderResourceView(posBuffer, &desc, &srvPos);
		hr = deviceMan11.GetDevice()->CreateShaderResourceView(skinBuffer, &desc, &srvSkin);
	}

	ID3D11UnorderedAccessView* uav;
	{
		D3D11_UNORDERED_ACCESS_VIEW_DESC desc;
		memset(&desc, 0, sizeof(desc));
		desc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
		desc.Format = DXGI_FORMAT_UNKNOWN;
		desc.Buffer.FirstElement = 0;
		desc.Buffer.NumElements = block.vertices.size();
		HRESULT hr = deviceMan11.GetDevice()->CreateUnorderedAccessView(skinnedPosBuffer, &desc, &uav);
	}

	computeShaderSkinning.Dispatch(BoneMatrices, srvPos, srvSkin, uav, block.vertices.size());
	SAFE_RELEASE(srvPos);
	SAFE_RELEASE(srvSkin);
	SAFE_RELEASE(uav);
}

void MeshRenderer11::Draw(const Mat BoneMatrices[BONE_MAX], int nBones, const Block& block) const
{
	Calc(BoneMatrices, block);

	deviceMan11.GetContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	shaderMan.Apply(shaderId);

	Mat matWorld, matView, matProj;
	matrixMan.Get(MatrixMan::WORLD, matWorld);
	matrixMan.Get(MatrixMan::VIEW, matView);
	matrixMan.Get(MatrixMan::PROJ, matProj);

	deviceMan11.GetContext()->OMSetDepthStencilState(pDSState, 1);
	deviceMan11.GetContext()->PSSetSamplers(0, 1, &pSamplerState);

	deviceMan11.GetContext()->IASetIndexBuffer(pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
	UINT strides[] = { sizeof(MeshColor) };
	UINT offsets[] = { 0 };
	ID3D11Buffer* vertexBuffers[] = { colorBuffer };
	deviceMan11.GetContext()->IASetVertexBuffers(0, dimof(vertexBuffers), vertexBuffers, strides, offsets);

	ID3D11ShaderResourceView* srvSkinnedPos;
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC desc;
		memset(&desc, 0, sizeof(desc));
		desc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
		desc.Format = DXGI_FORMAT_UNKNOWN;
		desc.BufferEx.FirstElement = 0;
		desc.BufferEx.NumElements = block.vertices.size();
		HRESULT hr = deviceMan11.GetDevice()->CreateShaderResourceView(skinnedPosBuffer, &desc, &srvSkinnedPos);
	}
	deviceMan11.GetContext()->VSSetShaderResources(0, 1, &srvSkinnedPos);
	SAFE_RELEASE(srvSkinnedPos);

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
		bufferMan.Write(constantBufferId, &cBuf);
		const auto buf = bufferMan.Get(constantBufferId);
		deviceMan11.GetContext()->VSSetConstantBuffers(0, 1, &buf);
		deviceMan11.GetContext()->PSSetConstantBuffers(0, 1, &buf);

		deviceMan11.GetContext()->DrawIndexed(matMap.faces * 3, matMap.faceStartIndex * 3, 0);
	}
}

