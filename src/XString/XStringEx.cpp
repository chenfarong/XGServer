
#include "XStringEx.h"

#include <iostream>     // std::cout
#include <algorithm>    // std::transform
#include <vector>       // std::vector
#include <functional>   // std::plus
#include <ctype.h>
#include <stdexcept>
#include <iterator>

#include <sstream>
#include <algorithm>
#include <string>
#include <string.h>
#include <cstdlib>
#include <cstdio>
#include <vector>
#include <time.h>
#include <stdarg.h>
#include <stdint.h>

using namespace std;

//
//ssize_t XStringList::Parse(const char *src)
//{
//    if (src==NULL) return -1;
//    
//    
//    size_t l = strlen(src);
//    
//    return Parse2(src, l);
//}

/*
ssize_t XStringList::Parse2(const char* src,size_t fl)
{
    if (src==NULL) return -1;
    
    aryString.clear();
    
    size_t l = fl;
    
    string str;
    for (size_t i=0; i<l; i++) {
        
        
        if (src[i]!='\r' && src[i]!='\n') str.append(&src[i],1);
        
        
        if (src[i]=='\r' || src[i]=='\n'){
            if (!str.empty()) aryString.push_back(str);
            str.clear();
        }
        
    }
    
    if (!str.empty()) aryString.push_back(str);
    
    return aryString.size();
}
*/

void test_xto_sting()
{
	std::string s = XToString(5);
	s += XToString("====");
	s += XToString(0.2f);
	std::cout << s << std::endl;
	exit(0);
}


void XStringList::ParseString(const char* _spchar /*= "\r\n"*/, const char _close /*= '"'*/)
{
	ABM abm;
	lines.clear();
	//    aryString.clear();

	size_t lpBufSize = m_dat.length();

	string sp = _spchar;
	//	size_t p=0;
	char c = 0;
	bool spl = false;
	string lsi;

	abm.a = 0;
	abm.b = 0;



	for (size_t i = 0; i < lpBufSize; i++)
	{
		c = m_dat[i];

		//是分割符
		if (c == _close)
		{
			//if (lsi.length()>0)
			if (abm.a != abm.b)
			{
				//aryString.push_back(lsi);
				//lsi.clear();
				abm.b = i;
				lines.push_back(abm);
				abm.a = i + 1;
			}
			spl = !spl;
		}

		//存在分割符还没闭合
		if (spl)
		{
			abm.b++;//lsi.push_back(c);
			continue;
		}

		//是不是分行符
		if (sp.find(c) != std::string::npos)
		{

			//            if (abm.a!=abm.b)
			if (abm.size() > 0)
			{
				abm.b = i;
				lines.push_back(abm);
				//                aryString.push_back(lsi);
				//                lsi.clear();

			}
			//abm.b++;
			//            abm.a++;
			abm.a = i + 1;
			abm.b = abm.a;
			continue;
		}
		else{
			abm.b++;//lsi.push_back(c);
		}

	}

	//    if (lsi.length()>0) aryString.push_back(lsi);
	if (abm.a != abm.b){
		abm.b++;
		lines.push_back(abm);
	}

}




/*
 
 _spchar 如果在字符串中含有分隔符 就直接分开
 */
/*
size_t XStringList::ParseEx(const char* lpBuf,size_t lpBufSize,const char* _spchar)
{
    if (lpBuf==NULL) return -1;
    
    aryString.clear();
    
    size_t l = lpBufSize;
    string sps=string(_spchar);
    string str;
    for (size_t i=0; i<l; i++) {
        
        if (sps.find(lpBuf[i])==std::string::npos){
            str.append(&lpBuf[i],1);
        }
        
        if (sps.find(lpBuf[i])!=std::string::npos){
            if (!str.empty()) aryString.push_back(str);
            str.clear();
        }
        
    }
    
    if (!str.empty()) aryString.push_back(str);
    
    return aryString.size();
}
*/

size_t XStringList::ParseEx2(const char* lpBuf,size_t lpBufSize,const char* _spchar, const char _close)
{
	if (lpBufSize<1) lpBufSize = strlen(lpBuf);
    
	m_dat.clear();
	m_dat.append(lpBuf,lpBufSize);
	
	ParseString(_spchar, _close);

    return lines.size();
    
}

//删除前后的空格
void XStringList::FixABSpace()
{
    char c=0;
    ABM abm;
    for (size_t i=0; i<lines.size(); i++) {
        abm = lines[i];
        //去掉前面的
        for (size_t k=abm.a; k<abm.b; k++) {
            c=m_dat[k];
            if (c==' ') lines[i].a++;
            else break;
        }
        //去掉后面的
        for (size_t k=abm.b-1; k>abm.a; k--) {
            c=m_dat[k];
            if (c==' ') lines[i].b--;
            else break;
        }        
    }
}

