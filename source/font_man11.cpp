#include "stdafx.h"

#define IS_HANGUL(c)	( (c) >= 0xAC00 && (c) <= 0xD7A3 )
#define IS_HANGUL2(c)	( ( (c) >= 0x3130 && (c) <= 0x318F ) || IS_HANGUL(c) )	// hangul + jamo

#define FONT_MAN_CHAR_W	16
#define FONT_MAN_CHAR_H	16

#define FONT_MAN_FONT_H	16

#define TEX_W		512
#define TEX_H		512

struct FontVertex {
	Vec2 pos;
	Vec2 coord;
};

static Vec2 fontVertAlign[] =
{
	Vec2(0, 0),
	Vec2(0, 1),
	Vec2(1, 0),
	Vec2(1, 1),
};

static BOOL isKorean(int code)
{
	return IS_HANGUL2(code) || code < 0x80;
}

static HFONT CreateAsianFont(int code, int height)
{
	BOOL isK = isKorean(code);
	const char *fontName = isK ? "GulimChe" : "MS Gothic";
	const DWORD charset = isK ? HANGUL_CHARSET : SHIFTJIS_CHARSET;
	return CreateFontA(height,			// Height Of Font
		0,								// Width Of Font
		0,								// Angle Of Escapement
		0,								// Orientation Angle
		FW_BOLD,						// Font Weight
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

FontMan11::FontMan11()
{
	memset(charCache, 0, sizeof(charCache));
	memset(uniToIndex, 0, sizeof(uniToIndex));
	cursor = curX = curY = 0;
	texture = TexMan::INVALID_TMID;
	shader = ShaderMan::INVALID_SMID;
	dirty = false;
}

FontMan11::~FontMan11()
{
	Destroy();
}

bool FontMan11::Init()
{
	bool result = false;
	if (!texSrc.Create(TEX_W, TEX_H)) {
		goto DONE;
	}
	texture = texMan.CreateDynamicTexture("$FontMan", TEX_W, TEX_H);

	static D3D11_INPUT_ELEMENT_DESC elements[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 8, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	shader = shaderMan.Create("font", elements, dimof(elements));

	ibo = afCreateQuadListIndexBuffer(SPRITE_MAX);
	vbo = afCreateDynamicVertexBuffer(SPRITE_MAX * sizeof(FontVertex));

	result = true;
DONE:
	return result;
}

void FontMan11::Destroy()
{
	// TODO: delete texture
//	texMan.Delete(texture);
//	texture = TexMan::INVALID_TMID;

//  shaderMan.Delete(shader);
//	shader = ShaderMan::INVALID_SMID;

	texSrc.Destroy();
	afSafeDeleteBuffer(ibo);
	afSafeDeleteBuffer(vbo);
}

bool FontMan11::Build(int index, int code)
{
	HFONT font = NULL, fontOld = NULL;
	bool result = false;
	DIB	dib;

	const BOOL applySoftAA = (code <= 0x80) || !isKorean(code);
	int size = applySoftAA ? 2 : 1;

	if (!dib.Create(FONT_MAN_CHAR_W * size, FONT_MAN_CHAR_H * size)) {
		return false;
	}

	font = CreateAsianFont(code, FONT_MAN_FONT_H * size);
	if (!font) {
		return false;
	}

	{
		dib.Clear();
		WCHAR buf[] = { code, '\0' };
		HDC hdc = dib.getDC();
		HFONT oldFont = (HFONT)SelectObject(hdc, font);
		SetTextColor(hdc, RGB(255, 255, 255));
		SetBkColor(hdc, RGB(0, 0, 0));
		TextOutW(hdc, 0, 0, buf, wcslen(buf));
		SelectObject(hdc, (HGDIOBJ)oldFont);

		if (applySoftAA) {
			DIB	dib2;
			if (!dib2.Create(FONT_MAN_CHAR_W, FONT_MAN_CHAR_H)) {
				goto DONE;
			}
			dib.ApplySoftAA(dib2);
			dib2.DibToDXFont();
			dib2.Blt(texSrc, curX, curY);
		} else {
			dib.DibToDXFont();
			dib.Blt(texSrc, curX, curY);
		}
		dirty = true;
		CharCache& cache = charCache[index];
		cache.code = code;
		cache.x = curX;
		cache.y = curY;
		cache.w = FONT_MAN_CHAR_W;
		cache.h = FONT_MAN_CHAR_H;

		curX += FONT_MAN_CHAR_W;
		if (curX >= TEX_W) {
			curX = 0;
			curY += FONT_MAN_CHAR_H;
		}
		if (curY >= TEX_H) {
			curY = 0;
		}
	}

	result = true;
DONE:
	if (font) DeleteObject(font);
	return result;
}

int FontMan11::Cache(int code)
{
	assert(code >= 0 && code <= 0xffff);
	int listIndex = uniToIndex[code];
	int index;
	if (charCache[listIndex].code != code) {
		uniToIndex[code] = cursor;
		index = cursor;
		Build(cursor, code);
		if (++cursor >= LIST_MAX){
			cursor = 0;
		}
	} else {
		index = uniToIndex[code];
	}
	return index;
}

void FontMan11::FlushToTexture()
{
	if (!dirty) {
		return;
	}
	dirty = false;
	texMan.Write(texture, texSrc.ReferPixels());
}

void FontMan11::Render()
{
	for (int i = 0; i < numSprites; i++) {
		Cache(charSprites[i].code);
	}
	FlushToTexture();

	static FontVertex verts[4 * SPRITE_MAX];
	for (int i = 0; i < numSprites; i++) {
		CharSprite& c = charSprites[i];
		float xSize = c.code < 256 ? 0.5f : 1.0f;

		for (int j = 0; j < dimof(fontVertAlign); j++) {
			verts[i * 4 + j].pos = c.pos + fontVertAlign[j] * Vec2(xSize, 1.0f) * Vec2(FONT_MAN_CHAR_W, FONT_MAN_CHAR_H);
		}
	}
	

}

void FontMan11::DrawChar(Vec2& pos, int code)
{
	int index = Cache(code);
	float xSize = code < 256 ? 0.5f : 1.0f;

	if (numSprites >= SPRITE_MAX) {
		return;
	}

	CharSprite& c = charSprites[numSprites++];
	c.code = code;
	c.pos = pos;

	pos.x += xSize;
}

void FontMan11::DrawText(Vec2 pos, const WCHAR *text)
{
	int len = wcslen(text);
	for (int i = 0; i < len; i++)
	{
		DrawChar(pos, text[i]);
	}
}

extern FontMan11 fontMan11;
