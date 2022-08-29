#pragma once
//常用的MEX工具函数
#include <algorithm>
#include <Mex类型.h>
using namespace matlab::data;
using namespace matlab::mex;
static ArrayFactory 数组工厂;
template <typename T>
concept 数值 = std::is_arithmetic<T>::value;

/*
MATLAB转C++
这些函数输入均为Array&&，表示函数返回后输入值将不可用，请使用std::move将Array转换为Array&&
*/

/*
* 将输入MATLAB标量转换为对应C++类型
* @param Array&& 输入MATLAB数组不能为空，且数据类型必须与C++类型一致
*/
template<typename T>
inline T 万能转码(Array&& 输入)
{
	return TypedArray<T>(std::move(输入))[0];
}
/*
* 将输入MATLAB标量uint64转换为C++指针
* @param Array&& 输入MATLAB数组不能为空，且数据类型必须是uint64
*/
template <typename T>
	requires std::is_pointer<T>::value
inline T 万能转码(Array&& 输入)
{
	return (T)万能转码<size_t>(std::move(输入));
}
/*
* 将输入MATLAB标量字符串转换为std::u16string
* @param Array&& 输入MATLAB数组可以是字符串标量或字符向量
* @return String 本质上就是std::u16string
* @throw InvalidArrayTypeException 输入不是字符串或字符向量
*/
template<>
String 万能转码<String>(Array&& 输入);
/*
* 将输入MATLAB标量字符串转换为CharArray
* @param Array&& 输入MATLAB数组可以是字符串标量或字符向量
* @return CharArray
* @throw InvalidArrayTypeException 输入不是字符串或字符向量
*/
template<>
CharArray 万能转码<CharArray>(Array&& 输入);
/*
* 将输入MATLAB标量字符串转换为std::string
* @param Array&& 输入MATLAB数组可以是字符串标量或字符向量
* @return std::string，MATLAB字符是UTF16编码，将自动转换为UTF8返回
* @throw InvalidArrayTypeException 输入不是字符串或字符向量
*/
template<>
std::string 万能转码<std::string>(Array&& 输入);
/*
* 将输入MATLAB字符串数组、字符向量或字符向量元胞数组转换为std::unique_ptr<std::string[]>
* @param Array&& 输入MATLAB数组可以是字符串数组、字符向量或字符向量元胞数组
* @return std::unique_ptr<std::string[]>，MATLAB字符是UTF16编码，将自动转换为UTF8返回
* @throw InvalidArrayTypeException 输入不是字符串或字符向量
*/
template<>
std::unique_ptr<std::string[]> 万能转码<std::unique_ptr<std::string[]>>(Array&& 输入);
/*
* 将输入MATLAB字符串数组、字符向量或字符向量元胞数组转换为std::unique_ptr<std::string[]>
* @param Array&& 输入MATLAB数组可以是字符串数组、字符向量或字符向量元胞数组
* @return std::unique_ptr<std::string[]>，MATLAB字符是UTF16编码，将自动转换为UTF8返回
* @throw InvalidArrayTypeException 输入不是字符串或字符向量
*/
template<>
std::unique_ptr<std::string[]> 万能转码<std::unique_ptr<std::string[]>>(Array&& 输入);

//C++转MATLAB

/*
* 将输入C++指针数组转换为MATLAB缓冲指针，用于创建MATLAB数组
* @param const T* 指针，T必须有直接对应的MATLAB数据类型
* @param UINT64 尺寸，指针数组中的元素数目
* @return buffer_ptr_t<T>，可以用【数组工厂.createArrayFromBuffer】将此缓冲指针转换为MATLAB数组
*/
template <typename T>
inline buffer_ptr_t<T> 万能转码(const T* 指针, size_t 尺寸)
{
	buffer_ptr_t<T>缓冲 = 数组工厂.createBuffer<T>(尺寸);
	std::copy_n(指针, 尺寸, 缓冲.get());
	return 缓冲;
}
/*
* 将输入C++指针转换为MATLAB标量uint64
* @param const void* 指针
* @return TypedArray<uint64_t>，MATLAB uint64 标量
*/
inline TypedArray<uint64_t>万能转码(const void* 指针)
{
	return 数组工厂.createScalar((uint64_t)指针);
}
/*
* 将输入C++数值类型转换为MATLAB数值标量
* @param T 输入，必须具有对应的MATLAB数值类型
* @return TypedArray<T>，MATLAB数值标量
*/
template <数值 T>
inline TypedArray<T> 万能转码(T 输入)
{
	return 数组工厂.createScalar(输入);
}
//字符串转换
template<class T>
requires std::_Is_any_of_v<T,String,MATLABString,CharArray>
T 万能转码(const char*);
/*
* 将输入C样式UTF8字符串转换为String，自动转换为UTF16编码
* @param const char* C样式UTF8字符串，以0结尾
* @return String，本质上是std::u16string，可转换为MATLABString
*/
template<>
String 万能转码<String>(const char*);
/*
* 将输入C样式UTF8字符串转换为MATLABString，自动转换为UTF16编码
* @param const char* C样式UTF8字符串，以0结尾
* @return MATLABString，可用于组成StringArray
*/
template<>
inline MATLABString 万能转码<MATLABString>(const char* UTF8)
{
	return 万能转码<String>(UTF8);
}
/*
* 将输入C样式UTF8字符串转换为CharArray，自动转换为UTF16编码
* @param const char* C样式UTF8字符串，以0结尾
* @return CharArray，MATLAB字符向量
*/
template<>
inline CharArray 万能转码<CharArray>(const char* UTF8)
{
	return 数组工厂.createCharArray(万能转码<String>(UTF8));
}
/*
* 将输入多个std::string转换为StringArray，自动转换为UTF16编码
* @param const std::string* 多个std::string指针
* @param size_t 指针中有多少个字符串
* @return StringArray，MATLAB字符串数组
*/
StringArray 万能转码(const std::string* UTF8, size_t 个数);
/*
* 将输入多个C样式字符串const char*转换为StringArray，自动转换为UTF16编码
* @param const char* const* 多个C样式字符串指针
* @param size_t 指针中有多少个字符串
* @return StringArray，MATLAB字符串数组
*/
StringArray 万能转码(const char* const* UTF8, size_t 个数);
//增强功能，可以使用如下三个宏定义在一个MEX文件函数中定义多个API

#define API声明(函数名) void 函数名(ArgumentList& outputs,ArgumentList& inputs)
#define API索引 constexpr void (*(API[]))(ArgumentList&, ArgumentList&) =
#define API调用 API[万能转码<UINT8>(std::move(inputs[0]))](outputs, inputs);
//增强功能，可以返回一个异常值并补齐额外的输出值
void 异常输出补全(ArgumentList& outputs);