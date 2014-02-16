#include "stdafx.h"

void *LoadFile(const char *fileName);

static const char *bvhPerfume[] =
{
	"RightWrist",
	"LeftWrist",
	"RightElbow",
	"LeftElbow",
	"RightShoulder",
	"LeftShoulder",
	"RightCollar",
	"LeftCollar",
	"Head",
	"Neck",
	"Chest4",
	"Chest3",
	"Chest2",
	"Chest",
	"Hips",
	"RightHip",
	"LeftHip",
	"RightKnee",
	"LeftKnee",
	"RightAnkle",
	"LeftAnkle",
	"RightToe",
	"LeftToe",
};

static const char *bvhCmu[] =
{
	"RightHand",
	"LeftHand",
	"RightForeArm",
	"LeftForeArm",
	"RightArm",
	"LeftArm",
	"RightShoulder",
	"LeftShoulder",
	"Head",
	"Neck",
	"Spine1",
	"Spine1",
	"Spine",
	"LowerBack",
	"Hips",
	"RightUpLeg",
	"LeftUpLeg",
	"RightLeg",
	"LeftLeg",
	"RightFoot",
	"LeftFoot",
	"RightToeBase",
	"LeftToeBase",
};

static const char **bvhPresets[] = {bvhPerfume, bvhCmu};

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
		if (strchr("0123456789-.", *p))
		{
			break;
		}
		p++;
	}

	double result = atof(p);

	while (*p) {
		if (!strchr("0123456789-.eE", *p)) {
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

int Bvh::GetDepth(BONE_ID id)
{
	int depth = 0;
	BvhFrame* f = &m_frames[id];
	for (; f->parentId >= 0; depth++) {
		f = &m_frames[f->parentId];
	}
	return depth;
}

void Bvh::CreateBoneTypeToIdTbl()
{
	for (auto& it : bvhPresets) {
		int cnt = 0;
		for (int i = 0; i < BT_MAX; i++) {
			for (BONE_ID id = 0; id < (BONE_ID)m_frames.size(); id++) {
				const BvhFrame& f = m_frames[id];
				if (!strcmp(f.name, it[i])) {
					cnt++;
					boneTypeToIdTbl[i] = id;
				}
			}
		}
		if (cnt == BT_MAX) {
			return;
		}
	}
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

	CalcCombinedOffsets();
	for (BONE_ID i = 0; i < (BONE_ID)m_frames.size(); i++) {
		CalcBoneOffsetMatrix(i);
	}
	CreateBoneMesh();
	CreateBoneTypeToIdTbl();
}

void Bvh::CreateBoneMesh()
{
	m_block.Clear();

	for (BONE_ID i = 0; (unsigned)i < m_frames.size(); i++)	{
		BvhFrame& f2 = m_frames[i];
		BONE_ID pId = f2.parentId;
		if (pId < 0) {
			continue;
		}
		int depth = GetDepth(pId);
		BvhFrame& f1 = m_frames[pId];
		Vector3 v1 = f1.offsetCombined;
		Vector3 v2 = f2.offsetCombined;

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
		CreateCone(m_block, v1, v2, pId, depthToColor[depth % dimof(depthToColor)]);
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
	f.rotIndices.x = -1;
	f.rotIndices.y = -1;
	f.rotIndices.z = -1;
	f.posIndices.x = -1;
	f.posIndices.y = -1;
	f.posIndices.z = -1;
	m_frames.push_back(f);
	return m_frames.size() - 1;
}

void Bvh::_linkFrame(BONE_ID parentFrameId, BONE_ID childFrameId)
{
	BvhFrame* frameParent = &m_frames[parentFrameId];
	BvhFrame* frameChild = &m_frames[childFrameId];

	frameChild->parentId = parentFrameId;
	frameChild->offsetCombined = frameChild->offset * frameParent->offsetCombined;

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

void Bvh::CalcBoneOffsetMatrix(BONE_ID frameId)
{
	BvhFrame& frame = m_frames[frameId];
	frame.boneOffsetMatrix = v2m(-frame.offsetCombined);
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
			frame.offset.x = _getF(child);
			frame.offset.y = _getF(child);
			frame.offset.z = -_getF(child);
			frame.offsetCombined = Vec3();

			if (parentFrameId >= 0) {
				_linkFrame(parentFrameId, frameId);
			}

			CalcBoneOffsetMatrix(frameId);

			if ("CHANNELS" == _getToken(child)) {
				int nChannels = _getI(child);
				for (int i = 0; i < nChannels; i++) {
					std::string t = _getToken(child);
					if (t == "Xposition") {
						frame.posIndices.x = channels++;
					} else if (t == "Yposition") {
						frame.posIndices.y = channels++;
					} else if (t == "Zposition") {
						frame.posIndices.z = channels++;
					} else if (t == "Xrotation") {
						frame.rotIndices.x = channels++;
					} else if (t == "Yrotation") {
						frame.rotIndices.y = channels++;
					} else if (t == "Zrotation") {
						frame.rotIndices.z = channels++;
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
	const Vec3& v = f.offset;
	const Vec3& v2 = f.offsetCombined;
	printf("(%3.3f,%3.3f,%3.3f) (%3.3f,%3.3f,%3.3f)", v.x, v.y, v.z, v2.x, v2.y, v2.z);
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
		rawMotion.push_back(_getF(m));
	}
}

void Bvh::PreCalculateMotion()
{
	motion.poses.resize(motionFrames);
	for (int i = 0; i < motionFrames; i++) {
		const float* mot = &rawMotion[i * channels];
		Pose& pose = motion.poses[i];

		for (auto& it : m_frames) {
			Quat q[3];
			int idxMin = std::min((uint32_t)it.rotIndices.x, std::min((uint32_t)it.rotIndices.y, (uint32_t)it.rotIndices.z));
			if (it.rotIndices.x >= 0) {
				q[it.rotIndices.x - idxMin] = Quat(Vec3(1,0,0), -mot[it.rotIndices.x] * XM_PI / 180);
			}
			if (it.rotIndices.y >= 0) {
				q[it.rotIndices.y - idxMin] = Quat(Vec3(0,1,0), -mot[it.rotIndices.y] * XM_PI / 180);
			}
			if (it.rotIndices.z >= 0) {
				q[it.rotIndices.z - idxMin] = Quat(Vec3(0,0,1), mot[it.rotIndices.z] * XM_PI / 180);
			}
			pose.quats.push_back(q[2] * q[1] * q[0]);
		}
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
	PreCalculateMotion();
	free(img);

	printf("===============DumpFrames begin\n");
	DumpFrames(0, 0);
	printf("===============DumpFrames end\n");

}

Bvh::~Bvh()
{
	m_meshRenderer.Destroy();
}

void Bvh::CalcFrameMatrices()
{
	for (auto& f : m_frames) {
		f.result = f.frameTransformMatrix * (f.parentId >= 0 ? m_frames[f.parentId].result : Matrix());
	}
}

void Bvh::CalcCombinedOffsets()
{
	for (auto& f : m_frames) {
		f.offsetCombined = f.offset + (f.parentId >= 0 ? m_frames[f.parentId].offsetCombined : Vec3());
	}
}

void Bvh::CalcAnimation(double time)
{
	int frame = (int)(time / frameTime);
	frame %= motionFrames;

	const float* mot = &rawMotion[frame * channels];
	const Pose& pose = motion.poses[frame];

	for (BONE_ID i = 0; i < (BONE_ID)m_frames.size(); i++) {
		auto& it = m_frames[i];
		Vec3 translate;
		Quaternion q = pose.quats[i];
		if (it.posIndices.x >= 0) {
			translate = Vec3(mot[it.posIndices.x], mot[it.posIndices.y], -mot[it.posIndices.z]);
		} else {
			translate = it.offset;
		}
		
		it.frameTransformMatrix = q2m(q) * v2m(translate);
	}
	CalcFrameMatrices();
}

void Bvh::ResetAnim()
{
	for (auto& it : m_frames) {
		it.frameTransformMatrix = v2m(it.offset);
	}
	CalcFrameMatrices();
}

void Bvh::Draw(int animId, double time)
{
	if (!m_block.indices.size()) {
		return;
	}

	Mat BoneMatrices[BONE_MAX];
	assert(m_frames.size() <= dimof(BoneMatrices));

	if (animId != 0) {
		for (auto& it : m_frames) {
			it.frameTransformMatrix = v2m(it.offset);
		}
		CalcFrameMatrices();
	} else {
		CalcAnimation(time);
	}

	if (g_type == "pivot") {
		for (BONE_ID i = 0; (unsigned)i < m_frames.size(); i++)	{
			BvhFrame& f = m_frames[i];
			BoneMatrices[i] = f.result;
		}
		debugRenderer.DrawPivots(BoneMatrices, m_frames.size());
	} else {
		for (BONE_ID i = 0; (unsigned)i < m_frames.size(); i++)	{
			BvhFrame& f = m_frames[i];
			BoneMatrices[i] = f.boneOffsetMatrix * f.result;
		}
		m_meshRenderer.Draw(BoneMatrices, dimof(BoneMatrices), m_block);
	}
}

void Bvh::GetRotAnim(Quat quats[BONE_MAX], double time) const
{
	int frame = (int)(time / frameTime);
	frame %= motionFrames;

	const Pose& pose = motion.poses[frame];

	for (BONE_ID i = 0; (unsigned)i < m_frames.size(); i++)	{
		const BvhFrame& f = m_frames[i];
		quats[i] = pose.quats[i];
	}
}

BONE_ID Bvh::BoneNameToId(const char* name) const
{
	for (BONE_ID i = 0; (unsigned)i < m_frames.size(); i++)	{
		const BvhFrame& f = m_frames[i];
		if (!strcmp(f.name, name)) {
			return i;
		}
	}
	return -1;
}

BONE_ID Bvh::BoneTypeToId(BoneType type) const
{
	return boneTypeToIdTbl[type];
}

void Bvh::FixBones(const char* name)
{
	BONE_ID id = BoneNameToId(name);

	BvhFrame& f = m_frames[id];
	if (f.childId < 0) {
		return;
	}
	BONE_ID childId = f.childId;
	while (childId >= 0) {
		for (auto& it : motion.poses) {
			assert(id < (BONE_ID)it.quats.size());
			assert(childId < (BONE_ID)it.quats.size());
			it.quats[childId] *= it.quats[id];
		}
		childId = m_frames[childId].siblingId;
	}
	for (auto& it : motion.poses) {
		assert(id < (BONE_ID)it.quats.size());
		it.quats[id] = Quaternion();
	}
}

void Bvh::LinkTo(const char* me, const char* linkTo)
{
	BONE_ID id = BoneNameToId(me);

	BvhFrame& f = m_frames[id];
	if (f.childId < 0) {
		return;
	}
	BONE_ID parentId = f.parentId;
	if (parentId < 0) {
		return;
	}
	UnlinkFromParent(id);

	BONE_ID linkToId = BoneNameToId(linkTo);
	if (linkToId < 0) {
		return;
	}

	assert(f.siblingId < 0);
	assert(f.parentId < 0);
	f.parentId = linkToId;
	f.siblingId = m_frames[linkToId].childId;
	m_frames[linkToId].childId = id;

	f.offset -= m_frames[linkToId].offset;

	CalcCombinedOffsets();
	for (BONE_ID i = 0; i < (BONE_ID)m_frames.size(); i++) {
		CalcBoneOffsetMatrix(i);
	}
	CreateBoneMesh();
}

bool Bvh::UnlinkFromParent(BONE_ID id)
{
	BvhFrame& f = m_frames[id];
	if (f.parentId < 0) {
		return false;
	}
	BvhFrame& p = m_frames[f.parentId];
	if (p.childId == id) {
		p.childId = f.siblingId;
	}
	else {
		BvhFrame* mySibling = &m_frames[p.childId];
		do {
			if (mySibling->siblingId == id)
			{
				mySibling->siblingId = f.siblingId;
			}
			mySibling = mySibling->siblingId >= 0 ? &m_frames[mySibling->siblingId] : nullptr;
		} while (mySibling);
	}
	f.parentId = -1;
	f.siblingId = -1;
	return true;
}
