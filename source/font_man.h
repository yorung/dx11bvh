class FontMan
{
	struct CharSignature {
		int code;
		int fontSize;
		inline int GetOrder() const { return (code << 8) | fontSize;}
		bool operator < (const CharSignature& r) const { return GetOrder() < r.GetOrder(); }
		bool operator == (const CharSignature& r) const { return GetOrder() == r.GetOrder(); }
	};
	struct CharCache {
		Vec2 srcPos;
		Vec2 srcWidth;
		Vec2 distDelta;
		float step;
	};
	struct CharSprite {
		Vec2 pos;
		CharSignature signature;
	};
	typedef std::map<CharSignature, CharCache> Caches;
	Caches caches;
	int curX, curY, curLineMaxH;
	TexMan::TMID texture;
	DIB texSrc;

	static const int SPRITE_MAX = 512;
	static const int SPRITE_VERTS = SPRITE_MAX * 4;
	CharSprite charSprites[SPRITE_MAX];
	int numSprites;
	ShaderMan::SMID shader;
	AFBufObj ibo;
	AFBufObj vbo;

	int screenW, screenH;
#ifndef GL_TRUE
	ID3D11SamplerState* pSamplerState;
	ID3D11DepthStencilState* pDSState;
	ID3D11BlendState* blendState;
#endif
	bool dirty;
	bool Build(const CharSignature& signature);
	bool Cache(const CharSignature& code);
	void DrawChar(Vec2& pos, const CharSignature& sig);
	void ClearCache();
	void MakeFontBitmap(const char* fontName, const CharSignature& code, DIB& dib, CharCache& cache) const;
public:
	FontMan();
	~FontMan();
	bool Init(int scrW, int scrH);
	void Destroy();
	void FlushToTexture();
	void DrawString(Vec2 pos, int fontSize, const wchar_t *text);
	void DrawString(Vec2 pos, int fontSize, const char *text);
	void Render();
};

extern FontMan fontMan;