size_t XStringList::LoadFromString(string src,const char* lineMark,const char _closeMark)
{
    ParseEx2(src.c_str(),src.length(),lineMark,_closeMark);
    FixABSpace();
    return lines.size();
}



int XStringList::GetFeildLineVauleInt(string keyname)
{
    int r=0;
    string str=GetFeildLineVauleString(keyname);
    if (!str.empty()) r=strtol(str.c_str(),NULL,10);
    return r;
}

string XStringList::GetFeildLineVauleString(string keyname)
{
    if (keyname.empty()) return "";
    
    XStringList sl;
    string s,v,fi;
//    fi=string(fieldn);
//    XString::TrimLR(fi);
    
    //取＝前 和后
    for (size_t i=0; i<size(); i++) {
        sl.LoadFromString(at(i),"=");
        if(sl[0].compare(keyname)==0 && sl.size()>1)
        {
//            memset(strbuf,0,4096);
//            strncpy(strbuf, v.c_str(), min(4096, (int)v.length()));
            return sl[1];
        }
    }
    return "";
}

//ssize_t XStringList::GetVaulePos(const char* fieldn)
//{
/*    
    if (fieldn==NULL) return -1;
    string s,v,fi;
    fi=string(fieldn);
    XString::TrimLR(fi);
    
    //取＝前 和后
    for (size_t i=0; i<aryString.size(); i++) {
        XString::Split4Equal(aryString[i],s,v);
        XString::TrimLR(s);
        XString::TrimLR(v);
        if(s.compare(fi)==0 && v.length()>0){
            return i;
            break;
        }
    }
*/ 
//    return -1;
//}




void XStringList::Test()
{
    const  char* s={"1\n\r22\r\n333\"44 \r44\"  55555  \r\n666666 7 1 22\0"};
    XStringList strlist;
    strlist.LoadFromString(s);
    strlist.List();
    
    strlist.FixABSpace();
    strlist.List();
    
//    strlist.Parse(s);
//    for (size_t i=0; i<strlist.aryString.size(); i++) {
//        printf("%s\n",strlist.aryString[i].c_str());
//    } 
//    int r = strlist.GetVaule("1a");
//    printf("%s,%d\n","1a",r);
    
    
}


void XStringList::List()
{
    for (size_t i=0; i<size(); i++) {
        printf("%zd:(%s)\n",i,this->at(i).c_str());
    }
}

//void XStringList::Add(const char* text)
//{
//    if (text==NULL) return;
//    string str=string(text);
//    aryString.push_back(str);
//}

size_t XStringList::size()
{
    return lines.size();
}

void XStringList::push_back(string str)
{
	ABM abm;
	abm.a = m_dat.size();
	abm.b = str.length()+abm.a;
	lines.push_back(abm);
	m_dat.append(str);
}

size_t XStringList::LoadFromFile(const char* fname)
{
    string str,strbuf;
    string fstr=string(fname);
    ifstream infile;
    infile.open(fname,std::ios_base::in);
    
//    aryString.clear();
    
    while (infile.good())
    {
        std::getline(infile,str);
        //str.replace(0, -1, "\r");
//        unsigned long apos=str.find("\r");
//        str = str.substr(0,apos);
//fixme        aryString.push_back(str);
//		str.append("\r");
//        strbuf.append(str);
		push_back(str);
    }
    
    infile.close();
    
    //ParseEx2(strbuf.c_str(),strbuf.length(),"\r\n",'"');
    //LoadFromString(strbuf);
    return lines.size();
}

string XStringList::operator[](size_t idx)
{
    string res;
    if (idx<size()){
        ABM abm=lines[idx];
        res=m_dat.substr(abm.a,abm.size());
    }
    return res;
}



string XStringList::at(size_t idx)
{
    return operator[](idx);
}


/////

XTokenizer::XTokenizer(const std::string& _str, const std::string& _delim)
{
	m_dat = _str;

	string _dat = _str;
	_dat = TokenClear(_dat, " \r\n");
	ParseEx2(_dat.c_str(), _dat.length(), _delim.c_str(), '"');
}

XTokenizer::XTokenizer(const char* _str, std::size_t sz,const std::string& _delim)
{
	m_dat.clear();
	if (sz < 1) sz = strlen(_str);
	m_dat.append(_str,sz);
	string _dat = m_dat;
	_dat = TokenClear(_dat, " \r\n");
//	printf("%s",_dat.c_str());
	ParseEx2(_dat.c_str(), _dat.length(), _delim.c_str(), '"');
}

