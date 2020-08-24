//------------------------------------------------------------------------------
// MIT License
//------------------------------------------------------------------------------
// 
// Copyright (c) 2020 Thakee Nathees
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//------------------------------------------------------------------------------

#ifdef ___________________USAGE___________________
// USAGE:
//     Note that you must define VAR_IMPLEMENTATION in exactly one source file. Use
//     UNDEF_VAR_DEFINES to undef var.h macros (like DEBUG_BREAK, DEBUG_PRINT...)
//     if you don't want them.

#define VAR_IMPLEMENTATION
#include "var.h"
using namespace varh;

// SAMPLES:
#include <iostream>
#define print(x) std::cout << (x) << std::endl
int main() {
	var v;
	v = 3.14;           print(v); // prints 3.14 float
	v = "hello world!"; print(v); // prints the String

	v = String("string"); v = Vect2f(1.2, 3.4); v = Vect2i(1, 2);
	v = Map(); v = Array(1, 2.3, "hello world!", Array(4, 5, 6));

	class Aclass : public Object {
	public: String to_string() const { return "Aclass"; }
	};
	v = newptr<Aclass>(); print(v); // prints Aclass
}

#endif // ___________________USAGE___________________

#define VAR_H_HEADER_ONLY

#ifndef _VAR_H
#define _VAR_H


#ifndef STRING_H
#define STRING_H


#ifndef VARHCORE_H
#define VARHCORE_H

#include <assert.h>
#include <cstring>
#include <iostream>
#include <memory>
#include <ostream>
#include <sstream>
#include <stdarg.h>
#include <stdio.h>
#include <string>
#include <type_traits>
#include <typeinfo>

#define _USE_MATH_DEFINES
#include <map>
#include <unordered_map>
#include <math.h>
#include <vector>

#define func var

#define STRCAT2(m_a, m_b) m_a##m_b
#define STRCAT3(m_a, m_b, m_c) m_a##m_b##m_c
#define STRCAT4(m_a, m_b, m_c, m_d) m_a##m_b##m_c##m_d

#define STR(m_mac) #m_mac
#define STRINGIFY(m_mac) STR(m_mac)
#define PLACE_HOLDER_MACRO

namespace varh {
template<typename T>
using ptr = std::shared_ptr<T>;

template<typename T>
using stdvec = std::vector<T>;

// TODO: refactor map to has table (unordered_map) it require hash<var>() implementation.
template<typename T1, typename T2>
using stdmap = std::map<T1, T2>;

template<typename T1, typename T2>
using stdhashtable = std::unordered_map<T1, T2>;

template<typename T, typename... Targs>
inline ptr<T> newptr(Targs... p_args) {
	return std::make_shared<T>(p_args...);
}

template<typename T1, typename T2>
inline ptr<T1> ptrcast(T2 p_ptr) {
	return std::static_pointer_cast<T1>(p_ptr);
}

#define VSNPRINTF_BUFF_SIZE 8192

#if defined(DOUBLE_AS_REAL)
typedef double real_t;
#else
typedef float real_t;
#endif
} // namespace varh

#ifndef DEBUG_BUILD
#if defined(_DEBUG) || defined(DEBUG)
#define DEBUG_BUILD
#endif
#endif

#ifdef DEBUG_BUILD
#ifdef _MSC_VER
#define DEBUG_BREAK() __debugbreak()
#else
#define DEBUG_BREAK() __builtin_trap()
#endif
#else
#define DEBUG_BREAK()
#endif

#define DEBUG_PRINT(m_msg, ...) \
	printf("DEBUG: %s\n\tat: %s(%s:%li)", m_msg, __FUNCTION__, __FILE__, __LINE__)

