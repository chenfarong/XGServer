

#include "attrib.h"

#include <string>
#include <sstream>
#include <locale.h>
#include <fstream>
#include <bitset>
#include <stdint.h>

#include "utf8util.h"

using namespace std;


template<class T>
std::string XToString(T _value)
{
	std::stringstream ss;
	ss << (T)_value;
	return ss.str();
}

template<class T>
T XStringTo(const char* _src, size_t _l)
{
	T r;
	std::stringstream ss;
	ss.write(_src, _l);
	ss >> r;
	return r;
}


typedef uint16_t g_size_t;

namespace CxStrUtil
{
	inline size_t wslength(const wstring& ws)
	{
		size_t requiredSize = 0;
		string curLocale = setlocale(LC_ALL, NULL); // curLocale = "C";
		setlocale(LC_ALL, "zh-CN"); //设置成中文
#if defined(_MSC_VER)
		wcstombs_s(&requiredSize, NULL, 0, ws.c_str(), 0);
#else
		requiredSize = wcstombs(NULL, ws.c_str(), 0) + 1;
#endif
		setlocale(LC_ALL, curLocale.c_str());
		return requiredSize;
	}
};

// https://msdn.microsoft.com/en-us/library/k1f9b8cy.aspx

std::string ws2s(const std::wstring& ws)
{
	size_t   i;
	size_t requiredSize=0;
	std::string curLocale = setlocale(LC_ALL, NULL); // curLocale = "C";
//	setlocale(LC_ALL, "chs"); //设置成中文
	setlocale(LC_ALL, "zh-CN"); //设置成中文
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


	_Dest[requiredSize-1] = '\0';

	std::string result = _Dest;
	delete[]_Dest;
	setlocale(LC_ALL, curLocale.c_str());

	return result;
}

std::wstring s2ws(const std::string& s)
{
	size_t requiredSize = 0;
	std::string curLocale = setlocale(LC_ALL, NULL); // curLocale = "C";
	setlocale(LC_ALL, "zh-CN");
	const char* _Source = s.c_str();
	
	//requiredSize = mbstowcs(NULL, _Source, 0)+1; // C4996
	///* Add one to leave room for the NULL terminator */
#if defined(_MSC_VER)
	mbstowcs_s(&requiredSize, NULL, 0, _Source, 0);
	requiredSize += 1;
#else
	requiredSize = mbstowcs(NULL, _Source, 0)+1;
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

	_Dest[requiredSize-1] = '\0';
	std::wstring result = _Dest;
	delete[]_Dest;

	setlocale(LC_ALL, curLocale.c_str());
	return result;
}


//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////

CxVar::CxVar()
{
	type = V_TYPE_UNKNOWN;
	//s = NULL;
	//ws = NULL;
	//blob = NULL;
	//box = NULL;
	//method = NULL;
	//varMap = NULL;
	//varList = NULL;
	clear();
}

//CxVar::CxVar(const char* _value)
//{
//	assert(_value);
//	type = V_TYPE_STRING;
//	if (_value) {
//		s = new std::string(_value);
//	}
//}

CxVar::CxVar(const float _value)
{
	type = V_TYPE_FLOAT;
	f = _value;
}

CxVar::CxVar(const double _value)
{
	type = V_TYPE_DOUBLE;
	d = _value;
}

CxVar::CxVar(int32_t _value)
{
	type = V_TYPE_I32;
	i32 = _value;
}


//CxVar::CxVar(int _value)
//{
//	type = V_TYPE_I32;
//	i32 = _value;
//}

CxVar::CxVar(int8_t _value)
{
	type = V_TYPE_I8;
	i8 = _value;
}

CxVar::CxVar(int16_t _value)
{
	type = V_TYPE_I16;
	i16 = _value;
}

CxVar::CxVar(int64_t _value)
{
	type = V_TYPE_I64;
	i64 = _value;
}

CxVar::CxVar(uint8_t _value)
{
	type = V_TYPE_U8;
	u8 = _value;
}

CxVar::CxVar(uint16_t _value)
{
	type = V_TYPE_U16;
	u16 = _value;
}

CxVar::CxVar(uint32_t _value)
{
	type = V_TYPE_U32;
	u32 = _value;
}

CxVar::CxVar(uint64_t _value)
{
	type = V_TYPE_U64;
	u64 = _value;
}

CxVar::CxVar(const std::string _value)
{
	type = V_TYPE_STRING;
	s = NULL;
	*this = _value;
}

CxVar::CxVar(const std::wstring _value)
{
	type = V_TYPE_WSTRING;
	ws = NULL;
	*this = _value;
}

CxVar::CxVar(char* _lpBuf, unsigned long _size)
{
	type = V_TYPE_BLOB;
	blob = NULL;
	SetBlob(_lpBuf, _size);
}


CxVar::CxVar(const CxVarList& _value)
{
	type = V_TYPE_LIST;
	varList = NULL;
	*this = _value;
}

