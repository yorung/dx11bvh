class BufferMan
{
public:
	typedef int BMID;
	std::vector<UBOID> insts;
public:
	BMID Create(int size);
	void Destroy();
	UBOID Get(BMID id);
};

extern BufferMan bufferMan;
