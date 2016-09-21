#include <map>
#include <memory>
#include <vector>
#include <deque>
#include "FileSystem.h"

File::File() {
	m_pFile = nullptr;
}

File::~File() {
	m_pFile = nullptr;
}

bool File::open(std::string strFileName, size_t nFlag) {

	if (m_pFile) {
		close();
	}

	int err = 0;
		
	switch (nFlag) {
		case 0:
			err = fopen_s(&m_pFile, strFileName.c_str(), "rb");
			break;
		case 1:
			err = fopen_s(&m_pFile, strFileName.c_str(), "wb");
			break;
		case 2:
			err = fopen_s(&m_pFile, strFileName.c_str(), "ab");
			break;
	}

	m_strFileName = strFileName;

	return err == 0;
}

int File::write(size_t nBytes, void* pBuffer) {
	if (m_pFile == nullptr || pBuffer == nullptr) {
		return -1;
	}
	return fwrite(pBuffer, 1, nBytes, m_pFile);
}

bool File::skip(size_t nBytes) {
	if (m_pFile == nullptr) {
		return false;
	}
	return fseek(m_pFile, nBytes, SEEK_CUR) == 0;
		
}

bool File::setOffset(size_t nBytes, size_t nFlag) {
	if (m_pFile == nullptr) {
		return false;
	}
	return fseek(m_pFile, nBytes, nFlag) == 0;
}

size_t File::getOffset() {
	if (m_pFile == nullptr) {
		return -1;
	}
	return ftell(m_pFile);
}

size_t File::getSize() {
	size_t cur = getOffset();
	setOffset(0, SEEK_END);
	size_t ret = getOffset();
	setOffset(cur);
	return ret;
}

int File::read(void* pBuffer, size_t nBytes) {
	if (m_pFile == nullptr || pBuffer == nullptr || nBytes <= 0) {
		return -1;
	}

	return nBytes == false ? 0 : fread(pBuffer, 1, nBytes, m_pFile);
}

unsigned char* File::read(size_t nBytes) {
	if (m_pFile == nullptr || nBytes <= 0) {
		return nullptr;
	}

	unsigned char* pBuffer = new unsigned char[nBytes];
	if (fread(pBuffer, 1, nBytes, m_pFile) == nBytes) {
		return pBuffer;
	}
	return nullptr;
}

void File::close() {
	if (m_pFile) {
		fclose(m_pFile);
	}
	m_pFile = nullptr;
}


