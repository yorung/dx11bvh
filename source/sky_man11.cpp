#include <stdafx.h>

SkyMan skyMan;

BufferMan::BMID SkyMan::constantBufferId = -1;

SkyMan::SkyMan()
{
	texId = TexMan::INVALID_TMID;
	shaderId = ShaderMan::INVALID_SMID;
	sampler = nullptr;
	depthStencilState = nullptr;
}

SkyMan::~SkyMan()
{
	assert(!sampler);
	assert(!depthStencilState);
}

void SkyMan::Create(const char *strCubeMapFile, const char *shader)
{
	Destroy();

	texId = texMan.Create(strCubeMapFile);
	shaderId = shaderMan.Create(shader, nullptr, 0);

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
	deviceMan11.GetContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	deviceMan11.GetContext()->Draw(4, 0);

	tx = nullptr;
	deviceMan11.GetContext()->PSSetShaderResources(0, 1, &tx);
}

void SkyMan::Destroy()
{
	SAFE_RELEASE(sampler);
	SAFE_RELEASE(depthStencilState);
}
