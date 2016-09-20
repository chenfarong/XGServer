

#ifndef attrib_h__
#define attrib_h__

#include <iostream>
#include <ostream>
#include <sstream>
#include <fstream>
#include <map>
#include <list>
#include <unordered_map>
#include <set>
#include <string>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <locale.h>

#include "tinyxml2.h"
//#include "ByteBuffer.h"
//#include "ByteReader.h"

#define XATTR_SAFE_DELETE(a) if(a) delete a; a=NULL

#define XATTR_DATA_VERSION "CxVar 1.5"
#define X_VENDL "\n"

#define X_MAKE_PAIR64(l, h)  uint64_t( uint32_t(l) | ( uint64_t(h) << 32 ) )
#define X_PAIR64_HIPART(x)   (uint32_t)((uint64_t(x) >> 32) & uint64_t(0x00000000FFFFFFFF))
#define X_PAIR64_LOPART(x)   (uint32_t)(uint64_t(x)         & uint64_t(0x00000000FFFFFFFF))

#define X_MAKE_PAIR32(l, h)  uint32_t( uint16_t(l) | ( uint32_t(h) << 16 ) )
#define X_PAIR32_HIPART(x)   (uint16_t)((uint32_t(x) >> 16) & 0x0000FFFF)
#define X_PAIR32_LOPART(x)   (uint16_t)(uint32_t(x)         & 0x0000FFFF)



#if defined( __GNUC__ )
#pragma pack(1)
#else
#pragma pack(push,1)
#endif


namespace CxStrUtil
{

	inline void LineToStringVector(std::string& _src,std::vector<std::string>& sv,char spc=',')
	{
		std::string::size_type  pos = 0;
		std::string::size_type  posA = 0;
		do {
			pos = _src.find(spc, posA);
			if (pos != std::string::npos)
			{
				sv.push_back(_src.substr(posA, pos));
				posA = pos + 1;
			}
			else {
				sv.push_back(_src.substr(posA));
			}
		} while (pos != std::string::npos);
	}

};



enum VAR_TYPE
{
	V_TYPE_UNKNOWN,
	
	V_TYPE_I8, 
	V_TYPE_I16,
	V_TYPE_I32,  //	V_TYPE_INT,  //含BOOL
	V_TYPE_I64,

	V_TYPE_U8,
	V_TYPE_U16,
	V_TYPE_U32,
	V_TYPE_U64,

	V_TYPE_FLOAT,
	V_TYPE_DOUBLE,

	V_TYPE_STRING,
	V_TYPE_WSTRING,

	V_TYPE_LIST,
	V_TYPE_MAP, //容器V_TYPE_FIELD,

	V_TYPE_BLOB, //块数据

	V_TYPE_VOID,
	V_TYPE_BOX, //盒子

	V_TYPE_MAX,
};

struct CxVar;
struct CxVarField;

class CxVarName;

typedef unsigned long x_size_t;
typedef unsigned long DWORD;

//typedef std::map<CxVarName, CxVar*>  CxVarMap;
//typedef std::map<CxVarName, CxVar*>::iterator CxVarMapIterator;

typedef std::list<CxVar*>  CxVarList;
typedef std::list<CxVar*>::iterator  CxVarListIterator;

//typedef std::map<uint64_t, GxVarList> GxVarTable;

typedef std::map<std::string, CxVarField> CxVarFieldMap;

class CxAttribute;


struct CxVarBox
{
	uint64_t tag;
	int64_t  p[3];
};

struct CxVarBlob
{
	char* dat;
	unsigned long len; //数据长度
	
	CxVarBlob() {
		len = 0;
		dat = NULL;
	}

	CxVarBlob(const void* _lpBuf, unsigned long _size)
		:dat(0)
	{
		assert(_lpBuf);
		resize(_size);
		memcpy(dat, _lpBuf, _size);
		dat[_size] = 0;
	}

	virtual ~CxVarBlob() 
	{
		if (dat) 
		{
			free(dat);
			dat = NULL;
		}
	}

	void resize(unsigned long _size)
	{
		len = _size;
		if (dat) {
			dat = (char*)realloc(dat,_size + 1);
		}
		else {
			dat = (char*)malloc(_size + 1);
		}
		dat[_size] = 0;
	}

};