CxVar::CxVar(const CxVarFieldMap& _value)
{
	type = V_TYPE_MAP;
	varMap = NULL;
	*this = _value;
}

CxVar::CxVar(const CxVar& _val)
{
	this->CopyFrom(&_val);
}

//CxVar::CxVar(void* _value)
//{
//	type = V_TYPE_VOID;
//	SetPtr(_value);
//}

CxVar::CxVar(const CxVarBox & _value)
{
	type = V_TYPE_BOX;
	this->box = new CxVarBox();
	memcpy(this->box, &_value, sizeof(CxVarBox));
}



VAR_TYPE CxVar::GetVarType(const char * _typeName)
{
	if (_typeName)
	{
		if (strcmp("string", _typeName) == 0) return V_TYPE_STRING;
		if (strcmp("int8", _typeName) == 0) return V_TYPE_I8;
		if (strcmp("int16", _typeName) == 0) return V_TYPE_I16;
		if (strcmp("int32", _typeName) == 0) return V_TYPE_I32;
		if (strcmp("int64", _typeName) == 0) return V_TYPE_I64;
		if (strcmp("uint8", _typeName) == 0) return V_TYPE_U8;
		if (strcmp("uint16", _typeName) == 0) return V_TYPE_U16;
		if (strcmp("uint32", _typeName) == 0) return V_TYPE_U32;
		if (strcmp("uint64", _typeName) == 0) return V_TYPE_U64;
		if (strcmp("wstring", _typeName) == 0) return V_TYPE_WSTRING;
		if (strcmp("float", _typeName) == 0) return V_TYPE_FLOAT;
		if (strcmp("double", _typeName) == 0) return V_TYPE_DOUBLE;

		if (strcmp("list", _typeName) == 0) return V_TYPE_LIST;
		if (strcmp("map", _typeName) == 0) return V_TYPE_MAP;
		if (strcmp("blob", _typeName) == 0) return V_TYPE_BLOB;

		if (strcmp("box", _typeName) == 0) return V_TYPE_BOX;
		if (strcmp("pointer", _typeName) == 0) return V_TYPE_VOID;
	}
	return V_TYPE_UNKNOWN;
}

CxVar * CxVar::createWithTypeName(const char * _typeName)
{
	assert(_typeName);
	int type = GetVarType(_typeName);

	assert(V_TYPE_UNKNOWN != type);

	return createWithType(type);
}

CxVar * CxVar::createWithType(int _type)
{
	CxVar* _var = new CxVar();
	_var->type = _type;

	if (_var->type == V_TYPE_LIST) _var->varList = new CxVarList();
	if (_var->type == V_TYPE_MAP) _var->varMap = new CxVarFieldMap();
	if (_var->type == V_TYPE_BOX) _var->box = new CxVarBox();

	
	return _var;
}






//
//void GxCntPropertySaveToXml(CxVarMap& _Cnt,std::ostream& _Str)
//{
//	if (_Cnt.size() == 0) return;
//
//	_Str << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" <<std::endl;
//	_Str << "<Property>" << std::endl;
//
//	CxVarMapIterator it = _Cnt.begin();
//	for (;it != _Cnt.end();it++)
//	{
//
//		_Str << "<p name=\""<< it->first.name <<"\"  value=\"" << it->second->ToString() << "\"  />" << std::endl;
//	}
//	_Str << "</Property>" << std::endl;
//}




CxVar& CxVar::operator=(const CxVar& _val)
{
	this->CopyFrom(&_val);
	return *this;
}

CxVar & CxVar::operator=(const bool & _val)
{
	assert(type == V_TYPE_I32);
	i32 = _val;
	return *this;
}

CxVar & CxVar::operator=(const float & _val)
{
	assert(type == V_TYPE_FLOAT);
	f = _val;
	return *this;
}

CxVar & CxVar::operator=(const double & _val)
{
	assert(type == V_TYPE_DOUBLE);
	d = _val;
	return *this;
}

CxVar& CxVar::operator=(const int& _val)
{
	assert(type == V_TYPE_I32);
	i32 = _val;
	return *this;
}

CxVar& CxVar::operator=(const int8_t& _val)
{
	assert(type == V_TYPE_I8);
	i8 = _val;
	return *this;
}

CxVar& CxVar::operator=(const int16_t& _val)
{
	assert(type == V_TYPE_I16);
	i16 = _val;
	return *this;
}

CxVar& CxVar::operator=(const int64_t& _val)
{
	assert(type == V_TYPE_I64);
	i64 = _val;
	return *this;
}

CxVar& CxVar::operator=(const uint8_t& _val)
{
	assert(type == V_TYPE_U8);
	u8 = _val;
	return *this;
}

CxVar& CxVar::operator=(const uint16_t& _val)
{
	assert(type == V_TYPE_U16);
	u16 = _val;
	return *this;
}

CxVar& CxVar::operator=(const uint32_t& _val)
{
	assert(type == V_TYPE_U32);
	u32 = _val;
	return *this;
}
CxVar& CxVar::operator=(const uint64_t& _val)
{
	assert(type == V_TYPE_U64);
	u64 = _val;
	return *this;
}


