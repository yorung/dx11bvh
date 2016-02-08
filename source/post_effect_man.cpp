#include <stdafx.h>

PostEffectMan::PostEffectMan()
{
	shaderId = ShaderMan::INVALID_SMID;
}

PostEffectMan::~PostEffectMan()
{
	assert(!sampler);
	assert(!ubo);
}

void PostEffectMan::Create(const char *shader)
{
	Destroy();

	shaderId = shaderMan.Create(shader, nullptr, 0, BM_NONE, DSM_DISABLE);
	ubo = afCreateUBO(sizeof(Mat));
	sampler = afCreateSampler(SF_POINT, SW_REPEAT);
}

void PostEffectMan::Draw(SRVID srv)
{
	if (!shaderId) {
		return;
	}

	shaderMan.Apply(shaderId);

	Mat matV, matP;
	matrixMan.Get(MatrixMan::VIEW, matV);
	matrixMan.Get(MatrixMan::PROJ, matP);
	Mat invVP = inv(matV * matP);

	afWriteBuffer(ubo, &invVP, sizeof(Mat));
	afBindBufferToBindingPoint(ubo, 0);
	afBindSamplerToBindingPoint(sampler, 0);
	afBindTextureToBindingPoint(srv, 0);
	afDrawTriangleStrip(4);
	afBindBufferToBindingPoint(0, 0);
	afBindTextureToBindingPoint(0, 0);
}

void PostEffectMan::Destroy()
{
	afSafeDeleteSampler(sampler);
	afSafeDeleteBuffer(ubo);
}