struct CxVarFlag
{
	bool save;		//保存 持久化的时候写入 没有此就不保存
	bool pub;		//公开
	bool client;	//客户端
	bool visible;	//视觉效果

	CxVarFlag()
	{
		save = true;
		pub = true;
		client = false;
		visible = false;
	}

};


struct CxVarField
{
	CxVar* value;
	CxVarFlag flag;
	
	CxVarField() {
		value = NULL;
	}

	CxVarField(CxVar* v) {
		value = v;
	}

//	virtual ~CxVarField();
};


struct CxVar
{
	union
	{
		int8_t  i8;
		int16_t i16;
		int32_t i32;
		int64_t i64;

		uint8_t  u8;
		uint16_t u16;
		uint32_t u32;
		uint64_t u64;

		double  d;
		float f;

		std::string* s;
		std::wstring* ws;

		CxVarBlob* blob;
		CxVarBox* box;

		CxVarList* varList;
		CxVarFieldMap* varMap;

		void* method;  //程序函数指针 此种类型不进行持久化
	
//		GxVarTable* varTable; //不用支持此种
	};

	int type;	//数据类型

//-----------------------------

	CxVar();
	virtual ~CxVar();

//	CxVar(int _value);
	CxVar(int8_t _value);
	CxVar(int16_t _value);
	CxVar(int32_t _value);
	CxVar(int64_t _value);

	CxVar(uint8_t _value);
	CxVar(uint16_t _value);
	CxVar(uint32_t _value);
	CxVar(uint64_t _value);

//	CxVar(const char* _value);
	CxVar(const float _value);
	CxVar(const double _value);

	CxVar(const std::string _value);
	CxVar(const std::wstring _value);

	CxVar(char* _lpBuf, unsigned long _size);

	CxVar(const CxVarList& _value);
	
	CxVar(const CxVarFieldMap& _value);

	CxVar(const CxVar& _value);
		
//	CxVar(void* _value);

	CxVar(const CxVarBox& _value);


	CxVar& operator=(const CxVar& _val);

	CxVar& operator=(const bool& _val);
	CxVar& operator=(const float& _val);
	CxVar& operator=(const double& _val);

	
	CxVar& operator=(const int& _val);
	CxVar& operator=(const int8_t& _val);
	CxVar& operator=(const int16_t& _val);
//	CxVar& operator=(const int32_t& _val);
	CxVar& operator=(const int64_t& _val);

	CxVar& operator=(const uint8_t& _val);
	CxVar& operator=(const uint16_t& _val);
	CxVar& operator=(const uint32_t& _val);
	CxVar& operator=(const uint64_t& _val);

	CxVar& operator=(const std::string& _val);
	CxVar& operator=(const std::wstring& _val);

	CxVar& operator=(const CxVarList& _value);

	CxVar& operator=(const CxVarFieldMap& _value);

	void clear();

	void SetBool(bool _value);
	void SetInt(int _value);
	void SetFloat(float _value);

	void SetI8(int8_t _value);
	void SetI16(int16_t _value);
	void SetI32(int32_t _value);
	void SetI64(int64_t _value);

	void SetU8(uint8_t _value);
	void SetU16(uint16_t _value);
	void SetU32(uint32_t _value);
	void SetU64(uint64_t _value);
	
	void SetDouble(double _value);

	void SetString(std::string _value);
	void SetWString(std::wstring _value);

	void SetBlob(char* _lpBuf, unsigned long _size);

	void SetBox(uint64_t tag, int32_t p0, int32_t p1);

	void SetPtr(void* _value);

	void SetVarList(const CxVarList& _value); 

	void SetVarMap(const CxVarFieldMap& _value); 

	void SetValueFrom(const char* _src);

	void CopyFrom(const CxVar* _var);

	std::string ToString();
	static std::string GetTypeString(int _type);


	friend std::ostream &operator<<(std::ostream&, const CxVar&);
	friend std::istream &operator>>(std::istream&, CxVar&);

	operator int8_t();
	operator int16_t();
	operator int32_t();
	operator int64_t();

