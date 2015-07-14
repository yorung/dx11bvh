#include "stdafx.h"

#ifdef __d3d11_h__
ID3D11Buffer* afCreateIndexBuffer(const AFIndex* indi, int numIndi)
{
	ID3D11Buffer* indexBuffer;
	D3D11_SUBRESOURCE_DATA subresData = { indi, 0, 0 };
	deviceMan11.GetDevice()->CreateBuffer(&CD3D11_BUFFER_DESC(numIndi * sizeof(AFIndex), D3D11_BIND_INDEX_BUFFER), &subresData, &indexBuffer);
	return indexBuffer;
}

ID3D11Buffer* afCreateVertexBuffer(int size, const void* data)
{
	ID3D11Buffer* vbo;
	D3D11_SUBRESOURCE_DATA subresData = { data, 0, 0 };
	deviceMan11.GetDevice()->CreateBuffer(&CD3D11_BUFFER_DESC(size, D3D11_BIND_VERTEX_BUFFER), &subresData, &vbo);
	return vbo;
}

ID3D11Buffer* afCreateDynamicVertexBuffer(int size)
{
	ID3D11Buffer* vbo;
	deviceMan11.GetDevice()->CreateBuffer(&CD3D11_BUFFER_DESC(size, D3D11_BIND_VERTEX_BUFFER, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE), nullptr, &vbo);
	return vbo;
}

UBOID afCreateUBO(int size)
{
	ID3D11Buffer* ubo;
	deviceMan11.GetDevice()->CreateBuffer(&CD3D11_BUFFER_DESC(size, D3D11_BIND_CONSTANT_BUFFER, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE), nullptr, &ubo);
	return ubo;
}

SAMPLERID afCreateSampler()
{
	D3D11_SAMPLER_DESC  desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	desc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	desc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	desc.MaxAnisotropy = 1;
	desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	desc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	desc.MaxLOD = D3D11_FLOAT32_MAX;
	ID3D11SamplerState* sampler;
	deviceMan11.GetDevice()->CreateSamplerState(&desc, &sampler);
	return sampler;
}

void afBindBufferToBindingPoint(UBOID ubo, UINT uniformBlockBinding)
{
	deviceMan11.GetContext()->VSSetConstantBuffers(uniformBlockBinding, 1, &ubo);
	deviceMan11.GetContext()->PSSetConstantBuffers(uniformBlockBinding, 1, &ubo);
}

void afBindTextureToBindingPoint(TexMan::TMID tex, UINT textureBindingPoint)
{
	ID3D11ShaderResourceView* tx = texMan.Get(tex);
	deviceMan11.GetContext()->VSSetShaderResources(textureBindingPoint, 1, &tx);
	deviceMan11.GetContext()->PSSetShaderResources(textureBindingPoint, 1, &tx);
}

void afBindSamplerToBindingPoint(ID3D11SamplerState* sampler, UINT textureBindingPoint)
{
	deviceMan11.GetContext()->VSSetSamplers(textureBindingPoint, 1, &sampler);
	deviceMan11.GetContext()->PSSetSamplers(textureBindingPoint, 1, &sampler);
}

void afWriteBuffer(ID3D11Buffer* p, const void* buf, int size)
{
#ifdef _DEBUG
	D3D11_BUFFER_DESC desc;
	p->GetDesc(&desc);
	if (size > (int)desc.ByteWidth) {
		return;
	}
#endif
	D3D11_MAPPED_SUBRESOURCE m;
	HRESULT hr = deviceMan11.GetContext()->Map(p, 0, D3D11_MAP_WRITE_DISCARD, 0, &m);
	memcpy(m.pData, buf, size);
	deviceMan11.GetContext()->Unmap(p, 0);
}


IBOID afCreateQuadListIndexBuffer(int numQuads)
{
	std::vector<AFIndex> indi;
	int numIndi = numQuads * 6;
	indi.resize(numIndi);
	for (int i = 0; i < numIndi; i++)
	{
		static int tbl[] = { 0, 1, 2, 1, 3, 2 };
		int rectIdx = i / 6;
		int vertIdx = i % 6;
		indi[i] = rectIdx * 4 + tbl[vertIdx];
	}
	return afCreateIndexBuffer(&indi[0], numIndi);
}

void afDrawIndexedTriangleList(int numIndices, int start)
{
	deviceMan11.GetContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	deviceMan11.GetContext()->DrawIndexed(numIndices, start, 0);
}

void afEnableBackFaceCulling(bool cullBack)
{
	ID3D11RasterizerState* rs;
	CD3D11_RASTERIZER_DESC rasterDesc(D3D11_DEFAULT);
	rasterDesc.CullMode = cullBack ? D3D11_CULL_BACK : D3D11_CULL_NONE;
	rasterDesc.FrontCounterClockwise = TRUE;
	deviceMan11.GetDevice()->CreateRasterizerState(&rasterDesc, &rs);
	deviceMan11.GetContext()->RSSetState(rs);
	SAFE_RELEASE(rs);
}

void afBlendMode(BlendMode mode)
{
	if (mode == BM_NONE) {
		deviceMan11.GetContext()->OMSetBlendState(nullptr, nullptr, 0xffffffff);
		return;
	}

	CD3D11_BLEND_DESC bdesc(D3D11_DEFAULT);
	bdesc.RenderTarget[0].BlendEnable = TRUE;
	bdesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	bdesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	bdesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	bdesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	bdesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	bdesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	bdesc.RenderTarget[0].RenderTargetWriteMask = D3D10_COLOR_WRITE_ENABLE_ALL;
	ID3D11BlendState* bs;
	deviceMan11.GetDevice()->CreateBlendState(&bdesc, &bs);
	FLOAT factor[] = {0, 0, 0, 0};
	deviceMan11.GetContext()->OMSetBlendState(bs, factor, 0xffffffff);
	SAFE_RELEASE(bs);
}

void afDepthStencilMode(bool depth)
{
	ID3D11DepthStencilState* ds;
	D3D11_DEPTH_STENCIL_DESC dsDesc = CD3D11_DEPTH_STENCIL_DESC(CD3D11_DEFAULT());
	dsDesc.DepthEnable = depth;
	dsDesc.StencilEnable = FALSE;
	deviceMan11.GetDevice()->CreateDepthStencilState(&dsDesc, &ds);
	deviceMan11.GetContext()->OMSetDepthStencilState(ds, 1);
	SAFE_RELEASE(ds);
}

VAOID afCreateVAO(ShaderMan::SMID program, const InputElement elements[], int numElements, int numBuffers, VBOID const* vertexBufferIds, const int* strides, IBOID ibo)
{
	return new FakeVAO(program, elements, numElements, numBuffers, vertexBufferIds, strides, nullptr, ibo);
}

#endif