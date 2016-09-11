

#include "utf8util.h"

namespace utf8util
{
int IsTextUTF8(const char* str, ULONGLONG length)
{
	int i;
	DWORD nBytes = 0;//UFT8可用1-6个字节编码,ASCII用一个字节
	UCHAR chr;
	BOOL bAllAscii = TRUE; //如果全部都是ASCII, 说明不是UTF-8
	for (i = 0; i < length; i++)
	{
		chr = *(str + i);
		if ((chr & 0x80) != 0) // 判断是否ASCII编码,如果不是,说明有可能是UTF-8,ASCII用7位编码,但用一个字节存,最高位标记为0,o0xxxxxxx
			bAllAscii = FALSE;
		if (nBytes == 0) //如果不是ASCII码,应该是多字节符,计算字节数
		{
			if (chr >= 0x80)
			{
				if (chr >= 0xFC && chr <= 0xFD)
					nBytes = 6;
				else if (chr >= 0xF8)
					nBytes = 5;
				else if (chr >= 0xF0)
					nBytes = 4;
				else if (chr >= 0xE0)
					nBytes = 3;
				else if (chr >= 0xC0)
					nBytes = 2;
				else
				{
					return FALSE;
				}
				nBytes--;
			}
		}
		else //多字节符的非首字节,应为 10xxxxxx
		{
			if ((chr & 0xC0) != 0x80)
			{
				return FALSE;
			}
			nBytes--;
		}
	}
	if (nBytes > 0) //违返规则
	{
		return FALSE;
	}
	if (bAllAscii) //如果全部都是ASCII, 说明不是UTF-8
	{
		return FALSE;
	}
	return TRUE;
}


bool Utf8toWStr(const std::string& utf8str, std::wstring& wstr)
{
	try
	{
		size_t len = utf8::distance(utf8str.c_str(), utf8str.c_str() + utf8str.size());
		wstr.resize(len);

		if (len)
			utf8::utf8to16(utf8str.c_str(), utf8str.c_str() + utf8str.size(), &wstr[0]);
	}
	catch (...)
	{
		wstr = L"";
		return false;
	}

	return true;
}

size_t utf8length(std::string& utf8str)
{
	try
	{
		return utf8::distance(utf8str.c_str(), utf8str.c_str() + utf8str.size());
	}
	catch (...)
	{
		utf8str = "";
		return 0;
	}
}

bool WStrToUtf8(std::wstring wstr, std::string& utf8str)
{
	try
	{
		std::string utf8str2;
		utf8str2.resize(wstr.size() * 4);                   // allocate for most long case

		char* oend = utf8::utf16to8(wstr.c_str(), wstr.c_str() + wstr.size(), &utf8str2[0]);
		utf8str2.resize(oend - (&utf8str2[0]));             // remove unused tail
		utf8str = utf8str2;
	}
	catch (...)
	{
		utf8str = "";
		return false;
	}

	return true;
}


//inline static bool IsUTF8Continuation(char p) {
//	return (p & 0x80) != 0;
//}

// Anything in the high order range of UTF-8 is assumed to not be whitespace. This isn't
// correct, but simple, and usually works.
//static bool IsWhiteSpace(char p) {
//	return !IsUTF8Continuation(p) && isspace(static_cast<unsigned char>(p));
//}

const char* ReadBOM(const char* p, bool* bom)
{
	XASSERT(p);
	XASSERT(bom);
	*bom = false;
	const unsigned char* pu = reinterpret_cast<const unsigned char*>(p);
	// Check for BOM:
	if (*(pu + 0) == 0xefU
		&& *(pu + 1) == 0xbbU
		&& *(pu + 2) == 0xbfU) {
		*bom = true;
		p += 3;
	}
	XASSERT(p);
	return p;
}

//const char* SkipWhiteSpace(const char* p)
//{
//	XASSERT(p);
//	while (IsWhiteSpace(*p)) {
//		++p;
//	}
//	XASSERT(p);
//	return p;
//}




}


