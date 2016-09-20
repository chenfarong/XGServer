

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



	// https://msdn.microsoft.com/en-us/library/k1f9b8cy.aspx

	std::string ws2s(const std::wstring& ws, char const* _Locale)
	{
		size_t   i;
		size_t requiredSize = 0;
		std::string curLocale = setlocale(LC_ALL, NULL); // curLocale = "C";
														 //	setlocale(LC_ALL, "chs"); //设置成中文
		if (_Locale == NULL) _Locale = "zh-CN";
		setlocale(LC_ALL, _Locale); //设置成中文
		const wchar_t* _Source = ws.c_str();

		/*
		#if defined(_WINDOWS)
		int rc = wcstombs_s(&requiredSize,NULL,ws.length(), _Source, 0) + 1; // C4996
		#else
		requiredSize = wcstombs(NULL, _Source, 0) + 1; // C4996
		#endif
		*/
		//requiredSize = wcstombs(NULL, _Source, 0) + 1; // C4996
		// Note: wcstombs is deprecated; consider using wcstombs_s

		//wcstombs_s(&requiredSize,0, NULL, _Source, INT32_MAX);
#if defined(_MSC_VER)
		wcstombs_s(&requiredSize, NULL, 0, _Source, 0);
		requiredSize += 1;
#else
		requiredSize = wcstombs(NULL, ws.c_str(), 0) + 1;
#endif


		//	size_t _Dsize = 2 * ws.size() + 1;
		char *_Dest = new char[requiredSize];
		//	memset(_Dest, 0, _Dsize);

#if defined(_MSC_VER)
		wcstombs_s(&i, _Dest, requiredSize - 1, _Source, requiredSize - 1);
#else
		wcstombs(_Dest, _Source, requiredSize);
#endif


		_Dest[requiredSize - 1] = '\0';

		std::string result = _Dest;
		delete[]_Dest;
		setlocale(LC_ALL, curLocale.c_str());

		return result;
	}

	std::wstring s2ws(const std::string& s, char const* _Locale)
	{
		size_t requiredSize = 0;

		std::string curLocale = setlocale(LC_ALL, NULL); // curLocale = "C";
		if (_Locale == NULL) _Locale = "zh-CN";
		setlocale(LC_ALL, _Locale); //设置成中文

		const char* _Source = s.c_str();

		//requiredSize = mbstowcs(NULL, _Source, 0)+1; // C4996
		///* Add one to leave room for the NULL terminator */
#if defined(_MSC_VER)
		mbstowcs_s(&requiredSize, NULL, 0, _Source, 0);
		requiredSize += 1;
#else
		requiredSize = mbstowcs(NULL, _Source, 0) + 1;
#endif

		//size_t _Dsize = s.size() + 1;
		wchar_t *_Dest = new wchar_t[requiredSize];
		//wmemset(_Dest, 0, _Dsize);
		//mbstowcs(_Dest, _Source, requiredSize);
#if defined(_MSC_VER)
		mbstowcs_s(&requiredSize, _Dest, requiredSize - 1, _Source, strlen(_Source));
#else
		mbstowcs(_Dest, _Source, requiredSize);
#endif

		_Dest[requiredSize - 1] = '\0';
		std::wstring result = _Dest;
		delete[]_Dest;

		setlocale(LC_ALL, curLocale.c_str());
		return result;
	}


}


