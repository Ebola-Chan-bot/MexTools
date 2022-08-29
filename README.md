使用本工具快速生成 MATLAB C++ MEX 数据API文件函数。

MATLAB C++ MEX 数据API文件函数本质上就是个实现了一套MEX标准接口的动态链接库，扩展名mexw64。你只需要在 Visual Studio 中新建一个C++动态链接库项目，将输出扩展名改为mexw64，包含【Mex实现.h】，然后实现其中声明的MexFunction::operator()，就可以生成出合法的MEX文件函数。

此外，操作MATLAB数组通常还需要包含【Mex类型.h】；【Mex工具.h】则提供了一系列工具函数：
Mex工具.h公开接口
```C++
#pragma once
#include <algorithm>
#include <Mex类型.h>
using namespace matlab::data;
using namespace matlab::mex;
//创建MATLAB数组所必需的工厂对象
static ArrayFactory 数组工厂;
template <typename T>
concept 数值 = std::is_arithmetic<T>::value;

//MATLAB转C++

/*
* 将输入MATLAB标量转换为对应C++类型
* @param Array& 输入MATLAB数组不能为空，且数据类型必须与C++类型一致
*/
template<typename T>
inline T 万能转码(Array& 输入);
/*
* 将输入MATLAB标量uint64转换为C++指针
* @param Array& 输入MATLAB数组不能为空，且数据类型必须是uint64
*/
template <typename T>
	requires std::is_pointer<T>::value
inline T 万能转码(Array& 输入);
/*
* 将输入MATLAB标量字符串转换为std::u16string
* @param Array& 输入MATLAB数组可以是字符串标量或字符向量
* @return String 本质上就是std::u16string
* @throw InvalidArrayTypeException 输入不是字符串或字符向量
*/
template<>
String 万能转码<String>(Array& 输入);
/*
* 将输入MATLAB标量字符串转换为CharArray
* @param Array& 输入MATLAB数组可以是字符串标量或字符向量
* @return CharArray
* @throw InvalidArrayTypeException 输入不是字符串或字符向量
*/
template<>
CharArray 万能转码<CharArray>(Array& 输入);
/*
* 将输入MATLAB标量字符串转换为std::string
* @param Array& 输入MATLAB数组可以是字符串标量或字符向量
* @return std::string，MATLAB字符是UTF16编码，将自动转换为UTF8返回
* @throw InvalidArrayTypeException 输入不是字符串或字符向量
*/
template<>
std::string 万能转码<std::string>(Array& 输入);
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
inline buffer_ptr_t<T> 万能转码(const T* 指针, UINT64 尺寸);
/*
* 将输入C++指针转换为MATLAB标量uint64
* @param const void* 指针
* @return TypedArray<uint64_t>，MATLAB uint64 标量
*/
inline TypedArray<uint64_t>万能转码(const void* 指针);
/*
* 将输入C++数值类型转换为MATLAB数值标量
* @param T 输入，必须具有对应的MATLAB数值类型
* @return TypedArray<T>，MATLAB数值标量
*/
template <数值 T>
inline TypedArray<T> 万能转码(T 输入);
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
inline MATLABString 万能转码<MATLABString>(const char* UTF8);
/*
* 将输入C样式UTF8字符串转换为CharArray，自动转换为UTF16编码
* @param const char* C样式UTF8字符串，以0结尾
* @return CharArray，MATLAB字符向量
*/
template<>
inline CharArray 万能转码<CharArray>(const char* UTF8);
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
#define API调用 API[万能转码<UINT8>(inputs[0])](outputs, inputs);
//增强功能，可以返回一个异常值并补齐额外的输出值
void 异常输出补全(ArgumentList& outputs);
```
一个示例实现，用单个MEX文件实现一个OirReader类，通过CreateOirReader创建对象，DestroyOirReader销毁对象。该示例的完整代码[参见此处](https://github.com/Silver-Fang/Image5D/blob/master/OirReaderMex/MexFunction.cpp)。
```C++
//声明API的语法
API声明(CreateOirReader)
{
	const String 文件路径 = 万能转码<String>(inputs[1]);
	//Oir读入器是另外实现的C++类构造
	outputs[0] = 万能转码(new Oir读入器((wchar_t*)文件路径.c_str()));
}
API声明(DestroyOirReader)
{
	delete 万能转码<Oir读入器*>(inputs[1]);
	//成功结构是返回一个表示操作成功的MATLAB类型
	outputs[0] = 成功结构;
}
void MexFunction::operator()(ArgumentList& outputs, ArgumentList& inputs)
{
	//API索引列出此Mex文件函数支持的所有API。建议在MATLAB端定义一个uint8枚举类，依次与API索引一一对应
	API索引
	{
		CreateOirReader,
		DestroyOirReader,
	};
	try
	{
		//实际调用API在此处，并捕获异常。这里使用了inputs[0]作为索引，调用API索引指向的函数指针，实现API分流。
		API调用;
		//第一个返回值是成功或异常的状态值，因此需要在MATLAB端根据第1个返回值判断是否出现异常
		outputs[0] = 成功结构;
	}
	//Image5D异常是该项目自定义的异常类型，可转换为StructArray
	catch (Image5D异常& 异常)
	{
		outputs[0] = 异常;
		//异常条件下不会产生正常的返回值，因此需要补满所需数目的返回值以免MATLAB端出错
		异常输出补全(outputs);
	}
}
```