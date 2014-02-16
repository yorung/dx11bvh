#include "stdafx.h"

struct BoneConvTbl {
	char* x;
	std::vector<const char*> bvh;
};

static const BoneConvTbl boneConvTbl[] =
{
	{ "Bip01_L_Hand", {"RightHand"} },
	{ "Bip01_R_Hand", {"LeftHand"} },
	{ "Bip01_L_Forearm", {"RightForeArm" } },
	{ "Bip01_R_Forearm", {"LeftForeArm" } },
	{ "Bip01_L_UpperArm", {"RightArm" } },
	{ "Bip01_R_UpperArm", {"LeftArm" } },
	{ "Bip01_L_Clavicle", {"RightShoulder" } },
	{ "Bip01_R_Clavicle", {"LeftShoulder" } },
	{ "Bip01_Head", {"Head" } },
	{ "Bip01_Neck", {"Neck" } },
	{ "Bip01_Spine3", {"Chest4" } },
	{ "Bip01_Spine2", {"Chest1" } },
	{ "Bip01_Spine1", {"Spine" } },
	{ "Bip01_Spine", {"LowerBack" } },
	{ "Bip01_Pelvis", {"Hips" } },
	{ "Bip01_L_Thigh", {"RightUpLeg" } },
	{ "Bip01_R_Thigh", {"LeftUpLeg" } },
	{ "Bip01_L_Calf", {"RightLeg" } },
	{ "Bip01_R_Calf", {"LeftLeg" } },
	{ "Bip01_L_Foot", {"RightFoot" } },
	{ "Bip01_R_Foot", {"LeftFoot" } },
	{ "Bip01_L_Toe0", {"RightToeBase" } },
	{ "Bip01_R_Toe0", {"LeftToeBase" } },
};

BONE_ID GetBvhBoneIdByTinyBoneName(const char* tinyBoneName, const Bvh* bvh)
{
	for (auto& t : boneConvTbl) {
		if (strcmp(t.x, tinyBoneName)) {
			continue;
		}
		for (auto b : t.bvh) {
			BONE_ID bvhBoneId = bvh->BoneNameToId(b);
			if (bvhBoneId >= 0) {
				return bvhBoneId;
			}
		}
		return -1;
	}
	return -1;
}