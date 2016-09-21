#include "stdafx.h"
#include "FileSystem.h"
#include "zconf.h"
#include "zlib.h"

using namespace std;

void dec(char* szBuffer, int nSize) {
	if (!szBuffer) return;

	BYTE b;

	char key[18] = { 15, -81, 42, 3, -123, 66, -109, 103, -46, -36, -94, 64, -115, 113, -103, -9, -65, -103 };
	for (int i = 0; i < nSize; ++i)
	{
		char xor = key[i % 18];
		b = *szBuffer;
		b ^= xor;
		*szBuffer = b;
		szBuffer++;
	}
}

void createDir(const char * szPath)
{
	char *work = _strdup(szPath), *ptr;

	for (ptr = work; *ptr; ++ptr)
	{
		if (*ptr == '\\')
		{
			*ptr = 0;
			CreateDirectoryA(work, NULL);
			*ptr = '\\';
		}
	}
	free(work);
}

int main() {

	printf("MasangSoft MRS\n");

begin:
	cout << "\nPlease enter the file name: ";
	string strFile;
	getline(cin, strFile);

	File* pFile = new File();
	if(!pFile->open(strFile + ".mrs")) {
		cout << "File not found\n";
		goto begin;
	}

	pFile->setOffset(-(int)sizeof(MZIPDIRHEADER), SEEK_END);
	size_t offSet = pFile->getOffset();

	MZIPDIRHEADER dh;
	pFile->read((char*)&dh, sizeof(MZIPDIRHEADER));
	dec((char*)&dh, sizeof(MZIPDIRHEADER));

	if (dh.sig != MRS2_ZIP_CODE && dh.sig != MRS_ZIP_CODE) {
		cout << "File is not valid wrong signature " << dh.sig << "\n";
		goto begin;
	}

	pFile->setOffset(offSet - dh.dirSize, SEEK_SET);

	char* szBuffer = new char[dh.dirSize + dh.nDirEntries * sizeof(MZIPDIRFILEHEADER)];
	pFile->read(szBuffer, dh.dirSize);
	dec(szBuffer, dh.dirSize);

	MZIPDIRFILEHEADER ** pDir = (MZIPDIRFILEHEADER **)(szBuffer + dh.dirSize);

	char* szTemp = move(szBuffer);
	for (int i = 0; i < dh.nDirEntries; i++) {
		MZIPDIRFILEHEADER& p = *(MZIPDIRFILEHEADER*)szTemp;

		if (p.sig != MZIPDIRFILEHEADER::SIGNATURE && p.sig != MZIPDIRFILEHEADER::SIGNATURE2)
		{
			cout << "Dir File header wrong signature " << p.sig << "\n";
			break;
		}

		szTemp += sizeof(p);

		for (int j = 0; j < p.fnameLen; j++) {
			if (szTemp[j] == '/') {
				szTemp[j] = '\\';
			}
		}

		pDir[i] = &p;

		szTemp += p.fnameLen + p.xtraLen + p.cmntLen;
	}

#define GetFileName(i, szDest) \
{ \
	if (szDest != NULL) \
	{ \
		memcpy(szDest, pDir[i]->GetName(), pDir[i]->fnameLen); \
		szDest[pDir[i]->fnameLen] = '\0'; \
	} \
} \

	for (int i = 0; i < dh.nDirEntries; i++)
	{
		char szFileName[_MAX_PATH];
		GetFileName(i, szFileName);

		MZIPDIRFILEHEADER* dh = pDir[i];
	
		pFile->setOffset(dh->hdrOffset, SEEK_SET);

		MZIPLOCALHEADER lh;
		pFile->read(&lh, sizeof(MZIPLOCALHEADER));
		dec((char*)&lh, sizeof(MZIPLOCALHEADER));

		if (lh.sig != MZIPLOCALHEADER::SIGNATURE && lh.sig != MZIPLOCALHEADER::SIGNATURE2) {
			cout << "Local header wrong signature " << lh.sig << "\n";
			break;
		}

		pFile->skip(lh.fnameLen + lh.xtraLen);

		if (lh.compression != MZIPLOCALHEADER::COMP_DEFLAT) {
			cout << "Local header wrong compression (" << lh.compression << ")\n";
			break;
		}

		char* szBufferC = new char[lh.cSize];
		pFile->read(szBufferC, lh.cSize);

		char* szBufferU = new char[lh.ucSize];
		z_stream stream;

		stream.zalloc = (alloc_func)0;
		stream.zfree = (free_func)0;
		stream.opaque = (voidpf)0;

		stream.next_in = (Bytef*)szBufferC;
		stream.avail_in = (uInt)lh.cSize;
		stream.next_out = (Bytef*)szBufferU;
		stream.avail_out = min(dh->ucSize, lh.ucSize);

		int err = inflateInit2(&stream, -MAX_WBITS);
		if (err == Z_OK)
		{
			err = inflate(&stream, Z_FINISH);
			inflateEnd(&stream);
			if (err == Z_STREAM_END)
			{
				err = Z_OK;
			}
			inflateEnd(&stream);
		}

		if (err == Z_OK)
		{
			char szFinalPath[256];
			sprintf_s(szFinalPath, "MRS\\%s\\%s", strFile.c_str(), szFileName);

			for (int j = 0; j < sizeof(szFinalPath); j++)
			{
				if (szFinalPath[j] == '/')
					szFinalPath[j] = '\\';
			}

			createDir(szFinalPath);
			printf("%s\n", szFinalPath);

			File* pTemp = new File();
			if (!pTemp->open(szFinalPath, FF_WRITE)) {
				cout << "Failed to write " << szFinalPath << "\n";
				continue;
			}

			pTemp->write(dh->ucSize, szBufferU);
			pTemp->close();
		}

		delete[] szBufferC;
		delete[] szBufferU;
	}

	pFile->close();

	goto begin;

    return 0;
}