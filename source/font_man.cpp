#include "stdafx.h"

#define IS_HANGUL(c)	( (c) >= 0xAC00 && (c) <= 0xD7A3 )
#define IS_HANGUL2(c)	( ( (c) >= 0x3130 && (c) <= 0x318F ) || IS_HANGUL(c) )	// hangul + jamo

#define TEX_W		512
#define TEX_H		512

FontMan fontMan;

struct FontVertex {
	Vec2 pos;
	Vec2 coord;
};

static Vec2 fontVertAlign[] =
{
	Vec2(0, 0),
	Vec2(1, 0),
	Vec2(0, 1),
	Vec2(1, 1),
};

static bool isKorean(int code)
{
	return IS_HANGUL2(code) || code < 0x80;
}

#ifdef _MSC_VER
static HFONT CreateAsianFont(int code, int height)
{
	BOOL isK = isKorean(code);
	const char *fontName = isK ? "Gulim" : "MS Gothic";
	const DWORD charset = isK ? HANGUL_CHARSET : SHIFTJIS_CHARSET;
	return CreateFontA(height,			// Height Of Font
		0,								// Width Of Font
		0,								// Angle Of Escapement
		0,								// Orientation Angle
		FW_MEDIUM,						// Font Weight
		FALSE,							// Italic
		FALSE,							// Underline
		FALSE,							// Strikeout
		charset,						// Character Set Identifier
		OUT_TT_PRECIS,					// Output Precision
		CLIP_DEFAULT_PRECIS,			// Clipping Precision
		ANTIALIASED_QUALITY,			// Output Quality
		FF_DONTCARE | DEFAULT_PITCH,	// Family And Pitch
		fontName);						// Font Name
}
#endif




static void afVerify(bool ok) {
	if (ok) {
		return;
	}
	aflog("afVerify: Fatal");
	while(strlen(" ")) {
	}
}

#ifndef _MSC_VER
void FontMan::MakeFontBitmap(const char* fontName, const CharSignature& sig, DIB& dib, CharCache& cache) const
{
	int code = sig.code;

	jclass myview = jniEnv->FindClass(boundJavaClass);
	jmethodID method = jniEnv->GetStaticMethodID(myview, "makeFontBitmap", "(Ljava/lang/String;Ljava/lang/String;I[F)[B");
//	jmethodID method = jniEnv->GetStaticMethodID(myview, "makeFontBitmap", "(Ljava/lang/String;II)[B");
	if (method == 0) {
		aflog("Java method not found!");
		afVerify(false);
	}

	jchar codeInUnicode[2] = {(jchar)code, 0};

	jfloatArray floatArray = jniEnv->NewFloatArray(5);
	jobject arrayAsJObject = jniEnv->CallStaticObjectMethod(myview, method, jniEnv->NewStringUTF(fontName), jniEnv->NewString(codeInUnicode, 1), (jint)sig.fontSize, floatArray);
	aflog("Java method called and returned");

	{
		jfloat* pFloatArray = jniEnv->GetFloatArrayElements(floatArray, NULL);
		cache.distDelta = Vec2(pFloatArray[0], pFloatArray[1]);
		cache.srcWidth = Vec2(pFloatArray[2], pFloatArray[3]);
		cache.step = pFloatArray[4];
		jniEnv->ReleaseFloatArrayElements(floatArray, pFloatArray, 0);
	}
	if (!arrayAsJObject) {
		aflog("Java method returned null; it's white space");
		return;
	}

	jbyteArray array = (jbyteArray)arrayAsJObject;
	jbyte* byteArray = jniEnv->GetByteArrayElements(array, NULL);
	jsize arrayLen = jniEnv->GetArrayLength(array);
	aflog("arrayLen=%d", arrayLen);

	int expectedLen = cache.srcWidth.x * cache.srcWidth.y * 4;
	bool result = false;
	if (arrayLen != expectedLen) {
		aflog("wrong size! returned=%d expected=%d", arrayLen, expectedLen);
		afVerify(false);
	} else {
		dib.Create(cache.srcWidth.x, cache.srcWidth.y, 32);
		memcpy(dib.ReferPixels(), byteArray, arrayLen);
	}
	jniEnv->ReleaseByteArrayElements(array, byteArray, 0);
	dib.DibToDXFont();
}
#endif

FontMan::FontMan()
{
	ClearCache();
	texture = TexMan::INVALID_TMID;
	shader = ShaderMan::INVALID_SMID;
	dirty = false;
}

FontMan::~FontMan()
{
	Destroy();
}

void FontMan::ClearCache()
{
	caches.clear();
	curX = curY = curLineMaxH = 0;
}