/*
size_t XTokenizer::Parse(const char* lpBuf,ssize_t lpBufSize,const char* _spchar, char _close)
{
    
	if (lpBuf == NULL) return 0;
	if (_spchar == NULL) _spchar = " ";

    token_str.clear();
//    m_dat=string(lpBuf);
    m_dat.clear();
    m_dat.append(lpBuf,lpBufSize);
    
	string sp=_spchar;
//	size_t p=0;
	char c=0;
	bool spl=false;
	string lsi;

	if (lpBufSize==-1) lpBufSize=strlen(lpBuf);

	for(ssize_t i=0;i<lpBufSize;i++)
	{
		c = lpBuf[i];

		if (c==_close){ 
			
			if (lsi.length()>0)
			{
				token_str.push_back(lsi);
				lsi.clear();
			}
			spl=!spl;
		}

		if (spl){
			if (c!=_close)  lsi.push_back(c);
			continue;
		}

		if ( sp.find(c)!=std::string::npos )
		{
			if (lsi.length()>0)
			{
				token_str.push_back(lsi);
				lsi.clear();
				continue;
			}
		}else
        {
            if (c!=_close ) lsi.push_back(c);
        }

	}

	if (lsi.length()>0) token_str.push_back(lsi);

	return token_str.size();

}
*/

bool XTokenizer::IsExistToken(const std::string& keyname)
{
	/*
	std::list<string>::iterator it = token_str.begin();
	for (; it != token_str.end();it++)
	{
		if (keyname.compare(*it) == 0) return true;
	}
	*/

	for(size_t i=0;i<size();i++){
		if (keyname.compare(at(i)) == 0) return true;
	}

	return false;
}

const char* XTokenizer::GetTokenValue(std::string keyname)
{
	/*
	std::list<string>::iterator it = token_str.begin();
	for (; it != token_str.end(); it++)
	{
		if (keyname.compare(*it) == 0){
			it++;
			if (it != token_str.end()){
				cur_token = it;
				return (*it).c_str();
			}
		}
	}
	*/
	for(size_t i=0;i<size();i++){
		if (at(i).compare(keyname)==0 && size()>i) return at(i+1).c_str();
	}

	return NULL;
}


int XTokenizer::GetTokenValueInt(const std::string& keyname)
{
	const char* s = GetTokenValue(keyname);
	if (s) return atoi(s);
	return 0;
}

int XTokenizer::GetTokenValueInt(unsigned int _pos)
{
/*
	if (token_str.size() < _pos) return 0;

	std::list<string>::iterator it = token_str.begin();// +_pos;

	for (unsigned int i = 0; i < _pos; i++){
		it++;
//		printf("%s\n", it->c_str());
	}
//	if (it == token_str.end()) return 0;
	//printf("==[%s]==\n", it->c_str());
	if (it->length() > 0) return atoi(it->c_str());
*/
	
	return atoi(at(_pos).c_str());
	
}

double XTokenizer::GetTokenValueFloat(const std::string& keyname)
{
	const char* s = GetTokenValue(keyname);
	if (s) return strtod(s,NULL);
	return 0;
}

void XTokenizer::Print()
{
/*
	std::list<string>::iterator it = token_str.begin();
	for (; it != token_str.end(); it++)
	{
		printf("%s\n",(*it).c_str());
	}
*/
	for(size_t i=0;i<size();i++){
		printf("%s\n",at(i).c_str());
	}
}

bool XTokenizer::FirstTokenIs(const std::string& keyname)
{
	if (size()<1) return false;
	//std::list<string>::iterator it = token_str.begin();

	//把前面的空格 末尾的空格 回车符号去掉
	string s=TokenClear(at(0));
//	printf("==%s==\n",s.c_str());
	return s.compare(keyname)==0;
}

/*
 void ToUpperString(string &str)
{
    transform(str.begin(), str.end(), str.begin(), (int (*)(int))toupper);
}
void ToLowerString(string &str)
{
    transform(str.begin(), str.end(), str.begin(), (int (*)(int))tolower);
}
 */

inline void strToLower(std::string& str)
{
	std::transform(str.begin(), str.end(), str.begin(), (int (*)(int))tolower);
}

bool XTokenizer::FirstTokenIsIcase(const std::string & keyname)
{
	if (size()<1) return false;
	string s = TokenClear(at(0));
	strToLower(s);
//	std::transform(s.begin(), s.end(), s.begin(), tolower);

	string s1 = keyname;
	strToLower(s1);
//	std::transform(s1.begin(), s1.end(), s1.begin(), tolower);
	return s.compare(s1) == 0;
}


