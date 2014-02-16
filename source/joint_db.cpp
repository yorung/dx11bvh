#include "stdafx.h"

static char *xBones[] =
{
	"Bip01_L_Hand",
	"Bip01_R_Hand",
	"Bip01_L_Forearm",
	"Bip01_R_Forearm",
	"Bip01_L_UpperArm",
	"Bip01_R_UpperArm",
	"Bip01_L_Clavicle",
	"Bip01_R_Clavicle",
	"Bip01_Head",
	"Bip01_Neck",
	"Bip01_Spine3",
	"Bip01_Spine2",
	"Bip01_Spine1",
	"Bip01_Spine",
	"Bip01_Pelvis",
	"Bip01_L_Thigh",
	"Bip01_R_Thigh",
	"Bip01_L_Calf",
	"Bip01_R_Calf",
	"Bip01_L_Foot",
	"Bip01_R_Foot",
	"Bip01_L_Toe0",
	"Bip01_R_Toe0",
};


struct BoneConvTbl {
	char* x;
	std::vector<const char*> bvh;
};

static const BoneConvTbl boneConvTbl[] =
{
	{"Bip01_L_Hand", {"RightHand", "RightWrist"}},
	{"Bip01_R_Hand", {"LeftHand", "LeftWrist"} },
	{"Bip01_L_Forearm", {"RightForeArm", "RightElbow"} },
	{"Bip01_R_Forearm", {"LeftForeArm", "LeftElbow"} },
	{"Bip01_L_UpperArm", {"RightArm", "RightShoulder"} },
	{"Bip01_R_UpperArm", {"LeftArm", "LeftShoulder"} },
	{"Bip01_L_Clavicle", {"RightShoulder", "RightCollar"} },
	{"Bip01_R_Clavicle", {"LeftShoulder", "LeftCollar"} },
	{"Bip01_Head", {"Head"} },
	{"Bip01_Neck", {"Neck"} },
	{"Bip01_Spine3", {"Chest4"} },
//	{"Bip01_Spine2", {"Chest1"} },
	{"Bip01_Spine1", {"Spine", "Chest2"} },
	{"Bip01_Spine", {"LowerBack", "Chest"} },
	{"Bip01_Pelvis", {"Hips"} },
	{"Bip01_L_Thigh", {"RightUpLeg", "RightHip"} },
	{"Bip01_R_Thigh", {"LeftUpLeg", "LeftHip"} },
	{"Bip01_L_Calf", {"RightLeg", "RightKnee"} },
	{"Bip01_R_Calf", {"LeftLeg", "LeftKnee"} },
	{"Bip01_L_Foot", {"RightFoot", "RightAnkle"} },
	{"Bip01_R_Foot", {"LeftFoot", "LeftAnkle"} },
	{"Bip01_L_Toe0", {"RightToeBase", "RightToe"} },
	{"Bip01_R_Toe0", {"LeftToeBase", "LeftToe"} },
};

BONE_ID GetBvhBoneIdByTinyBoneName(const char* tinyBoneName, const Bvh* bvh)
{
	BoneType boneType = BT_INVALID;
	for (int i = 0; i < dimof(xBones); i++) {
		if (!strcmp(xBones[i], tinyBoneName)) {
			boneType = (BoneType)i;
		}
	}
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