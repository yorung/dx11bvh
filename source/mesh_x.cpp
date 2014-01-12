#include "stdafx.h"

void *LoadFile(const char *fileName)
{
	bool result = false;
	FILE *f = fopen(fileName, "rb");
	int _size;
	void *ptr = NULL;

	if (!f) {
		return nullptr;
	}

	if (fseek(f, 0, SEEK_END)) {
		goto DONE;
	}
	_size = ftell(f);
	if (_size < 0) {
		goto DONE;
	}
	if (fseek(f, 0, SEEK_SET)) {
		goto DONE;
	}
	ptr = calloc(_size + 1, 1);
	if (!ptr) {
		goto DONE;
	}
	if (_size > 0) {
		if (!fread(ptr, _size, 1, f)) {
			goto DONE;
		}
	}
	result = true;
DONE:
	if (f) {
		fclose(f);
	}
	if (result){
		return ptr;
	} else {
		if (ptr) {
			free(ptr);
		}
		return nullptr;
	}
}

static void _enterBrace(char*& p)
{
	if (!p) {
		return;
	}

	p = strchr(p, '{');
	if (p) {
		p++;
	}
}

static char* _leaveBrace(char* p)
{
	if (!p) {
		return nullptr;
	}

	int depth = 0;
	while (p && *p) {
		if (*p == '{') {
			depth++;
		} else if (*p == '}') {
			if (--depth < 0) {
				p++;
				return p;
			}
		}
		p++;
	}
	return nullptr;
}

static double _getD(char*& p)
{
	if (!p)	{
		return 0;
	}

	while (true) {
		if (strchr("0123456789-", *p))
		{
			break;
		}
		p++;
	}

	double result = atof(p);

	while (*p) {
		if (!strchr("0123456789-.", *p)) {
			break;
		}
		p++;
	}
	return result;
}

static int _getI(char*& p)
{
	return (int)_getD(p);
}

static float _getF(char*& p)
{
	return (float)_getD(p);
}

static void _getFloat3Array(char*& p, std::vector<XMFLOAT3>& vertices, int nVertices)
{
	for (int i = 0; i < nVertices; i++) {
		XMFLOAT3 f3;
		f3.x = _getF(p);
		f3.y = _getF(p);
		f3.z = _getF(p);
		vertices.push_back(f3);
	}
}

static void _getFloat2Array(char*& p, std::vector<XMFLOAT2>& vertices, int nVertices)
{
	for (int i = 0; i < nVertices; i++) {
		XMFLOAT2 f2;
		f2.x = _getF(p);
		f2.y = _getF(p);
		vertices.push_back(f2);
	}
}

static void _getVertexColors(char*& p, std::vector<XMFLOAT4>& vertices, int nVertices)
{
	for (int i = 0; i < nVertices; i++) {
		XMFLOAT4 f4;
		_getF(p);
		f4.x = _getF(p);
		f4.y = _getF(p);
		f4.z = _getF(p);
		f4.w = _getF(p);
		vertices.push_back(f4);
	}
}

static void _getMatrix(char*& p, XMFLOAT4X4& m)
{
	if (!p) {
		XMStoreFloat4x4(&m, XMMatrixIdentity());
		return;
	}
	m._11 = _getF(p);
	m._12 = _getF(p);
	m._13 = _getF(p);
	m._14 = _getF(p);
	m._21 = _getF(p);
	m._22 = _getF(p);
	m._23 = _getF(p);
	m._24 = _getF(p);
	m._31 = _getF(p);
	m._32 = _getF(p);
	m._33 = _getF(p);
	m._34 = _getF(p);
	m._41 = _getF(p);
	m._42 = _getF(p);
	m._43 = _getF(p);
	m._44 = _getF(p);
}

static int _getIndices(char*& p, std::vector<unsigned>& indices, int nOrgFaces, std::vector<bool>& isOrgFace4Vertices)
{
	int nDividedTotalFaces = 0;
	for (int i = 0; i < nOrgFaces; i++) {
		int nVertices = _getI(p);
		assert(nVertices == 3 || nVertices == 4);
		int begin = indices.size();
		indices.push_back(_getI(p));
		indices.push_back(_getI(p));
		indices.push_back(_getI(p));
		nDividedTotalFaces++;
		if (nVertices == 4) {
			indices.push_back(indices[begin + 2]);
			indices.push_back(_getI(p));
			indices.push_back(indices[begin]);
			nDividedTotalFaces++;
		}
		isOrgFace4Vertices.push_back(nVertices == 4);
	}
	return nDividedTotalFaces;
}

static std::string _getToken(const char* p)
{
	while(*p && !isalpha(*p)) {
		if (*p == '{') {
			return "";
		}
		p++;
	}

	std::string r;
	while (*p && (isalnum(*p) || *p == '_')) {
		r = r + *p++;
	}

	return r;
}

static std::string _getString(char*& p)
{
	char* p1 = strchr(p, '\"');
	if (!p1) {
		return "";
	}
	p1++;
	char* p2 = strchr(p1, '\"');
	if (!p2) {
		return "";
	}

	std::string r;
	for (p = p1; p < p2; p++) {
		r = r + *p;
	}
	p++;

	return r;
}

static char* _searchNoNameChildTag(char* from)
{
	char* p = from;

	if (!p) {
		return nullptr;
	}

	int depth = 0;
	while (p && *p) {
		if (*p == '{') {
			return p + 1;
		} else if (*p == '}') {
			if (--depth < 0) {
				return nullptr;
			}
		} else if (isalpha(*p)) {
			_enterBrace(p);
			depth++;
			continue;
		}
		p++;
	}
	return nullptr;
}

static char* _searchChildTag(char* from, const char *tag, std::string* name = nullptr)
{
	if (*tag == '\0') {
		return _searchNoNameChildTag(from);
	}

	char* p = from;

	if (!p)
		return nullptr;

	int tagLen = strlen(tag);

	int depth = 0;
	while (p && *p) {
		if (*p == '#') {
			p = strchr(p, '\n');
			if (!p) {
				return nullptr;
			}
		} else if (*p == '{') {
			depth++;
		} else if (*p == '}') {
			if (--depth < 0) {
				return nullptr;
			}
		} else if (depth == 0 && isalpha(*p)) {

			if (!strncmp(p, tag, tagLen) && !isalpha(p[tagLen])) {
				p += tagLen + 1;
				if (name) {
					*name = _getToken(p);
				}
				_enterBrace(p);
				return p;
			}
			_enterBrace(p);
			depth++;
			continue;
		}
		p++;
	}
	return nullptr;
}

