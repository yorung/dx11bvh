class DIB {
	int m_w, m_h;
	int m_pitch;
	void *m_p;
#ifdef _MSC_VER
	HBITMAP m_hbm;
	HDC m_hdc;
	HBITMAP	m_hbmOld;
	DIBSECTION m_ds;
#endif
	void FillAlpha();
  public:
	typedef uint32_t pixel;
	DIB();
	~DIB();
	void Destroy();
	bool Create(int w, int h);
	bool Create(int w, int h, int bits, int level = 255);
#ifdef _MSC_VER
	bool LoadFromBmp(const char *file);
	HDC getDC() const { return m_hdc; }
	bool Blt(HDC target, int dstX, int dstY, int w, int h);
#endif
	bool Blt(DIB *target, int x, int y, int w, int h, int srcX, int srcY);
	bool Blt(DIB& target, int dstX, int dstY);
	bool ApplySoftAA(DIB& target);
	void DibToGL();
	void DibToGLFont();
	void DibToDXFont();
	void Clear(pixel color = 0);
	int getW() { return m_w; }
	int getH() { return m_h; }
	pixel getPixel(int x, int y){ return *referPixel(x, y); }
	void setPixel(int x, int y, pixel px){ *referPixel(x, y) = px; }
	pixel *referPixel(int x, int y){ return (pixel*)((char*)m_p + x * sizeof(pixel) + y * m_pitch); }
	pixel *ReferPixels(){ return (pixel*)m_p; }
	int GetByteSize() { return m_pitch * m_h; }
};
