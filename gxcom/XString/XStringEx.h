#ifndef _XStringEx_h_
#define _XStringEx_h_

#include <string.h>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <list>
#include <map>
#include <stdint.h>

#include <string>
#include <vector>
#include <list>
#include <fstream>
#include <assert.h>
#include <cstdarg>
#include <time.h>
#include <ctype.h>
#include <sstream>
#include <algorithm>

//#include "StrFormat.h"
//#include "StrUtil.h"


using namespace std;

typedef unsigned char uchar;

class XTextFile;


class XString
{
public:
	static size_t ReplaceAll(string& str, string old_str, string new_str)
	{
		size_t rc = 0;
		size_t ol = old_str.length();
//		size_t nl = new_str.length();

		while(1) 
		{
			std::string::size_type p = str.find(old_str);
			if (p == std::string::npos) break;
			str.replace(str.begin() + p, str.begin() + p + ol, new_str);
			rc++;
		}

		return rc;
	}

	static size_t ReplaceForSQLComment(string& str)
	{
		size_t rc = ReplaceAll(str, "--", "&#45&#45");
		rc+= ReplaceAll(str, "/*", "&#47&#42");
		rc += ReplaceAll(str, "*/", "&#42&#47");
		return rc;
	}


	static inline void xsnprintf(char * buf, size_t len, const char * info, ...)
	{
		assert(len >= sizeof(char));
		int rs = 0;
		va_list args;
		va_start(args, info);
#ifdef _MSC_VER
		rs = ::_vsnprintf_s(buf, len - 1, len - 1, info, args);
#else
		rs = vsnprintf(buf, len - 1, info, args);
#endif
		va_end(args);

		if (rs<0) { buf[len - 1] = 0; }

	}


	/**
	找到 %s 的位置 判断其长度 累加 其他的留 256
	效率比较低 注意谨慎使用
	*/
	static std::string createWithFormat(const char* fmt, ...)
	{
		int rs = 0;
		const char* _start = NULL;
		const char* _end = NULL;
		size_t total = 256;
		va_list ap;
		va_start(ap, fmt);
		_end = fmt;

		do
		{
			_start = strstr(_end, "%");
			if (_start == NULL) break;
			_end = _start + 1;
			if (_end[0] == '%') continue;

			if (_end[0] == 's') {
				total += strlen(va_arg(ap, const char*));
			}
			else {
				total += 256;
				va_arg(ap, int);
			}

		} while (1);
		va_end(ap);

		va_start(ap, fmt);
		char *buf = new char[total + 1];
		//memset(buf, 0, total + 1);
#ifdef _MSC_VER
		rs = ::_vsnprintf_s_l(buf, total - 1, total - 1, fmt, NULL, ap);
		//rs = vsprintf(buf, fmt, ap);
		//rs = vsnprintf(buf, total - 1, fmt, ap);
#else
		rs = vsnprintf(buf, total - 1, fmt, ap);
#endif
		buf[rs] = 0;
		std::string s;
		s.append(buf, rs);
		delete[] buf;

		va_end(ap);
		return s;
	}


	static std::string createWithTime(time_t t)
	{
		char buf[256];
		struct tm tt = { 0 };
#if defined(_MSC_VER)
		localtime_s(&tt, &t);
#else //linux
		localtime_r(&t, &tt);
#endif

		xsnprintf(buf,256, "%d-%02d-%02d %02d:%02d:%02d",
			tt.tm_year + 1900, tt.tm_mon + 1, tt.tm_mday,
			tt.tm_hour, tt.tm_min, tt.tm_sec);
		return buf;
	}

	static std::string createWithBool(bool v)
	{
		return v ? "true" : "false";
	}

	/**
	去除前面的空格
	*/
	static size_t TrimSpaceLeft(std::string& str)
	{
		size_t k = 0;
		//string::iterator i;
		//for (i = str.begin(); i != str.end(); i++) {
		//	if (!::isspace(*i)) {
		//		break;
		//	}
		//	k++;
		//}
		//if (i == str.end()) {
		//	str.clear();
		//}
		//else {
		//	if(i!=str.begin()) str.erase(str.begin(), i);
		//}
		string ch;
		for (size_t i = 0; i < str.length(); i++)
		{
			ch = str.substr(i, 1);
			if (ch.compare(" ") == 0) { k++;continue; }
			break;
		}
		if(k>0) str.erase(0, k);
		return k;
	}

	/**
	去除后面的空格
	*/
	static size_t TrimSpaceRight(std::string& str)
	{
		size_t k = 0;
		//string::iterator i;
		//for (i = str.end() - 1; ; i--) {
		//	if (!isspace(*i)) {
		//		if(i+1!=str.end()) str.erase(i + 1, str.end());
		//		break;
		//	}
		//	k++;
		//	if (i == str.begin()) {
		//		str.clear();
		//		break;
		//	}
		//}
		string ch;
		size_t i = str.length() - 1;
		for (; i >=0; i--)
		{
			ch = str.substr(i, 1);
			if (ch.compare(" ") == 0) {
				k++; continue; 
			}
			break;
		}
		if(k>0) str.erase(i+1);
		return k;
	}

	static size_t TrimSpaceLeftAndRight(std::string& str)
	{
		//return 0;
		return TrimSpaceLeft(str) + TrimSpaceRight(str);
	}


};


