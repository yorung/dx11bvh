class FontMan11
{
	struct CharCache {
		int x, y, w, h;
		int code;
	};
	struct CharSprite {
		Vec2 pos;
		int code;
	};
	static const int LIST_MAX = 1024;		// number of characters containable in the ring buffer
	CharCache charCache[LIST_MAX];
	int uniToIndex[65536];			// Unicode to cache index table
	int cursor;						// current index
	int curX, curY;
	TexMan::TMID texture;
	DIB texSrc;

	static const int SPRITE_MAX = 256;
	static const int SPRITE_VERTS = SPRITE_MAX * 4;
	CharSprite charSprites[SPRITE_MAX];
	int numSprites;
	ShaderMan::SMID shader;
	AFBufObj ibo;
	AFBufObj vbo;
	ID3D11SamplerState* pSamplerState;
	ID3D11DepthStencilState* pDSState;
	ID3D11BlendState* blendState;

	bool dirty;
	bool Build(int index, int code);
	int Cache(int code);
	void DrawChar(Vec2& pos, int code);
public:
	FontMan11();
	~FontMan11();
	bool Init();
	void Destroy();
	void FlushToTexture();
	void DrawString(Vec2 pos, const WCHAR *text);
	void Render();
};

extern FontMan11 fontMan;
