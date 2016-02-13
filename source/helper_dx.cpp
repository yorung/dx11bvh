#include "stdafx.h"

#ifdef __d3d11_h__
IBOID afCreateIndexBuffer(const AFIndex* indi, int numIndi)
{
	IBOID indexBuffer;
	D3D11_SUBRESOURCE_DATA subresData = { indi, 0, 0 };
	deviceMan11.GetDevice()->CreateBuffer(&CD3D11_BUFFER_DESC(numIndi * sizeof(AFIndex), D3D11_BIND_INDEX_BUFFER), &subresData, &indexBuffer);
	return indexBuffer;
}

VBOID afCreateVertexBuffer(int size, const void* data)
{
	VBOID vbo;
	D3D11_SUBRESOURCE_DATA subresData = { data, 0, 0 };
	deviceMan11.GetDevice()->CreateBuffer(&CD3D11_BUFFER_DESC(size, D3D11_BIND_VERTEX_BUFFER), &subresData, &vbo);
	return vbo;
}

VBOID afCreateDynamicVertexBuffer(int size)
{
	VBOID vbo;
	deviceMan11.GetDevice()->CreateBuffer(&CD3D11_BUFFER_DESC(size, D3D11_BIND_VERTEX_BUFFER, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE), nullptr, &vbo);
	return vbo;
}

UBOID afCreateUBO(int size)
{
	UBOID ubo;
	deviceMan11.GetDevice()->CreateBuffer(&CD3D11_BUFFER_DESC(size, D3D11_BIND_CONSTANT_BUFFER, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE), nullptr, &ubo);
	return ubo;
}

SRVID afCreateTexture2D(AFDTFormat format, const IVec2& size, void *image)
{
	CD3D11_TEXTURE2D_DESC desc(format, size.x, size.y, 1, 1, D3D11_BIND_SHADER_RESOURCE);
	D3D11_SUBRESOURCE_DATA r = { image, (uint32_t)size.x * 4, 0 };
	ComPtr<ID3D11Texture2D> tex;
	ComPtr<ID3D11ShaderResourceView> srv;
	deviceMan11.GetDevice()->CreateTexture2D(&desc, &r, &tex);
	deviceMan11.GetDevice()->CreateShaderResourceView(tex.Get(), nullptr, &srv);
	return srv;
}

SRVID afCreateTexture2D(AFDTFormat format, const TexDesc& afDesc, int mipCount, const AFTexSubresourceData datas[])
{
	ComPtr<ID3D11Texture2D> tex;
	CD3D11_TEXTURE2D_DESC desc(format, afDesc.size.x, afDesc.size.y, afDesc.arraySize, mipCount, D3D11_BIND_SHADER_RESOURCE, D3D11_USAGE_DEFAULT, 0, 1, 0, afDesc.isCubeMap ? D3D11_RESOURCE_MISC_TEXTURECUBE : 0);
	deviceMan11.GetDevice()->CreateTexture2D(&desc, datas, &tex);
	CD3D11_SHADER_RESOURCE_VIEW_DESC srvDesc(afDesc.isCubeMap ? D3D_SRV_DIMENSION_TEXTURECUBE : D3D_SRV_DIMENSION_TEXTURE2D, desc.Format, 0, -1);
	ComPtr<ID3D11ShaderResourceView> srv;
	deviceMan11.GetDevice()->CreateShaderResourceView(tex.Get(), &srvDesc, &srv);
	return srv;
}

SRVID afCreateDynamicTexture(AFDTFormat format, const IVec2& size)
{
	CD3D11_TEXTURE2D_DESC desc(format, size.x, size.y, 1, 1, D3D11_BIND_SHADER_RESOURCE, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE);
	ComPtr<ID3D11Texture2D> tex;
	ComPtr<ID3D11ShaderResourceView> srv;
	deviceMan11.GetDevice()->CreateTexture2D(&desc, nullptr, &tex);
	deviceMan11.GetDevice()->CreateShaderResourceView(tex.Get(), nullptr, &srv);
	return srv;
}

SAMPLERID afCreateSampler(SamplerFilter filter, SamplerWrap wrap)
{
	D3D11_SAMPLER_DESC  desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.AddressU = wrap;
	desc.AddressV = wrap;
	desc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	desc.MaxAnisotropy = 1;
	desc.Filter = filter;
	desc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	desc.MaxLOD = D3D11_FLOAT32_MAX;
	ComPtr<ID3D11SamplerState> sampler;
	deviceMan11.GetDevice()->CreateSamplerState(&desc, &sampler);
	return sampler;
}

void afBindBufferToBindingPoint(UBOID ubo, UINT uniformBlockBinding)
{
	deviceMan11.GetContext()->VSSetConstantBuffers(uniformBlockBinding, 1, ubo.GetAddressOf());
	deviceMan11.GetContext()->PSSetConstantBuffers(uniformBlockBinding, 1, ubo.GetAddressOf());
}

void afBindTextureToBindingPoint(SRVID srv, UINT textureBindingPoint)
{
	deviceMan11.GetContext()->PSSetShaderResources(textureBindingPoint, 1, srv.GetAddressOf());
}

void afBindSamplerToBindingPoint(SAMPLERID sampler, UINT textureBindingPoint)
{
	deviceMan11.GetContext()->PSSetSamplers(textureBindingPoint, 1, sampler.GetAddressOf());
}

void afWriteBuffer(const IBOID p, const void* buf, int size)
{
#ifdef _DEBUG
	D3D11_BUFFER_DESC desc;
	p->GetDesc(&desc);
	if (size > (int)desc.ByteWidth) {
		return;
	}
#endif
	D3D11_MAPPED_SUBRESOURCE m;
	HRESULT hr = deviceMan11.GetContext()->Map(p.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &m);
	memcpy(m.pData, buf, size);
	deviceMan11.GetContext()->Unmap(p.Get(), 0);
}

