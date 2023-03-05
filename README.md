本项目发布在 NuGet MATLAB.MexTools，GitHub上只有标签

使用本工具快速生成 MATLAB C++ MEX 数据API文件函数。

MATLAB C++ MEX 数据API文件函数本质上就是个实现了一套MEX标准接口的动态链接库，扩展名mexw64。你只需要在 Visual Studio 中新建一个C++动态链接库项目，安装此NuGet包，将输出扩展名改为mexw64，包含【Mex实现.h】，然后实现其中声明的MexFunction::operator()，就可以生成出合法的MEX文件函数。此外，操作MATLAB数组通常还需要包含【Mex类型.h】。

[埃博拉酱的MATLAB扩展](https://github.com/Silver-Fang/MATLAB-Extension)项目就使用了本库参与的MEX文件函数，你可以查看此项目的C++代码，进一步了解如何使用本库编写MEX文件函数。

为了进一步简化MEX编程工作流，【Mex工具.h】还提供了一系列MEX编程常用的工具函数。【Mex工具.h】公开接口：
```C++
#include<Mex类型.h>
#include<array>
using namespace matlab::data;
using namespace matlab::mex;
namespace Mex工具
{
	enum Mex异常;
	//此using在将动态类型枚举ArrayType转为静态类型
	template<ArrayType T>
	using 动态类型转静态;
	constexpr uint8_t 类型总数;
	//此数组存储了每种动态类型枚举对应的静态类型的尺寸。使用方法示例：类型尺寸[(int)inputs[1].getType()]
	constexpr std::array<uint8_t, 类型总数>类型尺寸;

	/*
	* 根据动态类型选择类模板并返回成员
	* MATLAB数组使用一个枚举值指示数组的动态类型。当你试图编写一个泛型函数时，一般来说需要用一个冗长的switch语句将其正确转换为C++静态类型，然后调用对应的函数模板。本函数封装此过程，根据MATLAB动态类型，实例化对应的模板类，取其返回成员。调用方可按需求设计一个模板类来包装类型特定的方法。本文件中的万能转码函数就用此方法实现标量转换，实现方法亦可作为参考示例。
	* @param 模板，必须至少接受一个类型参数作为第一个模板参数，至少包含一个名为“返回”的静态成员，此成员的类型必须与运行时输入的MATLAB类型（即第一个模板参数）无关；如果是静态函数，则函数的所有输入参数和返回值类型必须与运行时输入的MATLAB类型无关。模板的第一个参数应当为所有MATLAB对应类型提供“返回”静态成员，还应当为nullptr_t类型也提供“返回”静态成员——这用于对不支持的MATLAB类型进行异常处理。例如，你可以为nullptr_t类型特化的返回成员提供一个异常输出。
	* @param Ts，将要传递给模板的后续模板参数。如果你提供的模板不具有或具有可选的后续模板参数，则此处的后续模板参数也是可选的。
	* @param 类型，运行时得到的MATLAB动态类型枚举
	* @return 返回模板类名为“返回”的成员
	*/
	template<template <typename T, typename...Ts>typename 模板, typename...Ts>
	constexpr auto 动态类型选择模板(ArrayType 类型);

	/*
	MATLAB转C++
	这些函数输入均为Array&&，表示函数返回后输入值将不可用，请使用std::move将Array转换为Array&&
	*/

	/*
	* 将输入MATLAB标量转换为对应C++类型
	* @param Array&& 输入MATLAB数组不能为空，且数据类型必须与C++类型一致
	*/
	template<typename T>
	inline T 万能转码(Array&& 输入);
	/*
	* 将输入MATLAB标量uint64转换为C++指针
	* @param Array&& 输入MATLAB数组不能为空，且数据类型必须是uint64
	*/
	template <typename T>
		requires std::is_pointer_v<T>
	inline T 万能转码(Array&& 输入);
	/*
	* 将输入的非指针类POD类型转换为C++类型标量，支持数据类型自动转换。此函数模板使用了【动态类型选择模板】，传入的第一个模板参数是【内部::转换结构】。你可以查看此转换结构的定义方式，了解动态类型选择模板的使用方法。由于转换结构的返回成员是一个函数，动态类型选择模板会自动返回一个类型转换函数指针，然后将输入数组传递给此函数指针实现类型转换。
	* @param Array&& 输入MATLAB数组不能为空。如果MATLAB类型与C++类型不一致会自动转换。
	* @throw Mex异常：类型转换失败
	*/
	template<typename T>
		requires (std::is_pod_v<T> && !std::is_pointer_v<T>)
	inline T 万能转码(Array&& 输入);
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
	/*
	使用动态类型创建一个无类型缓冲，执行内存拷贝然后打包为 MATLAB Array
	泛型编程的一个常见情境，就是我们不关心用户提供的运行时数据类型，只是单纯进行内存拷贝。但是MATLAB官方提供的 MEX API 不允许无类型的内存拷贝，因此这里提供一个无类型缓冲区，可以接受一个运行时类型枚举，创建一个无类型缓冲，完成数据拷贝后再打包成无类型的 MATLAB Array。
	*/
	struct UntypedBuffer
	{
		void* const 指针;
		const uint64_t 字节数;
		//创建无类型缓冲需要提供一个运行时类型枚举和元素个数（不是字节数）。字节数等于元素数*类型尺寸[(int)类型]。
		static std::unique_ptr<UntypedBuffer>创建(ArrayType 类型, uint64_t 元素数);
		virtual ~UntypedBuffer() {}
		//打包后本对象变为不可用，所有数据封装在 MATLAB Array 中
		virtual Array 打包()noexcept = 0;
	protected:
		UntypedBuffer(void* 指针, uint64_t 字节数) :指针(指针), 字节数(字节数) {}
	};
	/*
	从无类型的 MATLAB Array 拷贝数据
	泛型编程的一个常见情境，就是我们不关心用户提供的运行时数据类型，只是单纯进行内存拷贝。但是MATLAB官方提供的 MEX API 不允许无类型的内存拷贝。使用UntypedArray可以将 MATLAB Array 包装起来，提供无类型内存拷贝的接口，你可以无视数据类型将任意MATLAB数组拷贝到指定内存。
	不同于UntypedBuffer，这里不提供直接指向数组开头的内存指针。这是因为MATLAB数组的实现并不是简单的将数据连续排列在内存中，而有可能是按需动态生成的。
	*/
	struct UntypedArray
	{
		const uint64_t 字节数;
		virtual ~UntypedArray() {}
		//从MATLAB数组取得无类型的数组
		static std::unique_ptr<UntypedArray> 创建(Array&& 数组);
		//将数组中的内容无视数据类型直接拷贝到指针
		virtual void 拷贝(void* 输出)const = 0;

	protected:
		UntypedArray(uint64_t 字节数) :字节数(字节数) {}
	};

	//C++转MATLAB
	
	//这里使用static而不是extern，因为从其它编译单元链接的变量不一定能在DllMain阶段完成初始化，会造成意外错误。
	static ArrayFactory 数组工厂;

	/*
	* 将输入C++指针数组转换为MATLAB缓冲指针，用于创建MATLAB数组
	* @param const T* 指针，T必须有直接对应的MATLAB数据类型
	* @param UINT64 尺寸，指针数组中的元素数目
	* @return buffer_ptr_t<T>，可以用【数组工厂.createArrayFromBuffer】将此缓冲指针转换为MATLAB数组
	*/
	template <typename T>
	inline buffer_ptr_t<T> 万能转码(const T* 指针, size_t 尺寸);
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
	template <typename T>
		requires (std::is_pod_v<T> && !std::is_pointer_v<T>)
	inline TypedArray<T> 万能转码(T 输入);
	//字符串转换
	template<class T>
		requires std::_Is_any_of_v<T, String, MATLABString, CharArray>
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
#define API调用 API[万能转码<uint8_t>(std::move(inputs[0]))](outputs, inputs);
//增强功能，可以返回一个异常值并补齐额外的输出值
	void 异常输出补全(ArgumentList& outputs);
}
```
