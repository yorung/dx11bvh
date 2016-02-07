#include <stdafx.h>

BufferMan::BMID PostEffectMan::constantBufferId = -1;

PostEffectMan::PostEffectMan()
{
	shaderId = ShaderMan::INVALID_SMID;
	sampler = nullptr;
	depthStencilState = nullptr;
}

PostEffectMan::~PostEffectMan()
{
	assert(!sampler);
	assert(!depthStencilState);
}

void PostEffectMan::Create(const char *shader)
{
	Destroy();

	shaderId = shaderMan.Create(shader, nullptr, 0, BM_NONE, DSM_DISABLE);

	if (constantBufferId < 0)
	{
		constantBufferId = bufferMan.Create(sizeof(Mat));
	}

	deviceMan11.GetDevice()->CreateSamplerState(&CD3D11_SAMPLER_DESC(D3D11_DEFAULT), &sampler);

	CD3D11_DEPTH_STENCIL_DESC desc(D3D11_DEFAULT);
	desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	deviceMan11.GetDevice()->CreateDepthStencilState(&desc, &depthStencilState);

}

void PostEffectMan::Draw(ID3D11ShaderResourceView* shaderResourceView)
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
	deviceMan11.GetContext()->PSSetShaderResources(0, 1, &shaderResourceView);

	deviceMan11.GetContext()->OMSetDepthStencilState(depthStencilState, 0);
	deviceMan11.GetContext()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	deviceMan11.GetContext()->Draw(4, 0);

	buf = nullptr;
	deviceMan11.GetContext()->VSSetConstantBuffers(0, 1, &buf);
	deviceMan11.GetContext()->PSSetConstantBuffers(0, 1, &buf);
	ID3D11ShaderResourceView* dummy = nullptr;
	deviceMan11.GetContext()->PSSetShaderResources(0, 1, &dummy);
}

void PostEffectMan::Destroy()
{
	SAFE_RELEASE(sampler);
	SAFE_RELEASE(depthStencilState);
}