	operator uint8_t();
	operator uint16_t();
	operator uint32_t();
	operator uint64_t();

	operator bool();
	operator double();
	operator float();

	operator std::string();
	operator std::wstring();


	operator CxVarList();
	operator CxVarFieldMap();

	/** 
	*  @date        2016/07/15 20:58 
	*  @brief       文件头写入版本号 (CxVar 数量) 
	*  @param[in]    std::ostream& _oss, int count = 1
	*  @param[out]   
	*  @return      
	*  @pre          
	*  @remarks      
	*  @see          
	*/  
	//static void WriteVersionHead(std::ostream& _oss, int count = 1);

	/** 
	*  @date        2016/07/15 20:37 
	*  @brief       读取文件头，并判断文件格式是否正确 
	*  @param[in]    std::istream& _iss 
	*  @param[out]   int& count
	*  @return      int  -1 文件不正确; 0  文件正确; >0 文件版本号太高
	*  @pre          
	*  @remarks      
	*  @see          
	*/  
	//static int ReadVersionHead(std::istream& _iss, int& count);

	static VAR_TYPE GetVarType(const char* _typeName);
	static CxVar* createWithTypeName(const char* _typeName);
	static CxVar* createWithType(int _type);

};


class CxVarName
{
public:
	bool visible;
	bool save;
	bool myself;

	std::string name;

public:

	CxVarName() {};

	CxVarName(std::string _name) 
	{
		name = _name;
	}

	bool operator==(const CxVarName& _Right) const
	{
		return (name.compare(_Right.name)==0);
	}

	bool operator<(const CxVarName& _Right) const
	{
		return name < _Right.name;
//		 return (std::_Hash_seq((const unsigned char*)name.c_str(),name.length()) <
//			 std::_Hash_seq((const unsigned char*)_Right.name.c_str(),_Right.name.length()));
	}

	CxVarName& operator=(const CxVarName& _Right)
	{
		name = _Right.name;
		visible = _Right.visible;
		save = _Right.save;
		myself = _Right.myself;
		return *this;
	}

};





//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////

template<class _Ch, class _Tr>
std::basic_istream<_Ch, _Tr>& operator >> (std::basic_istream<_Ch, _Tr>& _iss, CxVarFieldMap& _var)
{
	int type;
	_iss >> type;
	if (type != V_TYPE_MAP) return _iss; //assert(type == V_TYPE_MAP);

	size_t k = 0;
	_iss >> k;
	for (size_t i = 0; i < k; i++)
	{
		std::string kname;
		_iss >> kname;
		CxVarField vf;
		vf.value = new CxVar();
		_iss >> (*vf.value);
		_var.insert(std::pair<std::string, CxVarField>(kname, vf));
	}
	return _iss;
}

template<class _Ch, class _Tr>
std::basic_ostream<_Ch, _Tr>& operator << (std::basic_ostream<_Ch, _Tr>& _oss, CxVarFieldMap& _var)
{
	_oss << V_TYPE_MAP << X_VENDL;
	_oss << _var.size() << X_VENDL;
	for (auto it = _var.begin();
		it != _var.end(); it++)
	{
		_oss << it->first << X_VENDL;
		_oss << *(it->second.value) << X_VENDL;
	}
	return _oss;
}


/**
复制指定的内容部分
1 1 
2 3
4 7
*/
//void CxVarMapCopy(CxVarMap& des, CxVarMap& src, char filter=1);







class CxVarHelper
{
public:
	static int hex_to_binary(void *dest, const char *data)
	{
		int value;
		char* _dest = (char*)dest;
		unsigned int k = 0;

		while (*data != '\0') {
			if (*data >= '0' && *data <= '9')
				value = (*data - '0') << 4;
			else if (*data >= 'a' && *data <= 'f')
				value = (*data - 'a' + 10) << 4;
			else if (*data >= 'A' && *data <= 'F')
				value = (*data - 'A' + 10) << 4;
			else
				return -1;

			data++;
			if (*data >= '0' && *data <= '9')
				value |= *data - '0';
			else if (*data >= 'a' && *data <= 'f')
				value |= *data - 'a' + 10;
			else if (*data >= 'A' && *data <= 'F')
				value |= *data - 'A' + 10;
			else
				return -1;

			//buffer_append_c(dest, value);
			char ch = value;
			_dest[k] = ch;
			data++;
			k++;
		}

		return 0;
	}
	