CxVar& CxVar::operator=(const string& _val)
{
	assert(type == V_TYPE_STRING);
	clear();
	type = V_TYPE_STRING; // 
	s = new std::string(_val);
	return *this;
}
CxVar& CxVar::operator=(const std::wstring& _val)
{
	assert(type == V_TYPE_WSTRING);
	clear();
	type = V_TYPE_WSTRING;
	ws = new std::wstring(_val);
	return *this;
}

CxVar& CxVar::operator=(const CxVarList&  _val)
{
	assert(type == V_TYPE_LIST);
	clear();
	type = V_TYPE_LIST;
	varList = new CxVarList();

	//这里进行列表中的内容复制
	for (auto it = _val.begin(); it != _val.end(); it++)
	{
		varList->push_back(new CxVar(*(*it)));
	}

	return *this;
}


CxVar& CxVar::operator=(const CxVarFieldMap& _val)
{
	assert(type == V_TYPE_MAP);
	this->SetVarMap(_val);
	return *this;
}



CxVar::~CxVar()
{
	//printf("%s----%s\n",__FUNCTION__, this->ToString().c_str());
	clear();
}


void CxVar::clear()
{
	switch (type)
	{
	case V_TYPE_WSTRING:
		XATTR_SAFE_DELETE(ws);
	case V_TYPE_STRING:
		XATTR_SAFE_DELETE(s);
		break;
	case V_TYPE_BLOB:
		XATTR_SAFE_DELETE(blob);
		break;
	case V_TYPE_BOX:
		XATTR_SAFE_DELETE(box);
		break;
	case V_TYPE_LIST:
		if (varList) 
		{
			while (!varList->empty())
			{
				auto it = varList->begin();
				delete *it;
				varList->erase(it);
			}
			XATTR_SAFE_DELETE(varList);
		}
		break;
	case V_TYPE_MAP:
	
		if (varMap) 
		{

			while (!varMap->empty()) {
				auto it = varMap->begin();
				CxVar* data = it->second.value;
				if(data) delete data;
				varMap->erase(it);
			}
			XATTR_SAFE_DELETE(varMap);
		}

		break;
	default:
		break;
	}

//	type = V_TYPE_UNKNOWN; //一定不要执行此行
	s = NULL;
	ws = NULL;
	blob = NULL;
	box = NULL;
	method = NULL;
	varMap = NULL;
	varList = NULL;

	u64 = 0;

}

void CxVar::SetInt(int _value)
{
	assert(type == V_TYPE_I32);
	i32 = _value;
}

void CxVar::SetBool(bool _value)
{
	assert(type == V_TYPE_I32);
	i32 = _value;
}

//////////////////////////////////////////////////////////////////////////

void CxVar::SetFloat(float _value)
{
	assert(type == V_TYPE_FLOAT);
	f = _value;
}

void CxVar::SetDouble(double _value)
{
	assert(type == V_TYPE_DOUBLE);
	d = _value;
}

void CxVar::SetI8(int8_t _value)
{
	assert(type == V_TYPE_I8);
	i8 = _value;
}
void CxVar::SetI16(int16_t _value)
{
	assert(type == V_TYPE_I16);
	i16 = _value;
}
void CxVar::SetI32(int32_t _value)
{
	assert(type == V_TYPE_I32);
	i32 = _value;
}

void CxVar::SetI64(int64_t _value)
{
	assert(type == V_TYPE_I64);
	i64 = _value;
}

void CxVar::SetU8(uint8_t _value)
{
	assert(type == V_TYPE_U8);
	u8 = _value;
}
void CxVar::SetU16(uint16_t _value)
{
	assert(type == V_TYPE_U16);
	u16 = _value;
}
void CxVar::SetU32(uint32_t _value)
{
	assert(type == V_TYPE_U32);
	u32 = _value;
}
void CxVar::SetU64(uint64_t _value)
{
	assert(type == V_TYPE_U64);
	u64 = _value;
}

void CxVar::SetBlob(char* _lpBuf, unsigned long _size)
{
	assert(type == V_TYPE_BLOB);
	clear();
	type = V_TYPE_BLOB;
	blob = new CxVarBlob(_lpBuf, _size);
}
void CxVar::SetBox(uint64_t tag, int32_t p0, int32_t p1)
{
	assert(type == V_TYPE_BOX);
	clear();
	type = V_TYPE_BOX;
	box = new CxVarBox{ tag, p0, p1 };
}

void CxVar::SetString(std::string _value){
	assert(type == V_TYPE_STRING);
	clear();
	type = V_TYPE_STRING;
	s = new string(_value);
}

void CxVar::SetWString(std::wstring _value){
	assert(type == V_TYPE_WSTRING);
	clear();
	type = V_TYPE_WSTRING;
	ws = new wstring(_value);
}



void CxVar::SetPtr(void* _value){
	assert(type == V_TYPE_VOID);
	this->method = _value;
}



