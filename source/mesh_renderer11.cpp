#include "stdafx.h"

struct MeshConstantBuffer
{
	Mat matW;
	Mat matV;
	Mat matP;
	Vec4 faceColor;
	Vec4 emissive;
	Vec3 camPos;
	float padding1;
};

MeshRenderer11::MeshRenderer11()
{
	posBuffer = nullptr;
	colorBuffer = nullptr;
	skinBuffer = nullptr;
	skinnedPosBuffer = nullptr;
	iboId = nullptr;
	pSamplerState = nullptr;
	vao = 0;
}

MeshRenderer11::~MeshRenderer11()
{
	Destroy();
}

void MeshRenderer11::Destroy()
{
	SAFE_RELEASE(posBuffer);
	SAFE_RELEASE(skinBuffer);
	SAFE_RELEASE(skinnedPosBuffer);
	SAFE_RELEASE(pSamplerState);
	afSafeDeleteVAO(vao);
	afSafeDeleteBuffer(uboId);
	afSafeDeleteBuffer(iboId);
	afSafeDeleteBuffer(colorBuffer);
}

void MeshRenderer11::Init(const Block& block)
{
	block.Verify();
	if (!block.vertices.empty() && !block.indices.empty() && !block.color.empty()) {
		Init(block.vertices.size(), &block.vertices[0], &block.color[0], &block.skin[0], block.indices.size(), &block.indices[0]);
	}
}

void MeshRenderer11::Init(int numVertices, const MeshVertex* vertices, const MeshColor* color, const MeshSkin* skin, int numIndices, const AFIndex* indices)
{
	Destroy();

	static D3D11_INPUT_ELEMENT_DESC layout[] = {
		{ "COLOR", 0, DXGI_FORMAT_R8G8B8A8_UNORM, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 4, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	shaderId = shaderMan.Create("dx11mesh", layout, dimof(layout), BM_NONE, true);

	deviceMan11.GetDevice()->CreateBuffer(&CD3D11_BUFFER_DESC(numVertices * sizeof(MeshVertex), D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS, D3D11_USAGE_DEFAULT, 0, D3D11_RESOURCE_MISC_BUFFER_STRUCTURED, sizeof(MeshVertex)), nullptr, &skinnedPosBuffer);
	D3D11_SUBRESOURCE_DATA subresData = { vertices, 0, 0 };
	deviceMan11.GetDevice()->CreateBuffer(&CD3D11_BUFFER_DESC(numVertices * sizeof(MeshVertex), D3D11_BIND_SHADER_RESOURCE, D3D11_USAGE_DEFAULT, 0, D3D11_RESOURCE_MISC_BUFFER_STRUCTURED, sizeof(MeshVertex)), &subresData, &posBuffer);
	subresData.pSysMem = skin;
	HRESULT hr = deviceMan11.GetDevice()->CreateBuffer(&CD3D11_BUFFER_DESC(numVertices * sizeof(MeshSkin), D3D11_BIND_SHADER_RESOURCE, D3D11_USAGE_DEFAULT, 0, D3D11_RESOURCE_MISC_BUFFER_STRUCTURED, sizeof(MeshSkin)), &subresData, &skinBuffer);
	colorBuffer = afCreateVertexBuffer(numVertices * sizeof(MeshColor), color);
	iboId = afCreateIndexBuffer(indices, numIndices);
	uboId = afCreateUBO(sizeof(MeshConstantBuffer));

	int strides[] = {sizeof(MeshColor)};
	VBOID vbos[] = {colorBuffer};
	vao = afCreateVAO(shaderId, layout, dimof(layout), 1, vbos, strides, iboId);

	CD3D11_SAMPLER_DESC descSamp(D3D11_DEFAULT);
	descSamp.AddressU = descSamp.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	deviceMan11.GetDevice()->CreateSamplerState(&descSamp, &pSamplerState);
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

	shaderMan.Apply(shaderId);

	Mat matWorld, matView, matProj;
	matrixMan.Get(MatrixMan::WORLD, matWorld);
	matrixMan.Get(MatrixMan::VIEW, matView);
	matrixMan.Get(MatrixMan::PROJ, matProj);

	deviceMan11.GetContext()->PSSetSamplers(0, 1, &pSamplerState);
	afBindVAO(vao);

	{
		ComPtr<ID3D11ShaderResourceView> srvSkinnedPos;
		CD3D11_SHADER_RESOURCE_VIEW_DESC desc(D3D_SRV_DIMENSION_BUFFEREX, DXGI_FORMAT_UNKNOWN, 0, block.vertices.size());
		HRESULT hr = deviceMan11.GetDevice()->CreateShaderResourceView(skinnedPosBuffer, &desc, &srvSkinnedPos);
		deviceMan11.GetContext()->VSSetShaderResources(0, 1, srvSkinnedPos.GetAddressOf());
	}

	MeshConstantBuffer cBuf;
	cBuf.matW = matWorld;
	cBuf.matV = matView;
	cBuf.matP = matProj;
	cBuf.camPos = fastInv(matView).GetRow(3);
	for (int j = 0; (unsigned)j < block.materialMaps.size(); j++) {
		const MaterialMap& matMap = block.materialMaps[j];
		const Material* mat = matMan.Get(matMap.materialId);
		assert(mat);
		cBuf.faceColor = mat->faceColor;
		cBuf.emissive = mat->emissive;
		afWriteBuffer(uboId, &cBuf, sizeof(cBuf));
		afBindBufferToBindingPoint(uboId, 2);
		afBindTextureToBindingPoint(mat->tmid, 0);
		afDrawIndexedTriangleList(matMap.faces * 3, matMap.faceStartIndex * 3);
	}
	afBindTextureToBindingPoint(0, 0);
}