bool XTokenizer::SecondTokenIs(const std::string& keyname)
{
	if (size()<2) return false;
	//std::list<string>::iterator it = token_str.begin();
	//it++;
	//if (it == token_str.end()) return false;
	string s = TokenClear(at(1));
	return s.compare(keyname) == 0;
}




string XTokenizer::TokenClear(string str, string rstr, int _type)
{
	if (str.length()<1) return "";
	string result=str;
	char sp;
	size_t i=0;
	//处理前面
	for(;i<str.length();i++){
		sp=str[i];
		if (rstr.find(sp)==std::string::npos) break;
	}
	result.erase(0,i);

	if (result.length()<1) return result;

	if (_type == 0) return result;

	//处理后面
	i=result.length()-1;
	for(;i>=0;i--){
		sp=result[i];
		if (rstr.find(sp)==std::string::npos) break;
	}
	i++;
	result.erase(i,result.length()-i);

	return result;
}

//还没测试
std::string XTokenizer::GetTokenRemain(const std::string& keyname)
{
	std::string result;
	string::size_type pos = m_dat.find(keyname);

	if (pos != string::npos){
		size_t _start = pos + keyname.length();
		size_t _len = m_dat.length() - _start;
		result = m_dat.substr(_start, _len);
	}

	return result;
}

size_t XTokenizer::GetTokenIndex(const string& keyname)
{
	for (size_t i = 0; i < size(); i++) {
		if (at(i).compare(keyname) == 0 ) return i;
	}
	return 0;
}

std::string XTokenizer::GetTokenValueString(const string& keyname)
{
	string res;
	//const char* s = GetTokenValue(keyname);
	//if (s) res = string(s);
	size_t idx = GetTokenIndex(keyname)+1;
	if (idx < size()) return this->at(idx);
	return res;
}

std::string XTokenizer::GetString(unsigned int _pos)
{
	/*
	if (token_str.size() < _pos) return 0;
	std::list<string>::iterator it = token_str.begin();// +_pos;

	for (unsigned int i = 0; i < _pos; i++){
		it++;
		//		printf("%s\n", it->c_str());
	}
	*/
	return TokenClear(at(_pos));
}

void test_XTokenizer()
{
	XTokenizer tok("GAME DML replace into xserver () values () ;"," \r\n");
	string str=tok.GetTokenRemain("DML");
	printf("%s\n",str.c_str());
}



////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
void XTextParamList::LoadFromFile(std::string filename)
{

	string str;
	ifstream infile;

	//FIXME 这里要读UTF8文件格式

//	valueMap.clear();

	infile.open(filename.c_str(), std::ios_base::in);

	while (infile.good())
	{
		std::getline(infile, str);
		ParseLine(str);
	}

	infile.close();
}

void XTextParamList::Parse(const char * _lpBuf, size_t _size)
{
	if (_lpBuf == NULL) return;

//	valueMap.clear();
	
	XStringList sl;
	std::string str;
	str.append(_lpBuf, _size);

	sl.LoadFromString(str);
	for (size_t i = 0; i < sl.size(); i++)
	{
		str = sl.at(i);
		ParseLine(str);
	}
}

std::string XTextParamList::GetValue(std::string keyname)
{
	return valueMap[keyname];
}

bool XTextParamList::IsExist(std::string keyname)
{
	return (valueMap.find(keyname) != valueMap.end());
}

void XTextParamList::Print()
{
	for (auto it = valueMap.begin(); it != valueMap.end(); it++)
	{
		cout << it->first << " = " << it->second << endl;
	}
}

void XTextParamList::ParseLine(std::string line)
{
	//第一个字母除空格外如果是 # 直接退出
	for (size_t i = 0; i < line.length(); i++)
	{
		char ch = line[i];
		if (::isspace(int(ch))) continue;
		if (ch == '#') return;
		break;
	}

	//找到第一个 =
	std::string::size_type pos = line.find('=');
	if (pos == std::string::npos) return;

	std::string name = line.substr(0, pos);
	std::string value = line.substr(pos + 1);

	//去除前后空格
	XString::TrimSpaceLeftAndRight(name);
	XString::TrimSpaceLeftAndRight(value);

	if (name.empty() || value.empty()) return;

	valueMap[name] = value;
	
}

std::string XTextParamList::GetValue(std::string filename, std::string keyname)
{
	XTextParamList tpl;
	tpl.LoadFromFile(filename);
	return tpl.GetValue(keyname);
}



