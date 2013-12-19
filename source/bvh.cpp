#include "stdafx.h"

void *LoadFile(const char *fileName);

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

static std::string _getToken(char*& p)
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
			_getToken(p);
			continue;
		}
		p++;
	}
	return nullptr;
}

static void InitVertex(MeshVertex& v, BONE_ID boneId)
{
	v.blendIndices.x = v.blendIndices.y = v.blendIndices.z = v.blendIndices.w = boneId;
	v.color = 0xffffffff;
	v.blendWeights.x = v.blendWeights.y = v.blendWeights.z = 0;
	v.normal.x = 1;
	v.normal.y = 0;
	v.normal.z = 0;
	v.uv.x = v.uv.y = 0;
	v.xyz.x = v.xyz.y = v.xyz.z = 0;
}

Bvh::Bvh(const char *fileName)
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

	unsigned idx = 0;
	for (BONE_ID i = 0; (unsigned)i < m_frames.size(); i++)	{
		BvhFrame& f2 = m_frames[i];
		int p = f2.parentId;
		if (p < 0) {
			continue;
		}
		BvhFrame& f1 = m_frames[p];
		XMVECTOR v1 = XMLoadFloat3(&f1.offsetCombined);
		XMVECTOR v2 = XMLoadFloat3(&f2.offsetCombined);
		XMVECTOR sub = XMVectorSubtract(v2, v1);
		XMVECTOR eyeDir = XMVectorSet(0, 0, 0.1f, 0);
		XMVECTOR v1L = XMVector3Cross(sub, eyeDir);
		XMVECTOR v1R = XMVector3Cross(eyeDir, sub);
		MeshVertex vert[3];
		for (auto& it : vert) {
			InitVertex(it, i);
		}
		XMStoreFloat3(&vert[0].xyz, XMVectorAdd(v1, v1L));
		XMStoreFloat3(&vert[1].xyz, XMVectorAdd(v1, v1R));
		XMStoreFloat3(&vert[2].xyz, v2);
		for (auto& it : vert) {
			m_block.vertices.push_back(it);
			m_block.indices.push_back(idx++);
		}
	}

	int sizeVertices = m_block.vertices.size() * sizeof(m_block.vertices[0]);
	int sizeIndices = m_block.indices.size() * sizeof(m_block.indices[0]);
	m_meshRenderer.Init(sizeVertices, sizeIndices, &m_block.vertices[0], &m_block.indices[0]);

	Material mat;
	mat.faceColor.x = 1.0f;
	mat.faceColor.y = 1.0f;
	mat.faceColor.z = 1.0f;
	mat.faceColor.w = 1.0f;
	mat.power = 1.0f;
	mat.specular.x = 1.0f;
	mat.specular.y = 1.0f;
	mat.specular.z = 1.0f;
	mat.specular.w = 1.0f;
	mat.emissive.x = 1.0f;
	mat.emissive.y = 1.0f;
	mat.emissive.z = 1.0f;
	mat.emissive.w = 1.0f;
	mat.tmid = texMan.Create("white.bmp", true);

	MaterialMap map;
	map.materialId = matMan.Create(mat);
	map.faceStartIndex = 0;
	map.faces = m_block.indices.size() / 3;
	m_block.materialMaps.push_back(map);
}

static DWORD _conv1To255(float f, int bit)
{
	return DWORD(f * 255) << bit;
}

static DWORD _convF4ToU32(XMFLOAT4 f)
{
	return _conv1To255(f.x, 24) | _conv1To255(f.y, 16) | _conv1To255(f.z, 8) | _conv1To255(f.w, 0);
}

BONE_ID Bvh::_getFrameIdByName(const char* name)
{
	for (BONE_ID i = 0; (unsigned)i < m_frames.size(); i++) {
		if (!strcmp(m_frames[i].name, name)) {
			return i;
		}
	}
	BvhFrame f;
	strncpy(f.name, name, sizeof(f.name));
	f.parentId = -1;
	f.childId = -1;
	f.siblingId = -1;
	m_frames.push_back(f);
	return m_frames.size() - 1;
}

bool Bvh::ParseMesh(char* imgFrame, Block& block, BONE_ID frameId)
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


	p = imgMesh;

	return !!vertices.size();
}