static MatMan::MMID _getMaterial(char*& p)
{
	Material mat;
	mat.faceColor.x = _getF(p);
	mat.faceColor.y = _getF(p);
	mat.faceColor.z = _getF(p);
	mat.faceColor.w = _getF(p);
	mat.power = _getF(p);
	mat.specular.x = _getF(p);
	mat.specular.y = _getF(p);
	mat.specular.z = _getF(p);
	mat.specular.w = 1.0f;
	mat.emissive.x = _getF(p);
	mat.emissive.y = _getF(p);
	mat.emissive.z = _getF(p);
	mat.emissive.w = 1.0f;
	std::string textureFilename = "white.bmp";
	char *tx = _searchChildTag(p, "TextureFilename");
	if (tx) {
		textureFilename = _getString(tx);
	}
	mat.tmid = texMan.Create(textureFilename.c_str(), true);	// load it at current directory
	return matMan.Create(mat);
}

void CreateCone(Block& b, XMVECTOR v1, XMVECTOR v2, BONE_ID boneId, DWORD color);

int MeshX::GetDepth(BONE_ID id)
{
	int depth = 0;
	Frame* f = &m_frames[id];
	for (; f->parentId >= 0; depth++) {
		f = &m_frames[f->parentId];
	}
	return depth;
}

void MeshX::CreateBoneMesh()
{
	for (BONE_ID i = 0; (unsigned)i < m_frames.size(); i++)	{
		Frame& f2 = m_frames[i];
		BONE_ID pId = f2.parentId;
		if (pId < 0) {
			continue;
		}
		int depth = GetDepth(pId);
		Frame& f1 = m_frames[pId];
		XMVECTOR dummy;
		XMVECTOR v1 = XMMatrixInverse(&dummy, XMLoadFloat4x4(&f1.boneOffsetMatrix)).r[3];
		XMVECTOR v2 = XMMatrixInverse(&dummy, XMLoadFloat4x4(&f2.boneOffsetMatrix)).r[3];

		if (XMMatrixIsIdentity(XMLoadFloat4x4(&f1.boneOffsetMatrix))) {
		//	v1 = XMLoadFloat4x4(&f1.frameTransformMatrixOrg).r[3];
			continue;
		}

		if (XMMatrixIsIdentity(XMLoadFloat4x4(&f2.boneOffsetMatrix))) {
		//	v2 = XMLoadFloat4x4(&f2.frameTransformMatrixOrg).r[3];
			continue;
		}

		static const DWORD depthToColor[] = {
			0xffffffff,
			0xffffff00,
			0xffff00ff,
			0xffff0000,
			0xff00ffff,
			0xff00ff00,
			0xff0000ff,
			0xff000000,
		};
		CreateCone(bones, v1, v2, pId, depthToColor[depth % dimof(depthToColor)]);
	}

	int sizeVertices = bones.vertices.size() * sizeof(bones.vertices[0]);
	int sizeIndices = bones.indices.size() * sizeof(bones.indices[0]);
	if (sizeVertices && sizeIndices) {
		bonesRenderer.Init(sizeVertices, sizeIndices, &bones.vertices[0], &bones.indices[0]);
	}

	Material mat;
	mat.faceColor.x = 0.6f;
	mat.faceColor.y = 0.6f;
	mat.faceColor.z = 0.6f;
	mat.faceColor.w = 1.0f;
	mat.power = 1.0f;
	mat.specular.x = 1.0f;
	mat.specular.y = 1.0f;
	mat.specular.z = 1.0f;
	mat.specular.w = 1.0f;
	mat.emissive.x = 0.4f;
	mat.emissive.y = 0.4f;
	mat.emissive.z = 0.4f;
	mat.emissive.w = 1.0f;
	mat.tmid = texMan.Create("white.bmp", true);

	MaterialMap map;
	map.materialId = matMan.Create(mat);
	map.faceStartIndex = 0;
	map.faces = bones.indices.size() / 3;
	bones.materialMaps.push_back(map);
}

void MeshX::CreatePivotMesh()
{
	for (BONE_ID i = 0; (unsigned)i < m_frames.size(); i++)	{
		Frame& f = m_frames[i];

		XMVECTOR dummy;
		XMMATRIX matLocal = XMMatrixInverse(&dummy, XMLoadFloat4x4(&f.boneOffsetMatrix));

		XMVECTOR v = matLocal.r[3];
		float len = 50.0f;
		CreateCone(pivots, v, XMVector3TransformCoord(XMVectorSet(len, 0, 0, 0), matLocal), i, 0xff0000ff);
		CreateCone(pivots, v, XMVector3TransformCoord(XMVectorSet(0, len, 0, 0), matLocal), i, 0xff00ff00);
		CreateCone(pivots, v, XMVector3TransformCoord(XMVectorSet(0, 0, len, 0), matLocal), i, 0xffff0000);
	}

	int sizeVertices = pivots.vertices.size() * sizeof(pivots.vertices[0]);
	int sizeIndices = pivots.indices.size() * sizeof(pivots.indices[0]);
	if (sizeVertices && sizeIndices) {
		pivotsRenderer.Init(sizeVertices, sizeIndices, &pivots.vertices[0], &pivots.indices[0]);
	}

	Material mat;
	mat.faceColor.x = 0.6f;
	mat.faceColor.y = 0.6f;
	mat.faceColor.z = 0.6f;
	mat.faceColor.w = 1.0f;
	mat.power = 1.0f;
	mat.specular.x = 1.0f;
	mat.specular.y = 1.0f;
	mat.specular.z = 1.0f;
	mat.specular.w = 1.0f;
	mat.emissive.x = 0.4f;
	mat.emissive.y = 0.4f;
	mat.emissive.z = 0.4f;
	mat.emissive.w = 1.0f;
	mat.tmid = texMan.Create("white.bmp", true);

	MaterialMap map;
	map.materialId = matMan.Create(mat);
	map.faceStartIndex = 0;
	map.faces = pivots.indices.size() / 3;
	pivots.materialMaps.push_back(map);
}

