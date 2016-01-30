#include <stdafx.h>

SkyMan skyMan;

SkyMan::~SkyMan()
{
	assert(!sampler);
	assert(!depthStencilState);
	assert(!blendState);
	assert(!texId);
}

void SkyMan::Create(const char *texFileName, const char *shader)
{
	Destroy();

	texId = afLoadTexture(texFileName, texDesc);
	shaderId = shaderMan.Create(shader, nullptr, 0, BM_NONE, false);
	uboId = afCreateUBO(sizeof(Mat));

	deviceMan11.GetDevice()->CreateSamplerState(&CD3D11_SAMPLER_DESC(D3D11_DEFAULT), &sampler);
	CD3D11_DEPTH_STENCIL_DESC desc(D3D11_DEFAULT);
	desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	deviceMan11.GetDevice()->CreateDepthStencilState(&desc, &depthStencilState);
	CD3D11_BLEND_DESC bdesc(D3D11_DEFAULT);
	deviceMan11.GetDevice()->CreateBlendState(&bdesc, &blendState);
}

void SkyMan::Draw()
{
	if (!depthStencilState) {
		return;
	}

	shaderMan.Apply(shaderId);

	Mat matV, matP;
	matrixMan.Get(MatrixMan::VIEW, matV);
	matrixMan.Get(MatrixMan::PROJ, matP);
	matV._41 = matV._42 = matV._43 = 0;
	Mat invVP = inv(matV * matP);

	afWriteBuffer(uboId, &invVP, sizeof(invVP));
	afBindBufferToBindingPoint(uboId, 0);
	afBindTextureToBindingPoint(texId, 0);

	deviceMan11.GetContext()->PSSetSamplers(0, 1, &sampler);
	deviceMan11.GetContext()->OMSetDepthStencilState(depthStencilState, 0);
	deviceMan11.GetContext()->OMSetBlendState(blendState, nullptr, 0xffffffff);
	afDrawTriangleStrip(4);
	afBindTextureToBindingPoint(0, 0);
}

void SkyMan::Destroy()
{
	SAFE_RELEASE(sampler);
	SAFE_RELEASE(depthStencilState);
	SAFE_RELEASE(blendState);
	afSafeDeleteBuffer(uboId);
	afSafeDeleteTexture(texId);
}
