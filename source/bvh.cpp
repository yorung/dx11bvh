#include "stdafx.h"

static float bvhScale = 3.0f;
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
		if (*p == '{' || *p == '}') {
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

static void InitVertex(MeshVertex& v, BONE_ID boneId, int depth)
{
	DWORD depthToColor[] = {
		0xffffffff,
		0xffffff00,
		0xffff00ff,
		0xffff0000,
		0xff00ffff,
		0xff00ff00,
		0xff0000ff,
		0xff000000,
	};

	v.blendIndices.x = v.blendIndices.y = v.blendIndices.z = v.blendIndices.w = boneId;
	v.color = depthToColor[depth % dimof(depthToColor)];
	v.blendWeights.x = v.blendWeights.y = v.blendWeights.z = 0;
	v.normal.x = 1;
	v.normal.y = 0;
	v.normal.z = 0;
	v.uv.x = v.uv.y = 0;
	v.xyz.x = v.xyz.y = v.xyz.z = 0;
}

int Bvh::GetDepth(BONE_ID id)
{
	int depth = 0;
	BvhFrame* f = &m_frames[id];
	for (; f->parentId >= 0; depth++) {
		f = &m_frames[f->parentId];
	}
	return depth;
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
		BONE_ID pId = f2.parentId;
		if (pId < 0) {
			continue;
		}
		int depth = GetDepth(pId);
		BvhFrame& f1 = m_frames[pId];
		XMVECTOR v1 = XMLoadFloat3(&f1.offsetCombined);
		XMVECTOR v2 = XMLoadFloat3(&f2.offsetCombined);
		XMVECTOR boneDir = XMVectorSubtract(v2, v1);
		XMVECTOR eyeDir = XMVectorSet(0, 0, 0.15f, 0);
		XMVECTOR vRot0 = XMVector3Cross(boneDir, eyeDir);
		XMVECTOR vRot90 = XMVector3Cross(vRot0, XMVector3Normalize(boneDir));
		XMVECTOR vRotLast = XMVectorAdd(v1, vRot0);
		static const int div = 10;
		for (int j = 0; j < div; j++) {
			MeshVertex vert[3];
			for (auto& it : vert) {
				InitVertex(it, pId, GetDepth(pId));
			}
			float rad = XM_2PI / div * (j + 1);
			XMVECTOR vRot = XMVectorAdd(v1, XMVectorScale(vRot0, cosf(rad)) + XMVectorScale(vRot90, sinf(rad)));
			XMStoreFloat3(&vert[0].xyz, vRotLast);
			XMStoreFloat3(&vert[1].xyz, vRot);
			XMStoreFloat3(&vert[2].xyz, v2);
			XMVECTOR normal = XMVector3Cross(XMVectorSubtract(vRot, vRotLast), XMVectorSubtract(v2, vRot));
			for (auto& it : vert) {
				XMStoreFloat3(&it.normal, normal);
				m_block.vertices.push_back(it);
				m_block.indices.push_back(idx++);
			}
			vRotLast = vRot;
		}
	}

	int sizeVertices = m_block.vertices.size() * sizeof(m_block.vertices[0]);
	int sizeIndices = m_block.indices.size() * sizeof(m_block.indices[0]);
	if (sizeVertices && sizeIndices) {
		m_meshRenderer.Init(sizeVertices, sizeIndices, &m_block.vertices[0], &m_block.indices[0]);
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
	f.rotIndies.x = -1;
	f.rotIndies.y = -1;
	f.rotIndies.z = -1;
	f.posIndies.x = -1;
	f.posIndies.y = -1;
	f.posIndies.z = -1;
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
			if (name == "Site") {
				name = std::string("End of ") + m_frames[parentFrameId].name;
			}
			BONE_ID frameId = _getFrameIdByName(name.c_str());
			BvhFrame& frame = m_frames[frameId];

			_getToken(child);	// "OFFSET"
			frame.offset.x = _getF(child) * bvhScale;
			frame.offset.y = _getF(child) * bvhScale;
			frame.offset.z = -_getF(child) * bvhScale;

			frame.offsetCombined.x = 0;
			frame.offsetCombined.y = 0; 
			frame.offsetCombined.z = 0;

			if (parentFrameId >= 0) {
				_linkFrame(parentFrameId, frameId);
			}

			XMVECTOR dummy;
			XMStoreFloat4x4(&frame.boneOffsetMatrix, XMMatrixInverse(&dummy, (XMMatrixTranslation(frame.offsetCombined.x, frame.offsetCombined.y, frame.offsetCombined.z))));

			if ("CHANNELS" == _getToken(child)) {
				int nChannels = _getI(child);
				for (int i = 0; i < nChannels; i++) {
					std::string t = _getToken(child);
					if (t == "Xposition") {
						frame.posIndies.x = channels++;
					} else if (t == "Yposition") {
						frame.posIndies.y = channels++;
					} else if (t == "Zposition") {
						frame.posIndies.z = channels++;
					} else if (t == "Xrotation") {
						frame.rotIndies.x = channels++;
					} else if (t == "Yrotation") {
						frame.rotIndies.y = channels++;
					} else if (t == "Zrotation") {
						frame.rotIndies.z = channels++;
					}
				}
			}

			ParseFrame("JOINT", child, frameId);
			ParseFrame("End", child, frameId);
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

	channels = 0;
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
	BvhFrame& f = m_frames[frameId];
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

void Bvh::CalcAnimation(double time)
{
	int frame = (int)(time / frameTime);
	frame %= motionFrames;

	const float* mot = &motion[frame * channels];

	for (auto& it : m_frames) {
		XMMATRIX rotMat = XMMatrixIdentity(), scaleMat = XMMatrixIdentity(), transMat = XMMatrixIdentity();
	//	if (it.posIndies.x >= 0) {
	//		transMat = XMMatrixTranslation(mot[it.posIndies.x] * bvhScale, mot[it.posIndies.y] * bvhScale, -mot[it.posIndies.z] * bvhScale);
	//	} else {
			transMat = XMMatrixTranslation(it.offset.x, it.offset.y, it.offset.z);
	//	}
		if (it.rotIndies.x >= 0) {
//			rotMat = XMMatrixRotationZ(mot[it.rotIndies.z] * XM_PI / 180) * XMMatrixRotationX(-mot[it.rotIndies.x] * XM_PI / 180) * XMMatrixRotationY(-mot[it.rotIndies.y] * XM_PI / 180);
			rotMat = XMMatrixIdentity();
		}
		XMStoreFloat4x4(&it.frameTransformMatrix, scaleMat * rotMat * transMat);
	}
}


void Bvh::Draw(int animId, double time)
{
	if (!m_block.indices.size()) {
		return;
	}

	XMMATRIX BoneMatrices[50];
	assert(m_frames.size() <= dimof(BoneMatrices));

	CalcAnimation(time);

	CalcFrameMatrices(0, XMMatrixIdentity());

	for (BONE_ID i = 0; (unsigned)i < m_frames.size(); i++)	{
		BvhFrame& f = m_frames[i];
		XMMATRIX frameTransform = XMLoadFloat4x4(&f.result);
		XMMATRIX boneOffset = XMLoadFloat4x4(&f.boneOffsetMatrix);
		BoneMatrices[i] = boneOffset * frameTransform;
	}

	m_meshRenderer.Draw(BoneMatrices, dimof(BoneMatrices), m_block);
}

void Bvh::CalcBones(XMMATRIX BoneMatrices[50], double time)
{
	CalcAnimation(time);

	CalcFrameMatrices(0, XMMatrixIdentity());

	for (BONE_ID i = 0; (unsigned)i < m_frames.size(); i++)	{
		BvhFrame& f = m_frames[i];
		XMMATRIX frameTransform = XMLoadFloat4x4(&f.result);
		XMMATRIX boneOffset = XMLoadFloat4x4(&f.boneOffsetMatrix);
		BoneMatrices[i] = boneOffset * frameTransform;
//		BoneMatrices[i] = frameTransform;
	}
}

BONE_ID Bvh::BoneNameToId(const char* name)
{
	for (BONE_ID i = 0; (unsigned)i < m_frames.size(); i++)	{
		BvhFrame& f = m_frames[i];
		if (!strcmp(f.name, name)) {
			return i;
		}
	}
	return -1;
}