void afWriteTexture(SRVID srv, const TexDesc& desc, const void* buf)
{
	ComPtr<ID3D11Resource> res;
	srv->GetResource(&res);
	assert(res);
	ComPtr<ID3D11Texture2D> tx;
	res.As(&tx);
	assert(tx);

//	D3D11_TEXTURE2D_DESC desc;
//	tx->GetDesc(&desc);

	D3D11_MAPPED_SUBRESOURCE m;
	HRESULT hr = deviceMan11.GetContext()->Map(tx.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &m);
	memcpy(m.pData, buf, desc.size.x * desc.size.y * 4);
	deviceMan11.GetContext()->Unmap(tx.Get(), 0);
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

void afDrawIndexedTriangleStrip(int numIndices, int start)
{
	deviceMan11.GetContext()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	deviceMan11.GetContext()->DrawIndexed(numIndices, start, 0);
}

void afDrawIndexedTriangleList(int numIndices, int start)
{
	deviceMan11.GetContext()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	deviceMan11.GetContext()->DrawIndexed(numIndices, start, 0);
}

void afDrawTriangleStrip(int numVertices, int start)
{
	deviceMan11.GetContext()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	deviceMan11.GetContext()->Draw(numVertices, start);
}

void afDrawLineList(int numVertices, int start)
{
	deviceMan11.GetContext()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);
	deviceMan11.GetContext()->Draw(numVertices, start);
}

void afCullMode(CullMode cullMode)
{
	ID3D11RasterizerState* rs;
	CD3D11_RASTERIZER_DESC rasterDesc(D3D11_DEFAULT);
	switch (cullMode) {
	case CM_DISABLE:
		rasterDesc.CullMode = D3D11_CULL_NONE;
		break;
	case CM_CCW:
		rasterDesc.CullMode = D3D11_CULL_BACK;
		rasterDesc.FrontCounterClockwise = TRUE;
		break;
	case CM_CW:
		rasterDesc.CullMode = D3D11_CULL_BACK;
		rasterDesc.FrontCounterClockwise = FALSE;
		break;
	}
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

void afDepthStencilMode(DepthStencilMode mode)
{
	ComPtr<ID3D11DepthStencilState> ds;
	D3D11_DEPTH_STENCIL_DESC desc = CD3D11_DEPTH_STENCIL_DESC(CD3D11_DEFAULT());
	switch(mode) {
	case DSM_DISABLE:
		desc.DepthEnable = FALSE;
		break;
	case DSM_DEPTH_ENABLE:
		break;	// same as default
	case DSM_DEPTH_CLOSEREQUAL_READONLY:
		desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
		desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
		break;
	}
	deviceMan11.GetDevice()->CreateDepthStencilState(&desc, &ds);
	deviceMan11.GetContext()->OMSetDepthStencilState(ds.Get(), 1);
}

VAOID afCreateVAO(const InputElement elements[], int numElements, int numBuffers, VBOID* const vertexBufferIds, const int* strides, IBOID ibo)
{
	(void)elements;
	(void)numElements;
	return new FakeVAO(numBuffers, vertexBufferIds, strides, nullptr, ibo);
}

void AFRenderTarget::InitForDefaultRenderTarget()
{
	Destroy();
	renderTargetView = deviceMan11.GetDefaultRenderTarget();
	renderTargetView->AddRef();
	depthStencilView = deviceMan11.GetDefaultDepthStencil();
	depthStencilView->AddRef();
}

void AFRenderTarget::Init(IVec2 size, DXGI_FORMAT colorFormat)
{
	Destroy();
	texSize = size;
	CD3D11_TEXTURE2D_DESC tDesc(colorFormat, size.x, size.y, 1, 1, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS);
	ID3D11Texture2D* tex;
	HRESULT hr = deviceMan11.GetDevice()->CreateTexture2D(&tDesc, NULL, &tex);
	CD3D11_RENDER_TARGET_VIEW_DESC rDesc(D3D11_RTV_DIMENSION_TEXTURE2D, tDesc.Format);
	hr = deviceMan11.GetDevice()->CreateRenderTargetView(tex, &rDesc, &renderTargetView);
	CD3D11_SHADER_RESOURCE_VIEW_DESC sDesc(D3D11_SRV_DIMENSION_TEXTURE2D, tDesc.Format);
	hr = deviceMan11.GetDevice()->CreateShaderResourceView(tex, &sDesc, &shaderResourceView);
	CD3D11_UNORDERED_ACCESS_VIEW_DESC uDesc(D3D11_UAV_DIMENSION_TEXTURE2D, tDesc.Format);
	hr = deviceMan11.GetDevice()->CreateUnorderedAccessView(tex, &uDesc, &unorderedAccessView);
	SAFE_RELEASE(tex);

	depthStencilView = deviceMan11.GetDefaultDepthStencil();
	depthStencilView->AddRef();
}

void AFRenderTarget::Destroy()
{
	SAFE_RELEASE(renderTargetView);
	SAFE_RELEASE(shaderResourceView);
	SAFE_RELEASE(unorderedAccessView);
	SAFE_RELEASE(depthStencilView);
}

void AFRenderTarget::BeginRenderToThis()
{
	deviceMan11.GetContext()->OMSetRenderTargets(1, &renderTargetView, depthStencilView);
	float clearColor[4] = { 0.0f, 0.2f, 0.0f, 0.0f };
	deviceMan11.GetContext()->ClearRenderTargetView(renderTargetView, clearColor);
	deviceMan11.GetContext()->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}
#endif