void Bvh::_linkFrame(BONE_ID parentFrameId, BONE_ID childFrameId)
{
	BvhFrame* frameParent = &m_frames[parentFrameId];
	BvhFrame* frameChild = &m_frames[childFrameId];

	frameChild->parentId = parentFrameId;
	XMStoreFloat3(&frameChild->offsetCombined, XMVectorAdd(XMLoadFloat3(&frameParent->offsetCombined), XMLoadFloat3(&frameChild->offset)));

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

void Bvh::ParseFrame(const char* frameStr, char* p, BONE_ID parentFrameId)
{
	while(p) {
		std::string name;
		char* child = _searchChildTag(p, frameStr, &name);
		if (child) {
			BONE_ID frameId = _getFrameIdByName(name.c_str());
			BvhFrame& frame = m_frames[frameId];

			_getToken(child);	// "OFFSET"
			frame.offset.x = _getF(child);
			frame.offset.y = _getF(child);
			frame.offset.z = _getF(child);

			frame.offsetCombined.x = 0;
			frame.offsetCombined.y = 0; 
			frame.offsetCombined.z = 0;

			if (parentFrameId >= 0) {
				_linkFrame(parentFrameId, frameId);
			}

			ParseFrame("JOINT", child, frameId);
		}
		p = _leaveBrace(child);
	}
}

void Bvh::DumpFrames(BONE_ID frameId, int depth) const
{
	const BvhFrame& f = m_frames[frameId];
	for (int i = 0; i < depth; i++) {
		printf(" ");
	}
	printf("%s(%d) p=%d s=%d c=%d ", f.name, frameId, f.parentId, f.siblingId, f.childId);
	const XMFLOAT3& m = f.offset;
	const XMFLOAT3& m2 = f.offsetCombined;
	printf("(%3.3f,%3.3f,%3.3f) (%3.3f,%3.3f,%3.3f)", m.x, m.y, m.z, m2.x, m2.y, m2.z);
	printf("\n");
	if (f.siblingId >= 0) {
		DumpFrames(f.siblingId, depth);
	}
	if (f.childId >= 0) {
		DumpFrames(f.childId, depth + 1);
	}
}

void Bvh::ParseMotion(const char *p)
{
	char *m = (char*)strstr(p, "MOTION");
	motionFrames = _getI(m);
	frameTime = _getF(m);
	while (m && *m) {
		motion.push_back(_getF(m));
	}
}

void Bvh::LoadSub(const char *fileName)
{
	void *img = LoadFile(fileName);
	if (!img) {
		return;
	}

	char* body = (char*)img;

	ParseFrame("ROOT", body, -1);
	ParseMotion(body);
	free(img);

	printf("===============DumpFrames begin\n");
	DumpFrames(0, 0);
	printf("===============DumpFrames end\n");

}

Bvh::~Bvh()
{
	m_meshRenderer.Destroy();
}

void Bvh::CalcFrameMatrices(BONE_ID frameId, XMMATRIX& parent)
{
/*
	Frame& f = m_frames[frameId];
	XMMATRIX result = XMLoadFloat4x4(&f.frameTransformMatrix) * parent;
	XMStoreFloat4x4(&f.result, result);
	if (f.siblingId >= 0) {
		CalcFrameMatrices(f.siblingId, parent);
	}
	if (f.childId >= 0) {
		CalcFrameMatrices(f.childId, result);
	}
*/
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

void Bvh::CalcAnimation(int animId, double time)
{
/*
	if (animId < 0 || animId >= (int)m_animationSets.size()) {
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
*/
}


void Bvh::Draw(int animId, double time)
{
	if (!m_block.indices.size()) {
		return;
	}

	XMMATRIX BoneMatrices[50];
	assert(m_frames.size() <= dimof(BoneMatrices));

	for (auto& it : BoneMatrices) {
		it = XMMatrixIdentity();
	}

	/*
	CalcAnimation(animId, time * m_animTicksPerSecond);
	CalcFrameMatrices(0, XMMatrixIdentity());

	for (BONE_ID i = 0; (unsigned)i < m_frames.size(); i++)	{
		BvhFrame& f = m_frames[i];
		XMMATRIX frameTransform = XMLoadFloat4x4(&f.result);
		XMMATRIX boneOffset = XMLoadFloat4x4(&f.boneOffsetMatrix);
		BoneMatrices[i] = boneOffset * frameTransform;
	}

*/
	m_meshRenderer.Draw(BoneMatrices, dimof(BoneMatrices), m_block);
}