MeshX::MeshX(const char *fileName)
{
    char strPath[MAX_PATH];
    strcpy_s(strPath, MAX_PATH, fileName);
	std::for_each(strPath, strPath + strlen(strPath), [] (char& c) { c = c == '\\' ? '/' : c; });
    const char* fileNameWithoutPath = fileName;
    if (char* p = strrchr(strPath, '/')) {
        *p = '\0';
		fileNameWithoutPath = p + 1;
    }

	char strCWD[MAX_PATH];
    GetCurrentDirectoryA(MAX_PATH, strCWD);
	if(!SetCurrentDirectoryA(strPath)) {
		MessageBoxW(GetActiveWindow(), L"SetCurrentDirectoryA error", L"", MB_OK );
	}

	LoadSub(fileNameWithoutPath);
	
    SetCurrentDirectoryA(strCWD);

	int sizeVertices = m_block.vertices.size() * sizeof(m_block.vertices[0]);
	int sizeIndices = m_block.indices.size() * sizeof(m_block.indices[0]);
	m_meshRenderer.Init(sizeVertices, sizeIndices, &m_block.vertices[0], &m_block.indices[0]);

	CreatePivotMesh();
	CreateBoneMesh();
}

static DWORD _conv1To255(float f, int bit)
{
	return DWORD(f * 255) << bit;
}

static DWORD _convF4ToU32(XMFLOAT4 f)
{
	return _conv1To255(f.x, 24) | _conv1To255(f.y, 16) | _conv1To255(f.z, 8) | _conv1To255(f.w, 0);
}

void MeshX::_pushMaterialMap(Block& block, const MaterialMap& map)
{
	if (map.faces) {
		block.materialMaps.push_back(map);
	}
}

BONE_ID MeshX::_getFrameIdByName(const char* name)
{
	for (BONE_ID i = 0; (unsigned)i < m_frames.size(); i++) {
		if (!strcmp(m_frames[i].name, name)) {
			return i;
		}
	}
	Frame f;
	strncpy(f.name, name, sizeof(f.name));
	f.parentId = -1;
	f.childId = -1;
	f.siblingId = -1;
	XMStoreFloat4x4(&f.frameTransformMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&f.frameTransformMatrixOrg, XMMatrixIdentity());
	XMStoreFloat4x4(&f.boneOffsetMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&f.initialMatrix, XMMatrixIdentity());
	m_frames.push_back(f);
	return m_frames.size() - 1;
}

struct SkinWeights {
	std::vector<int> vertexIndices;
	std::vector<float> vertexWeight;
	std::string frameName;
	int frameId;
	XMFLOAT4X4 mtx;
	const SkinWeights& operator=(const SkinWeights& r) {
		vertexIndices = r.vertexIndices;
		vertexWeight = r.vertexWeight;
		frameName = r.frameName;
		frameId = r.frameId;
		mtx = r.mtx;
		return *this;
	}
	SkinWeights() {}
	SkinWeights(const SkinWeights& r) {
		*this = r;
	}
};

void MeshX::_storeWeight(MeshVertex& v, int frameId, float weight)
{
	if (v.blendWeights.x == 0) {
		v.blendWeights.x = weight;
		v.blendIndices.x = frameId;
		return;
	}
	if (v.blendWeights.y == 0) {
		v.blendWeights.y = weight;
		v.blendIndices.y = frameId;
		return;
	}
	if (v.blendWeights.z == 0) {
		v.blendWeights.z = weight;
		v.blendIndices.z = frameId;
		return;
	}
	v.blendIndices.w = frameId;
}