void CxVar::SetVarList(const CxVarList& _value)
{
	type = V_TYPE_LIST;
	varList = new CxVarList();
	for (auto it = _value.begin(); it != _value.end(); it++)
	{
		CxVar* v =new CxVar(**it);
		varList->push_back(v);
	}
}

void CxVar::SetVarMap(const CxVarFieldMap& _value)
{
	type = V_TYPE_MAP;
	clear();
	varMap = new CxVarFieldMap();
	for (auto it = _value.begin(); it != _value.end(); it++)
	{
		(*varMap)[it->first].value = new CxVar();// new CxVar(*(it->second.value));
		(*varMap)[it->first].value->CopyFrom(it->second.value);
	}
}


void CxVar::SetValueFrom(const char * _src)
{
	if (type == V_TYPE_STRING
		|| type == V_TYPE_WSTRING)
		clear();

	if (_src == NULL) return;

	assert(type != V_TYPE_UNKNOWN);

	size_t l = strlen(_src);
	
	switch (type) {
	case V_TYPE_I8:
		i8 = XStringTo<int8_t>(_src, l);
		break;
	case V_TYPE_I16:
		i16= XStringTo<int16_t>(_src, l);
		break;

	case V_TYPE_I32:
		i32 = XStringTo<int32_t>(_src, l);
		break;
	case V_TYPE_I64:
		i64 = XStringTo<int64_t>(_src, l);
		break;
	case V_TYPE_U8:
		u8 = XStringTo<uint8_t>(_src, l);
		break;
	case V_TYPE_U16:
		u16 = XStringTo<uint16_t>(_src, l);
		break;
	case V_TYPE_U32:
		u32 = XStringTo<uint32_t>(_src, l);
		break;
	case V_TYPE_U64:
		u64 = XStringTo<uint64_t>(_src, l);
		break;
	case V_TYPE_DOUBLE:
		d = XStringTo<double>(_src, l);
		break;
	case V_TYPE_FLOAT:
		f = XStringTo<float>(_src, l);
		break;

	case V_TYPE_STRING:
		s = new std::string(_src);
		break;
	case V_TYPE_WSTRING:
		std::wstring wstr= s2ws(std::string(_src));
		ws = new std::wstring(wstr);
		break;
	}

}

void CxVar::CopyFrom(const CxVar * _var)
{
	if (_var == NULL) return;
	clear();

	type = _var->type;

	if (_var->type == V_TYPE_LIST)
	{
		this->SetVarList(*_var->varList);
		return;
	}

	if (_var->type == V_TYPE_MAP)
	{
		SetVarMap(*_var->varMap);
		return;
	}

	//memcpy(this, &_val, sizeof(CxVar)); //导致程序错误

	switch (_var->type)
	{
	case V_TYPE_STRING:
	{
		if (_var->s) s = new std::string(*_var->s); else s = NULL;
	}
	break;
	case V_TYPE_WSTRING:
	{
		if (_var->ws) ws = new std::wstring(*_var->ws); else ws = NULL;
	}
	break;
	case V_TYPE_BLOB:
		blob = new CxVarBlob(_var->blob->dat, _var->blob->len);
		break;
	case V_TYPE_BOX:
		box = new CxVarBox();
		memcpy(box, _var->box, sizeof(CxVarBox));
		break;
	case V_TYPE_I8:
		i8 = _var->i8;
		break;
	case V_TYPE_I16:
		i16 = _var->i16;
		break;
	case V_TYPE_I32:
		i32 = _var->i32;
		break;
	case V_TYPE_I64:
		i64 = _var->i64;
		break;
	case V_TYPE_U8:
		u8 = _var->u8;
		break;
	case V_TYPE_U16:
		u16 = _var->u16;
		break;
	case V_TYPE_U32:
		u32 = _var->u32;
		break;
	case V_TYPE_U64:
		u64 = _var->u64;
		break;
	case V_TYPE_DOUBLE:
		d = _var->d;
		break;
	case V_TYPE_FLOAT:
		f = _var->f;
		break;
	}
}



