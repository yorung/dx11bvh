class BufferMan
{
public:
	typedef int BMID;
	std::vector<ID3D11Buffer*> insts;
public:
	BMID Create(int size);
	void Destroy();
	ID3D11Buffer* Get(BMID id);
	void Write(BMID id, const void* buf);
};

extern BufferMan bufferMan;

