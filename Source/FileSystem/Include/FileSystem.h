#pragma once

#define FF_READ				0
#define FF_WRITE			1
#define FF_APPEND			2

class File  {

public:
	FILE* m_pFile;
	std::string m_strFileName;

public:
	File();
	~File();

	bool open(std::string strFileName, size_t nFlag = FF_READ);
	int write(size_t nBytes, void* pBuffer);

	bool skip(size_t nBytes);
	bool setOffset(size_t nBytes, size_t nFlag = SEEK_SET);

	size_t getOffset();
	size_t getSize();

	int read(void* pBuffer, size_t nBytes);

	unsigned char* read(size_t nBytes);

	void close();
};