#pragma once
#include <stdint.h>
#include <xhash> 
#include <iostream>
#include "DLL.h"
#include "Hash.h"
#include "Memory.h"
namespace vengine
{
	class DLL_CLASS string
	{
		friend std::ostream& operator << (std::ostream& out, const string& obj) noexcept;
		friend std::istream& operator >> (std::istream& in, string& obj) noexcept;
	private:
		char* ptr = nullptr;
		uint64_t lenSize = 0;
		uint64_t capacity = 0;
		bool Equal(char const* str, uint64_t count) const noexcept;
	public:
		string(const string& a, const string& b) noexcept;
		string(const string& a, const char* b) noexcept;
		string(const char* a, const string& b) noexcept;
		string(const string& a, char b) noexcept;
		string(char a, const string& b) noexcept;
		uint64_t size() const noexcept { return lenSize; }
		uint64_t length() const noexcept { return lenSize; }
		uint64_t getCapacity() const noexcept { return capacity; }
		string() noexcept;
		string(const char* cstr) noexcept;
		string(const char* cstrBegin, const char* cstrEnd) noexcept;
		string(const string& data) noexcept;
		string(uint64_t size, char c) noexcept;
		void clear() noexcept
		{
			if (ptr)
				ptr[0] = 0;
			lenSize = 0;
		}
		void push_back(char c) noexcept
		{
			(*this) += c;
		}
		bool empty() const noexcept
		{
			return lenSize == 0;
		}
		string& operator=(const string& data) noexcept;
		string& operator=(const char* data) noexcept;
		string& operator=(char data) noexcept;
		string& assign(const string& data) noexcept
		{
			return operator=(data);
		}
		string& assign(const char* data) noexcept {
			return operator=(data);
		}
		void push_back_all(char const* c, uint64_t newStrLen) noexcept;
		string& assign(char data) noexcept {
			return operator=(data);
		}
		void reserve(uint64_t targetCapacity) noexcept;
		char* data() const noexcept { return ptr; }
		char const* begin() const noexcept { return ptr; }
		char const* end() const noexcept { return ptr + lenSize; }
		void resize(uint64_t newSize) noexcept;
		char const* c_str() const noexcept { return ptr; }
		string operator+(const string& str) const noexcept
		{
			return string(*this, str);
		}
		string operator+(const char* str) const noexcept
		{
			return string(*this, str);
		}
		string operator+(char str) const noexcept
		{
			return string(*this, str);
		}
		string& operator+=(const string& str) noexcept;
		string& operator+=(const char* str) noexcept;
		string& operator+=(char str) noexcept;
		char& operator[](uint64_t index) noexcept;
		char const& operator[](uint64_t index) const noexcept;
		bool operator==(const string& str) const noexcept
		{
			if (str.lenSize != lenSize) return false;
			return Equal(str.data(), str.lenSize);
		}
		bool operator==(const char* str) const noexcept
		{
			auto sz = strlen(str);
			if (sz != lenSize) return false;
			return Equal(str, sz);
		}
		bool operator!=(const string& str) const noexcept
		{
			return !operator==(str);
		}
		bool operator!=(const char* str) const noexcept
		{
			return !operator==(str);
		}
		void erase(uint64_t index) noexcept;
		~string() noexcept;
	};
	class DLL_CLASS wstring
	{
	private:
		wchar_t* ptr = nullptr;
		uint64_t lenSize = 0;
		uint64_t capacity = 0;
		bool Equal(wchar_t const* str, uint64_t count) const noexcept;
		static uint64_t wstrLen(wchar_t const* ptr)
		{
			uint64_t sz = 0;
			while (ptr[sz] != 0)
			{
				sz++;
			}
			return sz;
		}
	public:
		wstring(const wstring& a, const wstring& b) noexcept;
		wstring(const wstring& a, const wchar_t* b) noexcept;
		wstring(const wchar_t* a, const wstring& b) noexcept;
		wstring(const wstring& a, wchar_t b) noexcept;
		wstring(wchar_t a, const wstring& b) noexcept;
		uint64_t size() const noexcept { return lenSize; }
		uint64_t length() const noexcept { return lenSize; }
		uint64_t getCapacity() const noexcept { return capacity; }
		wstring() noexcept;
		wstring(const wchar_t* wchr) noexcept;
		wstring(const wchar_t* wchr, const wchar_t* wchrEnd) noexcept;
		wstring(const char* wchr) noexcept;
		wstring(const char* wchr, const char* wchrEnd) noexcept;
		wstring(const wstring& data) noexcept;
		wstring(uint64_t size, wchar_t c) noexcept;
		wstring(string const& str) noexcept;
		void clear() noexcept
		{
			if (ptr)
				ptr[0] = 0;
			lenSize = 0;
		}
		void push_back(wchar_t c) noexcept
		{
			(*this) += c;
		}
		bool empty() const noexcept
		{
			return lenSize == 0;
		}
		wstring& operator=(const wstring& data) noexcept;
		wstring& operator=(const wchar_t* data) noexcept;
		wstring& operator=(wchar_t data) noexcept;
		wstring& assign(const wstring& data) noexcept
		{
			return operator=(data);
		}
		wstring& assign(const wchar_t* data) noexcept {
			return operator=(data);
		}
		wstring& assign(wchar_t data) noexcept {
			return operator=(data);
		}
		wchar_t const* begin() const noexcept { return ptr; }
		wchar_t const* end() const noexcept { return ptr + lenSize; }
		void reserve(uint64_t targetCapacity) noexcept;
		wchar_t* data() const noexcept { return ptr; }
		void resize(uint64_t newSize) noexcept;
		wchar_t const* c_str() const noexcept { return ptr; }
		wstring operator+(const wstring& str) const noexcept
		{
			return wstring(*this, str);
		}
		wstring operator+(const wchar_t* str) const noexcept
		{
			return wstring(*this, str);
		}
		wstring operator+(wchar_t str) const noexcept
		{
			return wstring(*this, str);
		}
		wstring& operator+=(const wstring& str) noexcept;
		wstring& operator+=(const wchar_t* str) noexcept;
		wstring& operator+=(wchar_t str) noexcept;
		wchar_t& operator[](uint64_t index) noexcept;
		wchar_t const& operator[](uint64_t index) const noexcept;
		bool operator==(const wstring& str) const noexcept
		{
			if (str.lenSize != lenSize) return false;
			return Equal(str.data(), str.lenSize);
		}
		bool operator==(const wchar_t* str) const noexcept
		{
			auto sz = wstrLen(str);
			if (sz != lenSize) return false;
			return Equal(str, sz);
		}
		bool operator!=(const string& str) const noexcept
		{
			return !operator==(str);
		}
		bool operator!=(const char* str) const noexcept
		{
			return !operator==(str);
		}
		void erase(uint64_t index) noexcept;
		~wstring() noexcept;
	};
	template <class _Elem, class _UTy>
	_Elem* UIntegral_to_buff(_Elem* _RNext, _UTy _UVal) noexcept { // format _UVal into buffer *ending at* _RNext
		static_assert(std::is_unsigned_v<_UTy>, "_UTy must be unsigned");

#ifdef _WIN64
		auto _UVal_trunc = _UVal;
#else // ^^^ _WIN64 ^^^ // vvv !_WIN64 vvv

		constexpr bool _Big_uty = sizeof(_UTy) > 4;
		if
			_CONSTEXPR_IF(_Big_uty) { // For 64-bit numbers, work in chunks to avoid 64-bit divisions.
			while (_UVal > 0xFFFFFFFFU) {
				auto _UVal_chunk = static_cast<unsigned long>(_UVal % 1000000000);
				_UVal /= 1000000000;

				for (int _Idx = 0; _Idx != 9; ++_Idx) {
					*--_RNext = static_cast<_Elem>('0' + _UVal_chunk % 10);
					_UVal_chunk /= 10;
				}
			}
		}

		auto _UVal_trunc = static_cast<unsigned long>(_UVal);
#endif // _WIN64

		do {
			*--_RNext = static_cast<_Elem>('0' + _UVal_trunc % 10);
			_UVal_trunc /= 10;
		} while (_UVal_trunc != 0);
		return _RNext;
	}
	template <class _Ty>
	inline string IntegerToString(const _Ty _Val)  noexcept { // convert _Val to string
		static_assert(std::is_integral_v<_Ty>, "_Ty must be integral");
		using _UTy = std::make_unsigned_t<_Ty>;
		char _Buff[21]; // can hold -2^63 and 2^64 - 1, plus NUL
		char* const _Buff_end = std::end(_Buff);
		char* _RNext = _Buff_end;
		const auto _UVal = static_cast<_UTy>(_Val);
		if (_Val < 0) {
			_RNext = UIntegral_to_buff(_RNext, static_cast <_UTy>(0 - _UVal));
			*--_RNext = '-';
		}
		else {
			_RNext = UIntegral_to_buff(_RNext, _UVal);
		}


		return string(_RNext, _Buff_end);
	}
	inline string to_string(double _Val) noexcept
	{
		const auto _Len = static_cast<size_t>(_CSTD _scprintf("%f", _Val));
		string _Str(_Len, '\0');
		_CSTD sprintf_s(&_Str[0], _Len + 1, "%f", _Val);
		return _Str;
	}
	inline string to_string(float _Val) noexcept
	{
		return to_string((double)_Val);
	}