#if defined(DEBUG_BUILD)
#define VAR_ASSERT(m_cond)                                                                                            \
	if (!(m_cond)) {                                                                                                  \
		printf("ASSERTION FAILED: %s = false\n\tat: %s(%s:%li)", #m_cond, __FUNCTION__, __FILE__, __LINE__);          \
		DEBUG_BREAK();                                                                                                \
	} else (void(0))
#else
#define VAR_ASSERT
#endif

namespace varh {
class VarError : public std::exception {
public:
	enum Type {
		OK,
		NULL_POINTER,
		INVALID_INDEX,
		INVALID_KEY,
		INVALID_CASTING,
		INVALID_GET_NAME,
		INVALID_SET_VALUE,
		INVALID_ARGUMENT,
		INVALID_ARG_COUNT,

		NOT_IMPLEMENTED,
		OPERATOR_NOT_SUPPORTED,
		ZERO_DIVISION,

		_ERROR_MAX_,
	};

	const char* what() const noexcept override { return msg.c_str(); }
	Type get_type() const { return type; }
	static std::string get_err_name(Type p_type);

	VarError() {}
	VarError(Type p_type) { type = p_type; }
	VarError(Type p_type, const std::string& p_msg) {
		type = p_type;
		msg = p_msg;
	}
	

private:
	Type type = OK;
	std::string msg;
};
}

#endif // VARHCORE_H

namespace varh {

class var;
class Object;

class String {
public:

	// Constructors.
	String()                           {}
	String(const std::string& p_copy)  { _data = p_copy; }
	String(const char* p_copy)         { _data = p_copy; }
	String(const String& p_copy)       { _data = p_copy._data; }
	String(char p_char)                { _data = p_char; }
	explicit String(int p_i)                    { _data = std::to_string(p_i); }
	explicit String(int64_t p_i)                { _data = std::to_string(p_i); }
	explicit String(size_t p_i)                 { _data = std::to_string(p_i); }
	explicit String(double p_d)                 { _data = std::to_string(p_d); }
	~String()                          {}

	// Methods.
	var call_method(const String& p_method, const stdvec<var>& p_args);

	static String format(const char* p_format, ...);
	int64_t to_int() const { return std::stoll(_data); } // TODO: this will throw std::exceptions
	double to_float() const { return std::stod(_data); }
	String get_line(uint64_t p_line) const;
	size_t hash() const { return std::hash<std::string>{}(_data); }

	String substr(size_t p_start, size_t p_end) const;
	bool endswith(const String& p_str) const;
	bool startswith(const String& p_str) const;
	// String strip(p_delemeter = " "); lstrip(); rstrip();
	// Array split(p_delimeter = " ");

	// operators.
	char operator[](int64_t p_index) const {
		if (0 <= p_index && p_index < (int64_t)size())
			return _data[p_index];
		if ((int64_t)size() * -1 <= p_index && p_index < 0)
			return _data[size() + p_index];
		throw VarError(VarError::INVALID_INDEX, "");
	}
	char& operator[](int64_t p_index) {
		if (0 <= p_index && p_index < (int64_t)size())
			return _data[p_index];
		if ((int64_t)size() * -1 <= p_index && p_index < 0)
			return _data[size() + p_index];
		throw VarError(VarError::INVALID_INDEX, "");
	}

	operator std::string() const                   { return _data; }
	// operator bool() {} don't implement this don't even delete
	
	bool operator==(const String & p_other) const  { return _data == p_other._data; }
	bool operator!=(const String & p_other) const  { return _data != p_other._data; }
	bool operator<(const String& p_other) const    { return _data < p_other._data; }

	String operator+(char p_c) const               { return _data + p_c; }
	String operator+(int p_i) const                { return _data + std::to_string(p_i); }
	String operator+(double p_d) const             { return _data + std::to_string(p_d); }
	String operator+(const char* p_cstr) const     { return _data + p_cstr; }
	String operator+(const String& p_other) const  { return _data + p_other._data; }
	// String operator+(var& p_obj) const          { TODO: }

	String& operator+=(char p_c)                   { _data += p_c;                 return *this; }
	String& operator+=(int p_i)                    { _data += std::to_string(p_i); return *this; }
	String& operator+=(double p_d)                 { _data += std::to_string(p_d); return *this; }
	String& operator+=(const char* p_cstr)         { _data += p_cstr;              return *this; }
	String& operator+=(const String& p_other)      { _data += p_other._data;       return *this; }
	// String& operator+(var& p_obj)               { TODO: }

	String& operator=(char p_c)                   { _data = p_c;                 return *this; }
	String& operator=(int p_i)                    { _data = std::to_string(p_i); return *this; }
	String& operator=(double p_d)                 { _data = std::to_string(p_d); return *this; }
	String& operator=(const char* p_cstr)         { _data = p_cstr;              return *this; }
	String& operator=(const String& p_other)      { _data = p_other._data;       return *this; }
	// String& operator=(var& p_obj)               { TODO: }

	// Wrappers.
	size_t size() const                   { return _data.size(); }
	const char* c_str() const             { return _data.c_str(); }
	String& append(const String& p_other) { _data.append(p_other); return *this; }

private:
	friend class var;
	friend std::ostream& operator<<(std::ostream& p_ostream, const String& p_str);
	friend std::istream& operator>>(std::istream& p_ostream, String& p_str);
	std::string _data;
};

// Global operators. TODO: implement more
bool operator==(const char* p_cstr, const String& p_str);
bool operator!=(const char* p_cstr, const String& p_str);


}

#endif // STRING_H

#ifndef  ARRAY_H
#define  ARRAY_H

//include "varhcore.h"

namespace varh {

class Array {
public:
	// Methods.
	Array() {
		_data = newptr<stdvec<var>>();
	}
	Array(const ptr<std::vector<var>>& p_data) {
		_data = p_data;
	}
	Array(const Array& p_copy) {
		_data = p_copy._data;
	}
	template <typename... Targs>
	Array(Targs... p_args) {
		_data = newptr<stdvec<var>>();
		_make_va_arg_array(p_args...);
	}

	std::vector<var>* get_data() {
		return _data.operator->();
	}
	std::vector<var>* get_data() const {
		return _data.operator->();
	}

	Array copy(bool p_deep = true) const;

	// Wrappers.
	// TODO: throw all errors with VarError
	size_t size() const { return _data->size(); }
	bool empty() const { return _data->empty(); }
	void push_back(const var& p_var) { _data->push_back(p_var); }
	void pop_back() { _data->pop_back(); }
	Array& append(const var& p_var) { push_back(p_var); return *this; }
	var& pop() { var& ret = this->operator[](size() - 1); pop_back(); return ret; } 
	var& operator[](int64_t p_index) const {
		if (0 <= p_index && p_index < (int64_t)size())
			return _data->operator[](p_index);
		if ((int64_t)size() * -1 <= p_index && p_index < 0)
			return _data->operator[](size() + p_index);
		throw VarError(VarError::INVALID_INDEX, "");
	}
	var& operator[](int64_t p_index) { 
		if (0 <= p_index && p_index < (int64_t)size())
			return _data->operator[](p_index);
		if ((int64_t)size() * -1 <= p_index && p_index < 0)
			return _data->operator[](size() + p_index);
		throw VarError(VarError::INVALID_INDEX, "");
	}
	std::vector<var>::const_iterator begin() const { return (*_data).begin(); }
	std::vector<var>::const_iterator end() const { return (*_data).end(); }
	void clear() { (*_data).clear(); }
	var& at(size_t p_pos) { return (*_data).at(p_pos); }
	var& back() { return (*_data).back(); }
	var& front() { return (*_data).front(); }
	// TODO: 

	// Operators.
	operator bool() const { return empty(); }
	String to_string() const; // operator String() const;
	bool operator ==(const Array& p_other) const;
	Array& operator=(const Array& p_other);
	Array operator+(const Array& p_other) const;
	Array& operator+=(const Array& p_other);

private:
	friend class var;
	ptr<std::vector<var>> _data;
	friend std::ostream& operator<<(std::ostream& p_ostream, const Array& p_arr);

	// va_args constructor internal
	template <typename T, typename... Targs>
	constexpr void _make_va_arg_array(T p_val, Targs... p_args) {
		push_back(p_val);
		_make_va_arg_array(p_args...);
	}
	void _make_va_arg_array() { return; }
};

}

#endif // ARRAY_H

#ifndef VECTOR_H
#define VECTOR_H

//include "varhcore.h"

#define VECT2_DEFAULT_CMP_OP_OVERLOAD(m_op)            \
bool operator m_op (const Vect2<T>& p_other) const {   \
	return get_length() m_op p_other.get_length();     \
}
#define VECT3_DEFAULT_CMP_OP_OVERLOAD(m_op)            \
bool operator m_op (const Vect3<T>& p_other) const {   \
	return get_length() m_op p_other.get_length();     \
}

#define MISSED_ENUM_CHECK(m_max_enum, m_max_value) \
    static_assert((int)m_max_enum == m_max_value, "MissedEnum: " STRINGIFY(m_max_enum) " != " STRINGIFY(m_value) \
        "\n\tat: " __FILE__ "(" STRINGIFY(__LINE__) ")")

namespace varh {

template<typename T>
struct Vect2 {
	union { T x, width; };
	union { T y, height; };

	// Constructors
	Vect2(T p_x = 0, T p_y = 0) : x(p_x), y(p_y) {}
	Vect2(const T* p_arr) : x(p_arr[0]), y(p_arr[1]) {}
	template<typename T2>
	Vect2(const Vect2<T2>& p_copy) : x((T)p_copy.x), y((T)p_copy.y) {}

	// Methods.
	real_t get_length() const {
		return (real_t)sqrtf((real_t)(x * x + y * y));
	}
	float get_angle() const {
		if (x == 0) return (float)M_PI_2;
		return atanf(y / x);
	}

	// Operators.
	Vect2<T> operator+(const Vect2<T>& p_other) const {
		return Vect2<T>(x + p_other.x, y + p_other.y);
	}
	Vect2<T> operator-(const Vect2<T>& p_other) const {
		return Vect2<T>(x - p_other.x, y - p_other.y);
	}
	Vect2<T> operator*(const Vect2<T>& p_other) const {
		return Vect2<T>(x * p_other.x, y * p_other.y);
	}
	template <typename T2>
	Vect2<T> operator*(T2 p_val) const {
		return Vect2<T>(x * p_val, y * p_val);
	}
	Vect2<T> operator/(const Vect2<T>& p_other) const {
		if (p_other.x == 0 || p_other.y == 0)
			throw VarError(VarError::ZERO_DIVISION, "");
		return Vect2<T>(x / p_other.x, y / p_other.y);
	}

	VECT2_DEFAULT_CMP_OP_OVERLOAD(<);
	VECT2_DEFAULT_CMP_OP_OVERLOAD(<=);
	VECT2_DEFAULT_CMP_OP_OVERLOAD(>);
	VECT2_DEFAULT_CMP_OP_OVERLOAD(>=);
	bool operator==(const Vect2<T>& p_other) const {
		return x == p_other.x && y == p_other.y;
	}
	bool operator!=(const Vect2<T>& p_other) const {
		return !operator==(p_other);
	}

	Vect2<T>& operator+=(const Vect2<T>& p_other) {
		x += p_other.x; y += p_other.y;
		return *this;
	}
	Vect2<T>& operator-=(const Vect2<T>& p_other) {
		x -= p_other.x; y -= p_other.y;
		return *this;
	}

	Vect2<T>& operator*=(const Vect2<T>& p_other) {
		x *= p_other.x; y *= p_other.y;
		return *this;
	}

	Vect2<T>& operator/=(const Vect2<T>& p_other) {
		if (p_other.x == 0 || p_other.y == 0)
			throw VarError(VarError::ZERO_DIVISION, "");
		x /= p_other.x; y /= p_other.y;
		return *this;
	}

	operator bool() const { return x == 0 && y == 0; }
	String to_string() const {
		return String("Vect2(")
			.append(std::to_string(x)).append(", ")
			.append(std::to_string(y)).append(")");
	}
};


template<typename T>
struct Vect3 {
	union { T x, width; };
	union { T y, height; };
	union { T z, depth; };

	// Constructors.
	Vect3(T p_x = 0, T p_y = 0, T p_z = 0) : x(p_x), y(p_y), z(p_z) {}
	Vect3(const T* p_arr) : x(p_arr[0]), y(p_arr[1]), z(p_arr[2]) {}
	template<typename T2>
	Vect3(const Vect3<T2>& p_copy) : x((T)p_copy.x), y((T)p_copy.y), z((T)p_copy.z) {}

	// Methods.
	real_t get_length() const {
		return (real_t)sqrtf((real_t)(x * x + y * y + z * z));
	}

	// Operators.
	Vect3<T> operator+(const Vect3<T>& p_other) const {
		return Vect3<T>(x + p_other.x, y + p_other.y, z + p_other.z);
	}
	Vect3<T> operator-(const Vect3<T>& p_other) const {
		return Vect3<T>(x - p_other.x, y - p_other.y, z - p_other.z);
	}
	Vect3<T> operator*(const Vect3<T>& p_other) const {
		return Vect3<T>(x * p_other.x, y * p_other.y, z * p_other.z);
	}
	Vect3<T> operator/(const Vect3<T>& p_other) const {
		if (p_other.x == 0 || p_other.y == 0 || p_other.z == 0)
			throw VarError(VarError::ZERO_DIVISION, "");
		return Vect3<T>(x / p_other.x, y / p_other.y, z / p_other.z);
	}

	VECT2_DEFAULT_CMP_OP_OVERLOAD(<);
	VECT2_DEFAULT_CMP_OP_OVERLOAD(<=);
	VECT2_DEFAULT_CMP_OP_OVERLOAD(>);
	VECT2_DEFAULT_CMP_OP_OVERLOAD(>=);
	bool operator==(const Vect3<T>& p_other) const {
		return x == p_other.x && y == p_other.y && z == p_other.z;
	}
	bool operator!=(const Vect3<T>& p_other) const {
		return !operator==(p_other);
	}

	Vect3<T>& operator+=(const Vect3<T>& p_other) {
		x += p_other.x; y += p_other.y; z += p_other.z;
		return *this;
	}
	Vect3<T>& operator-=(const Vect3<T>& p_other) {
		x -= p_other.x; y -= p_other.y; z -= p_other.z;
		return *this;
	}
	Vect3<T>& operator*=(const Vect3<T>& p_other) {
		x *= p_other.x; y *= p_other.y; z *= p_other.z;
		return *this;
	}
#define M_OP_TEMPLATE(T2)                    \
	Vect3<T>& operator*=(T2 p_val) const {   \
		x *= p_val; y *= p_val; z *= p_val;  \
		return *this;                        \
	}
	M_OP_TEMPLATE(float)
	M_OP_TEMPLATE(int)
#undef M_OP_TEMPLATE

	template<typename T2>
	Vect3<T>& operator/=(const Vect3<T2>& p_other) {
		if (p_other.x == 0 || p_other.y == 0 || p_other.z == 0)
			throw VarError(VarError::ZERO_DIVISION, "");
		x /= (T)p_other.x; y /= (T)p_other.y; z /= (T)p_other.z;
		return *this;
	}

	operator bool() const { return x == 0 && y == 0 && z == 0; }
	String to_string() const {
		return String("Vect3(")
			.append(std::to_string(x)).append(", ")
			.append(std::to_string(y)).append(", ")
			.append(std::to_string(z)).append(")");
	}
};

template<typename T>
std::ostream& operator<<(std::ostream& p_ostream, const Vect2<T>& p_vect) {
	p_ostream << p_vect.to_string();
	return p_ostream;
}
template<typename T>
std::ostream& operator<<(std::ostream& p_ostream, const Vect3<T>& p_vect) {
	p_ostream << p_vect.to_string();
	return p_ostream;
}

/* typedefs */
typedef Vect2<double> Vect2f;
typedef Vect2<int64_t> Vect2i;
typedef Vect3<double> Vect3f;
typedef Vect3<int64_t> Vect3i;

typedef Vect2f Size;
typedef Vect2f Point;

}

#endif //VECTOR_H

#ifndef  MAP_H
#define  MAP_H

//include "varhcore.h"


namespace varh {

class var;
class String;

class Map {
public:
	// Mehtods.
	Map() {
		_data = std::make_shared<stdmap<var, var>>();
	}
	Map(const ptr<stdmap<var, var>>& p_data) {
		_data = p_data;
	}
	Map(const Map& p_copy) {
		_data = p_copy._data;
	}

	stdmap<var, var>* get_data() {
		return _data.operator->();
	}
	stdmap<var, var>* get_data() const {
		return _data.operator->();
	}

	Map copy(bool p_deep = true) const;

	// Wrappers.
	size_t size() const { return _data->size(); }
	bool empty() const { return _data->empty(); }
	void insert(const var& p_key, const var& p_value);
	var& operator[](const var& p_key) const;
	var& operator[](const var& p_key);
	var& operator[](const char* p_key) const;
	var& operator[](const char* p_key);
	stdmap<var, var>::iterator begin() const;
	stdmap<var, var>::iterator end() const;
	stdmap<var, var>::iterator find(const var& p_key) const;
	void clear() { _data->clear(); }
	bool has(const var& p_key) const;
	// TODO:

	// Operators.
	operator bool() const { return empty(); }
	String to_string() const; // operator String() const;
	bool operator ==(const Map& p_other) const;
	Map& operator=(const Map& p_other);

private:
	friend class var;
	ptr<stdmap<var, var>> _data;
	friend std::ostream& operator<<(std::ostream& p_ostream, const Map& p_dict);
};

}

#endif // MAP_H

#ifndef OBJECT_H
#define OBJECT_H

//include "varhcore.h"

#ifndef INHERITS_OBJECT_ADDNL
#define INHERITS_OBJECT_ADDNL
#endif //INHERITS_OBJECT_ADDNL

#define INHERITS_OBJECT(m_class, m_inherits)                                                         \
public:                                                                                              \
	static  const char* get_class_name_s() { return STR(m_class); }                                  \
	virtual const char* get_class_name() const override { return get_class_name_s(); }               \
	static  const char* get_parent_class_name_s() { return STR(m_inherits); }                        \
	virtual const char* get_parent_class_name() const override { return get_parent_class_name_s(); } \
	INHERITS_OBJECT_ADDNL(m_class, m_inherits)                                                       \
private:

namespace varh {

class Object {
public:

	// Operators.
	operator String()  const { return to_string(); }
	Object& operator=(const Object& p_copy) = default;
	var operator()(stdvec<var>& p_vars);

	bool operator==(const var& p_other) const { return __eq(p_other); }
	bool operator!=(const var& p_other) const { return !operator == (p_other); }
	bool operator<=(const var& p_other) const { return __lt(p_other) || __eq(p_other); }
	bool operator>=(const var& p_other) const { return __gt(p_other) || __eq(p_other); }
	bool operator< (const var& p_other) const { return __lt(p_other); }
	bool operator> (const var& p_other) const { return __gt(p_other); }

	var operator+(const var& p_other) const;
	var operator-(const var& p_other) const;
	var operator*(const var& p_other) const;
	var operator/(const var& p_other) const;

	var& operator+=(const var& p_other);
	var& operator-=(const var& p_other);
	var& operator*=(const var& p_other);
	var& operator/=(const var& p_other);

	var& operator[](const var& p_key) const;
	var& operator[](const var& p_key);

	// Virtual methods.
	// These double underscore methdos will be used as operators callback in the compiler.

	static var call_method(ptr<Object> p_self, const String& p_name, stdvec<var>& p_args);  // instance.p_name(args)
	virtual var __call(stdvec<var>& p_vars);                             // instance(args)

	virtual bool __has(const String& p_name) const;
	virtual var& __get(const String& p_name);

	virtual bool __has_mapped(const String& p_name) const;
	virtual var& __get_mapped(const var& p_key);
	//virtual void __set_mapped(const var& p_key, const var& p_val);

	virtual var __add(const var& p_other) const;
	virtual var __sub(const var& p_other) const;
	virtual var __mul(const var& p_other) const;
	virtual var __div(const var& p_other) const;

	virtual var& __add_eq(const var& p_other);
	virtual var& __sub_eq(const var& p_other);
	virtual var& __mul_eq(const var& p_other);
	virtual var& __div_eq(const var& p_other);

	virtual bool __gt(const var& p_other) const;
	virtual bool __lt(const var& p_other) const;
	virtual bool __eq(const var& p_other) const;

	virtual String to_string() const { return String::format("[Object:%i]", this);  }

	// Methods.
	virtual ptr<Object> copy(bool p_deep)         const { throw VarError(VarError::NOT_IMPLEMENTED); }
	static  const char* get_class_name_s()              { return "Object"; }
	virtual const char* get_class_name()          const { return get_class_name_s(); }
	static  const char* get_parent_class_name_s()       { return nullptr; }
	virtual const char* get_parent_class_name()   const { return get_parent_class_name_s(); }

	static void _bind_data() {} // TODO:

private:
	friend class var;
};

}


#endif //OBJECT_H

#define DATA_PTR_CONST(T) reinterpret_cast<const T *>(_data._mem)
#define DATA_PTR_OTHER_CONST(T) reinterpret_cast<const T *>(p_other._data._mem)

#define DATA_PTR(T) reinterpret_cast<T *>(_data._mem)
#define DATA_PTR_OTHER(T) reinterpret_cast<T *>(p_other._data._mem)

#define DATA_MEM_SIZE 4 * sizeof(real_t)

// TODO: var fn = &func; fn(); operator(){}

namespace varh {

class var {
public:
	enum Type {
		_NULL,
		BOOL,
		INT,
		FLOAT,
		STRING,

		// math types
		VECT2F,
		VECT2I,
		VECT3F,
		VECT3I,

		// misc types
		ARRAY,
		MAP,
		OBJECT,

		_TYPE_MAX_,
	};

	static String get_type_name_s(var::Type p_type);
	String get_type_name() const;

	/* constructors */
	var();
	var(const var& p_copy);
	var(bool p_bool);
	var(int p_int);
	var(int64_t p_int);
	var(float p_float);
	var(double p_double);
	var(const char* p_cstring);
	var(const String& p_string);
	var(const Vect2f& p_vect2f);
	var(const Vect2i& p_vect2i);
	var(const Vect3f& p_vect3f);
	var(const Vect3i& p_vect3i);
	var(const Array& p_array);
	var(const Map& p_map);
	~var();
	
	template <typename T=Object>
	var(const ptr<T>& p_ptr) {
		type = OBJECT;
		_data._obj = p_ptr;
	}

	// Methods.
	inline Type get_type() const { return type; }
	void clear();
	var copy(bool p_deep = false) const;

	const char* get_class_name() const;
	const char* get_parent_class_name() const;

	// Operators.
	operator bool() const;
	operator int64_t() const;
	operator int() const { return (int)operator int64_t(); }
	operator float() const { return (float)operator double(); }
	operator double() const;
	operator String() const;   // int.operator String() is invalid casting.
	String to_string() const;  // int.to_string() is valid.
	// this treated as: built-in C++ operator[](const char *, int), conflict with operator[](size_t)
	// operator const char* () const;
	operator Vect2f() const;
	operator Vect2i() const;
	operator Vect3f() const;
	operator Vect3i() const;
	operator Array() const;
	operator Map() const;
	operator ptr<Object>() const;

	template <typename T>
	ptr<T> cast_to() const {
		return ptrcast<T>(operator ptr<Object>());
	}

#define _VAR_OP_DECL(m_ret, m_op, m_access)                                                        \
	m_ret operator m_op (bool p_other) m_access { return operator m_op (var(p_other)); }           \
	m_ret operator m_op (int64_t p_other) m_access { return operator m_op (var(p_other)); }        \
	m_ret operator m_op (int p_other)     m_access { return operator m_op (var(p_other)); }        \
	m_ret operator m_op (double p_other) m_access { return operator m_op (var(p_other)); }         \
	m_ret operator m_op (const char* p_other) m_access { return operator m_op (var(p_other)); }    \
	m_ret operator m_op (const var& p_other) m_access
#define VAR_OP_DECL(m_ret, m_op, m_access) _VAR_OP_DECL(m_ret, m_op, m_access)

	VAR_OP_DECL(bool, ==, const);
	VAR_OP_DECL(bool, !=, const);
	VAR_OP_DECL(bool, < , const);
	VAR_OP_DECL(bool, > , const);
	VAR_OP_DECL(bool, <=, const);
	VAR_OP_DECL(bool, >=, const);

	var operator++();
	var operator++(int);
	var operator--();
	var operator--(int);
	bool operator !() const { return !operator bool(); }
	var& operator[](const var& p_key);

	var __get(const String& p_name) const;
	void __set(const String& p_name, const var& p_value);
	var __get_mapped(const var& p_key) const;
	void __set_mapped(const var& p_key, const var& p_value);

	template <typename... Targs>
	var __call(Targs... p_args) {
		stdvec<var> args;
		__build_args_recur(args, p_args...);
		return __call_internal(args);
	}
	template <typename... Targs>
	var call_method(const String& p_method, Targs... p_args) {
		stdvec<var> args;
		__build_args_recur(args, p_args...);
		return call_method_internal(p_method, args);
	}

private:
	var __call_internal(stdvec<var>& p_args);
	// TODO: call static func in every var classes.
	var call_method_internal(const String& p_method, stdvec<var>& p_args);
	template <typename T, typename... Targs>
	constexpr void __build_args_recur(stdvec<var>& p_args_arr, T p_val, Targs... p_args) {
		p_args_arr.push_back(p_val);
		__build_args_recur(p_args_arr, p_args...);
	}
	void __build_args_recur(stdvec<var>& p_args_arr) { return; }
public:

	VAR_OP_DECL(var, +, const);
	VAR_OP_DECL(var, -, const);
	VAR_OP_DECL(var, *, const);
	VAR_OP_DECL(var, /, const);
	VAR_OP_DECL(var, %, const);

	/* assignments */
	var& operator=(const var& p_other);
	template<typename T=Object>
	var& operator=(const ptr<T>& p_other) {
		clear_data();
		type = OBJECT;
		_data._obj = ptrcast<Object>(p_other);
		return *this;
	}
	VAR_OP_DECL(var&, +=, PLACE_HOLDER_MACRO);
	VAR_OP_DECL(var&, -=, PLACE_HOLDER_MACRO);
	VAR_OP_DECL(var&, *=, PLACE_HOLDER_MACRO);
	VAR_OP_DECL(var&, /=, PLACE_HOLDER_MACRO);
	VAR_OP_DECL(var&, %=, PLACE_HOLDER_MACRO);

private:
	struct VarData {
		VarData() : _float(.0f) {}
		~VarData() {}

		Map _map;
		Array _arr;
		ptr<Object> _obj;

		union {
			String _string;

			bool _bool;
			int64_t _int;
			double _float;
			uint8_t _mem[DATA_MEM_SIZE];
		};
	};

	// Methods.
	void copy_data(const var& p_other);
	void clear_data();

	// Members.
	static var tmp;
	Type type;
	VarData _data;
	friend std::ostream& operator<<(std::ostream& p_ostream, const var& p_var);
};

}

// undefine all var.h macros defined in varcore.h
// this makes the user(carbon) independent of'em
#if defined(UNDEF_VAR_DEFINES)
#if !defined(VAR_H_HEADER_ONLY)

#undef func
#undef STRCAT2
#undef STRCAT3
#undef STRCAT4
#undef STR
#undef STRINGIFY
#undef PLACE_HOLDER
#undef VSNPRINTF_BUFF_SIZE
#undef DEBUG_PRINT
#undef DEBUG_BREAK
#undef VAR_ASSERT
#undef MISSED_ENUM_CHECK
#undef UNDEF_VAR_DEFINES

#endif
#endif

#endif // _VAR_H

//--------------- VAR_IMPLEMENTATION -------------------

#if defined(VAR_IMPLEMENTATION)

//include "_var.h"

// add __FUNCTION__, __LINE__, __FILE__ to VarError.
#define THROW_ERROR(m_type, m_msg) \
	throw VarError(m_type, m_msg)

#define D_VEC(m_vect, m_dim, m_t) STRCAT3(m_vect, m_dim, m_t)

namespace varh {

var var::tmp;

std::ostream& operator<<(std::ostream& p_ostream, const String& p_str) {
	p_ostream << p_str.operator std::string();
	return p_ostream;
}
std::istream& operator>>(std::istream& p_istream, String& p_str) {
	p_istream >> p_str._data;
	return p_istream;
}

std::ostream& operator<<(std::ostream& p_ostream, const var& p_var) {
	p_ostream << p_var.to_string();
	return p_ostream;
}
std::ostream& operator<<(std::ostream& p_ostream, const Array& p_arr) {
	p_ostream << p_arr.to_string();
	return p_ostream;
}
std::ostream& operator<<(std::ostream& p_ostream, const Map& p_map) {
	p_ostream << p_map.to_string();
	return p_ostream;
}

static const char* _error_names[VarError::_ERROR_MAX_] = {
	"OK",
	"NULL_POINTER",
	"INVALID_INDEX",
	"INVALID_KEY",
	"INVALID_CASTING",
	"INVALID_GET_NAME",
	"INVALID_SET_VALUE",
	"NOT_IMPLEMENTED",
	"OPERATOR_NOT_SUPPORTED",
	"ZERO_DIVISION",
	//_ERROR_MAX_
};

std::string VarError::get_err_name(VarError::Type p_type) {
	return _error_names[p_type];
}

// String -----------------------------------------------
var String::call_method(const String& p_method, const stdvec<var>& p_args) {
	if (p_method == "to_int") {
		if (p_args.size() != 0) THROW_ERROR(VarError::INVALID_ARG_COUNT, "Expected at exactly 0 argument.");
		return to_int();
	} else if (p_method == "to_float") {
		if (p_args.size() != 0) THROW_ERROR(VarError::INVALID_ARG_COUNT, "Expected at exactly 0 argument.");
		return to_float();
	} else if (p_method == "get_line") {
		if (p_args.size() != 0) THROW_ERROR(VarError::INVALID_ARG_COUNT, "Expected at exactly 0 argument.");
		if (p_args[0].get_type() != var::INT) THROW_ERROR(VarError::INVALID_ARGUMENT, "Expected a numeric value at argument 0.");
		return get_line(p_args[0].operator int64_t());
	} else if (p_method == "hash") {
		if (p_args.size() != 0) THROW_ERROR(VarError::INVALID_ARG_COUNT, "Expected at exactly 0 argument.");
		return (int64_t)hash();
	} else if (p_method == "substr") {
		if (p_args.size() != 2) THROW_ERROR(VarError::INVALID_ARG_COUNT, "Expected at exactly 0 argument.");
		if (p_args[0].get_type() != var::INT) THROW_ERROR(VarError::INVALID_ARGUMENT, "Expected a numeric value at argument 0.");
		if (p_args[1].get_type() != var::INT) THROW_ERROR(VarError::INVALID_ARGUMENT, "Expected a numeric value at argument 1.");
		return substr((size_t)p_args[0].operator int64_t(), (size_t)p_args[1].operator int64_t());
	} else if (p_method == "endswith") {
		if (p_args.size() != 1) THROW_ERROR(VarError::INVALID_ARG_COUNT, "Expected at exactly 0 argument.");
		if (p_args[0].get_type() != var::STRING) THROW_ERROR(VarError::INVALID_ARGUMENT, "Expected a string value at argument 0.");
		return endswith(p_args[0].operator String());
	} else if (p_method == "startswith") {
		if (p_args.size() != 1) THROW_ERROR(VarError::INVALID_ARG_COUNT, "Expected at exactly 0 argument.");
		if (p_args[0].get_type() != var::STRING) THROW_ERROR(VarError::INVALID_ARGUMENT, "Expected a string value at argument 0.");
		return startswith(p_args[0].operator String());
	}
	// TODO: more.
	throw VarError(VarError::INVALID_GET_NAME, ""); // TODO:
}

String String::format(const char* p_format, ...) {
	va_list argp;

	va_start(argp, p_format);

	static const unsigned int BUFFER_SIZE = VSNPRINTF_BUFF_SIZE;
	char buffer[BUFFER_SIZE + 1]; // +1 for the terminating character
	int len = vsnprintf(buffer, BUFFER_SIZE, p_format, argp);

	va_end(argp);
	
	if (len == 0) return String();
	return String(buffer);
}

String String::get_line(uint64_t p_line) const {
	const char* source = _data.c_str();
	uint64_t cur_line = 1;
	std::stringstream ss_line;

	while (char c = *source) {
		if (c == '\n') {
			if (cur_line >= p_line) break;
			cur_line++;
		} else if (cur_line == p_line) {
			ss_line << c;
		}
		source++;
	}

	ss_line << '\n';
	return ss_line.str();
}

String String::substr(size_t p_start, size_t p_end) const {
	return _data.substr(p_start, p_end - p_start);
}
bool String::endswith(const String& p_str) const {
	if (p_str.size() > _data.size()) return false;
	for (size_t i = 1; i <= p_str.size(); i++) {
		if (_data[_data.size() - i] != p_str[p_str.size() - i]) {
			return false;
		}
	}
	return true;
}
bool String::startswith(const String& p_str) const {
	if (p_str.size() > _data.size()) return false;
	for (size_t i = 0; i < p_str.size(); i++) {
		if (_data[i] != p_str[i]) {
			return false;
		}
	}
	return true;
}

bool operator==(const char* p_cstr, const String& p_str) {
	return p_str == String(p_cstr);
}
bool operator!=(const char* p_cstr, const String& p_str) {
	return p_str != String(p_cstr);
}

// Array -----------------------------------------------

String Array::to_string() const {
	std::stringstream ss;
	ss << "[ ";
	for (unsigned int i = 0; i < _data->size(); i++) {
		ss << _data->operator[](i).operator String();
		if (i != _data->size() - 1) ss << ", ";
		else ss << " ";
	}
	ss << "]";
	return ss.str();
}

bool Array::operator ==(const Array& p_other) const {
	if (size() != p_other.size())
		return false;
	for (size_t i = 0; i < size(); i++) {
		if (operator[](i) != p_other[i])
			return false;
	}
	return true;
}

Array Array::copy(bool p_deep) const {
	Array ret;
	for (size_t i = 0; i < size(); i++) {
		if (p_deep)
			ret.push_back(operator[](i).copy(true));
		else
			ret.push_back(operator[](i));
	}
	return ret;
}

Array Array::operator+(const Array& p_other) const {
	Array ret = copy();
	for (size_t i = 0; i < p_other.size(); i++) {
		ret.push_back(p_other[i].copy());
	}
	return ret;
}

Array& Array::operator+=(const Array& p_other) {
	for (size_t i = 0; i < p_other.size(); i++) {
		push_back(p_other[i].copy());
	}
	return *this;
}

Array& Array::operator=(const Array& p_other) {
	_data = p_other._data;
	return *this;
}

// Map  ----------------------------------------
String Map::to_string() const {
	std::stringstream ss;
	ss << "{ ";
	for (stdmap<var, var>::iterator it = (*_data).begin(); it != (*_data).end(); it++) {
		if (it != (*_data).begin()) ss << ", ";
		ss << it->first.to_string() << " : " << it->second.to_string();
	}
	ss << " }";
	return ss.str();
}

Map Map::copy(bool p_deep) const {
	Map ret;
	for (stdmap<var, var>::iterator it = (*_data).begin(); it != (*_data).end(); it++) {
		if (p_deep)
			ret[it->first] = it->second.copy(true);
		else
			ret[it->first] = it->second;
	}
	return ret;
}

var& Map::operator[](const var& p_key) const { return (*_data).operator[](p_key); }
var& Map::operator[](const var& p_key) { return (*_data).operator[](p_key);}
var& Map::operator[](const char* p_key) const { return (*_data).operator[](p_key); }
var& Map::operator[](const char* p_key) { return (*_data).operator[](p_key); }

stdmap<var, var>::iterator Map::begin() const { return (*_data).begin(); }
stdmap<var, var>::iterator Map::end() const { return (*_data).end(); }
stdmap<var, var>::iterator Map::find(const var& p_key) const { return (*_data).find(p_key); }

void Map::insert(const var& p_key, const var& p_value) { (*_data).insert(std::pair<var, var>(p_key, p_value)); }
bool Map::has(const var& p_key) const { return find(p_key) != end(); }

bool Map::operator ==(const Map& p_other) const {
	if (size() != p_other.size())
		return false;
	for (stdmap<var, var>::iterator it_other = p_other.begin(); it_other != p_other.end(); it_other++) {
		stdmap<var, var>::iterator it_self = find(it_other->first);
		if (it_self == end()) return false;
		if (it_self->second != it_other->second) return false;

	}
	return true;
}

Map& Map::operator=(const Map& p_other) {
	_data = p_other._data;
	return *this;
}

// Object -----------------------------------------------

var Object::operator+(const var& p_other) const { return __add(p_other); }
var Object::operator-(const var& p_other) const { return __sub(p_other); }
var Object::operator*(const var& p_other) const { return __mul(p_other); }
var Object::operator/(const var& p_other) const { return __div(p_other); }

var& Object::operator+=(const var& p_other) { return __add_eq(p_other); }
var& Object::operator-=(const var& p_other) { return __sub_eq(p_other); }
var& Object::operator*=(const var& p_other) { return __mul_eq(p_other); }
var& Object::operator/=(const var& p_other) { return __div_eq(p_other); }

// TODO: refactor operator[] here and var::operator[] support for string.
var& Object::operator[](const var& p_key) { return __get_mapped(p_key); }

#ifndef _VAR_H_EXTERN_IMPLEMENTATIONS
// call_method() should call it's parent if method not exists.
var Object::call_method(ptr<Object> p_self, const String& p_name, stdvec<var>& p_args) { throw VarError(VarError::INVALID_GET_NAME); }
#endif

var Object::__call(stdvec<var>& p_vars) { throw VarError(VarError::NOT_IMPLEMENTED); }
var Object::operator()(stdvec<var>& p_vars) { return __call(p_vars); }

bool Object::__has(const String& p_name) const { return false; }
var& Object::__get(const String& p_name) { throw VarError(VarError::INVALID_GET_NAME, String::format("Name \"%s\" not exists in object.", p_name)); }
//void Object::__set(const String& p_name, const var& p_val) { throw VarError(VarError::INVALID_SET_NAME, String::format("Name \"%s\" not exists in object.", p_name)); }

bool Object::__has_mapped(const String& p_name) const { return false; }
var& Object::__get_mapped(const var& p_key) { throw VarError(VarError::OPERATOR_NOT_SUPPORTED); }
//void Object::__set_mapped(const var& p_key, const var& p_val) { throw VarError(VarError::OPERATOR_NOT_SUPPORTED); }

var Object::__add(const var& p_other) const { throw VarError(VarError::OPERATOR_NOT_SUPPORTED); }
var Object::__sub(const var& p_other) const { throw VarError(VarError::OPERATOR_NOT_SUPPORTED); }
var Object::__mul(const var& p_other) const { throw VarError(VarError::OPERATOR_NOT_SUPPORTED); }
var Object::__div(const var& p_other) const { throw VarError(VarError::OPERATOR_NOT_SUPPORTED); }

var& Object::__add_eq(const var& p_other) { throw VarError(VarError::OPERATOR_NOT_SUPPORTED); }
var& Object::__sub_eq(const var& p_other) { throw VarError(VarError::OPERATOR_NOT_SUPPORTED); }
var& Object::__mul_eq(const var& p_other) { throw VarError(VarError::OPERATOR_NOT_SUPPORTED); }
var& Object::__div_eq(const var& p_other) { throw VarError(VarError::OPERATOR_NOT_SUPPORTED); }

bool Object::__gt(const var& p_other) const { throw VarError(VarError::OPERATOR_NOT_SUPPORTED); } // TODO: This will throw if
bool Object::__lt(const var& p_other) const { throw VarError(VarError::OPERATOR_NOT_SUPPORTED); } // object used as key in a Map.
bool Object::__eq(const var& p_other) const { throw VarError(VarError::OPERATOR_NOT_SUPPORTED); }

// var -----------------------------------------------

void var::clear() {
	clear_data();
	type = _NULL;
}

var var::copy(bool p_deep) const {
	switch (type) {
		case _NULL:
		case BOOL:
		case INT:
		case FLOAT:
		case STRING:
		case VECT2F:
		case VECT2I:
		case VECT3F:
		case VECT3I:
			return *this;
		case ARRAY: return _data._arr.copy(p_deep);
		case MAP: return _data._map.copy(p_deep);
		case OBJECT: return _data._obj->copy(p_deep);
			break;
	}
	MISSED_ENUM_CHECK(_TYPE_MAX_, 12);
			// TODO:
	return *this;
}

const char* var::get_class_name() const {
	switch (type) {
		case var::_NULL:  return "null";
		case var::BOOL:   return "bool";
		case var::INT:    return "int";
		case var::FLOAT:  return "float";
		case var::STRING: return "String";
		case var::VECT2F: return "Vect2f";
		case var::VECT2I: return "Vect2i";
		case var::VECT3F: return "Vect3f";
		case var::VECT3I: return "Vect3i";
		case var::ARRAY:  return "Array";
		case var::MAP:    return "Map";
		case var::OBJECT: return _data._obj->get_class_name();
	}
	MISSED_ENUM_CHECK(_TYPE_MAX_, 12);
	return nullptr;
}

const char* var::get_parent_class_name() const {
	switch (type) {
		case var::OBJECT:
			return _data._obj->get_parent_class_name();
		default:
			return nullptr;
	}
}


/* constructors */
var::var() {
	_data._bool = false;
	type = _NULL;
}

var::var(const var& p_copy) {
	copy_data(p_copy);
	type = p_copy.type;
}

var::var(bool p_bool) {
	type = BOOL;
	_data._bool = p_bool;
}

var::var(int p_int) {
	type = INT;
	_data._int = p_int;
}

var::var(int64_t p_int) {
	type = INT;
	_data._int = p_int;
}

var::var(float p_float) {
	type = FLOAT;
	_data._float = p_float;
}

var::var(double p_double) {
	type = FLOAT;
	_data._float = p_double;
}

var::var(const char* p_cstring) {
	type = STRING;
	new(&_data._string) String(p_cstring);
}

var::var(const String& p_string) {
	type = STRING;
	new(&_data._string) String(p_string);
}

#define VAR_VECT_CONSTRUCTOR(m_dim, m_t, m_T)             \
var::var(const D_VEC(Vect, m_dim, m_t)& p_vect) {         \
	type = D_VEC(VECT, m_dim, m_T);                       \
	std::memcpy(_data._mem, &p_vect, sizeof(_data._mem)); \
}
VAR_VECT_CONSTRUCTOR(2, f, F)
VAR_VECT_CONSTRUCTOR(2, i, I)
VAR_VECT_CONSTRUCTOR(3, f, F)
VAR_VECT_CONSTRUCTOR(3, i, I)
#undef VAR_VECT_CONSTRUCTOR


var::var(const Array& p_array) {
	type = ARRAY;
	_data._arr = p_array;
}

var::var(const Map& p_map) {
	type = MAP;
	_data._map = p_map;
}

var::~var() {
	clear();
}

/* operator overloading */

#define VAR_OP_PRE_INCR_DECR(m_op)                              \
var var::operator m_op () {                                     \
	switch (type) {                                             \
		case INT:  return m_op _data._int;                      \
		case FLOAT: return m_op _data._float;                   \
		default: throw VarError(VarError::INVALID_CASTING, ""); \
	}                                                           \
	return var();                                               \
}

#define VAR_OP_POST_INCR_DECR(m_op)                             \
var var::operator m_op(int) {                                   \
	switch (type) {                                             \
		case INT: return _data._int m_op;                       \
		case FLOAT: return _data._float m_op;                   \
		default: throw VarError(VarError::INVALID_CASTING, ""); \
	}                                                           \
	return var();                                               \
}
VAR_OP_PRE_INCR_DECR(++)
VAR_OP_PRE_INCR_DECR(--)
VAR_OP_POST_INCR_DECR(++)
VAR_OP_POST_INCR_DECR(--)
#undef VAR_OP_PRE_INCR_DECR
#undef VAR_OP_POST_INCR_DECR

var& var::operator=(const var& p_other) {
	copy_data(p_other);
	return *this;
}

var& var::operator[](const var& p_key) {
	switch (type) {
		// strings can't return char as var&
		case STRING: {
			// handle operator[] for string seperatly;
			throw VarError(VarError::NOT_IMPLEMENTED, "yet another BUG!!!");
		}
		case ARRAY: {
			return _data._arr[p_key.operator int64_t()];
		}
		case MAP:
			return _data._map[p_key];
		case OBJECT:
			return _data._obj->operator [](p_key);
	}
	throw VarError(VarError::NOT_IMPLEMENTED, "operator[] not implemented");
	return var::tmp;
}

var var::__get_mapped(const var& p_key) const {
	switch (type) {
		case STRING: {
			int index = (int)p_key; // TODO: check key is int.
			if (0 <= index && index < (int)_data._string.size())
				return String(_data._string[index]);
			if ((int)_data._string.size() * -1 <= index && index < 0)
				return String(_data._string[_data._string.size() + index]);
			throw VarError(VarError::INVALID_INDEX, ""); // TODO: better error msg
		} break;
		case ARRAY: {
			int index = (int)p_key; // TODO: check key is int.
			if (0 <= index && index < (int)_data._arr.size())
				return _data._arr[index];
			if ((int)_data._arr.size() * -1 <= index && index < 0)
				return _data._arr[_data._arr.size() + index];
			throw VarError(VarError::INVALID_INDEX, ""); // TODO: better error msg
		} break;
		case MAP:
			return _data._map[p_key];
		case OBJECT:
			return _data._obj->__get_mapped(p_key);
	}
	throw VarError(VarError::NOT_IMPLEMENTED, "operator[] not implemented");
	return var();
}
void var::__set_mapped(const var& p_key, const var& p_value) {
	switch (type) {
		case STRING: {
			int index = (int)p_key; // TODO: check key is int.
			// TODO: check value is string, size = 1.
			if (0 <= index && index < (int)_data._string.size())
				_data._string[index] = p_value.operator String()[0];
			if ((int)_data._string.size() * -1 <= index && index < 0)
				_data._string[_data._string.size() + index] = p_value.operator String()[0];
			throw VarError(VarError::INVALID_INDEX, ""); // TODO: better error msg
		} break;
		case ARRAY: {
			int index = (int)p_key; // TODO: check key is int.
			if (0 <= index && index < (int)_data._arr.size())
				_data._arr[index] = p_value;
			if ((int)_data._arr.size() * -1 <= index && index < 0)
				_data._arr[_data._arr.size() + index] = p_value;
			throw VarError(VarError::INVALID_INDEX, ""); // TODO: better error msg
		} break;
		case MAP:
			_data._map[p_key] = p_value;
		case OBJECT:
			_data._obj->__get_mapped(p_key) = p_value; // __set_mapped(p_key, p_value);
	}
	throw VarError(VarError::NOT_IMPLEMENTED, "operator[] not implemented");
}

#define VECT2_GET(m_t)                                            \
	if (p_name == "x" || p_name == "width") {                     \
		return (*DATA_PTR_CONST(STRCAT2(Vect2, m_t))).x;          \
	} else if (p_name == "y" || p_name == "height") {			  \
		return (*DATA_PTR_CONST(STRCAT2(Vect2, m_t))).y;          \
	} else throw VarError(VarError::INVALID_GET_NAME)

#define VECT3_GET(m_t)                                            \
	if (p_name == "x" || p_name == "width") {                     \
		return (*DATA_PTR_CONST(STRCAT2(Vect3, m_t))).x;          \
	} else if (p_name == "y" || p_name == "height") {			  \
		return (*DATA_PTR_CONST(STRCAT2(Vect3, m_t))).y;          \
	} else if (p_name == "z" || p_name == "depth") {			  \
		return (*DATA_PTR_CONST(STRCAT2(Vect3, m_t))).y;          \
	} else throw VarError(VarError::INVALID_GET_NAME)

#define VECT2_SET(m_t, m_cast)                                             \
	if (p_name == "x" || p_name == "width") {                              \
		(*DATA_PTR(STRCAT2(Vect2, m_t))).x = p_value.operator m_cast();    \
	} else if (p_name == "y" || p_name == "height") {			           \
		(*DATA_PTR(STRCAT2(Vect2, m_t))).y = p_value.operator m_cast();    \
	} else throw VarError(VarError::INVALID_GET_NAME)

#define VECT3_SET(m_t, m_cast)                                             \
	if (p_name == "x" || p_name == "width") {                              \
		(*DATA_PTR(STRCAT2(Vect3, m_t))).x = p_value.operator m_cast();    \
	} else if (p_name == "y" || p_name == "height") {			           \
		(*DATA_PTR(STRCAT2(Vect3, m_t))).y = p_value.operator m_cast();    \
	} else if (p_name == "z" || p_name == "depth") {			           \
		(*DATA_PTR(STRCAT2(Vect3, m_t))).y = p_value.operator m_cast();    \
	} else throw VarError(VarError::INVALID_GET_NAME)



var var::__get(const String& p_name) const {
	switch (type) {
		case VECT2F: VECT2_GET(f);
		case VECT2I: VECT2_GET(i);
		case VECT3F: VECT3_GET(f);
		case VECT3I: VECT3_GET(i);
		case OBJECT: return _data._obj->__get(p_name);
	}
	throw VarError(VarError::INVALID_GET_NAME); // TODO: more clear error.
	return var::tmp;
}

void var::__set(const String& p_name, const var& p_value) {
	switch (type) {
		case VECT2F: 
			if (p_value.type != var::FLOAT || p_value.get_type() != var::INT) throw VarError(VarError::INVALID_SET_VALUE);
			VECT2_SET(f, double);
			return;
		case VECT2I:
			if (p_value.type != var::FLOAT || p_value.get_type() != var::INT) throw VarError(VarError::INVALID_SET_VALUE);
			VECT2_SET(i, int64_t);
			return;
		case VECT3F:
			if (p_value.type != var::FLOAT || p_value.get_type() != var::INT) throw VarError(VarError::INVALID_SET_VALUE);
			VECT3_SET(f, double);
			return;
		case VECT3I:
			if (p_value.type != var::FLOAT || p_value.get_type() != var::INT) throw VarError(VarError::INVALID_SET_VALUE);
			VECT3_SET(i, int64_t);
			return;
		case OBJECT: 
			_data._obj->__get(p_name) = p_value;
			return;
	}
	throw VarError(VarError::INVALID_GET_NAME); // TODO: more clear error.
}

#undef VECT2_GET
#undef VECT3_GET

var var::__call_internal(stdvec<var>& p_args) {
	switch (type) {
		case var::_NULL:  throw VarError(VarError::NULL_POINTER, "");
		case var::BOOL:   throw VarError(VarError::OPERATOR_NOT_SUPPORTED, "boolean is not callable");
		case var::INT:    throw VarError(VarError::OPERATOR_NOT_SUPPORTED, "integer is not callable");
		case var::FLOAT:  throw VarError(VarError::OPERATOR_NOT_SUPPORTED, "float is not callable");
		case var::STRING: throw VarError(VarError::OPERATOR_NOT_SUPPORTED, "String is not callable");
		case var::VECT2F: throw VarError(VarError::OPERATOR_NOT_SUPPORTED, "Vectors are not callables");
		case var::VECT2I: throw VarError(VarError::OPERATOR_NOT_SUPPORTED, "Vectors are not callables");
		case var::VECT3F: throw VarError(VarError::OPERATOR_NOT_SUPPORTED, "Vectors are not callables");
		case var::VECT3I: throw VarError(VarError::OPERATOR_NOT_SUPPORTED, "Vectors are not callables");
		case var::ARRAY:  throw VarError(VarError::OPERATOR_NOT_SUPPORTED, "Array is not callable");
		case var::MAP:    throw VarError(VarError::OPERATOR_NOT_SUPPORTED, "Map is not callable");
		case var::OBJECT: return _data._obj->__call(p_args);
	}
	MISSED_ENUM_CHECK(_TYPE_MAX_, 12);
	return var();
}
var var::call_method_internal(const String& p_method, stdvec<var>& p_args) {
	switch (type) {
		case var::_NULL:  throw VarError(VarError::NULL_POINTER, "");
		case var::BOOL:   throw VarError(VarError::INVALID_GET_NAME, String::format("boolean has no attribute \"%s\"", p_method.c_str()));
		case var::INT:    throw VarError(VarError::INVALID_GET_NAME, String::format("int has no attribute \"%s\"", p_method.c_str()));
		case var::FLOAT:  throw VarError(VarError::INVALID_GET_NAME, String::format("float has no attribute \"%s\"", p_method.c_str()));
		case var::STRING: return _data._string.call_method(p_method, p_args);
		case var::VECT2F: throw "TODO"; // TODO:
		case var::VECT2I: throw "TODO"; // TODO:
		case var::VECT3F: throw "TODO"; // TODO:
		case var::VECT3I: throw "TODO"; // TODO:
		case var::ARRAY:  throw "TODO"; // TODO:
		case var::MAP:    throw "TODO"; // TODO:
		case var::OBJECT: return Object::call_method(_data._obj, p_method, p_args);
	}
	MISSED_ENUM_CHECK(_TYPE_MAX_, 12);
	return var();
}

/* casting */
var::operator bool() const {
	switch (type) {
		case _NULL: return false;
		case BOOL: return _data._bool;
		case INT: return _data._int != 0;
		case FLOAT: return _data._float != 0;
		case STRING: return _data._string.size() != 0;

		case VECT2F: return *DATA_PTR_CONST(Vect2f) == Vect2f();
		case VECT2I: return *DATA_PTR_CONST(Vect2i) == Vect2i();
		case VECT3F: return *DATA_PTR_CONST(Vect3f) == Vect3f();
		case VECT3I: return *DATA_PTR_CONST(Vect3f) == Vect3f();
		case ARRAY: return !_data._arr.empty();
		case MAP: return !_data._map.empty();
		case OBJECT: return _data._obj.operator bool();
	}
	MISSED_ENUM_CHECK(_TYPE_MAX_, 12);
	throw VarError(VarError::INVALID_CASTING, String::format("can't cast \"%s\" to \"bool\"", get_type_name().c_str()));
	return false;
}

var::operator int64_t() const {
	switch (type) {
		case BOOL: return _data._bool;
		case INT: return _data._int;
		case FLOAT: return (int)_data._float;
		//case STRING: return  _data._string.to_int(); // throws std::exception
	}
	throw VarError(VarError::INVALID_CASTING, String::format("can't cast \"%s\" to \"int\"", get_type_name().c_str()));
}

var::operator double() const {
	switch (type) {
		case BOOL: return (double)_data._bool;
		case INT: return (double)_data._int;
		case FLOAT: return _data._float;
		//case STRING: return  _data._string.to_float();
	}
	throw VarError(VarError::INVALID_CASTING, String::format("can't cast \"%s\" to \"float\"", get_type_name().c_str()));
}

var::operator String() const {
	if (type != STRING)
		throw VarError(VarError::INVALID_CASTING, String::format("can't cast \"%s\" to \"String\"", get_type_name().c_str()));
	return _data._string;
}

String var::to_string() const {
	switch (type) {
		case _NULL: return "None";
		case BOOL: return (_data._bool) ? "true" : "false";
		case INT: return String(_data._int);
		case FLOAT: return String(_data._float);
		case STRING: return _data._string;
		case VECT2F: return (*DATA_PTR_CONST(Vect2f)).to_string();
		case VECT2I: return (*DATA_PTR_CONST(Vect2i)).to_string();
		case VECT3F: return (*DATA_PTR_CONST(Vect3f)).to_string();
		case VECT3I: return (*DATA_PTR_CONST(Vect3i)).to_string();
		case ARRAY: return _data._arr.to_string();
		case MAP: return _data._map.to_string();
		case OBJECT: return _data._obj->to_string();
	}
	MISSED_ENUM_CHECK(_TYPE_MAX_, 12);
	DEBUG_BREAK(); // can't be reach here.
	return "";
}

#define VAR_VECT_CAST(m_dim, m_t)                                                       \
var::operator D_VEC(Vect, m_dim, m_t)() const {                                         \
	switch (type) {                                                                     \
		case D_VEC(VECT, m_dim, F): return *DATA_PTR_CONST(D_VEC(Vect, m_dim, f));      \
		case D_VEC(VECT, m_dim, I): return *DATA_PTR_CONST(D_VEC(Vect, m_dim, i));      \
		default: throw VarError(VarError::INVALID_CASTING, String::format("can't cast \"%s\" to \"" STRINGIFY(D_VEC(VECT, m_dim, I)) "\"", get_type_name().c_str()));\
	}                                                                                   \
	return D_VEC(Vect, m_dim, m_t)();                                                   \
}
VAR_VECT_CAST(2, f)
VAR_VECT_CAST(2, i)
VAR_VECT_CAST(3, f)
VAR_VECT_CAST(3, i)
#undef VAR_VECT_CAST

var::operator Array() const {
	if (type == ARRAY) {
		return _data._arr;
	}
	throw VarError(VarError::INVALID_CASTING, String::format("can't cast \"%s\" to \"Array\"", get_type_name().c_str()));
}

var::operator Map() const {
	if (type == MAP) {
		return _data._map;
	}
	throw VarError(VarError::INVALID_CASTING, String::format("can't cast \"%s\" to \"Map\"", get_type_name().c_str()));
}

var::operator ptr<Object>() const {
	if (type == OBJECT) {
		return _data._obj;
	}
	throw VarError(VarError::INVALID_CASTING, String::format("can't cast \"%s\" to \"Object\"", get_type_name().c_str()));
}

/* operator overloading */
		/* comparison */
#define VAR_SWITCH_VECT(m_dim, m_t, m_op)                                                                                           \
switch (p_other.type) {                                                                                                             \
	case D_VEC(VECT, m_dim, F): return *DATA_PTR_CONST(D_VEC(Vect, m_dim, m_t)) m_op *DATA_PTR_OTHER_CONST(D_VEC(Vect, m_dim, f));  \
	case D_VEC(VECT, m_dim, I): return *DATA_PTR_CONST(D_VEC(Vect, m_dim, m_t)) m_op *DATA_PTR_OTHER_CONST(D_VEC(Vect, m_dim, i));  \
}                                                                                                                                   \
break;

#define THROW_OPERATOR_NOT_SUPPORTED(m_op)                                                         \
do {                                                                                               \
	throw VarError(VarError::OPERATOR_NOT_SUPPORTED,                                               \
		String::format("operator \"" STR(m_op) "\" not supported on operands \"%s\" and \"%s\"",   \
			get_type_name().c_str(), p_other.get_type_name().c_str())                              \
	);                                                                                             \
} while(false)

bool var::operator==(const var& p_other) const {
	switch (type) {
		case _NULL: return false;
		case BOOL:   {
			switch (p_other.type) {
				case BOOL:  return _data._bool == p_other._data._bool;
				case INT:   return _data._bool == (bool) p_other._data._int;
				case FLOAT: return _data._bool == (bool) p_other._data._float;
			}
		} break;
		case INT:    {
			switch (p_other.type) {
				case BOOL:  return (bool) _data._int == p_other._data._bool;
				case INT:   return _data._int        == p_other._data._int;
				case FLOAT: return _data._int        == p_other._data._float;
			}
		} break;
		case FLOAT:  {
			switch (p_other.type) {
				case BOOL:  return (bool)_data._float == p_other._data._bool;
				case INT:   return _data._float       == p_other._data._int;
				case FLOAT: return _data._float       == p_other._data._float;
			}
		} break;
		case STRING: {
			if (p_other.type == STRING)
				return _data._string == p_other._data._string;
			break;
		}
		case VECT2F: { VAR_SWITCH_VECT(2, f, ==) } break;
		case VECT2I: { VAR_SWITCH_VECT(2, i, ==) } break;
		case VECT3F: { VAR_SWITCH_VECT(3, f, ==) } break;
		case VECT3I: { VAR_SWITCH_VECT(3, i, ==) } break;
		case ARRAY: {
			if (p_other.type == ARRAY) {
				return _data._arr == p_other.operator Array();
			}
			break;
		}
		case MAP: {
			if (p_other.type == MAP) {
				return _data._map == p_other.operator Map();
			}
			break;
		}
		case OBJECT: {
			if (p_other.type == OBJECT)
				return _data._obj == p_other._data._obj;
			break;
		}
	}
	MISSED_ENUM_CHECK(_TYPE_MAX_, 12);
	return false;
}

bool var::operator!=(const var& p_other) const {
	return !operator==(p_other);
}

bool var::operator<(const var& p_other) const {
	switch (type) {
		case _NULL: return false;
		case BOOL:  {
			switch (p_other.type) {
				case BOOL:  return (int)    _data._bool < (int)p_other._data._bool;
				case INT:   return (int)    _data._bool < p_other._data._int;
				case FLOAT: return (double) _data._bool < p_other._data._float;
			}
		} break;
		case INT:   {
			switch (p_other.type) {
				case BOOL:  return _data._int < (int)p_other._data._bool;
				case INT:   return _data._int < p_other._data._int;
				case FLOAT: return _data._int < p_other._data._float;
			}
		} break;
		case FLOAT: {
			switch (p_other.type) {
				case BOOL:  return _data._float < (double) p_other._data._bool;
				case INT:   return _data._float < p_other._data._int;
				case FLOAT: return _data._float < p_other._data._float;
			}
		} break;
		case STRING: {
			if (p_other.type == STRING)
				return _data._string < p_other._data._string;
			break;
		}
		case VECT2F: { VAR_SWITCH_VECT(2, f, < ) }  break;
		case VECT2I: { VAR_SWITCH_VECT(2, i, < ) }  break;
		case VECT3F: { VAR_SWITCH_VECT(3, f, < ) }  break;
		case VECT3I: { VAR_SWITCH_VECT(3, i, < ) }  break;
		case ARRAY: {
			if (p_other.type == ARRAY)
				return *_data._arr.get_data() < *p_other.operator Array().get_data();
			break;
		}
		case MAP: {
			break;
		}
		case OBJECT: {
			if (p_other.type == OBJECT)
				return _data._obj < p_other._data._obj;
			break;
		}
	}
	MISSED_ENUM_CHECK(_TYPE_MAX_, 12);
	// FIXME: a workaround for map keys as vars.
	return this < &p_other;
}

bool var::operator>(const var& p_other) const {
	switch (type) {
		case _NULL: return false;
		case BOOL:  {
			switch (p_other.type) {
				case BOOL:  return (int)    _data._bool > (int) p_other._data._bool;
				case INT:   return (int)    _data._bool > p_other._data._int;
				case FLOAT: return (double) _data._bool > p_other._data._float;
			}
		} break;
		case INT:   {
			switch (p_other.type) {
				case BOOL:  return _data._int > (int) p_other._data._bool;
				case INT:   return _data._int > p_other._data._int;
				case FLOAT: return _data._int > p_other._data._float;
			}
		} break;
		case FLOAT: {
			switch (p_other.type) {
				case BOOL:  return _data._float > (int)p_other._data._bool;
				case INT:   return _data._float > p_other._data._int;
				case FLOAT: return _data._float > p_other._data._float;
			}
		} break;
		case STRING: {
			if (p_other.type == STRING)
				return _data._string < p_other._data._string;
			break;
		}
		case VECT2F: { VAR_SWITCH_VECT(2, f, > ) } break;
		case VECT2I: { VAR_SWITCH_VECT(2, i, > ) } break;
		case VECT3F: { VAR_SWITCH_VECT(3, f, > ) } break;
		case VECT3I: { VAR_SWITCH_VECT(3, i, > ) } break;
		case ARRAY: {
			if (p_other.type == ARRAY)
				return *_data._arr.get_data() > *p_other.operator Array().get_data();
			break;
		}
		case MAP:
			break;
		case OBJECT: {
			if (p_other.type == OBJECT)
				return _data._obj > p_other._data._obj;
			break;
		}
	}
	MISSED_ENUM_CHECK(_TYPE_MAX_, 12);
	return this < &p_other;
}

bool var::operator<=(const var& p_other) const {
	return *this == p_other || *this < p_other;
}
bool var::operator>=(const var& p_other) const {
	return *this == p_other || *this > p_other;
}

var var::operator +(const var& p_other) const {
	switch (type) {
		case _NULL: return false;
		case BOOL:  {
			switch (p_other.type) {
				case BOOL:  return (int64_t) _data._bool + (int64_t) p_other._data._bool;
				case INT:   return (int64_t) _data._bool + p_other._data._int;
				case FLOAT: return (double) _data._bool  + p_other._data._float;
			}
		} break;
		case INT:   {
			switch (p_other.type) {
				case BOOL:  return _data._int          + (int64_t) p_other._data._bool;
				case INT:   return _data._int          + p_other._data._int;
				case FLOAT: return (double) _data._int + p_other._data._float;
			}

		} break;
		case FLOAT: {
			switch (p_other.type) {
				case BOOL:  return _data._float + (double) p_other._data._bool;
				case INT:   return _data._float + (double) p_other._data._int;
				case FLOAT: return _data._float + p_other._data._float;
			}
		} break;
		case STRING: {
			if (p_other.type == STRING)
				return _data._string + p_other._data._string;
			break;
		}
		case VECT2F: { VAR_SWITCH_VECT(2, f, + ) } break;
		case VECT2I: { VAR_SWITCH_VECT(2, i, + ) } break;
		case VECT3F: { VAR_SWITCH_VECT(3, f, + ) } break;
		case VECT3I: { VAR_SWITCH_VECT(3, i, + ) } break;
		case ARRAY: {
			if (p_other.type == ARRAY) {
				return _data._arr + p_other._data._arr;
			}
			break;
		}
		case MAP:
		case OBJECT:
			break;
	}
	MISSED_ENUM_CHECK(_TYPE_MAX_, 12);
	THROW_OPERATOR_NOT_SUPPORTED(+);
}

var var::operator-(const var& p_other) const {
	switch (type) {
		case _NULL: return false;
		case BOOL:  {
			switch (p_other.type) {
				case BOOL:  return (int64_t) _data._bool - (int64_t) p_other._data._bool;
				case INT:   return (int64_t) _data._bool - p_other._data._int;
				case FLOAT: return (double) _data._bool  - p_other._data._float;
			}
		} break;
		case INT:   {
			switch (p_other.type) {
				case BOOL:  return _data._int         - (int64_t)p_other._data._bool;
				case INT:   return _data._int         - p_other._data._int;
				case FLOAT: return (double)_data._int - p_other._data._float;
			}
		} break;
		case FLOAT: {
			switch (p_other.type) {
				case BOOL:  return _data._float - (double)p_other._data._bool;
				case INT:   return _data._float - (double)p_other._data._int;
				case FLOAT: return _data._float - p_other._data._float;
			}
		} break;
		case STRING:
			break;
		case VECT2F: { VAR_SWITCH_VECT(2, f, -) } break;
		case VECT2I: { VAR_SWITCH_VECT(2, i, -) } break;
		case VECT3F: { VAR_SWITCH_VECT(3, f, -) } break;
		case VECT3I: { VAR_SWITCH_VECT(3, i, -) } break;
		case ARRAY:
		case MAP:
		case OBJECT:
			break;
	}
	MISSED_ENUM_CHECK(_TYPE_MAX_, 12);
	THROW_OPERATOR_NOT_SUPPORTED(-);
}

var var::operator *(const var& p_other) const {
	switch (type) {
		case _NULL: return false;
		case BOOL:  {
			switch (p_other.type) {
				case BOOL:  return (int64_t)_data._bool * (int64_t)p_other._data._bool;
				case INT:   return (int64_t)_data._bool * p_other._data._int;
				case FLOAT: return (double)_data._bool  * p_other._data._float;
			}
		} break;
		case INT:   {
			switch (p_other.type) {
				case BOOL:  return _data._int         * (int64_t)p_other._data._bool;
				case INT:   return _data._int         * p_other._data._int;
				case FLOAT: return (double)_data._int * p_other._data._float;
				case STRING: {
					String ret;
					for (int64_t i = 0; i < _data._int; i++) {
						ret += p_other._data._string;
					}
					return ret;
				}
			}
		} break;
		case FLOAT: {
			switch (p_other.type) {
				case BOOL:  return _data._float * (double)p_other._data._bool;
				case INT:   return _data._float * (double)p_other._data._int;
				case FLOAT: return _data._float * p_other._data._float;
			}
		} break;
		case STRING:
			if (p_other.type == INT) {
				String ret;
				for (int64_t i = 0; i < p_other._data._int; i++) {
					ret += _data._string;
				}
				return ret;
			}
			break;
		case VECT2F: { VAR_SWITCH_VECT(2, f, *) } break;
		case VECT2I: { VAR_SWITCH_VECT(2, i, *) } break;
		case VECT3F: { VAR_SWITCH_VECT(3, f, *) } break;
		case VECT3I: { VAR_SWITCH_VECT(3, i, *) } break;
		case ARRAY:
		case MAP:
		case OBJECT:
			break;
	}
	MISSED_ENUM_CHECK(_TYPE_MAX_, 12);
	THROW_OPERATOR_NOT_SUPPORTED(*);
}

var var::operator /(const var& p_other) const {
	switch (type) {
		case _NULL: return false;
		case BOOL:  {
			switch (p_other.type) {
				case BOOL:
					if (p_other._data._bool == false) throw VarError(VarError::ZERO_DIVISION, "");
					return (int64_t)_data._bool / (int64_t)p_other._data._bool;
				case INT:
					if (p_other._data._int == 0) throw VarError(VarError::ZERO_DIVISION, "");
					return (int64_t)_data._bool / p_other._data._int;
				case FLOAT:
					if (p_other._data._float == 0.0) throw VarError(VarError::ZERO_DIVISION, "");
					return (double)_data._bool  / p_other._data._float;
			}
		} break;
		case INT:   {
			switch (p_other.type) {
				case BOOL:
					if (p_other._data._bool == false) throw VarError(VarError::ZERO_DIVISION, "");
					return _data._int         / (int64_t)p_other._data._bool;
				case INT:
					if (p_other._data._int == 0) throw VarError(VarError::ZERO_DIVISION, "");
					return _data._int         / p_other._data._int;
				case FLOAT:
					if (p_other._data._float == 0.0) throw VarError(VarError::ZERO_DIVISION, "");
					return (double)_data._int / p_other._data._float;
			}
		} break;
		case FLOAT: {
			switch (p_other.type) {
				case BOOL:
					if (p_other._data._bool == false) throw VarError(VarError::ZERO_DIVISION, "");
					return _data._float / (double)p_other._data._bool;
				case INT:
					if (p_other._data._int == 0) throw VarError(VarError::ZERO_DIVISION, "");
					return _data._float / (double)p_other._data._int;
				case FLOAT:
					if (p_other._data._float == 0.0) throw VarError(VarError::ZERO_DIVISION, "");
					return _data._float / p_other._data._float;
			}
		} break;
		case STRING:
			break;
		case VECT2F: { VAR_SWITCH_VECT(2, f, /) } break;
		case VECT2I: { VAR_SWITCH_VECT(2, i, /) } break;
		case VECT3F: { VAR_SWITCH_VECT(3, f, /) } break;
		case VECT3I: { VAR_SWITCH_VECT(3, i, /) } break;
		case ARRAY:
		case MAP:
		case OBJECT:
			break;
	}
	MISSED_ENUM_CHECK(_TYPE_MAX_, 12);
	THROW_OPERATOR_NOT_SUPPORTED(/);
}

var var::operator %(const var& p_other) const {
	switch (type) {
		case _NULL: return false;
		case BOOL: {
			switch (p_other.type) {
				case BOOL: return (int)(_data._bool) % (int)(p_other._data._bool);
				case INT: return (int)_data._bool % p_other._data._int;
			}
		}
		case INT: {
			switch (p_other.type) {
				case BOOL: return _data._int % (int)(p_other._data._bool);
				case INT: return _data._int % p_other._data._int;
			}
		}
	}
	MISSED_ENUM_CHECK(_TYPE_MAX_, 12);
	THROW_OPERATOR_NOT_SUPPORTED(%);
}
#undef VAR_RET_EQUAL
#undef VAR_SWITCH_VECT

#define VAR_SWITCH_VECT(m_dim, m_t, m_op)                                                                                               \
switch (p_other.type) {                                                                                                                 \
	case D_VEC(VECT, m_dim, F): *DATA_PTR(D_VEC(Vect, m_dim, m_t)) m_op *DATA_PTR_OTHER_CONST(D_VEC(Vect, m_dim, f)); return *this;     \
	case D_VEC(VECT, m_dim, I): *DATA_PTR(D_VEC(Vect, m_dim, m_t)) m_op *DATA_PTR_OTHER_CONST(D_VEC(Vect, m_dim, i)); return *this;     \
}                                                                                                                                       \
break;

var& var::operator+=(const var& p_other) {
	switch (type) {
		case _NULL:
		case BOOL: {
			switch (p_other.type) {
				case BOOL:  type = INT;   _data._int   = (int)_data._bool    + (int)p_other._data._bool; return *this;
				case INT:   type = INT;   _data._int   = (int)_data._bool    + p_other._data._int;       return *this;
				case FLOAT: type = FLOAT; _data._float = (double)_data._bool + p_other._data._float;     return *this;
			}
		} break;
		case INT:   {
			switch (p_other.type) {
				case BOOL:                _data._int   =         _data._int + (int)p_other._data._bool;  return *this;
				case INT:                 _data._int   =         _data._int + p_other._data._int;        return *this;
				case FLOAT: type = FLOAT; _data._float = (double)_data._int + p_other._data._float;      return *this;
			}
		} break;
		case FLOAT: {
			switch (p_other.type) {
				case BOOL:  _data._float = _data._float + (double)p_other._data._bool;  return *this;
				case INT:   _data._float = _data._float + (double)p_other._data._int;   return *this;
				case FLOAT: _data._float = _data._float + p_other._data._float;         return *this;
			}
		} break;
		case STRING: {
			if (p_other.type == STRING) {
				_data._string += p_other._data._string;
				return *this;
			}
			break;
		}
		case VECT2F: { VAR_SWITCH_VECT(2, f, +=) } break;
		case VECT2I: { VAR_SWITCH_VECT(2, i, +=) } break;
		case VECT3F: { VAR_SWITCH_VECT(3, f, +=) } break;
		case VECT3I: { VAR_SWITCH_VECT(3, i, +=) } break;
		case ARRAY: {
			if (p_other.type == ARRAY) {
				_data._arr += p_other._data._arr;
				return *this;
			}
			break;
		}
		case MAP:
		case OBJECT:
			break;
	}
	MISSED_ENUM_CHECK(_TYPE_MAX_, 12);
	THROW_OPERATOR_NOT_SUPPORTED(+=);
}

var& var::operator-=(const var& p_other) {
	switch (type) {
		case _NULL:
		case BOOL: {
			switch (p_other.type) {
				case BOOL:  type = INT;   _data._int   = (int)_data._bool    - (int)p_other._data._bool; return *this;
				case INT:   type = INT;   _data._int   = (int)_data._bool    - p_other._data._int;       return *this;
				case FLOAT: type = FLOAT; _data._float = (double)_data._bool - p_other._data._float;     return *this;
			}
		} break;
		case INT:   {
			switch (p_other.type) {
				case BOOL:                _data._int   = (int)   _data._int - (int)p_other._data._bool;  return *this;
				case INT:                 _data._int   =         _data._int - p_other._data._int;        return *this;
				case FLOAT: type = FLOAT; _data._float = (double)_data._int - p_other._data._float;      return *this;
			}
		} break;
		case FLOAT: {
			switch (p_other.type) {
				case BOOL:  _data._float = _data._float - (double)p_other._data._bool;  return *this;
				case INT:   _data._float = _data._float - (double)p_other._data._int;   return *this;
				case FLOAT: _data._float = _data._float - p_other._data._float;         return *this;
			}
		} break;
		case STRING:
			break;
		case VECT2F: { VAR_SWITCH_VECT(2, f, -=) } break;
		case VECT2I: { VAR_SWITCH_VECT(2, i, -=) } break;
		case VECT3F: { VAR_SWITCH_VECT(3, f, -=) } break;
		case VECT3I: { VAR_SWITCH_VECT(3, i, -=) } break;
		case ARRAY:
		case MAP:
		case OBJECT:
			break;
	}
	MISSED_ENUM_CHECK(_TYPE_MAX_, 12);
	THROW_OPERATOR_NOT_SUPPORTED(-=);
}


var& var::operator*=(const var& p_other) {
	switch (type) {
		case _NULL:
		case BOOL: {
			switch (p_other.type) {
				case BOOL:  type = INT;   _data._int   = (int)_data._bool    * (int)p_other._data._bool; return *this;
				case INT:   type = INT;   _data._int   = (int)_data._bool    * p_other._data._int;       return *this;
				case FLOAT: type = FLOAT; _data._float = (double)_data._bool * p_other._data._float;     return *this;
			}
		} break;
		case INT:   {
			switch (p_other.type) {
				case BOOL:                _data._int   = (int)   _data._int * (int)p_other._data._bool;  return *this;
				case INT:                 _data._int   =         _data._int * p_other._data._int;        return *this;
				case FLOAT: type = FLOAT; _data._float = (double)_data._int * p_other._data._float;      return *this;
				case STRING: {
					String self;
					for (int64_t i = 0; i < _data._int; i++) {
						self += p_other._data._string;
					}
					type = STRING; new(&_data._string) String(self); return *this;
				}
			}
		} break;
		case FLOAT: {
			switch (p_other.type) {
				case BOOL:  _data._float = _data._float * (double)p_other._data._bool;  return *this;
				case INT:   _data._float = _data._float * (double)p_other._data._int;   return *this;
				case FLOAT: _data._float = _data._float * p_other._data._float;         return *this;
			}
		} break;
		case STRING:
			if (p_other.type == INT) {
				String self;
				for (int64_t i = 0; i < p_other._data._int; i++) {
					self += _data._string;
				}
				_data._string = self; return *this;
			}
			break;
		case VECT2F: { VAR_SWITCH_VECT(2, f, *=) } break;
		case VECT2I: { VAR_SWITCH_VECT(2, i, *=) } break;
		case VECT3F: { VAR_SWITCH_VECT(3, f, *=) } break;
		case VECT3I: { VAR_SWITCH_VECT(3, i, *=) } break;
		case ARRAY:
		case MAP:
		case OBJECT:
			break;
	}
	MISSED_ENUM_CHECK(_TYPE_MAX_, 12);
	THROW_OPERATOR_NOT_SUPPORTED(*=);
}

var& var::operator/=(const var& p_other) {
	switch (type) {
		case _NULL:
		case BOOL: {
			switch (p_other.type) {
				case BOOL:
					if (p_other._data._bool == false) throw VarError(VarError::ZERO_DIVISION, "");
					type = INT;   _data._int   = (int)_data._bool    / (int)p_other._data._bool; return *this;
				case INT:
					if (p_other._data._int == 0) throw VarError(VarError::ZERO_DIVISION, "");
					type = INT;   _data._int   = (int)_data._bool    / p_other._data._int;       return *this;
				case FLOAT:
					if (p_other._data._float == 0.0) throw VarError(VarError::ZERO_DIVISION, "");
					type = FLOAT; _data._float = (double)_data._bool / p_other._data._float;     return *this;
			}
		} break;
		case INT:   {
			switch (p_other.type) {
				case BOOL:
					if (p_other._data._bool == false) throw VarError(VarError::ZERO_DIVISION, "");
					_data._int   = (int)   _data._int / (int)p_other._data._bool;  return *this;
				case INT:
					if (p_other._data._int == 0) throw VarError(VarError::ZERO_DIVISION, "");
					_data._int   =         _data._int / p_other._data._int;        return *this;
				case FLOAT:
					if (p_other._data._float == 0.0) throw VarError(VarError::ZERO_DIVISION, "");
					type = FLOAT; _data._float = (double)_data._int / p_other._data._float;      return *this;
			}
		} break;
		case FLOAT: {
			switch (p_other.type) {
				case BOOL:
					if (p_other._data._bool == false) throw VarError(VarError::ZERO_DIVISION, "");
					_data._float = _data._float / (double)p_other._data._bool;  return *this;
				case INT:
					if (p_other._data._int == 0) throw VarError(VarError::ZERO_DIVISION, "");
					_data._float = _data._float / (double)p_other._data._int;   return *this;
				case FLOAT:
					if (p_other._data._float == 0.0) throw VarError(VarError::ZERO_DIVISION, "");
					_data._float = _data._float / p_other._data._float;         return *this;
			}
		} break;
		case STRING:
			break;
		case VECT2F: { VAR_SWITCH_VECT(2, f, /=) } break;
		case VECT2I: { VAR_SWITCH_VECT(2, i, /=) } break;
		case VECT3F: { VAR_SWITCH_VECT(3, f, /=) } break;
		case VECT3I: { VAR_SWITCH_VECT(3, i, /=) } break;
		case ARRAY:
		case MAP:
		case OBJECT:
			break;
	}
	MISSED_ENUM_CHECK(_TYPE_MAX_, 12);
	THROW_OPERATOR_NOT_SUPPORTED(/=);
}

var& var::operator %=(const var& p_other) {
	switch (type) {
		case _NULL: break;
		//case BOOL: break;
		case INT: {
			switch (p_other.type) {
				//case BOOL: _data._int %= (int)(p_other._data._bool); return *this;
				case INT: _data._int %= p_other._data._int; return *this;
			}
		}
	}
	THROW_OPERATOR_NOT_SUPPORTED(%=);
}

void var::copy_data(const var& p_other) {
	clear_data();
	type = p_other.type;
	switch (p_other.type) {
		case var::_NULL: break;
		case var::BOOL:
			_data._bool = p_other._data._bool;
			break;
		case var::INT:
			_data._int = p_other._data._int;
			break;
		case var::FLOAT:
			_data._float = p_other._data._float;
			break;
		case var::STRING:
			new(&_data._string) String(p_other._data._string);
			break;
		case var::VECT2F:
		case var::VECT2I:
		case var::VECT3F:
		case var::VECT3I:
			for (int i = 0; i < DATA_MEM_SIZE; i++) {
				_data._mem[i] = p_other._data._mem[i];
			}
			break;
		case var::ARRAY:
			_data._arr = p_other._data._arr;
			break;
		case var::MAP:
			_data._map = p_other._data._map;
			break;
		case var::OBJECT:
			_data._obj = p_other._data._obj;
			return;
	}
	MISSED_ENUM_CHECK(_TYPE_MAX_, 12);
}

void var::clear_data() {
	switch (type) {
		case var::_NULL:
		case var::BOOL:
		case var::INT:
		case var::FLOAT:
		case var::VECT2F:
		case var::VECT2I:
		case var::VECT3F:
		case var::VECT3I:
			return;
		case var::STRING:
			_data._string.~String();
			return;
		case var::ARRAY:
			_data._arr._data = nullptr;
			break;
		case var::MAP:
			_data._map._data = nullptr;
			break;
		case var::OBJECT:
			_data._obj = nullptr;
			break;
	}
	MISSED_ENUM_CHECK(_TYPE_MAX_, 12);
}


String var::get_type_name_s(var::Type p_type) {
	static const char* _type_names[_TYPE_MAX_] = {
		"null",
		"bool",
		"int",
		"float",
		"String",
		"Vect2f",
		"Vect2i",
		"Vect3f",
		"Vect3i",
		"Array",
		"Map",
		"Object",
		//_TYPE_MAX_
	};
	MISSED_ENUM_CHECK(_TYPE_MAX_, 12);
	return _type_names[p_type];
}

String var::get_type_name() const {
	if (type == OBJECT) {
		return _data._obj->get_class_name();
	} else {
		return get_type_name_s(type);
	}
}

}

#undef VAR_CASE_OP
#undef VAR_SWITCH_VECT

#if defined(UNDEF_VAR_DEFINES)
#undef func
#undef STRCAT2
#undef STRCAT3
#undef STRCAT4
#undef STR
#undef STRINGIFY
#undef PLACE_HOLDER
#undef VSNPRINTF_BUFF_SIZE
#undef DEBUG_BREAK
#undef DEBUG_PRINT
#undef VAR_ASSERT
#undef MISSED_ENUM_CHECK
#undef UNDEF_VAR_DEFINES
#endif


#endif // VAR_IMPLEMENTATION