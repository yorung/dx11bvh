#include <stdafx.h>

ComputeShaderSkinning computeShaderSkinning;

BufferMan::BMID ComputeShaderSkinning::constantBufferId = -1;

struct BoneBuffer {
	Mat bone[BONE_MAX];
};

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

ComputeShaderSkinning::ComputeShaderSkinning()
{
	computeShader = nullptr;
}

ComputeShaderSkinning::~ComputeShaderSkinning()
{
	assert(!computeShader);
}

void ComputeShaderSkinning::Create(const char *shader)
{
	Destroy();

	ID3DBlob* pBlobCS;
	Compile(shader, pBlobCS);
	HRESULT hr = deviceMan11.GetDevice()->CreateComputeShader(pBlobCS->GetBufferPointer(), pBlobCS->GetBufferSize(), nullptr, &computeShader);

	if (constantBufferId < 0)
	{
		constantBufferId = bufferMan.Create(sizeof(BoneBuffer));
	}
}

void ComputeShaderSkinning::Dispatch(const Mat bone[BONE_MAX], ID3D11ShaderResourceView* srvPos, ID3D11ShaderResourceView* srvSkin, ID3D11UnorderedAccessView* unorderedAccessView, int nVert)
{
	bufferMan.Write(constantBufferId, bone);
	auto buf = bufferMan.Get(constantBufferId);

	ID3D11ShaderResourceView* srvList[] = {srvPos, srvSkin};
	deviceMan11.GetContext()->CSSetShader(computeShader, nullptr, 0);
	deviceMan11.GetContext()->CSSetConstantBuffers(0, 1, &buf);
	deviceMan11.GetContext()->CSSetShaderResources(0, 2, srvList);
	deviceMan11.GetContext()->CSSetUnorderedAccessViews(0, 1, &unorderedAccessView, nullptr);

	deviceMan11.GetContext()->Dispatch(nVert, 1, 1);

	ID3D11UnorderedAccessView* nullView = nullptr;
	deviceMan11.GetContext()->CSSetUnorderedAccessViews(0, 1, &nullView, nullptr);
	ID3D11ShaderResourceView* nullTex[] = {nullptr, nullptr};
	deviceMan11.GetContext()->CSSetShaderResources(0, 2, nullTex);
}

void ComputeShaderSkinning::Destroy()
{
	SAFE_RELEASE(computeShader);
}