	inline string to_string(int32_t _Val) noexcept
	{
		return IntegerToString(_Val);
	}
	inline string to_string(uint32_t _Val) noexcept
	{
		return IntegerToString(_Val);
	}
	inline string to_string(int16_t _Val) noexcept
	{
		return IntegerToString(_Val);
	}
	inline string to_string(uint16_t _Val) noexcept
	{
		return IntegerToString(_Val);
	}
	inline string to_string(int8_t _Val) noexcept
	{
		return IntegerToString(_Val);
	}
	inline string to_string(uint8_t _Val) noexcept
	{
		return IntegerToString(_Val);
	}
	inline string to_string(int64_t _Val) noexcept
	{
		return IntegerToString(_Val);
	}
	inline string to_string(uint64_t _Val) noexcept
	{
		return IntegerToString(_Val);
	}
	template <class _Ty>
	inline wstring IntegerToWString(const _Ty _Val) noexcept { // convert _Val to string
		static_assert(std::is_integral_v<_Ty>, "_Ty must be integral");
		using _UTy = std::make_unsigned_t<_Ty>;
		wchar_t _Buff[21]; // can hold -2^63 and 2^64 - 1, plus NUL
		wchar_t* const _Buff_end = std::end(_Buff);
		wchar_t* _RNext = _Buff_end;
		const auto _UVal = static_cast<_UTy>(_Val);
		if (_Val < 0) {
			_RNext = UIntegral_to_buff(_RNext, static_cast <_UTy>(0 - _UVal));
			*--_RNext = '-';
		}
		else {
			_RNext = UIntegral_to_buff(_RNext, _UVal);
		}

		return wstring(_RNext, _Buff_end);
	}



