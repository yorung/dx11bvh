#include "stdafx.h"

#define IS_HANGUL(c)	( (c) >= 0xAC00 && (c) <= 0xD7A3 )
#define IS_HANGUL2(c)	( ( (c) >= 0x3130 && (c) <= 0x318F ) || IS_HANGUL(c) )	// hangul + jamo

#define FONT_MAN_CHAR_W	16
#define FONT_MAN_CHAR_H	16

#define FONT_MAN_FONT_H	16

#define TEX_W		512
#define TEX_H		512

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
	result = true;
DONE:
	return result;
}

void FontMan11::Destroy()
{
	// TODO: delete texture
//	texMan.Delete(texture);
//	texture = TexMan::INVALID_TMID;

	texSrc.Destroy();
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

void FontMan11::FlushTexture()
{
	if (!dirty) {
		return;
	}
	dirty = false;
	texMan.Write(texture, texSrc.ReferPixels());
}

/*
void FontMan11::DrawChar(int code)
{
	int index = Cache(code);
	double x_size = code < 256 ? 0.5 : 1;
	if (dirty) {
		ogCreateTexture(texture, &texSrc, OG_FILTER_NONE, TRUE);
		dirty = false;
	}

	glBindTexture(GL_TEXTURE_2D, texture);
	ogAlpha(TRUE);

	glEnable(GL_TEXTURE_2D);
	glDisable(GL_CULL_FACE);

	glBegin(GL_QUADS);
	glNormal3d(0, 0, 1);
	{
		const CharCache& c = charCache[index];
		double coordx[] = { (double)c.x / TEX_W, (double)(c.x + c.w) / TEX_W, (double)(c.x + c.w) / TEX_W, (double)c.x / TEX_W };
		//		double coordy[] = { (double)c.y / TEX_H, (double)c.y / TEX_H, (double)(c.y + c.h) / TEX_H, (double)(c.y + c.h) / TEX_H };
		double coordy[] = { (double)(c.y + c.h) / TEX_H, (double)(c.y + c.h) / TEX_H, (double)c.y / TEX_H, (double)c.y / TEX_H };
		double vertx[] = { 0, 1, 1, 0 };
		double verty[] = { 0, 0, 1, 1 };
		for (int i = 0; i < 4; i++){
			glTexCoord2d(coordx[i], coordy[i]);
			//	glTexCoord2d(vertx[i], 1 - verty[i]);
			glVertex3d(vertx[i], 1 - verty[i], 0);
		}
	}
	glEnd();
	glTranslated(x_size, 0, 0);
	ogAlpha(FALSE);
}

void FontMan11::DrawText(const WCHAR *text)
{
	int len = wcslen(text);
	for (int i = 0; i < len; i++) {
		Cache(text[i]);
	}

	glPushMatrix();
	glScalef(FONT_MAN_CHAR_W, FONT_MAN_CHAR_W, 1);
	for (int i = 0; i < len; i++)
		fontMan.DrawChar(text[i]);
	glPopMatrix();
}
*/

extern FontMan11 fontMan11;