static InputElement elements[] = {
	CInputElement(0, "POSITION", SF_R32G32_FLOAT, 0),
	CInputElement(0, "TEXCOORD", SF_R32G32_FLOAT, 8),
};

bool FontMan::Init()
{
	Destroy();
	bool result = false;
	if (!texSrc.Create(TEX_W, TEX_H)) {
		goto DONE;
	}
	texture = texMan.CreateDynamicTexture("$FontMan", TEX_W, TEX_H);

	shader = shaderMan.Create("font");
	assert(shader);

	ibo = afCreateQuadListIndexBuffer(SPRITE_MAX);
	vbo = afCreateDynamicVertexBuffer(SPRITE_MAX * sizeof(FontVertex) * 4);
	int stride = sizeof(FontVertex);
	vao = afCreateVAO(shader, elements, dimof(elements), 1, &vbo, &stride, ibo);
#ifndef GL_TRUE
	{
		CD3D11_SAMPLER_DESC descSamp(D3D11_DEFAULT);
		descSamp.AddressU = descSamp.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		deviceMan11.GetDevice()->CreateSamplerState(&descSamp, &pSamplerState);
	}
#endif
	result = true;
DONE:
	return result;
}

void FontMan::Destroy()
{
	// TODO: delete texture
//	texMan.Delete(texture);
	texture = TexMan::INVALID_TMID;

//  shaderMan.Delete(shader);
	shader = ShaderMan::INVALID_SMID;

	texSrc.Destroy();
	afSafeDeleteBuffer(ibo);
	afSafeDeleteBuffer(vbo);
	afSafeDeleteVAO(vao);
#ifndef GL_TRUE
	SAFE_RELEASE(pSamplerState);
#endif
	ClearCache();
}

#ifdef _MSC_VER
void FontMan::MakeFontBitmap(const char* fontName, const CharSignature& sig, DIB& dib, CharCache& cache) const
{
	bool result = false;

	HFONT font = CreateAsianFont(sig.code, sig.fontSize);
	assert(font);

	dib.Clear();
	wchar_t buf[] = { sig.code, '\0' };
	HDC hdc = texSrc.getDC();
	assert(hdc);
	HFONT oldFont = (HFONT)SelectObject(hdc, font);
	const MAT2 mat = { {0,1}, {0,0}, {0,0}, {0,1} };
	GLYPHMETRICS met;
	DWORD sizeReq = GetGlyphOutlineW(hdc, (UINT)sig.code, GGO_GRAY8_BITMAP, &met, 0, nullptr, &mat);
	if (sizeReq) {
		DIB dib3;
		afVerify(dib3.Create(met.gmBlackBoxX, met.gmBlackBoxY, 8, 64));
		afVerify(dib.Create(met.gmBlackBoxX, met.gmBlackBoxY));
		int sizeBuf = dib3.GetByteSize();
		if (sizeReq != sizeBuf) {
			aflog("FontMan::Build() buf size mismatch! code=%d req=%d dib=%d\n", sig.code, sizeReq, sizeBuf);
			afVerify(false);
		}
		GetGlyphOutlineW(hdc, (UINT)sig.code, GGO_GRAY8_BITMAP, &met, sizeReq, dib3.ReferPixels(), &mat);
	//	SetTextColor(hdc, RGB(255, 255, 255));
	//	SetBkColor(hdc, RGB(0, 0, 0));
	//	TextOutW(hdc, 0, 0, buf, wcslen(buf));
		dib3.Blt(dib.getDC(), 0, 0, dib3.getW(), dib3.getH());
		dib.DibToDXFont();
	}
	SelectObject(hdc, (HGDIOBJ)oldFont);
	if (font) {
		DeleteObject(font);
	}

	cache.srcPos = Vec2((float)curX, (float)curY);
	cache.srcWidth = Vec2((float)met.gmBlackBoxX, (float)met.gmBlackBoxY);
	cache.step = (float)met.gmCellIncX;
	cache.distDelta = Vec2((float)met.gmptGlyphOrigin.x, (float)-met.gmptGlyphOrigin.y);
}
#endif

bool FontMan::Build(const CharSignature& signature)
{
	bool result = false;
	DIB	dib;
	CharCache cache;
	MakeFontBitmap("Gulim", signature, dib, cache);
	int remainX = texSrc.getW() - curX;
	if (remainX < dib.getW()) {
		curX = 0;
		curY += curLineMaxH;
		curLineMaxH = 0;
		aflog("FontMan::Build() new line\n");
	}
	int remainY = texSrc.getH() - curY;
	if (remainY < dib.getH()) {
		aflog("FontMan::Build() font texture is full!\n");
		return false;
	}
	curLineMaxH = std::max(curLineMaxH, dib.getH());

	cache.srcPos = Vec2((float)curX, (float)curY);
	if (dib.getW() > 0 && dib.getH() > 0) {
		dib.Blt(texSrc, curX, curY);
		dirty = true;
	}
	aflog("FontMan::Build() curX=%d curY=%d dib.getW()=%d dib.getH()=%d\n", curX, curY, dib.getW(), dib.getH());

	curX += (int)ceil(cache.srcWidth.x);
	caches[signature] = cache;
	return true;
}