bool MeshX::ParseMesh(char* imgFrame, Block& block, BONE_ID frameId)
{
	auto& vertices = block.vertices;
	auto& indices = block.indices;
	vertices.clear();
	indices.clear();
	block.materialMaps.clear();

	char *imgMesh = _searchChildTag(imgFrame, "Mesh");
	if (!imgMesh) {
		return false;
	}

	char *p = imgMesh;
	int nVertices = _getI(p);
	std::vector<XMFLOAT3> vertPos;
	_getFloat3Array(p, vertPos, nVertices);
	int nOrgFaces = _getI(p);
	std::vector<bool> isOrgFace4Vertices;
	int nDividedTotalFaces = _getIndices(p, indices, nOrgFaces, isOrgFace4Vertices);

	p = _searchChildTag((char*)imgMesh, "MeshTextureCoords");
	int nVerticesCoords = _getI(p);
	assert(nVertices == nVerticesCoords || nVerticesCoords == 0);
	std::vector<XMFLOAT2> texCoords;
	_getFloat2Array(p, texCoords, nVerticesCoords);

	if (nVertices > nVerticesCoords) {
		texCoords.resize(nVertices);
	}

	p = _searchChildTag((char*)imgMesh, "MeshVertexColors");
	int nVertexColors = _getI(p);
	std::vector<XMFLOAT4> vertexColors;
	_getVertexColors(p, vertexColors, nVertexColors);

	p = _searchChildTag((char*)imgMesh, "MeshNormals");
	int nNormals = _getI(p);
	std::vector<XMFLOAT3> normals;
	_getFloat3Array(p, normals, nNormals);
	int nOrgNormalFaces = _getI(p);
	std::vector<bool> isOrgFace4VerticesNormal;
	std::vector<unsigned> normalIndices;
	int nDividedTotalFacesNormal = _getIndices(p, normalIndices, nOrgNormalFaces, isOrgFace4VerticesNormal);
	if (p) {
		assert(nDividedTotalFacesNormal == nDividedTotalFaces);
		assert(nOrgNormalFaces == nOrgFaces);
	}

	p = _searchChildTag(imgMesh, "MeshMaterialList");
	int nMaterials = _getI(p);
	int nMaterialFaces = _getI(p);
	std::vector<int> materialIndices;
	for (int i = 0; i < nMaterialFaces; i++) {
		materialIndices.push_back(_getI(p));
	}

	std::vector<MatMan::MMID> materialIds;
	materialIds.resize(nMaterials);
	for (int i = 0; i < nMaterials; i++) {
		p = _searchChildTag(p, "Material");
		materialIds[i] = _getMaterial(p);
		p = _leaveBrace(p);
	}

	p = imgMesh;

	std::vector<SkinWeights> skinWeights;
	for (;;) {
		p = _searchChildTag(p, "SkinWeights");
		if (!p) {
			break;
		}
		SkinWeights skin;
		skin.frameName = _getString(p);
		int nVertices = _getI(p);
		for (int i = 0; i < nVertices; i++) {
			skin.vertexIndices.push_back(_getI(p));
		}
		for (int i = 0; i < nVertices; i++) {
			skin.vertexWeight.push_back(_getF(p));
		}
		_getMatrix(p, skin.mtx);
		skin.frameId = _getFrameIdByName(skin.frameName.c_str());
		p = _leaveBrace(p);
		skinWeights.push_back(skin);

		Frame& frameForSkin = m_frames[skin.frameId];
		frameForSkin.boneOffsetMatrix = skin.mtx;
	}

	MaterialMap map;
	map.materialId = 0;
	map.faceStartIndex = 0;
	map.faces = 0;

	for (int i = 0, primitiveIdx = 0; i < nMaterialFaces; i++) {
		MatMan::MMID id = materialIds[materialIndices[i]];
		if (map.materialId != id) {
			_pushMaterialMap(block, map);
			map.materialId = id;
			map.faceStartIndex = primitiveIdx;
			map.faces = 0;
		}
		int numPrimitives = isOrgFace4Vertices[i] ? 2 : 1;
		map.faces += numPrimitives;
		primitiveIdx += numPrimitives;
	}
	
	int remainFaces = indices.size() / 3 - map.faceStartIndex;
	if (remainFaces != map.faces) {
		map.faces = remainFaces;	// tiger.x!
	}
	_pushMaterialMap(block, map);

	for (int i = 0; i < nVertices; i++)	{
		MeshVertex v;
		v.xyz = vertPos[i];
		v.normal.x = 1;
		v.normal.y = 0;
		v.normal.z = 0;
		v.uv = texCoords[i];
		v.color = 0xffffffff;
		if (i < nVertexColors) {
			XMFLOAT4 f4 = vertexColors[i];
			v.color = _convF4ToU32(f4);
		}
		v.blendWeights.x = 0;
		v.blendWeights.y = 0;
		v.blendWeights.z = 0;
		v.blendIndices.x = 0;
		v.blendIndices.y = 0;
		v.blendIndices.z = 0;
		v.blendIndices.w = frameId;
		vertices.push_back(v);
	}

	for (auto it = skinWeights.begin(); it != skinWeights.end(); it++)
	{
		assert(it->vertexIndices.size() == it->vertexWeight.size());
		int cnt = it->vertexIndices.size();
		for (int i = 0; i < cnt; i++) {
			int idx = it->vertexIndices[i];
			float wgt = it->vertexWeight[i];
			_storeWeight(vertices[idx], it->frameId, wgt);
		}
	}

	if (normalIndices.size()) {
		assert(indices.size() == normalIndices.size());
		for (int i = 0; (unsigned)i < normalIndices.size(); i++) {
			vertices[indices[i]].normal = normals[normalIndices[i]];
		}
	} else {
		for (int i = 0; (unsigned)i < indices.size() / 3; i++) {
			int idx = i * 3;
			XMVECTOR v[3];
			v[0] = XMLoadFloat3(&vertices[indices[idx]].xyz);
			v[1] = XMLoadFloat3(&vertices[indices[idx + 1]].xyz);
			v[2] = XMLoadFloat3(&vertices[indices[idx + 2]].xyz);
			XMVECTOR result = XMVector3Cross(v[1] - v[0], v[2] - v[1]);
			XMStoreFloat3(&vertices[indices[idx]].normal, result);
			XMStoreFloat3(&vertices[indices[idx + 1]].normal, result);
			XMStoreFloat3(&vertices[indices[idx + 2]].normal, result);
		}
	}

	return !!vertices.size();
}

void MeshX::_mergeBlocks(Block& d, const Block& s)
{
	if (!s.vertices.size()) {
		return;
	}

	int verticeBase = d.vertices.size();
	int indicesBase = d.indices.size();
	std::for_each(s.vertices.begin(), s.vertices.end(), [&](const MeshVertex& v) { d.vertices.push_back(v); });
	std::for_each(s.indices.begin(), s.indices.end(), [&](unsigned i) { d.indices.push_back(i + verticeBase); });
	std::for_each(s.materialMaps.begin(), s.materialMaps.end(), [&](MaterialMap m) {
		m.faceStartIndex += indicesBase / 3;
		d.materialMaps.push_back(m);
	});
}

void MeshX::_linkFrame(BONE_ID parentFrameId, BONE_ID childFrameId)
{
	Frame* frameParent = &m_frames[parentFrameId];
	Frame* frameChild = &m_frames[childFrameId];

	frameChild->parentId = parentFrameId;
	if (frameParent->childId < 0) {
		frameParent->childId = childFrameId;
	} else {
		BONE_ID siblingId = frameParent->childId;
		while (m_frames[siblingId].siblingId >= 0) {
			siblingId = m_frames[siblingId].siblingId;
		}
		m_frames[siblingId].siblingId = childFrameId;
	}
}

void MeshX::ParseFrame(char* p, BONE_ID parentFrameId)
{
	while(p) {
		std::string name;
		char* child = _searchChildTag(p, "Frame", &name);
		if (child) {
			char* frameMat = _searchChildTag(child, "FrameTransformMatrix");
			BONE_ID frameId = _getFrameIdByName(name.c_str());
			Frame& frame = m_frames[frameId];
			_getMatrix(frameMat, frame.frameTransformMatrix);
			frame.frameTransformMatrixOrg = frame.frameTransformMatrix;
			_linkFrame(parentFrameId, frameId);

			Block b;
			ParseMesh(child, b, frameId);
			_mergeBlocks(m_block, b);

			ParseFrame(child, frameId);
		}
		p = _leaveBrace(child);
	}
}