std::string CxVar::ToString()
{

	if (type == V_TYPE_STRING) return s == NULL ? "" : (*s);
	if (type == V_TYPE_WSTRING) return ws == NULL ? "" : ws2s(*ws);

	if (type == V_TYPE_I8) return XToString<int32_t>((int32_t)i8);
	if (type == V_TYPE_I16) return XToString<int32_t>(i16);
	if (type == V_TYPE_I32) return XToString<int32_t>(i32);
	if (type == V_TYPE_I64) return XToString<int64_t>(i64);

	if (type == V_TYPE_U8) return XToString<uint32_t>((uint32_t)u8);
	if (type == V_TYPE_U16) return XToString<uint32_t>(u16);
	if (type == V_TYPE_U32) return XToString<uint32_t>(u32);
	if (type == V_TYPE_U64) return XToString<uint64_t>(u64);

	if (type == V_TYPE_FLOAT) return XToString<float>(f);
	if (type == V_TYPE_DOUBLE) return XToString<double>(d);



	if (type == V_TYPE_LIST && varList >0)
	{

		string s = "list size=" + XToString<size_t>(varList->size()) + "{\r\n";

		for (auto it = this->varList->begin(); it != this->varList->end(); it++){
			s += (CxVar::GetTypeString((*it)->type) + ":");
			s += ((*it)->ToString() + "\r\n");
		}
		s += "}\r\n";
		return s;
	}


	if (type == V_TYPE_MAP && varMap >0){
		string s = "map size=" + XToString<size_t>(varMap->size()) + "{\r\n";
		for (auto it = varMap->begin(); it != varMap->end(); it++)
		{
#if defined(_DEBUG)
//			assert(!it->first.empty());
#endif
			if(it->first.empty()) continue; //属性名字不能为空
			if(it->second.value) s += (it->first + "=>"+ CxVar::GetTypeString(it->second.value->type) +":"
				+ (it->second.value)->ToString() + "\r\n");
		}
		s += "}\r\n";
		return s;
	}

	if (type == V_TYPE_BLOB){
		string s;
		if (blob) s= CxVarHelper::binary_to_hex_case_string(this->blob->dat, this->blob->len,0);
		return s;
	}

	if (type == V_TYPE_BOX)
	{
		string s = "box:" + std::to_string(this->box->tag) + " p[0]:" + XToString<int64_t>(this->box->p[0]);
		return s;
	}

	return "";
}

std::string CxVar::GetTypeString(int _type)
{
	switch (_type)
	{
	case	V_TYPE_I8:  return "int8";
	case	V_TYPE_I16: return "int16";
	case	V_TYPE_I32: return "int32";
	case	V_TYPE_I64: return "int64";

	case	V_TYPE_U8: return "uint8";
	case	V_TYPE_U16: return "uint16";
	case	V_TYPE_U32: return "uint32";
	case	V_TYPE_U64: return "uint64";

	case	V_TYPE_FLOAT: return "float";
	case	V_TYPE_DOUBLE: return "double";

	case	V_TYPE_STRING: return "string";
	case	V_TYPE_WSTRING: return "wstring";

	case 	V_TYPE_BLOB: return "blob";

	case	V_TYPE_VOID: return "void*";
	case	V_TYPE_BOX: return "box";

	case	V_TYPE_LIST: return "list";
	case	V_TYPE_MAP: return "map";

	}
	return "UNKNOW";
}


