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


//#include "XStringList.h"

using namespace std;


class XStringList
{
public:
    struct ABM
    {
        size_t a,b;
        size_t size(){return b-a;}
    };
    vector<ABM> lines;
    
    string m_dat; //内容存储
    
public:
    XStringList(){}
    virtual ~XStringList(){}

protected:
	void ParseString(const char* _spchar = "\r\n", const char _close = '"');

public:
    
//    ssize_t Parse(const char* src);
//    ssize_t Parse2(const char* src,size_t fl);
    
    // _spchar 分隔字符串
//    size_t ParseEx(const char* lpBuf,size_t lpBufSize,const char* _spchar);
    
    /*
     _close 如果遇到这个符号 分割符号将被跳过，直到遇到第二个分割符号
     */
    size_t ParseEx2(const char* lpBuf,size_t lpBufSize=0,const char* _spchar="\r\n", const char _close='"');
    
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
    size_t size();
    
    size_t LoadFromFile(const char* fname);
    size_t LoadFromString(string src,const char* lineMark="\r\n ",const char _closeMark='"');

    virtual string operator[](size_t idx);
    string at(size_t idx);
	
	//从头开始 返回完全符合这个词的后面一个词串
	string NextLine(string str);
	
};

/*

闭合 *  "
注销 // #

*/
class XStringListEx : public XStringList 
{
public:
	list<string>  arrCloseString;   //闭合符号  /* "
	string  _curCloseString;	    //当前使用的闭合符合
	list<string>  arrIgnoreString;  //忽略行的符号 // #
	list<string>  arrLineString;	//行风格符 空格  \r \n  ;

	bool bIgnoreABSpace;			//是否忽略前后的空格
	
	XStringListEx();
	
};

/*
从最后一行开始找 找到就返回其中的值
*/
class XStringLine : public XStringListEx
{
public:
	// 找到 = 号后面的字符串 去掉后面的注释
	string GetFeildVaule(string keyname);
	
};

class XTokenizer : public XStringList
{
public:
	XTokenizer(){};

	XTokenizer(const std::string& _str, const std::string& _delim=" ");
	XTokenizer(const char* _str,std::size_t sz, const std::string& _delim=" ");
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
	const char* GetTokenValue(const std::string& keyname);

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

	size_t Parse(const char* lpBuf,ssize_t lpBufSize=-1,const char* _spchar=" ", char _close='"');
	
	void Print();

	bool FirstTokenIs(const std::string& keyname);
	bool SecondTokenIs(const std::string& keyname);

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



#endif