void MeshX::GetVertStatistics(std::vector<int>& cnts) const
{
	cnts.resize(m_frames.size());
	for (auto& it : cnts) {
		it = 0;
	}
	for (auto& it : m_block.vertices)
	{
		assert(it.blendIndices.x >= 0 && it.blendIndices.x < m_frames.size());
		assert(it.blendIndices.y >= 0 && it.blendIndices.y < m_frames.size());
		assert(it.blendIndices.z >= 0 && it.blendIndices.z < m_frames.size());
		assert(it.blendIndices.w >= 0 && it.blendIndices.w < m_frames.size());
		if (it.blendWeights.x > 0) {
			cnts[it.blendIndices.x]++;
		}
		if (it.blendWeights.y > 0) {
			cnts[it.blendIndices.y]++;
		}
		if (it.blendWeights.z > 0) {
			cnts[it.blendIndices.z]++;
		}
		if (it.blendWeights.x + it.blendWeights.y + it.blendWeights.z < 1.0f) {
			cnts[it.blendIndices.w]++;
		}
	}
}

void MeshX::GetAnimStatistics(std::vector<int>& animCnts) const
{
	animCnts.resize(m_frames.size());
	for (auto& it : animCnts) {
		it = 0;
	}
	for (auto& i : m_animationSets) {
		for (auto& j : i.animations) {
			assert(j.first >= 0 && j.first < (BONE_ID)m_frames.size());
			animCnts[j.first]++;
		}
	}
}

void MeshX::PrintStatistics() const
{
	std::vector<int> vCnts;
	GetVertStatistics(vCnts);
	std::vector<int> aCnts;
	GetAnimStatistics(aCnts);
	for (BONE_ID i = 0; i < (BONE_ID)m_frames.size(); i++)
	{
		const Frame& f = m_frames[i];
		printf("v=%d a=%d, parentId=%d, childId=%d, %s(%d)\n", vCnts[i], aCnts[i], f.parentId, f.childId, f.name, i);
	}
}

bool MeshX::UnlinkFrame(BONE_ID id)
{
	Frame& f = m_frames[id];
	if (f.childId >= 0) {
		return false;
	}
	if (f.parentId < 0) {
		return false;
	}
	Frame& p = m_frames[f.parentId];
	if (p.childId == id) {
		p.childId = f.siblingId;
	}
	else {
		Frame* mySibling = &m_frames[p.childId];
		do {
			if (mySibling->siblingId == id)
			{
				mySibling->siblingId = f.siblingId;
			}
			mySibling = &m_frames[mySibling->siblingId];
		} while (mySibling);
	}
	f.parentId = -1;

	for (auto& i : m_animationSets) {
		auto& j = i.animations.find(id);
		if (j != i.animations.end()) {
			i.animations.erase(j);
		}
	}

	return true;
}

void MeshX::DeleteDummyFrames()
{
	std::vector<int> cnts;

	bool deleted = false;
	do {
		deleted = false;
		GetVertStatistics(cnts);
		for (BONE_ID i = 0; i < (BONE_ID)m_frames.size(); i++)
		{
			const Frame& f = m_frames[i];
			int cnt = cnts[i];
			if (cnt) {
				continue;
			}
			if (f.childId >= 0)
			{
				continue;
			}
			deleted = UnlinkFrame(i);
		}
	} while (deleted);

	while (!m_frames.empty() && m_frames.rbegin()->parentId < 0 && m_frames.rbegin()->childId < 0) {
		m_frames.erase(m_frames.end() - 1);
	}
}

void MeshX::DumpFrames(BONE_ID frameId, int depth) const
{
	const Frame& f = m_frames[frameId];
	for (int i = 0; i < depth; i++) {
		printf(" ");
	}
//	printf("%s(%d) p=%d s=%d c=%d\n", f.name, frameId, f.parentId, f.siblingId, f.childId);
	printf("%s: ", f.name);
	for (int r = 0; r < 4; r++) {
		for (int c = 0; c < 4; c++) {
			float m = f.frameTransformMatrix.m[r][c];
			if (m - int(m)) {
				printf("%.3f,", m);
			}else{
				printf("%d,", (int)m);
			}
		}
	}
	printf("\n");
	if (f.siblingId >= 0) {
		DumpFrames(f.siblingId, depth);
	}
	if (f.childId >= 0) {
		DumpFrames(f.childId, depth + 1);
	}
}

static void ParseAnimationKeys(char* p, Animation& animation)
{
	for (;;) {
		char* key = _searchChildTag(p, "AnimationKey");
		if (!key) {
			break;
		}
		AnimationKey keys;
		keys.keyType = _getI(key);
		if (keys.keyType == 4) {  // matrix? (tiny.x!!! http://marupeke296.com/cgi-bin/cbbs/cbbs.cgi?mode=al2&namber=1571&rev=&no=0&P=R&KLOG=2)
			keys.keyType = 3;
		}
		int num = _getI(key);
		for (int i = 0; i < num; i++) {
			TimedFloatKeys k;
			k.time = (DWORD)_getI(key);
			int nValues = _getI(key);
			switch(keys.keyType) {
			case 0:		// rotation
				assert(nValues == 4);
				{
					XMFLOAT4 q;
					q.w = _getF(key);
					q.x = _getF(key);
					q.y = _getF(key);
					q.z = _getF(key);
				//	XMStoreFloat4x4(&k.mat, XMMatrixRotationQuaternion(XMLoadFloat4(&q)));
					XMStoreFloat4x4(&k.mat, XMMatrixRotationQuaternion(XMQuaternionInverse(XMLoadFloat4(&q))));
				}
				break;
			case 1:		// scale
				assert(nValues == 3);
				XMStoreFloat4x4(&k.mat, XMMatrixIdentity());
				k.mat._11 = _getF(key);
				k.mat._22 = _getF(key);
				k.mat._33 = _getF(key);
				break;
			case 2:		// position
				assert(nValues == 3);
				XMStoreFloat4x4(&k.mat, XMMatrixIdentity());
				k.mat._41 = _getF(key);
				k.mat._42 = _getF(key);
				k.mat._43 = _getF(key);
				break;
			case 3:		// matrix
				assert(nValues == 16);
				_getMatrix(key, k.mat);
				break;
			}
			keys.timedFloatKeys.push_back(k);
		}
		animation.animationKeys.push_back(keys);
		p = _leaveBrace(key);
	}
}

