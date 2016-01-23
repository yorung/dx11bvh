#include <stdafx.h>

SkyMan skyMan;

BufferMan::BMID SkyMan::constantBufferId = -1;

SkyMan::SkyMan()
{
	texId = TexMan::INVALID_TMID;
	shaderId = ShaderMan::INVALID_SMID;
}

SkyMan::~SkyMan()
{
	assert(!sampler);
	assert(!depthStencilState);
	assert(!blendState);
}

void SkyMan::Create(const char *strCubeMapFile, const char *shader)
{
	Destroy();

	texId = texMan.Create(strCubeMapFile);
	shaderId = shaderMan.Create(shader, nullptr, 0, BM_NONE, false);

	if (constantBufferId < 0)
	{
		constantBufferId = bufferMan.Create(sizeof(Mat));
	}

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
	Mat invVP = inv(matV * matP);

	bufferMan.Write(constantBufferId, &invVP);
	auto buf = bufferMan.Get(constantBufferId);
	deviceMan11.GetContext()->VSSetConstantBuffers(0, 1, &buf);
	deviceMan11.GetContext()->PSSetConstantBuffers(0, 1, &buf);

	deviceMan11.GetContext()->PSSetSamplers(0, 1, &sampler);
	ComPtr<ID3D11ShaderResourceView> tx = texMan.Get(texId);
	deviceMan11.GetContext()->PSSetShaderResources(0, 1, tx.GetAddressOf());
	deviceMan11.GetContext()->OMSetDepthStencilState(depthStencilState, 0);
	deviceMan11.GetContext()->OMSetBlendState(blendState, nullptr, 0xffffffff);
	afDrawTriangleStrip(4);

	tx = nullptr;
	deviceMan11.GetContext()->PSSetShaderResources(0, 1, &tx);
}

void SkyMan::Destroy()
{
	SAFE_RELEASE(sampler);
	SAFE_RELEASE(depthStencilState);
	SAFE_RELEASE(blendState);
}