std::ostream & operator << (std::ostream & _oss, const CxVar & _var)
{
	if (&_var == NULL) return _oss;

	if (_var.type == V_TYPE_VOID)
	{
//		_oss << _var.type << X_VENDL;
//		_oss << (uint32_t)0 << X_VENDL;
		return _oss;
	}
	

	//_oss.write((const char*)&_var.type, sizeof(char));
	//size_t msz = _var.varMap->size();// CxVarHelper::GetSaveSize(_var.varMap);

	if (_var.type == V_TYPE_MAP && _var.varMap )
	{
	   _oss << _var.type << X_VENDL;
	   
	   //这里存在跳过的内容
	   std::ostream::pos_type pos=_oss.tellp();
	   _oss << _var.varMap->size() << X_VENDL;
	   size_t k = 0;
        for(auto it=_var.varMap->begin();
			it!=_var.varMap->end();it++)
		{
			if (it->second.value) {
				if (it->second.value->type == V_TYPE_VOID) continue;//不要保存的内容
				_oss << it->first << X_VENDL;
				_oss << *(it->second.value) << X_VENDL;
				k++;
			}
		}
		if (k != _var.varMap->size()) {
			std::ostream::pos_type pos1 = _oss.tellp();
			_oss.seekp(pos);
			_oss << k << X_VENDL;
			_oss.seekp(pos1);
		}
		return _oss;	
	}

	if (_var.type == V_TYPE_MAP && _var.varMap==NULL)
	{
		_oss << _var.type << X_VENDL;
		_oss << 0 << X_VENDL;
		return _oss;
	}


//	msz= CxVarHelper::GetSaveSize(_var.varList);
	if (_var.type == V_TYPE_LIST && _var.varList )
	{
		_oss << _var.type << X_VENDL;
		std::ostream::pos_type pos = _oss.tellp();
		_oss << _var.varList->size() << X_VENDL;
		size_t k = 0;
		for (auto it = _var.varList->begin();
			it != _var.varList->end(); it++)
		{
			//_oss << it->first << X_VENDL;
			//if ((*it)->type == V_TYPE_VOID) continue;
			CxVar* _v1 = *it;
			if (_v1) {
				if (_v1->type == V_TYPE_VOID) continue;//不要保存的内容
				_oss << *(_v1) << X_VENDL; 
				k++;
			}
		}
		if (k != _var.varList->size()) {
			std::ostream::pos_type pos1 = _oss.tellp();
			_oss.seekp(pos);
			_oss << k << X_VENDL;
			_oss.seekp(pos1);
		}
		return _oss;
	}

	if (_var.type == V_TYPE_LIST && _var.varList==NULL)
	{
		_oss << _var.type << X_VENDL;
		_oss << 0 << X_VENDL;
		return _oss;
	}


	if(_var.type==V_TYPE_STRING /*&& _var.s*/)
	{
		_oss << _var.type << X_VENDL;
		if (_var.s) {
			_oss << _var.s->length() << X_VENDL;
			_oss << *(_var.s) << X_VENDL;
		}
		else { _oss << 0 << X_VENDL; }
		return _oss;
	}

	if (_var.type == V_TYPE_WSTRING /*&& _var.ws*/)
	{
		_oss << _var.type << X_VENDL;
		if (_var.ws) {
			std::string str = ws2s(*(_var.ws));
			_oss << str.length() << X_VENDL;
			_oss << str << X_VENDL;
		}
		else {
			_oss << 0 << X_VENDL;
		}
		return _oss;
	}

/*
	if (_var.type == V_TYPE_BOX && _var.box)
	{
		_oss << _var.type << X_VENDL;
		_oss.write((const char*)_var.box, sizeof(CxVarBox));
		_oss << X_VENDL;
		return _oss;
	}
*/
	if (_var.type == V_TYPE_BOX && _var.box)
	{
		_oss << _var.type << X_VENDL;
		//_oss.write((const char*)_var.box, sizeof(CxVarBox));
		//_oss << X_VENDL;
		_oss << _var.box->tag << X_VENDL;
		for(int i=0;i<3;i++)
			_oss << _var.box->p[i] << X_VENDL;
		return _oss;
	}

	if (_var.type == V_TYPE_BOX && _var.box==NULL)
	{
		_oss << _var.type << X_VENDL;
		//_oss.write((const char*)_var.box, sizeof(CxVarBox));
		//_oss << X_VENDL;
		CxVarBox box;
		memset(&box, 0, sizeof(CxVarBox));
		_oss << box.tag << X_VENDL;
		for (int i = 0; i < 3; i++)
			_oss << box.p[i] << X_VENDL;
		return _oss;
	}


	if (_var.type == V_TYPE_BLOB && _var.blob && _var.blob->dat)
	{
		_oss << _var.type << X_VENDL;
		_oss << _var.blob->len << X_VENDL;
		_oss.write((const char*)_var.blob->dat, _var.blob->len);
		_oss << X_VENDL;
		return _oss;
	}

	if (_var.type == V_TYPE_BLOB && _var.blob==NULL )
	{
		_oss << _var.type << X_VENDL;
		_oss << 0 << X_VENDL;
//		_oss.write((const char*)_var.blob->dat, _var.blob->len);
		_oss << X_VENDL;
		return _oss;
	}


	_oss << _var.type << X_VENDL;
	switch(_var.type)
	{
	case V_TYPE_U8:
		_oss << _var.u8 << X_VENDL;
		break;
	case V_TYPE_U16:
		_oss << _var.u16 << X_VENDL;
		break;
	case V_TYPE_U32:
		{
		_oss << _var.u32 << X_VENDL;
		break;
		}
	case V_TYPE_U64:
		_oss << _var.u64 << X_VENDL;
		break;
	case V_TYPE_I8:
		_oss << _var.i8 << X_VENDL;
		break;
	case V_TYPE_I16:
		_oss << _var.i16 << X_VENDL;
		break;
	case V_TYPE_I32:
		_oss << _var.i32 << X_VENDL;
		break;
	case V_TYPE_I64:
		_oss << _var.i64 << X_VENDL;
		break;
	case V_TYPE_FLOAT:
		_oss << _var.f << X_VENDL;
		break;
	case V_TYPE_DOUBLE:
		_oss << _var.d << X_VENDL;
		break;

	}
	
	return _oss;
}