void MeshX::ParseAnimations(char* p, AnimationSet& animationSet)
{
	for (;;) {
		char* anim = _searchChildTag(p, "Animation");
		if (!anim) {
			break;
		}
		Animation animation;
		ParseAnimationKeys(anim, animation);
		if (animation.animationKeys.size()) {
			char *frameName = _searchChildTag(anim, "");
			animation.frameName = _getToken(frameName);
			BONE_ID frameId = _getFrameIdByName(animation.frameName.c_str());

			animationSet.animations[frameId] = animation;
		}

		p = _leaveBrace(anim);
	}
}

void MeshX::ParseAnimationSets(char* p)
{
	for (;;) {
		char* animSet = _searchChildTag(p, "AnimationSet");
		if (!animSet) {
			break;
		}
		AnimationSet animationSet;
		ParseAnimations(animSet, animationSet);
		if (animationSet.animations.size()) {
			m_animationSets.push_back(animationSet);
		}
		p = _leaveBrace(animSet);
	}
}

void MeshX::LoadSub(const char *fileName)
{
	void *img = LoadFile(fileName);
	if (!img) {
		return;
	}

	BONE_ID frameId = _getFrameIdByName("@myroot");

	Block b;
	char* body = (char*)img + 16;

	ParseMesh(body, b, frameId);	// mesh without frame tag
	_mergeBlocks(m_block, b);

	ParseFrame(body, frameId);

	for (auto& f : m_frames) {
		if (f.parentId >= 0) {
			XMVECTOR dummy;
			XMStoreFloat4x4(&f.initialMatrix, XMMatrixInverse(&dummy, XMLoadFloat4x4(&f.boneOffsetMatrix)) * XMLoadFloat4x4(&m_frames[f.parentId].boneOffsetMatrix));
		}
	}

	ParseAnimationSets(body);

	char* animTick = _searchChildTag(body, "AnimTicksPerSecond");
	m_animTicksPerSecond = 4800;
	if (animTick) {
		m_animTicksPerSecond = _getI(animTick);
	}

	free(img);

	DeleteDummyFrames();

	printf("===============DumpFrames begin\n");
	DumpFrames(0, 0);
	printf("===============DumpFrames end\n");

	printf("===============DumpStatistics begin\n");
	PrintStatistics();
	printf("===============DumpStatistics end\n");

	MakeInitialMatrixPerfectTStance();
}

void MeshX::MakeInitialMatrixPerfectTStance()
{
	XMStoreFloat4x4(&m_frames[0].initialMatrix, XMLoadFloat4x4(&m_frames[0].initialMatrix) * XMMatrixRotationY(XM_PI));
}

MeshX::~MeshX()
{
	m_meshRenderer.Destroy();
	pivotsRenderer.Destroy();
	bonesRenderer.Destroy();
}

void MeshX::CalcFrameMatrices(BONE_ID frameId, XMMATRIX& parent)
{
	Frame& f = m_frames[frameId];
	XMMATRIX result = XMLoadFloat4x4(&f.frameTransformMatrix) * parent;
	XMStoreFloat4x4(&f.result, result);
	if (f.siblingId >= 0) {
		CalcFrameMatrices(f.siblingId, parent);
	}
	if (f.childId >= 0) {
		CalcFrameMatrices(f.childId, result);
	}
}

static XMMATRIX Interpolate(const XMMATRIX& m1, const XMMATRIX& m2, float ratio)
{
	XMVECTOR q1 = XMQuaternionRotationMatrix(m1);
	XMVECTOR q2 = XMQuaternionRotationMatrix(m2);
	XMVECTOR q3 = XMQuaternionSlerp(q1, q2, ratio);

	XMVECTOR t1 = m1.r[3];
	XMVECTOR t2 = m2.r[3];
	XMVECTOR t3 = XMVectorLerp(t1, t2, ratio);

	XMMATRIX m3 = XMMatrixRotationQuaternion(q3);
	m3.r[3] = t3;
	return m3;
}

void MeshX::CalcAnimation(int animId, double time)
{
	if (animId < 0 || animId >= (int)m_animationSets.size()) {
		for (auto& f : m_frames) {
			XMStoreFloat4x4(&f.frameTransformMatrix, XMLoadFloat4x4(&f.frameTransformMatrixOrg));
		}
		return;
	}
	int revAnimId = m_animationSets.size() - animId - 1;

	for (auto itAnimation : m_animationSets[revAnimId].animations)
	{
		Frame& f = m_frames[itAnimation.first];
		Animation& anim = itAnimation.second;
		if (anim.animationKeys.size() == 0) {
			continue;
		}
		if (anim.animationKeys.begin()->timedFloatKeys.size() == 0) {
			continue;
		}

		bool stored = false;
		XMMATRIX rotMat = XMMatrixIdentity(), scaleMat = XMMatrixIdentity(), transMat = XMMatrixIdentity();
		for (auto itKey : anim.animationKeys) {

			double maxTime = itKey.timedFloatKeys.rbegin()->time;
			if (maxTime <= 0) {
				continue;
			}
			int iTime = (int)time % (int)maxTime;
			double timeMod = fmod(time, maxTime);
	
			for (int i = 0; i < (int)itKey.timedFloatKeys.size() - 1; i++) {
				TimedFloatKeys& t1 = itKey.timedFloatKeys[i];
				TimedFloatKeys& t2 = itKey.timedFloatKeys[i + 1];
				if (iTime < (int)t1.time || iTime >= (int)t2.time) {
					continue;
				}
				XMMATRIX mat = Interpolate(XMLoadFloat4x4(&t1.mat), XMLoadFloat4x4(&t2.mat), (float)((timeMod - t1.time) / (t2.time - t1.time)));
				XMFLOAT4X4 f4x4;
				XMStoreFloat4x4(&f4x4, mat);
				switch (itKey.keyType) {
				case 3: f.frameTransformMatrix = f4x4; stored = true; break;
				case 0: rotMat = mat; break;
				case 1: scaleMat = mat; break;
				case 2: transMat = mat; break;
				}
				break;
			}
		}
		if (!stored) {
			XMStoreFloat4x4(&f.frameTransformMatrix, scaleMat * rotMat * transMat);
		}
	}
}


