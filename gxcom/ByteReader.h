

#ifndef reader_h__
#define reader_h__

#include "ByteConverter.h"

#include <string>
#include <algorithm>
#include <sstream>
#include <stdint.h>

typedef int64_t int64;
typedef int32_t int32;
typedef int16_t int16;
typedef int8_t int8;
typedef uint64_t uint64;
typedef uint32_t uint32;
typedef uint16_t uint16;
typedef uint8_t uint8;

class ByteReader
{
protected:
	const char* dat;
	size_t _rpos;
	size_t _total_size;

public:
	ByteReader(const void* _src,size_t _Size) 
	{
		_rpos = 0;
		dat = (const char*)_src;
		_total_size = _Size;
	};

	size_t rpos() const { return _rpos; }
	size_t size() const { return _total_size; }
	size_t remain() const { return _total_size - _rpos; };

	const char* Current() { return dat+_rpos; };

	template <typename T> T read(size_t pos) const
	{
		if (pos + sizeof(T) > size()) {
		//	throw ByteReaderException(false, pos, sizeof(T), size());
			
		}
		T val = *((T const*)&dat[pos]);
		EndianConvert(val);
		return val;
	}

	template <typename T> T read()
	{
		T r = read<T>(_rpos);
		_rpos += sizeof(T);
		return r;
	}

//////////////////////////////////////////////////////////////////////////

	ByteReader& operator >> (bool& value)
	{
		value = read<char>() > 0 ? true : false;
		return *this;
	}

	ByteReader& operator >> (uint8& value)
	{
		value = read<uint8>();
		return *this;
	}

	ByteReader& operator >> (uint16& value)
	{
		value = read<uint16>();
		return *this;
	}

	ByteReader& operator >> (uint32& value)
	{
		value = read<uint32>();
		return *this;
	}

	ByteReader& operator >> (uint64& value)
	{
		value = read<uint64>();
		return *this;
	}

	// signed as in 2e complement
	ByteReader& operator >> (int8& value)
	{
		value = read<int8>();
		return *this;
	}

	ByteReader& operator >> (int16& value)
	{
		value = read<int16>();
		return *this;
	}

	ByteReader& operator >> (int32& value)
	{
		value = read<int32>();
		return *this;
	}

	ByteReader& operator >> (int64& value)
	{
		value = read<int64>();
		return *this;
	}

	ByteReader& operator >> (float& value)
	{
		value = read<float>();
		return *this;
	}

	ByteReader& operator >> (double& value)
	{
		value = read<double>();
		return *this;
	}

	ByteReader& operator >> (std::string& value)
	{
		value.clear();
		while (rpos() < size())                         // prevent crash at wrong string format in packet
		{
			char c = read<char>();
			if (c == 0)
				break;
			value += c;
		}
		return *this;
	}

	uint64 readPackGUID()
	{
		uint64 guid = 0;
		uint8 guidmark = 0;
		(*this) >> guidmark;

		for (int i = 0; i < 8; ++i)
		{
			if (guidmark & (uint8(1) << i))
			{
				uint8 bit;
				(*this) >> bit;
				guid |= (uint64(bit) << (i * 8));
			}
		}

		return guid;
	}

	void read(uint8* dest, size_t len)
	{
		if (_rpos + len > size()) return;
//			throw ByteBufferException(false, _rpos, len, size());
		memcpy(dest, &dat[_rpos], len);
		_rpos += len;
	}

//////////////////////////////////////////////////////////////////////////


	template<typename T>
	void read_skip() { read_skip(sizeof(T)); }

	void read_skip(size_t skip)
	{
		if (_rpos + skip > size()) {
		//	throw ByteReaderException(false, _rpos, skip, size());
		}
		_rpos += skip;
	}

};

#define ByteReaderClass(a,b) a.read((uint8*)&b,sizeof(b))

#endif // reader_h__