std::istream & operator >> (std::istream & _iss, CxVar & _var)
{
	if (&_var == NULL) return _iss;
	_iss >> _var.type;
	
	if(_var.type==V_TYPE_MAP)
	{
		size_t k=0;
		_iss >> k;
		if (_var.varMap == NULL) _var.varMap = new CxVarFieldMap();
		for(size_t i=0;i<k;i++)
		{
			std::string kname;
			_iss >> kname;
			//CxVarField vf;
			//vf.value=new CxVar();
			//_iss >> (*vf.value);
			//_var.varMap->insert(std::pair<std::string, CxVarField>(kname, vf));
			CxVar* _v1= new CxVar();
			(*(_var.varMap))[kname].value = _v1;
			_iss >> (*_v1);
		}
		return _iss;
	}	


	if (_var.type == V_TYPE_LIST)
	{
		size_t k = 0;
		_iss >> k;
		if (_var.varList == NULL) _var.varList = new CxVarList();
		for (size_t i = 0; i < k; i++)
		{
			CxVar* v = new CxVar();
			_iss >> (*v);
			_var.varList->push_back(v);
		}
		return _iss;
	}


	if (_var.type == V_TYPE_STRING)
	{
		size_t k = 0;
		_iss >> k;
		if (k > 0) {
			std::string str;
			_iss >> str;
			_var.SetString(str);
		}
		return _iss;
	}

	if (_var.type == V_TYPE_WSTRING)
	{
		size_t k = 0;
		_iss >> k;
		if (k > 0) {
			std::string str;
			_iss >> str;
			std::wstring wstr = s2ws(str);
			_var.SetWString(wstr);
		}
		return _iss;
	}

/*
	if (_var.type == V_TYPE_BOX)
	{
		if (_var.box == NULL) _var.box = new CxVarBox();
		_iss.seekg(sizeof(X_VENDL) - 1,ios_base::cur);
		_iss.read((char*)_var.box, sizeof(CxVarBox));
		return _iss;
	}
*/

	if (_var.type == V_TYPE_BOX)
	{
		if (_var.box == NULL) _var.box = new CxVarBox();
		//_iss.seekg(sizeof(X_VENDL) - 1, ios_base::cur);
		//_iss.read((char*)_var.box, sizeof(CxVarBox));
		_iss >> _var.box->tag;
		for (int i = 0; i < 3; i++) {
			_iss >> _var.box->p[i];
		}

		return _iss;
	}


	if (_var.type == V_TYPE_VOID)
	{
//		int32_t i = 0;
//		_iss >> i;
		return _iss;
	}

	if (_var.type == V_TYPE_BLOB)
	{
		//if (_var.blob == NULL) _var.blob = new CxVarBlob();
		_var.clear();
		unsigned long bblen=0;
		_iss >> bblen;
	
/*
		char* bbc = (char*)malloc(bblen + 1);
		bbc[bblen] = 0;
		_iss.seekg(sizeof(X_VENDL) - 1, ios_base::cur);
		_iss.read(bbc, bblen);
		_var.blob = new CxVarBlob(bbc, bblen);
		free(bbc);
*/
		_iss.seekg(sizeof(X_VENDL) - 1, ios_base::cur);
		if (bblen>0) {
			_var.blob = new CxVarBlob();
			_var.blob->resize(bblen);
			_iss.read(_var.blob->dat, bblen);
		}
		return _iss;
	}


	switch(_var.type)
	{
	case V_TYPE_U8:
		_iss >> _var.u8;
		break;
	case V_TYPE_U32:
		_iss >> _var.u32;
		break;
	case V_TYPE_U16:
		_iss >> _var.u16;
		break;
	case V_TYPE_U64:
		_iss >> _var.u64;
		break;

	case V_TYPE_I8:
		_iss >> _var.i8;
		break;
	case V_TYPE_I16:
		_iss >> _var.i16;
		break;
	case V_TYPE_I32:
		_iss >> _var.i32;
		break;
	case V_TYPE_I64:
		_iss >> _var.i64;
		break;

	case V_TYPE_FLOAT:
		_iss >> _var.f;
		break;
	case V_TYPE_DOUBLE:
		_iss >> _var.d;
		break;


	}

	return _iss;
}




CxVar::operator int8_t(){ return i8; }
CxVar::operator int16_t(){ return i16; }
CxVar::operator int32_t() { return i32; }
CxVar::operator int64_t(){ return i64; }

CxVar::operator uint8_t(){ return u8; }
CxVar::operator uint16_t(){ return u16; }
CxVar::operator uint32_t(){ return u32; }
CxVar::operator uint64_t(){ return u64; }

CxVar::operator bool(){ return i32!=0;}

CxVar::operator double(){ return d; }
CxVar::operator float(){ return f; }

CxVar::operator std::string()
{
	return *s;
}

CxVar::operator std::wstring()
{
	return *ws;
}


CxVar::operator CxVarList(){ return *varList; }
CxVar::operator CxVarFieldMap(){ return *varMap; }


/**
版本号
CxVar 数量
*/
//void CxVar::WriteVersionHead(std::ostream& _oss, int count)
//{
//	string version = XATTR_DATA_VERSION;
//	StrWriteToStream(_oss, version);
//	_oss<<count;
//}

/**
返回 -1 文件不正确
返回 0  文件正确
返回 >0 文件版本号太高
*/
//int CxVar::ReadVersionHead(std::istream& _iss, int& count)
//{
//	string _version;
//	StrReadFromStream(_iss,_version);
//	if (_version.compare(XATTR_DATA_VERSION)!=0)
//	{
//		/*
//		vector<string>version_source = split(XATTR_DATA_VERSION, " ");
//		vector<string>version_read = split(_version, " ");
//		if (0 == version_read.size())
//		{
//			return -1;
//		}
//		if (version_source[0] != version_read[0])
//		{
//			return -1;
//		}
//		assert(version_source.size() < 2); // XATTR_DATA_VERSION 文件头格式不对
//		float version_source_number = atof(version_source[1].c_str());
//		float version_read_number = atof(version_read[1].c_str());
//		if (version_source_number < version_read_number)
//		{
//			return (int)(version_read_number - version_source_number + 1);
//		}
//		*/
//		return -1;
//	}
//	NumberTypeRead(_iss, count);
//	return 0;
//}





