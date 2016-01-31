#include "stdafx.h"

ShaderMan11 shaderMan;

static void Compile(const char* name, bool ps, ID3DBlob*& blob)
{
	char path[MAX_PATH];
	sprintf_s(path, sizeof(path), "fx/%s.fx", name);

	blob = nullptr;
	ID3DBlob* err = 0;
	WCHAR wname[MAX_PATH];
	MultiByteToWideChar(CP_ACP, 0, path, -1, wname, dimof(wname));
	D3DCompileFromFile(wname, nullptr, nullptr, ps ? "mainPS" : "mainVS", ps ? "ps_5_0" : "vs_5_0", D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_PACK_MATRIX_COLUMN_MAJOR, 0, &blob, &err);
	if(err) {
		MessageBoxA(nullptr, (const char*)err->GetBufferPointer(), name, MB_OK | MB_ICONERROR);
		SAFE_RELEASE(err);
	}
}

ShaderMan11::ShaderMan11()
{
	m_effects.push_back(Effect());	// make ID 0 invalid
}

ShaderMan11::~ShaderMan11()
{
	Destroy();
}

ShaderMan11::SMID ShaderMan11::Create(const char *name, const D3D11_INPUT_ELEMENT_DESC elements[], int numElements, BlendMode blendMode, bool useDepthBuffer)
{
	auto it = m_nameToId.find(name);
	if (it != m_nameToId.end())
	{
		return it->second;
	}

	Effect effect;
	memset(&effect, 0, sizeof(effect));

	ID3DBlob* pBlobPS;
	Compile(name, false, effect.pBlobVS);
	Compile(name, true, pBlobPS);
	HRESULT hr = deviceMan11.GetDevice()->CreateVertexShader(effect.pBlobVS->GetBufferPointer(), effect.pBlobVS->GetBufferSize(), nullptr, &effect.pVertexShader);
	hr = deviceMan11.GetDevice()->CreatePixelShader(pBlobPS->GetBufferPointer(), pBlobPS->GetBufferSize(), nullptr, &effect.pPixelShader);

	if (elements) {
		hr = deviceMan11.GetDevice()->CreateInputLayout(elements, numElements, effect.pBlobVS->GetBufferPointer(), effect.pBlobVS->GetBufferSize(), &effect.pInputLayout);
	//	assert(!hr);
	}
	SAFE_RELEASE(pBlobPS);

	effect.elements = elements;
	effect.numElements = numElements;
	effect.blendMode = blendMode;
	effect.useDepthBuffer = useDepthBuffer;

	m_effects.push_back(effect);
	return m_nameToId[name] = m_effects.size() - 1;
}

void ShaderMan11::Destroy()
{
	for (auto it = m_effects.begin(); it != m_effects.end(); it++)
	{
		SAFE_RELEASE(it->pInputLayout);
		SAFE_RELEASE(it->pVertexShader);
		SAFE_RELEASE(it->pPixelShader);
		SAFE_RELEASE(it->pBlobVS);
	}
	m_effects.clear();
	m_nameToId.clear();
}

void ShaderMan11::Reload()
{
	std::vector<Effect> effs = m_effects;
	std::vector<std::string> names;

	for (SMID i = 0; i < (SMID)m_effects.size(); i++) {
		auto it = std::find_if(m_nameToId.begin(), m_nameToId.end(), [i](std::pair<std::string, SMID> v) { return v.second == i; } );
		assert(it != m_nameToId.end());
		names.push_back(it->first);
	}
	Destroy();
	for (int i = 0; i < (int)names.size(); i++) {
		auto& ef = effs[i];
		Create(names[i].c_str(), ef.elements, ef.numElements, ef.blendMode, ef.useDepthBuffer);
	}
}

void ShaderMan11::Apply(SMID id)
{
	if (id >= 0 && id < (SMID)m_effects.size())
	{
		Effect& it = m_effects[id];
		deviceMan11.GetContext()->IASetInputLayout(it.pInputLayout);
		deviceMan11.GetContext()->VSSetShader(it.pVertexShader, nullptr, 0);
		deviceMan11.GetContext()->PSSetShader(it.pPixelShader, nullptr, 0);
		afBlendMode(it.blendMode);
		afDepthStencilMode(it.useDepthBuffer ? DSM_DEPTH_LESS_WRITE : DSM_DISABLE);
	}
}

ID3DBlob* ShaderMan11::GetVSBlob(SMID id)
{
	if (id >= 0 && id < (SMID)m_effects.size())
	{
		Effect& it = m_effects[id];
		return it.pBlobVS;
	}
	return nullptr;
}

FakeVAO::FakeVAO(ShaderMan11::SMID shaderId, const D3D11_INPUT_ELEMENT_DESC elements[], int numElements, int numBuffers, VBOID* const vbos_, const int strides_[], const UINT offsets_[], IBOID ibo_)
{
	inputLayout = nullptr;

	ibo = ibo_;
	vbos.resize(numBuffers);
	d3dBuffers.resize(numBuffers);
	strides.resize(numBuffers);
	offsets.resize(numBuffers);
	for (int i = 0; i < numBuffers; i++) {
		vbos[i] = vbos_[i];
		d3dBuffers[i] = vbos[i].Get();
		strides[i] = (UINT)strides_[i];
		offsets[i] = offsets_ ? offsets_[i] : 0;
	}
	if (elements) {
		ID3DBlob* vsBlob = shaderMan.GetVSBlob(shaderId);
		deviceMan11.GetDevice()->CreateInputLayout(elements, numElements, vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), &inputLayout);
	}
}

void FakeVAO::Apply()
{
	deviceMan11.GetContext()->IASetInputLayout(inputLayout.Get());
	deviceMan11.GetContext()->IASetVertexBuffers(0, vbos.size(), &d3dBuffers[0], &strides[0], &offsets[0]);
	deviceMan11.GetContext()->IASetIndexBuffer(ibo.Get(), AFIndexTypeToDevice, 0);
}
