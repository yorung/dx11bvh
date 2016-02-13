class FontMan
{
	struct CharSignature {
		wchar_t code;
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
	SRVID texture;
	DIB texSrc;

	static const int SPRITE_MAX = 4096;
	static const int SPRITE_VERTS = SPRITE_MAX * 4;
	CharSprite charSprites[SPRITE_MAX];
	int numSprites;
	ShaderMan::SMID shader = ShaderMan::INVALID_SMID;
	IBOID ibo;
	VBOID vbo;
	VAOID vao;
	SAMPLERID sampler;
	bool dirty = false;
	bool Build(const CharSignature& signature);
	bool Cache(const CharSignature& code);
	void DrawChar(Vec2& pos, const CharSignature& sig);
	void ClearCache();
	void MakeFontBitmap(const char* fontName, const CharSignature& code, DIB& dib, CharCache& cache) const;
public:
	FontMan();
	~FontMan();
	bool Init();
	void Destroy();
	void FlushToTexture();
	void DrawString(Vec2 pos, int fontSize, const wchar_t *text);
	void DrawString(Vec2 pos, int fontSize, const char *text);
	void Render();
	Vec2 MeasureString(int fontSize, const char *text);
};

extern FontMan fontMan;