	inline wstring to_wstring(double _Val) noexcept { // convert double to wstring
		const auto _Len = static_cast<size_t>(_CSTD _scwprintf(L"%f", _Val));
		wstring _Str(_Len, L'\0');
		_CSTD swprintf_s(&_Str[0], _Len + 1, L"%f", _Val);
		return _Str;
	}
	inline wstring to_wstring(float _Val) noexcept
	{
		return to_wstring((double)_Val);
	}

	inline wstring to_wstring(int32_t _Val) noexcept
	{
		return IntegerToWString(_Val);
	}
	inline wstring to_wstring(uint32_t _Val) noexcept
	{
		return IntegerToWString(_Val);
	}
	inline wstring to_wstring(int16_t _Val) noexcept
	{
		return IntegerToWString(_Val);
	}
	inline wstring to_wstring(uint16_t _Val) noexcept
	{
		return IntegerToWString(_Val);
	}
	inline wstring to_wstring(int8_t _Val) noexcept
	{
		return IntegerToWString(_Val);
	}
	inline wstring to_wstring(uint8_t _Val) noexcept
	{
		return IntegerToWString(_Val);
	}
	inline wstring to_wstring(int64_t _Val) noexcept
	{
		return IntegerToWString(_Val);
	}
	inline wstring to_wstring(uint64_t _Val) noexcept
	{
		return IntegerToWString(_Val);
	}
}


inline vengine::string operator+(char c, const vengine::string& str) noexcept
{
	return vengine::string(c, str);
}

inline vengine::string operator+(const char* c, const vengine::string& str) noexcept
{
	return vengine::string(c, str);
}

inline vengine::wstring operator+(wchar_t c, const vengine::wstring& str) noexcept
{
	return vengine::wstring(c, str);
}

inline vengine::wstring operator+(const wchar_t* c, const vengine::wstring& str) noexcept
{
	return vengine::wstring(c, str);
}

#include "Hash.h"
namespace vengine
{
	template <>
	struct hash<vengine::string>
	{
		inline size_t operator()(const vengine::string& str) const noexcept
		{
			return Hash::CharArrayHash(str.c_str(), str.size());
		}
	};
	template <>
	struct hash<vengine::wstring>
	{
		inline size_t operator()(const vengine::wstring& str) const noexcept
		{
			return Hash::CharArrayHash((const char*)str.c_str(), str.size() * 2);
		}
	};
}