void MeshX::Draw(int animId, double time)
{
	if (!m_block.indices.size()) {
		return;
	}

	XMMATRIX BoneMatrices[50];
	assert(m_frames.size() <= dimof(BoneMatrices));

	CalcAnimation(animId, time * m_animTicksPerSecond);
	CalcFrameMatrices(0, XMMatrixIdentity());

	for (BONE_ID i = 0; (unsigned)i < m_frames.size(); i++)	{
		Frame& f = m_frames[i];
		XMMATRIX frameTransform = XMLoadFloat4x4(&f.result);
		XMMATRIX boneOffset = XMLoadFloat4x4(&f.boneOffsetMatrix);
		BoneMatrices[i] = boneOffset * frameTransform;
	}

	if (g_type == "pivot") {
		pivotsRenderer.Draw(BoneMatrices, dimof(BoneMatrices), pivots);
	}
	if (g_type == "mesh") {
		m_meshRenderer.Draw(BoneMatrices, dimof(BoneMatrices), m_block);
	}
	if (g_type == "bone") {
		bonesRenderer.Draw(BoneMatrices, dimof(BoneMatrices), bones);
	}
}

static BONE_ID GetBvhBoneIdByTinyBoneName(const char* tinyBoneName, Bvh* bvh)
{
		struct BoneConvTbl {
		char* bvh;
		char* x;
	};
	const BoneConvTbl tbl[] =
	{
	//	{ "@myroot", "Hips" },
	//	{ "Scene_Root", "Hips" },
	//	{ "body", "Hips" },
	//	{ "Box01", "Hips" },
	//	{ "Bip01", "Hips" },
	//	{ "Bip01_Footsteps", "Hips" },
	//	{ "Bip01_Pelvis", "Hips" },
	//	{ "Bip01_Spine", "Chest2" },
	//	{ "Bip01_Spine1", "Chest2" },
	//	{ "Bip01_L_Thigh", "LeftKnee" },
	//	{ "Bip01_R_Thigh", "RightKnee" },
	//	{ "Bip01_R_Calf", "RightAnkle" },
	//	{ "Bip01_R_Foot", "RightAnkle" },
	//	{ "Bip01_R_Toe0", "RightToe" },
	//	{ "Dummy11", "End of RightToe" },
	//	{ "Bip01_L_Foot", "LeftAnkle" },
	//	{ "Bip01_L_Toe0", "LeftToe" },
	//	{ "Dummy16", "End of LeftToe" },
	//	{ "Bip01_Spine2", "Chest3" },
	//	{ "Bip01_Spine3", "Chest4" },
	//	{ "Bip01_Neck", "Neck" },
	//	{ "Bip01_R_UpperArm", "RightShoulder" },
	//	{ "Bip01_R_Finger0", "End of RightWrist" },
	//	{ "Bip01_R_Finger1", "End of RightWrist" },
	//	{ "Bip01_R_Finger11", "End of RightWrist" },
	//	{ "Bip01_R_Finger12", "End of RightWrist" },
	//	{ "Dummy02", "End of RightWrist" },
	//	{ "Bip01_R_Finger01", "End of RightWrist" },
	//	{ "Bip01_R_Finger02", "End of RightWrist" },
	//	{ "Dummy01", "End of RightWrist" },
	//	{ "Bip01_L_UpperArm", "LeftShoulder" },
	//	{ "Bip01_L_Hand", "LeftWrist" },
	//	{ "Bip01_L_Finger0", "End of LeftWrist" },
	//	{ "Bip01_L_Finger1", "End of LeftWrist" },
	//	{ "Bip01_L_Finger11", "End of LeftWrist" },
	//	{ "Bip01_L_Finger12", "End of LeftWrist" },
	//	{ "Dummy03", "End of LeftWrist" },
	//	{ "Bip01_L_Finger01", "End of LeftWrist" },
	//	{ "Bip01_L_Finger02", "End of LeftWrist" },
	//	{ "Dummy06", "End of LeftWrist" },
	//	{ "Dummy21", "End of LeftWrist" },


		{ "Bip01_L_Hand", "RightWrist" },
		{ "Bip01_R_Hand", "LeftWrist" },
		{ "Bip01_L_Forearm", "RightElbow" },
		{ "Bip01_R_Forearm", "LeftElbow" },
		{ "Bip01_L_UpperArm", "RightShoulder" },
		{ "Bip01_R_UpperArm", "LeftShoulder" },
		{ "Bip01_L_Clavicle", "RightCollar" },
		{ "Bip01_R_Clavicle", "LeftCollar" },
		{ "Bip01_Head", "Head" },
		{ "Bip01_Neck", "Neck" },
		{ "Bip01_Spine3", "Chest4" },
		{ "Bip01_Spine2", "Chest3" },
		{ "Bip01_Spine1", "Chest2" },
		{ "Bip01_Spine", "Chest" },
	//	{ "Bip01_Pelvis", "Hips" },			// ?
		{ "Bip01_L_Thigh", "RightHip" },
		{ "Bip01_R_Thigh", "LeftHip" },
		{ "Bip01_L_Calf", "RightKnee" },
		{ "Bip01_R_Calf", "LeftKnee" },
		{ "Bip01_L_Foot", "RightAnkle" },
		{ "Bip01_R_Foot", "LeftAnkle" },
		{ "Bip01_L_Toe0", "RightToe" },
		{ "Bip01_R_Toe0", "LeftToe" },
	};

	const char* bvhBoneName = nullptr;
	for (auto& t : tbl) {
		if (!strcmp(t.bvh, tinyBoneName)) {
			bvhBoneName = t.x;
			break;
		}
	}
	if (!bvhBoneName) {
		return -1;
	}
	BONE_ID bvhBoneId = bvh->BoneNameToId(bvhBoneName);
	return bvhBoneId;
}

XMMATRIX MeshX::GetWorldRotation(const char* frameName)
{
	BONE_ID id = _getFrameIdByName(frameName);
	XMFLOAT4X4 r = m_frames[id].result;
	r._41 = 0;
	r._42 = 0;
	r._43 = 0;
	return XMLoadFloat4x4(&r);
}