/**
复制指定的内容部分

int8_t visible; 1
int8_t save; 2
int8_t myself; 4

*/
//void CxVarMapCopy(CxVarMap& des, CxVarMap& src, char filter)
//{
//	std::bitset<8> bs(filter);
//	for (auto it = src.begin();
//		it != src.end();
//		it++)
//	{
//		//判断过滤器
//		bool choose = ( (it->first.visible==bs[0])
//			|| (it->first.save == bs[1])
//			|| (it->first.myself == bs[2])
//			);
//		
//		if (filter == -1) choose = true;
//
//		if (choose) {
//			des[CxVarName(it->first.name)] = new CxVar(*(it->second));
//		}
//	}
//}
//
//void CxVarMapCopyValue(CxVarMap & des, CxVarMap & src)
//{
//	for (auto it = des.begin();
//		it != des.end();
//		it++)
//	{
//		auto itt = src.find(it->first);
//		if (itt != src.end()) {
//			*(it->second) = *(itt->second);
//		}
//	}
//}
//

CxVar * CxVarHelper::CxVarFindFromMap(CxVarFieldMap & _cnt, std::string _name, bool _create, int _type)
{

	auto it = _cnt.find(_name);
	if (it != _cnt.end()) {
		return it->second.value;
	}

	if (_create) {
		CxVar* _v = CxVar::createWithType(_type);
		_cnt[_name].value = _v;
		return _v;
	}

	return nullptr;
}



/**
从XML文件创建容器结构
@param _text 是XML文本
@param _cnt_size 是XML文本的长度
*/
int CxVarHelper::StructLoadFromXml(CxVarFieldMap& _des, const char* filename)
{
	assert(filename);

	tinyxml2::XMLDocument doc;
	doc.LoadFile(filename);

	if (doc.Error()) {
		doc.PrintError();
		printf("文件没发现:%s\n", filename);
		exit(0);
	}

	tinyxml2::XMLElement* props = doc.RootElement()->FirstChildElement("properties");
	assert(props);

	for (tinyxml2::XMLElement* _attr = props->FirstChildElement(); _attr; _attr = _attr->NextSiblingElement())
	{
/*	
		CxVarName _vName;
		//CxVar _v;
		_vName.name = _attr->Name();
		_vName.visible = _attr->BoolAttribute("open");
		_vName.save = _attr->BoolAttribute("save");
		_vName.myself = _attr->BoolAttribute("myself");
*/
		//根据数据类型创建
		assert(_attr->Attribute("type"));

		//std::cout << _attr->Attribute("type") << std::endl;
		//std::cout.flush();

		_des[_attr->Name()].value= CxVar::createWithTypeName(_attr->Attribute("type"));

	}

#if(__TEST_VAR__>0)
	CxVar var(_Des);
	std::cout << var.ToString();
	std::cout.flush();
#endif

	return 0;
}


/**
从XML文件创建容器结构
@param _text 是XML文本
@param _cnt_size 是XML文本的长度
*/
int CxVarHelper::DataLoadFromXml(CxVarFieldMap& _des, tinyxml2::XMLElement* elm)
{
	assert(elm);

	for (auto it : _des)
	{
		std::string keyname = it.first;
		tinyxml2::XMLElement* alm = elm->FirstChildElement(keyname.c_str());
		
		if (alm && alm->GetText() && it.second.value)
		{
#if defined(_DEBUG)
//			std::cout << keyname << "=>" << alm->GetText() << "[" 
//				+ it.second.value->GetTypeString() + "]" << std::endl;
//			std::cout.flush();
#endif
			const char* chr = alm->GetText();
			if (utf8util::IsTextUTF8(chr, strlen(chr))) 
			{
				std::wstring wstr;
				utf8util::Utf8toWStr(std::string(chr), wstr);
				std::string str=ws2s(wstr);
				it.second.value->SetValueFrom(str.c_str());
			}else
			it.second.value->SetValueFrom(alm->GetText());
		}
	}
	return 0;
}

size_t CxVarHelper::GetSaveSize(CxVarFieldMap * _src)
{
	if (_src == NULL) return 0;
	size_t sz = _src->size();

	for (auto it = _src->begin(); it != _src->end(); it++)
	{
		if (it->second.value == NULL) {sz--; continue;}
		if(it->second.value->type==V_TYPE_VOID) { sz--; continue; }
	}

	return sz;
}

size_t CxVarHelper::GetSaveSize(CxVarList * _src)
{
	if (_src == NULL) return 0;
	size_t sz = _src->size();

	for (auto it = _src->begin(); it != _src->end(); it++)
	{
		//if ((*it)->value == NULL) { sz--; continue; }
		if ((*it)->type == V_TYPE_VOID) { sz--; continue; }
	}

	return sz;
}

//std::string CxVarHelper::ToString(CxVarMap & _Des)
//{
//	CxVar var(_Des);
//	return var.ToString();
//}


