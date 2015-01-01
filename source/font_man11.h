class FontMan11
{
	struct CharCache {
		int x, y, w, h;
		int code;
	};
	static const int LIST_MAX = 1024;		// number of characters containable in the ring buffer
	CharCache charCache[LIST_MAX];
	int uniToIndex[65536];			// Unicode to cache index table
	int cursor;						// current index
	int curX, curY;
	TexMan::TMID texture;
	DIB texSrc;
	bool dirty;
	bool Build(int index, int code);
	int Cache(int code);
public:
	FontMan11();
	~FontMan11();
	bool Init();
	void Destroy();
	void FlushTexture();
};

extern FontMan11 fontMan11;
