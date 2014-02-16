#include "stdafx.h"

static const char *xBones[] =
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