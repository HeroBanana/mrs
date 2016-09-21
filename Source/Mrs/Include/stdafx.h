#pragma once

#include <Windows.h>
#include <string>
#include <vector>
#include <functional>
#include <algorithm>
#include <tchar.h>
#include <io.h>
#include <iostream>

typedef unsigned long dword;
typedef unsigned short word;

#define MRS_ZIP_CODE	0x05030207
#define MRS2_ZIP_CODE	0x05030208

#pragma pack(2)
struct MZIPLOCALHEADER {
	enum {
		SIGNATURE = 0x04034b50,
		SIGNATURE2 = 0x85840000,
		COMP_STORE = 0,
		COMP_DEFLAT = 8,
	};

	dword   sig;
	word    version;
	word    flag;
	word    compression;      // COMP_xxxx
	word    modTime;
	word    modDate;
	dword   crc32;
	dword   cSize;
	dword   ucSize;
	word    fnameLen;         // Filename string follows header.
	word    xtraLen;          // Extra field follows filename.
};

struct MZIPDIRHEADER {
	enum {
		SIGNATURE = 0x06054b50,
	};

	dword   sig;
	word    nDisk;
	word    nStartDisk;
	word    nDirEntries;
	word    totalDirEntries;
	dword   dirSize;
	dword   dirOffset;
	word    cmntLen;
};

struct MZIPDIRFILEHEADER {
	enum {
		SIGNATURE = 0x02014b50,
		SIGNATURE2 = 0x05024b80,
		COMP_STORE = 0,
		COMP_DEFLAT = 8,
	};

	dword   sig;
	word    verMade;
	word    verNeeded;
	word    flag;
	word    compression;      // COMP_xxxx
	word    modTime;
	word    modDate;
	dword   crc32;
	dword   cSize;            // Compressed size
	dword   ucSize;           // Uncompressed size
	word    fnameLen;         // Filename string follows header.
	word    xtraLen;          // Extra field follows filename.
	word    cmntLen;          // Comment field follows extra field.
	word    diskStart;
	word    intAttr;
	dword   extAttr;
	dword   hdrOffset;

	char *GetName() const { return (char *)(this + 1); }
	char *GetExtra() const { return GetName() + fnameLen; }
	char *GetComment() const { return GetExtra() + xtraLen; }
};