template<typename T>
std::string XToString(T val)
{
	std::stringstream sss;
	sss << (T)val;
	return sss.str();
}

void test_xto_sting();

class XStringList
{
public:
/*  
	struct ABM
    {
        size_t a,b;
        size_t size(){return b-a;}
    };
*/
    vector<size_t> lines;
    
    string m_dat; //内容存储
    
public:
    XStringList(){}
    virtual ~XStringList(){}

protected:

	const char* m_xendl;
	const char* m_xclose;

	void ParseString(const char* _spchar = "\r\n", const char* _close = "\"");

public:
    
//    ssize_t Parse(const char* src);
//    ssize_t Parse2(const char* src,size_t fl);
    
    // _spchar 分隔字符串
//    size_t ParseEx(const char* lpBuf,size_t lpBufSize,const char* _spchar);
    
    /*
     _close 如果遇到这个符号 分割符号将被跳过，直到遇到第二个分割符号
     */
    size_t ParseEx2(const char* lpBuf,size_t lpBufSize=0,const char* _spchar="\r\n", const char* _close="\"");
    
public:    
    void FixABSpace();//删除前后的空格
public:    
   
    static void Test();
    
    int GetFeildLineVauleInt(string keyname);
    string GetFeildLineVauleString(string keyname);
//    ssize_t GetVaulePos(const char* fieldn);
    
    void List();
    
//    void Add(const char* text); //增加一行
	void push_back(string str);

public:
	/**
	行数
	*/
    size_t size();
    
    size_t LoadFromFile(const char* fname);
    size_t LoadFromString(string src,const char* lineMark="\r\n",const char* _closeMark="\"");

    virtual string operator[](size_t idx);
    string at(size_t idx);
	


	//从头开始 返回完全符合这个词的后面一个词串
	//string NextLine(string str);
	
};



class XTokenizer : public XStringList
{
public:
	XTokenizer(){};

	XTokenizer(const std::string& _str, const std::string& _delim=" ");
	XTokenizer(const char* _str,std::size_t sz=0, const std::string& _delim=" ");
	virtual ~XTokenizer(){};

//   int         countTokens();
//   bool        hasMoreTokens();
//   std::string nextToken();
//   int         nextIntToken();
//   double      nextFloatToken();
//   std::string nextToken(const std::string& delim);
//   std::string remainingString();
//   std::string filterNextToken(const std::string& filterStr);

	/**
	 *
	 * */
	const char* GetTokenValue(std::string keyname);

	int GetTokenValueInt(const std::string& keyname);
	double  GetTokenValueFloat(const std::string& keyname);

	//第几个位置上
	int GetTokenValueInt(unsigned int _pos);

	string GetString(unsigned int _pos);

	/**
	* 返回关键字后面的内容
	*/
	string GetTokenRemain(const std::string& keyname);

	/**
	 * 是否存在
	 * */
	bool IsExistToken(const std::string& keyname);
	
	size_t GetTokenIndex(const string& keyname);


//	size_t Parse(const char* lpBuf,ssize_t lpBufSize=-1,const char* _spchar=" ", char _close='"');
	
	void Print();

	bool FirstTokenIs(const std::string& keyname);
	bool SecondTokenIs(const std::string& keyname);

	//忽略大小写
	bool FirstTokenIsIcase(const std::string& keyname);

public:
//		string m_dat;
//   std::list<string>  token_str;
//   std::string  delim;
   static string TokenClear(string str,string rstr=" \r\n",int _type=1);
   string GetTokenValueString(const string& keyname);
protected:
//	std::list<string>::iterator cur_token;
	
};

void test_XTokenizer();

/**
文本参数
#字母开头的本行注释掉
=分开前后 如果行中不存在 = 抛弃
*/
class XTextParamList
{

	std::map<std::string, std::string> valueMap;
protected:
	
	void ParseLine(std::string line);

public:
	static std::string GetValue(std::string filename,std::string keyname);

	void LoadFromAsciiFile(std::string filename);
	void Parse(const char* _lpBuf, size_t _size);

	std::string GetValue(std::string keyname);
	std::wstring GetValueWString(std::string keyname);

	bool IsExist(std::string keyname);

	void Print();

	void LoadFile(const char* filename);

};

namespace XStringUtil
{
	bool utf8CharToUcs2Char(const char* utf8Tok, wchar_t* ucs2Char, unsigned int* utf8TokLen);
	void ucs2CharToUtf8Char(const wchar_t ucs2Char, char* utf8Tok);
	std::wstring utf8ToUcs2(const std::string& utf8Str);
	std::string ucs2ToUtf8(const std::wstring& ucs2Str);
};


class XTextFile
{
protected:
	uchar* dat;
	size_t		len;

public:
	static FILE* callfopen(const char* filepath, const char* mode)
	{
		assert(filepath);
		assert(mode);
#if defined(_MSC_VER) && (_MSC_VER >= 1400 ) && (!defined WINCE)
		FILE* fp = 0;
		errno_t err = fopen_s(&fp, filepath, mode);
		if (err) {
			return 0;
		}
#else
		FILE* fp = fopen(filepath, mode);
#endif
		return fp;
	}

public:
	XTextFile();
	virtual ~XTextFile();

	bool LoadFile(const char* filename);
	int LoadFile(FILE*);

	//operator std::string();
	const char* c_str() { return (const char*)dat; };
	size_t size() { return len; };

};

#endif