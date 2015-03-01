#include "stdafx.h"

ShaderMan11 shaderMan;

static void Compile(const char* name, bool ps, ID3D10Blob*& blob)
{
	char path[MAX_PATH];
	sprintf_s(path, sizeof(path), "fx/%s.fx", name);

	blob = nullptr;
	ID3D10Blob* err = 0;
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

ShaderMan11::SMID ShaderMan11::Create(const char *name, const D3D11_INPUT_ELEMENT_DESC elements[], int numElements)
{
	auto it = m_nameToId.find(name);
	if (it != m_nameToId.end())
	{
		return it->second;
	}

	Effect effect;
	memset(&effect, 0, sizeof(effect));

	ID3D10Blob* pBlobVS;
	ID3D10Blob* pBlobPS;
	Compile(name, false, pBlobVS);
	Compile(name, true, pBlobPS);
	HRESULT hr = deviceMan11.GetDevice()->CreateVertexShader(pBlobVS->GetBufferPointer(), pBlobVS->GetBufferSize(), nullptr, &effect.pVertexShader);
	hr = deviceMan11.GetDevice()->CreatePixelShader(pBlobPS->GetBufferPointer(), pBlobPS->GetBufferSize(), nullptr, &effect.pPixelShader);

	if (elements) {
		hr = deviceMan11.GetDevice()->CreateInputLayout(elements, numElements, pBlobVS->GetBufferPointer(), pBlobVS->GetBufferSize(), &effect.pInputLayout);
	//	assert(!hr);
	}
	SAFE_RELEASE(pBlobVS);
	SAFE_RELEASE(pBlobPS);

	effect.elements = elements;
	effect.numElements = numElements;

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
		Create(names[i].c_str(), effs[i].elements, effs[i].numElements);
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
	}
}