	static std::string binary_to_hex_case_string(const void *src, size_t size, int ucase)
	{
		const unsigned char *data = (const unsigned char *)src;
		std::string res;
		//	unsigned char *p;
		char base_char;
		int value;
		//	unsigned int k = 0;

		/* @UNSAFE */
		base_char = ucase ? 'A' : 'a';
		unsigned char ch = 0;

		//	p = dest;
		for (size_t i = 0; i < size; i++) {
			value = data[i] >> 4;

			//*p++ = value < 10 ? value + '0' : value - 10 + base_char;
			ch = value < 10 ? value + '0' : value - 10 + base_char;
			res.push_back(ch);

			value = data[i] & 0x0f;
			//*p++ = value < 10 ? value + '0' : value - 10 + base_char;
			ch = value < 10 ? value + '0' : value - 10 + base_char;
			res.push_back(ch);
		}

		return res;
	}

	/**
	从容器中查找一个值 如果不存在是否要创建
	*/
	static CxVar* CxVarFindFromMap(CxVarFieldMap& _cnt, std::string _name, bool _create = false, int _type = 0);
	static int StructLoadFromXml(CxVarFieldMap& _des, const char* filename);
	static int DataLoadFromXml(CxVarFieldMap& _des, tinyxml2::XMLElement* elm);
	static std::string ToString(CxVarFieldMap& _des)
	{
		return CxVar(_des).ToString();
	}


	static void SaveToFile(CxVarFieldMap& _src,std::string filename) {
		std::ofstream outf;
		outf.open(filename.c_str());
		if (outf.good()) {
			outf << _src;
			outf.close();
		}
	}

	static void LoadFromFile(CxVarFieldMap& _src, std::string filename) {
		std::ifstream inputf;
		inputf.open(filename.c_str());
		if (inputf.good()) {
			inputf >> _src;
			inputf.close();
		}
	}

	/**
	删除其中一个item
	*/
	static bool VarMapErase(CxVarFieldMap& _src, std::string kname) {
		 auto it= _src.find(kname);
		 if (it != _src.end()) {
			 if (it->second.value) delete it->second.value;
			 _src.erase(it);
			 return true;
		 }
		 return false;
	}



	static void CxVarFieldMapAdd(CxVarFieldMap& _src, std::string kname, CxVar* var)
	{
		//_src.insert(std::pair<std::string, CxVarField>(kname, CxVarField(var)));
		_src[kname].value =var;
	}

	static size_t GetSaveSize(CxVarFieldMap* _src);
	static size_t GetSaveSize(CxVarList* _src);


	static void CxVarMapCopyValue(CxVarFieldMap& des, const CxVarFieldMap& src)
	{
		for (auto it=src.begin();it!=src.end();it++)
		{
			//如果目标不存在就增加 如果存在是指针就跳过
			std::string kname = it->first;
			if (it->second.value == NULL) continue;
			if (it->second.value->type == V_TYPE_VOID) continue;

			if(des.find(kname) == des.end()) continue;

			if (des[kname].value && des[kname].value->type==V_TYPE_VOID) continue;

			CxVar* _v1 = des[kname].value;
			if (_v1 == NULL) { 
				des[kname].value = new CxVar(*((it->second.value))); 
			}
			else {
				_v1->CopyFrom(it->second.value);
			}
		}
	}


	static void CxVarListClear(CxVarList& _src)
	{
		while (!_src.empty())
		{
			auto it = _src.begin();
			delete (*it);
			_src.erase(it);
		}
	}

	static void CxVarMapClear(CxVarFieldMap& _src)
	{
		while (!_src.empty())
		{
			auto it = _src.begin();
			if((it->second).value) delete (it->second).value;
			_src.erase(it);
		}
	}


};



namespace CxVarUtil
{