bool FontMan::Cache(const CharSignature& sig)
{
	int code = sig.code;
	assert(code >= 0 && code <= 0xffff);
	Caches::iterator it = caches.find(sig);
	if (it != caches.end())
	{
		return true;
	}
	return Build(sig);
}

void FontMan::FlushToTexture()
{
	if (!dirty) {
		return;
	}
	dirty = false;
#ifdef GL_TRUE
	texMan.Write(texture, texSrc.ReferPixels(), texSrc.getW(), texSrc.getH());
#endif
#ifndef GL_TRUE
	texMan.Write(texture, texSrc.ReferPixels());
#endif
}

void FontMan::Render()
{
	if (!numSprites) {
		return;
	}
	for (int i = 0; i < numSprites; i++) {
		Cache(charSprites[i].signature);
	}
	FlushToTexture();

	Vec2 scrSize = systemMetrics.GetScreenSize();

	static FontVertex verts[4 * SPRITE_MAX];
	for (int i = 0; i < numSprites; i++) {
		CharSprite& cs = charSprites[i];
		Caches::iterator it = caches.find(cs.signature);
		if (it == caches.end()) {
			aflog("something wrong");
			continue;
		}
		const CharCache& cc = it->second;
		for (int j = 0; j < (int)dimof(fontVertAlign); j++) {
			verts[i * 4 + j].pos = (((cs.pos + cc.distDelta + fontVertAlign[j] * cc.srcWidth)) * Vec2(2, -2)) / scrSize + Vec2(-1, 1);
			verts[i * 4 + j].coord = (cc.srcPos + fontVertAlign[j] * cc.srcWidth) / Vec2(TEX_W, TEX_H);
		}
	}
	afWriteBuffer(vbo, verts, 4 * numSprites * sizeof(FontVertex));
	shaderMan.Apply(shader);


#ifndef GL_TRUE
	deviceMan11.GetContext()->PSSetSamplers(0, 1, &pSamplerState);
#endif

	afBindVAO(vao);
	afBindTextureToBindingPoint(texture, 0);
	afBlendMode(BM_ALPHA);
	afDepthStencilMode(false);
	afDrawIndexedTriangleList(ibo, numSprites * 6);
	afDepthStencilMode(true);
	afBlendMode(BM_NONE);

#ifdef GL_TRUE
	glBindVertexArray(0);
#endif
	numSprites = 0;
}

void FontMan::DrawChar(Vec2& pos, const CharSignature& sig)
{
	if (numSprites >= SPRITE_MAX) {
		return;
	}
	Cache(sig);

	if (sig.code != 32) {	// whitespace
		CharSprite& cs = charSprites[numSprites++];
		cs.signature = sig;
		cs.pos = pos;
	}

	Caches::iterator it = caches.find(sig);
	if (it != caches.end()) {
		pos.x += it->second.step;
	}
}

Vec2 FontMan::MeasureString(int fontSize, const char *text)
{
	int len = strlen(text);
	Vec2 pos(0, 0);
	Vec2 size(0, 0);
	for (int i = 0; i < len; i++)
	{
		CharSignature sig;
		sig.code = text[i];
		sig.fontSize = fontSize;
		Cache(sig);
		Caches::iterator it = caches.find(sig);
		if (it != caches.end()) {
			size.y = std::max(size.y, it->second.srcWidth.y);
			size.x = pos.x + it->second.distDelta.x + it->second.srcWidth.x;
			pos.x += it->second.step;
		}
	}
	return size;
}

void FontMan::DrawString(Vec2 pos, int fontSize, const wchar_t *text)
{
	int len = wcslen(text);
	for (int i = 0; i < len; i++)
	{
		CharSignature sig;
		sig.code = text[i];
		sig.fontSize = fontSize;
		DrawChar(pos, sig);
	}
}

void FontMan::DrawString(Vec2 pos, int fontSize, const char *text)
{
	int len = strlen(text);
	for (int i = 0; i < len; i++)
	{
		CharSignature sig;
		sig.code = text[i];
		sig.fontSize = fontSize;
		DrawChar(pos, sig);
	}
}