void MeshX::ApplyXLocalAxisToBvh(Bvh* bvh)
{
	for (BONE_ID i = 0; (unsigned)i < m_frames.size(); i++)	{
		Frame& f = m_frames[i];
		XMStoreFloat4x4(&f.frameTransformMatrix, XMLoadFloat4x4(&f.initialMatrix));
	}

	CalcFrameMatrices(0, XMMatrixIdentity());

	for (BONE_ID i = 0; (unsigned)i < m_frames.size(); i++)	{
		Frame& f = m_frames[i];

		BONE_ID bvhBoneId = GetBvhBoneIdByTinyBoneName(f.name, bvh);
		if (bvhBoneId >= 0) {
			bvh->SetLocalAxis(bvhBoneId, GetWorldRotation(f.name));
		}
	}
}

void MeshX::DrawBvh(Bvh* bvh, double time)
{
	ApplyXLocalAxisToBvh(bvh);

	XMMATRIX BoneTransForBvh[50];
	bvh->CalcBones(BoneTransForBvh, time);

	XMMATRIX BonesForX[50];
	for (auto& it : BonesForX) {
		it = XMMatrixIdentity();
	}

	assert(m_frames.size() <= dimof(BonesForX));
	for (BONE_ID i = 0; (unsigned)i < m_frames.size(); i++)	{
		Frame& f = m_frames[i];

//		XMStoreFloat4x4(&f.frameTransformMatrix, XMLoadFloat4x4(&f.initialMatrix));
		XMStoreFloat4x4(&f.frameTransformMatrix, XMLoadFloat4x4(&f.initialMatrix));


		BONE_ID bvhBoneId = GetBvhBoneIdByTinyBoneName(f.name, bvh);

		XMMATRIX rot = XMMatrixIdentity();

		if (bvhBoneId >= 0 && (!!strstr(f.name, "_UpperArm") || !!strstr(f.name, "_Thigh") || !!strstr(f.name, "_Calf") || !!strstr(f.name, "_Forearm"))) {
//		if (bvhBoneId >= 0 && !strcmp(f.name, "Bip01_R_UpperArm")) {
	//		BonesForX[i] = XMLoadFloat4x4(&f.boneOffsetMatrix) * BoneTransForBvh[bvhBoneId];
	//		BonesForX[i] = /*XMLoadFloat4x4(&f.boneOffsetMatrix) **/ BoneTransForBvh[bvhBoneId];
	//		BonesForX[i] = XMMatrixIdentity();
	//		XMMATRIX rot = XMMatrixRotationZ(sin(time * XM_PI / 10) * 50.0f * XM_PI / 180) * XMMatrixRotationX(cos(time * XM_PI / 10) * 50.0f * XM_PI / 180) * XMMatrixRotationY(sin(time * XM_PI / 8) * 50.0f * XM_PI / 180);
//			XMStoreFloat4x4(&f.frameTransformMatrix, rot);

//			XMVECTOR dummy;
//			XMStoreFloat4x4(&f.frameTransformMatrix, XMMatrixInverse(&dummy, XMLoadFloat4x4(&f.boneOffsetMatrix)));

//			BonesForX[i] = XMLoadFloat4x4(&f.boneOffsetMatrix) * rot * XMMatrixInverse(&dummy, XMLoadFloat4x4(&f.boneOffsetMatrix));


			const std::vector<BvhFrame>& bvhFrames = bvh->GetFrames();
			XMFLOAT4X4 bvhFrameTransMat = bvhFrames[bvhBoneId].frameTransformMatrix;
			bvhFrameTransMat._41 = 0;
			bvhFrameTransMat._42 = 0;
			bvhFrameTransMat._43 = 0;
			rot = XMLoadFloat4x4(&bvhFrameTransMat);

			float time = GetTickCount() / 1000.0f;
			rot = XMMatrixRotationZ(cos(time * XM_PI / 3) * 50.0f * XM_PI / 180);

		//	rot = XMMatrixRotationZ(sin(time * XM_PI / 10) * 10.0f * XM_PI / 180) * XMMatrixRotationX(cos(time * XM_PI / 10) * 10.0f * XM_PI / 180) * XMMatrixRotationY(sin(time * XM_PI / 8) * 10.0f * XM_PI / 180);
		}
		XMStoreFloat4x4(&f.frameTransformMatrix, rot * XMLoadFloat4x4(&f.initialMatrix));
	}

	CalcFrameMatrices(0, XMMatrixIdentity());
	/*
	for (BONE_ID i = 0; (unsigned)i < m_frames.size(); i++)	{
		Frame& f = m_frames[i];

		BONE_ID bvhBoneId = GetBvhBoneIdByTinyBoneName(f.name, bvh);
		if (bvhBoneId >= 0)
		{
			const BvhFrame& bh = bvh->GetFrames()[bvhBoneId];
			XMMATRIX boneOffset = XMLoadFloat4x4(&bh.boneOffsetMatrix);
			XMMATRIX frameTransform = XMLoadFloat4x4(&bh.result);
			BonesForX[i] = boneOffset * frameTransform;
		} else {
			XMMATRIX boneOffset = XMLoadFloat4x4(&f.boneOffsetMatrix);
			XMMATRIX frameTransform = XMLoadFloat4x4(&f.result);
			BonesForX[i] = boneOffset * frameTransform;
		}
	}*/
	for (BONE_ID i = 0; (unsigned)i < m_frames.size(); i++)	{
		Frame& f = m_frames[i];
		XMMATRIX boneOffset = XMLoadFloat4x4(&f.boneOffsetMatrix);
		XMMATRIX frameTransform = XMLoadFloat4x4(&f.result);
		BonesForX[i] = boneOffset * frameTransform;
	}

	if (g_type == "pivot") {
		pivotsRenderer.Draw(BonesForX, dimof(BonesForX), pivots);
	}
	if (g_type == "bone") {
		m_meshRenderer.Draw(BonesForX, dimof(BonesForX), m_block);
	}
	if (g_type == "mesh") {
		bonesRenderer.Draw(BonesForX, dimof(BonesForX), bones);
	}
}

