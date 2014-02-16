enum BoneType
{
	BT_INVALID = -1,
	BT_L_HAND,
	BT_R_HAND,
	BT_L_ARM2,
	BT_R_ARM2,
	BT_L_ARM1,
	BT_R_ARM1,
	BT_L_COLLAR,
	BT_R_COLLAR,
	BT_HEAD,
	BT_NECK,
	BT_SPINE3,
	BT_SPINE2,
	BT_SPINE1,
	BT_SPINE,
	BT_HIPS,
	BT_L_LEG1,
	BT_R_LEG1,
	BT_L_LEG2,
	BT_R_LEG2,
	BT_L_FOOT,
	BT_R_FOOT,
	BT_L_TOE,
	BT_R_TOE,
};

BONE_ID GetBvhBoneIdByTinyBoneName(const char* tinyBoneName, const Bvh* bvh);
