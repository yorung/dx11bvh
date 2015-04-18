#include <stdafx.h>

ComputeShaderMan computeShaderMan;

BufferMan::BMID ComputeShaderMan::constantBufferId = -1;

static void Compile(const char* name, ID3DBlob*& blob)
{
	blob = nullptr;
	ID3DBlob* err = 0;
	WCHAR wname[MAX_PATH];
	MultiByteToWideChar(CP_ACP, 0, name, -1, wname, dimof(wname));
	HRESULT hr = D3DCompileFromFile(wname, nullptr, nullptr, "mainCS", "cs_5_0", D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_PACK_MATRIX_COLUMN_MAJOR, 0, &blob, &err);
	if(err) {
		MessageBoxA(nullptr, (const char*)err->GetBufferPointer(), name, MB_OK | MB_ICONERROR);
		SAFE_RELEASE(err);
	}
}

ComputeShaderMan::ComputeShaderMan()
{
	computeShader = nullptr;
}

ComputeShaderMan::~ComputeShaderMan()
{
	assert(!computeShader);
}

void ComputeShaderMan::Create(const char *shader)
{
	Destroy();

	ID3DBlob* pBlobCS;
	Compile(shader, pBlobCS);
	HRESULT hr = deviceMan11.GetDevice()->CreateComputeShader(pBlobCS->GetBufferPointer(), pBlobCS->GetBufferSize(), nullptr, &computeShader);

	if (constantBufferId < 0)
	{
		constantBufferId = bufferMan.Create(sizeof(Mat));
	}
}

void ComputeShaderMan::Draw(ID3D11ShaderResourceView* shaderResourceView, ID3D11UnorderedAccessView* unorderedAccessView)
{
	Mat matV, matP;
	matrixMan.Get(MatrixMan::VIEW, matV);
	matrixMan.Get(MatrixMan::PROJ, matP);
	Mat invVP = inv(matV * matP);

	bufferMan.Write(constantBufferId, &invVP);
	auto buf = bufferMan.Get(constantBufferId);

	deviceMan11.GetContext()->CSSetShader(computeShader, nullptr, 0);
	deviceMan11.GetContext()->CSSetConstantBuffers(0, 1, &buf);
	deviceMan11.GetContext()->CSSetShaderResources(0, 1, &shaderResourceView);
	deviceMan11.GetContext()->CSSetUnorderedAccessViews(0, 1, &unorderedAccessView, nullptr);

	deviceMan11.GetContext()->Dispatch(SCR_W / 32, SCR_H / 8, 1);

	ID3D11UnorderedAccessView* nullView = nullptr;
	deviceMan11.GetContext()->CSSetUnorderedAccessViews(0, 1, &nullView, nullptr);
	ID3D11ShaderResourceView* nullTex = nullptr;
	deviceMan11.GetContext()->CSSetShaderResources(0, 1, &nullTex);
}

void ComputeShaderMan::Destroy()
{
	SAFE_RELEASE(computeShader);
}