	template<typename T>
	int CxVarMapValueSet(CxVarFieldMap& _src, std::string _name, T _val)
	{
		//TODO 名字支持 :: 分割

		auto it = _src.find(_name);
		if (it == _src.end()) return -1;

		CxVar* v = it->second.value;

		if (v == NULL) it->second.value=new CxVar(_val);
		else *v = _val;

		return 0;
	}

	template<typename T>
	int CxVarMapValueSetForce(CxVarFieldMap& _src, std::string _name, T _val)
	{
		//TODO 名字支持 :: 分割

		auto it = _src.find(_name);
		if (it == _src.end()) {
			_src[_name] = CxVarField(new CxVar(_val));
			return 0;
		}

		CxVar* v = it->second.value;

		if (v == NULL) it->second.value = new CxVar(_val);
		else *v = _val;

		return 0;
	}


//	bool CxVarMapValueSetString(CxVarFieldMap& _src, std::string _name, std::string _val)


	template<typename T>
	T CxVarMapValue(CxVarFieldMap& _src, std::string keyname)
	{
		//TODO 名字支持 :: 分割


		CxVar* v = _src[keyname].value;
		if (v == NULL) return (T)0;
		return (T)(*v);
	}

	//用于清理一个指针容器
	template <typename TP>
	void ClearList(std::list<TP>& c1)
	{
		while (!c1.empty())
		{
			std::list<TP>::iterator iter = c1.begin();
			delete *iter;
			*iter = NULL;
			c1.erase(iter);
		}
	}

	//用于清理一个map,第二个类型为指针
	template<typename T1, typename T2,
		template <class _Kty,
		class _Ty,
		class _Pr = std::less<_Kty>,
		class _Alloc = std::allocator<std::pair<const _Kty, _Ty> >
		> class M
	>
		void ClearMap(M<T1, T2, std::less<T1>, std::allocator<std::pair<const T1, T2> > >& c1)
	{
		typename M<T1, T2>::iterator iter = c1.begin();
		for (; iter != c1.end(); ++iter)
		{
			delete iter->second;
			iter->second = NULL;
		}
		c1.clear();
	}

};

std::string ws2s(const std::wstring& ws);
std::wstring s2ws(const std::string& s);



//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////

class CxAttribute
{
protected:
	std::map<std::string, CxVarField> cnt;

public:

	virtual ~CxAttribute();

	CxVar* operator[](const std::string& kname);

	void Assign(const std::string& kname, CxVar* _var);

	bool erase(const std::string& kname);

	void clear();

	template<typename T>
	inline void SetFieldValue(const std::string& kname, T _value)
	{
		auto it = cnt.find(kname);
		if (it == cnt.end()) {
			cnt[kname] = CxVarField(new CxVar(_value));
		}
		else {
			*(it->second.value) = _value;
		}
	}

	template<typename T>
	inline bool GetFieldValue(const std::string& kname, T& _result)
	{
		auto it = cnt.find(kname);
		if (it == cnt.end()) {
			return false;
		}
		else {
			_result = ((T)(*(it->second.value)));
			return true;
		}
	}


	template<typename T>
	inline T GetFieldValueNumber(const std::string& kname)
	{
		auto it = cnt.find(kname);
		if (it == cnt.end()) {
			return 0;
		}
		else {
			return ((T)(*(it->second.value)));
		}
	}

	
	inline std::string GetFieldValueString(const std::string& kname)
	{
		auto it = cnt.find(kname);
		if (it == cnt.end()) {
			return "";
		}
		else {
			return ((std::string)(*(it->second.value)));
		}
	}

	friend std::ostream &operator<<(std::ostream&, const CxAttribute&);
	friend std::istream &operator >> (std::istream&, CxAttribute&);


	operator std::string();

	std::string ToString();


	void LoadFromFile(std::string fname);
	void SaveToFile(std::string fname);

	void LoadStructFromXmlFile(std::string fname);
	void LoadDataFromXmlElement(tinyxml2::XMLElement*);

	void CopyValueFrom(const CxAttribute& _Right);

	inline std::map<std::string, CxVarField>& GetContainer() { return cnt; };

};



#if defined( __GNUC__ )
#pragma pack()
#else
#pragma pack(pop)
#endif

#endif // attrib_h__


