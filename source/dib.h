class DIB {
	int m_w, m_h;
	int m_pitch;
	void *m_p;
	HBITMAP m_hbm;
	HDC m_hdc;
	HBITMAP	m_hbmOld;
	DIBSECTION m_ds;
	void FillAlpha();
  public:
	typedef uint32_t pixel;
	DIB();
	~DIB();
	void Destroy();
	bool Create(int w, int h);
	bool Create(int w, int h, int bits);
	bool LoadFromBmp(const char *file);
	bool Blt(HDC hdc, int x, int y);
	bool Blt(DIB *target, int x, int y, int w, int h, int srcX, int srcY);
	bool Blt(DIB& target, int dstX, int dstY);
	bool ApplySoftAA(DIB& target);
	void DibToGL();
	void DibToGLFont();
	void DibToDXFont();
	void Clear();
	HDC getDC() { return m_hdc; }
	int getW() { return m_w; }
	int getH() { return m_h; }
	pixel getPixel(int x, int y){ return *referPixel(x, y); }
	void setPixel(int x, int y, pixel px){ *referPixel(x, y) = px; }
	pixel *referPixel(int x, int y){ return (pixel*)((BYTE*)m_p + x * sizeof(pixel) + y * m_pitch); }
	pixel *referPixels(){ return (pixel*)m_p; }
};
