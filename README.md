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
	//这里使用static而不是extern，因为从其它编译单元链接的变量不一定能在DllMain阶段完成初始化，会造成意外错误。
	static ArrayFactory 数组工厂;
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
	//此using在将动态类型枚举ArrayType转为静态类型
	template<ArrayType T>
	using 动态类型转静态 = 内部::动态类型转静态<T>::类型;
	constexpr uint8_t 类型总数 = 32;
	//此数组存储了每种动态类型枚举对应的静态类型的尺寸。使用方法示例：类型尺寸[(int)inputs[1].getType()]
	constexpr std::array<uint8_t, 类型总数>类型尺寸 = 内部::类型尺寸<std::make_integer_sequence<int, 类型总数>>;

	//增强功能，可以使用如下三个宏定义在一个MEX文件函数中定义多个API

#define API声明(函数名) void 函数名(ArgumentList& outputs,ArgumentList& inputs)
#define API索引 constexpr void (*(API[]))(ArgumentList&, ArgumentList&) =
#define API调用 const uint8_t 选项=万能转码<uint8_t>(std::move(inputs[0]));if(选项<std::extent_v<decltype(API)>)API[选项](outputs, inputs);else throw 不支持的API;
	/*
	出错时，将后续返回值设为空数组
	为了将C++异常传递给MATLAB，我们通常需要将MEX文件函数的第一个返回值保留作为错误代码
	*/
	void 异常输出补全(ArgumentList& outputs);

	/*
	将MATLAB简单类型标量转换为C++类型，自动执行必要的显式转换，还支持MATLAB稀疏矩阵。
	语法：Mex工具::万能转码<T>(std::move(输入))
	此函数支持以下转换：
	- 整数和浮点类型互转，遵循C++转换规则
	- MATLAB数值类型转复数（std::complex）
	- 整数转任意类型指针
	# 参数
	typename T，转换目标类型，支持所有可从MATLAB简单元素类型显式转换而来的类型
	Array&& 输入，MATLAB标量右值引用。函数返回后，输入数组将变成不可用。
	*/
	template<typename T>
	inline T 万能转码(Array&& 输入);
	/*
	将MATLAB字符行向量、字符串或字符行向量元胞标量转换为 UTF16 String（std::u16string）
	语法：Mex工具::万能转码<String>(std::move(输入))
	# 参数
	Array&& 输入，MATLAB标量右值引用。函数返回后，输入数组将变成不可用。
	*/
	template<>
	extern String 万能转码<String>(Array&& 输入);
	/*
	将MATLAB字符行向量、字符串或字符行向量元胞标量转换为 UTF16 MATLABString
	语法：Mex工具::万能转码<MATLABString>(std::move(输入))
	参数：Array&& 输入，MATLAB标量右值引用。函数返回后，输入数组将变成不可用。
	*/
	template<>
	extern MATLABString 万能转码<MATLABString>(Array&& 输入);
	/*
	将MATLAB字符行向量、字符串或字符行向量元胞标量转换为 UTF16 CharArray
	语法：Mex工具::万能转码<CharArray>(std::move(输入))
	参数：Array&& 输入，MATLAB标量右值引用。函数返回后，输入数组将变成不可用。
	*/
	template<>
	extern CharArray 万能转码<CharArray>(Array&& 输入);
	/*
	将MATLAB字符行向量、字符串或字符行向量元胞标量转换为 UTF8 std::string，使用 Win32 WideCharToMultiByte 执行UTF16到UTF8的转码
	语法：Mex工具::万能转码<std::string>(std::move(输入))
	参数：Array&& 输入，MATLAB标量右值引用。函数返回后，输入数组将变成不可用。
	*/
	template<>
	extern std::string 万能转码<std::string>(Array&& 输入);
	/*
	将MATLAB字符行向量、字符串数组或字符行向量元胞数组转换为 UTF16 StringArray。字符行向量将转换为字符串标量，字符串数组和字符行向量元胞数组将转换为各维尺寸相同的字符串数组。
	语法：Mex工具::万能转码<StringArray>(std::move(输入))
	参数：Array&& 输入，MATLAB标量右值引用。函数返回后，输入数组将变成不可用。
	*/
	template<>
	extern StringArray 万能转码<StringArray>(Array&& 输入);
	/*
	将C++类型转换为对应的MATLAB标量，执行必要的显式转换
	语法：Mex工具::万能转码<T输出>(std::move(输入));
	# 参数
	typename T输出，目标MATLAB数组类型。
	T输入&& 输入，可显式转换为MATLAB支持类型的输入值。例如，所有指针类型都能显式转换为MATLAB整数标量。对于简单类型，可以省略std::move。对于复杂类型，原输入值将不再可用。
	# 返回
	T输出，MATLAB简单标量
	*/
	template<typename T输出, typename T输入>
	inline T输出 万能转码(T输入&& 输入);
	/*
	将C++简单类型转换为对应的MATLAB简单标量，不执行转换
	语法：Mex工具::万能转码(输入);
	参数：T 输入，MATLAB元素类型的输入值。
	返回：MATLAB简单类型标量
	*/
	template<MATLAB简单元素 T>
	inline TypedArray<T>万能转码(T 输入);
	/*
	将C++复杂类型转换为对应的MATLAB复杂标量，执行移动构造
	语法：Mex工具::万能转码(std::move(输入));
	参数：T&& 输入，MATLAB复杂类型的输入值，必须使用std::move，那之后原输入值将不再可用
	返回：MATLAB复杂类型标量
	*/
	template<MATLAB复杂元素 T>
	inline TypedArray<T>万能转码(T&& 输入);
	/*
	将任意指针类型转换为 MATLAB size_t 标量
	语法：Mex工具::万能转码(输入);
	参数：const void* 输入，任意指针
	返回：TypedArray<size_t>，与指针数值相等的 MATLAB size_t 标量
	*/
	inline TypedArray<size_t>万能转码(const void* 输入);
	/*
	将任意MATLAB数组包装成元胞标量
	语法：Mex工具::万能转码(std::move(输入));
	参数：Array&& 输入，要包装的MATLAB数组。包入后，原输入数组将不再可用。
	返回：CellArray，包装了输入数组的元胞标量
	*/
	template<typename T = CellArray>
	inline CellArray 万能转码(Array&& 输入);
	/*
	将枚举类型转换成其基础类型的MATLAB标量
	语法：Mex工具::万能转码(输入)
	参数：T 输入，要转换的枚举类型
	返回：TypedArray<std::underlying_type_t<T>>，此枚举类基础类型的MATLAB标量
	*/
	template<typename T>
	TypedArray<std::underlying_type_t<T>> 万能转码(T 输入);
	template<MATLAB字符串 T>
	inline T 万能转码(const char*);
	/*
	将C样式UTF8字符串转换为 MATLAB UTF16 CharArray
	语法：Mex工具::万能转码<CharArray>(输入)
	参数：const char* 输入，0结尾的C样式UTF8字符串
	返回：CharArray MATLAB UTF16 字符行向量，使用 Win32 MultiByteToWideChar 执行转码
	*/
	template<>
	inline CharArray 万能转码<CharArray>(const char* 输入);
	/*
	将C样式UTF8字符串转换为 MATLAB UTF16 String
	语法：Mex工具::万能转码<String>(输入)
	参数：const char* 输入，0结尾的C样式UTF8字符串
	返回：String MATLAB UTF16 字符串元素，使用 Win32 MultiByteToWideChar 执行转码
	*/
	template<>
	extern String 万能转码<String>(const char* 输入);
	/*
	将C样式UTF8字符串转换为 UTF16 MATLABString
	语法：Mex工具::万能转码<MATLABString>(输入)
	参数：const char* 输入，0结尾的C样式UTF8字符串
	返回：MATLABString，MATLAB UTF16 字符串元素，使用 Win32 MultiByteToWideChar 执行转码
	*/
	template<>
	inline MATLABString 万能转码<MATLABString>(const char* 输入);
	/*
	将C样式UTF8字符串转换为 MATLAB UTF16 StringArray
	语法：Mex工具::万能转码<StringArray>(输入)
	参数：const char* 输入，0结尾的C样式UTF8字符串
	返回：StringArray MATLAB UTF16 字符串数组，使用 Win32 MultiByteToWideChar 执行转码
	*/
	template<>
	inline StringArray 万能转码<StringArray>(const char* 输入);
	template<MATLAB字符串 T>
	inline T 万能转码(const wchar_t*);
	/*
	将C样式 UTF16 wchar_t 字符串转换为 MATLAB CharArray。C++ wchar_t 和 MATLAB char16_t 本质上是同一种类型，不需要转码
	语法：Mex工具::万能转码<CharArray>(输入)
	参数：const wchar_t* 输入，0结尾的C样式 UTF16 wchar_t 字符串
	返回：CharArray MATLAB UTF16 字符行向量
	*/
	template<>
	inline CharArray 万能转码<CharArray>(const wchar_t* 输入);
	/*
	将C样式 UTF16 wchar_t 字符串转换为 MATLAB String。C++ wchar_t 和 MATLAB char16_t 本质上是同一种类型，不需要转码
	语法：Mex工具::万能转码<String>(输入)
	参数：const wchar_t* 输入，0结尾的C样式 UTF16 wchar_t 字符串
	返回：String MATLAB UTF16 字符串元素
	*/
	template<>
	inline String 万能转码<String>(const wchar_t* 输入);
	/*
	将C样式 UTF16 wchar_t 字符串转换为 MATLABString。C++ wchar_t 和 MATLAB char16_t 本质上是同一种类型，不需要转码
	语法：Mex工具::万能转码<MATLABString>(输入)
	参数：const wchar_t* 输入，0结尾的C样式 UTF16 wchar_t 字符串
	返回：MATLABString UTF16 字符串元素
	*/
	template<>
	inline MATLABString 万能转码<MATLABString>(const wchar_t* 输入);
	/*
	将C样式 UTF16 wchar_t 字符串转换为 MATLAB StringArray。C++ wchar_t 和 MATLAB char16_t 本质上是同一种类型，不需要转码
	语法：Mex工具::万能转码<StringArray>(输入)
	参数：const wchar_t* 输入，0结尾的C样式 UTF16 wchar_t 字符串
	返回：StringArray MATLAB UTF16 字符串数组
	*/
	template<>
	inline StringArray 万能转码<StringArray>(const wchar_t* 输入);
	//获取MATLAB数组的字节数，即元素字节数×元素个数
	inline size_t 数组字节数(const Array& 数组);
	/*
	将MATLAB简单数组拷贝到C++迭代器或指针。这里的指针指的是定义了[]索引, ++自增和*解引用运算符的任何类型；迭代器类似于指针，但不需要定义[]索引。从满数组，可以拷贝到迭代器；从稀疏矩阵，可以拷贝到指针。输出的数据类型必须能够从输入类型隐式转换。一个反例是，整数类型不能隐式转换为指针类型，因此接受指针的迭代器不能在此处使用。从任意数组可以拷贝到void*，不会进行任何类型转换或解析，直接拷贝原始字节。
	语法：Mex工具::万能转码(std::move(输入),输出);
	# 参数
	Array&& 输入，MATLAB数组右值引用。函数返回后，输入数组将不再可用。
	T& 输出，输出迭代器或指针。对于稀疏矩阵，只能输出到指针（定义[]索引操作）；对于满矩阵，可以输出到迭代器。无论哪种，都可以输出到void*。如果T不是const，函数返回后，输出迭代器将被指向输出序列的末尾。
	*/
	template<非const T>
	inline void 万能转码(Array&& 输入, T& 输出);
	/*
	将MATLAB简单数组拷贝到C++迭代器或指针。这里的指针指的是定义了[]索引, ++自增和*解引用运算符的任何类型；迭代器类似于指针，但不需要定义[]索引。从满数组，可以拷贝到迭代器；从稀疏矩阵，可以拷贝到指针。输出的数据类型必须能够从输入类型隐式转换。一个反例是，整数类型不能隐式转换为指针类型，因此接受指针的迭代器不能在此处使用。从任意数组可以拷贝到void*，不会进行任何类型转换或解析，直接拷贝原始字节。
	语法：Mex工具::万能转码(std::move(输入),输出);
	# 参数
	Array&& 输入，MATLAB数组右值引用。函数返回后，输入数组将不再可用。
	T&& 输出，输出迭代器或指针。对于稀疏矩阵，只能输出到指针（定义[]索引操作）；对于满矩阵，可以输出到迭代器。无论哪种，都可以输出到void*。如果T不是const，函数返回后，输出迭代器将被指向输出序列的末尾。
	*/
	template<typename T>
	inline void 万能转码(Array&& 输入, T&& 输出);
	/*
	将 MATLAB UTF16 字符行向量、字符串标量或字符向量元胞标量拷出为C样式0结尾的 UTF8 char*
	语法：Mex工具::万能转码(std::move(输入),输出)
	# 参数
	Array&& 输入，MATLAB UTF16 字符行向量、字符串标量或字符向量元胞标量。函数返回后，输入数组将不再可用。
	char*const& 输出，应确保有足够大的内存分配，输出的C样式字符串将以0结尾。
	# 返回值
	int，输出的UTF8字节数，包括尾随0
	*/
	int 万能转码(Array&& 输入, char* const& 输出);
	/*
	将 MATLAB UTF16 字符行向量、字符串标量或字符向量元胞标量拷出为C样式0结尾的 UTF8 char*
	语法：Mex工具::万能转码(std::move(输入),输出)
	# 参数
	Array&& 输入，MATLAB UTF16 字符行向量、字符串标量或字符向量元胞标量。函数返回后，输入数组将不再可用。
	char*& 输出，应确保有足够大的内存分配，输出的C样式字符串将以0结尾。函数返回后，此指针将指向输出序列的末尾。
	# 返回值
	int，输出的UTF8字节数，包括尾随0
	*/
	inline int 万能转码(Array&& 输入, char*& 输出);
	/*
	将MATLAB字符行向量、字符串标量或字符向量元胞标量拷出到接受wchar_t的迭代器，例如wchar_t*
	语法：Mex工具::万能转码(std::move(输入),输出)
	# 参数
	Array&& 输入，MATLAB UTF16 字符行向量、字符串标量或字符向量元胞标量。函数返回后，输入数组将不再可用。
	隐写迭代器<wchar_t> T& 输出迭代器，应接受wchar_t输入，确保有足够大的内存分配。输出的字符串不会在结尾加0。如果T不是const，函数返回后，输出迭代器将被指向输出序列的末尾。
	*/
	template<同类迭代器<wchar_t> T>
	void 万能转码(Array&& 输入, T& 输出);
	/*
	将MATLAB字符行向量、字符串标量或字符向量元胞标量拷出到接受wchar_t的迭代器，例如wchar_t*
	语法：Mex工具::万能转码(std::move(输入),输出)
	# 参数
	Array&& 输入，MATLAB UTF16 字符行向量、字符串标量或字符向量元胞标量。函数返回后，输入数组将不再可用。
	隐写迭代器<wchar_t> T&& 输出迭代器，应接受wchar_t输入，确保有足够大的内存分配。输出的字符串不会在结尾加0。如果T不是const，函数返回后，输出迭代器将被指向输出序列的末尾。
	*/
	template<同类迭代器<wchar_t> T>
	inline void 万能转码(Array&& 输入, T&& 输出);
	/*
	将 MATLAB UTF16 字符行向量、字符串数组或字符向量元胞数组拷出到接受 UTF8 std::string 的迭代器
	语法：Mex工具::万能转码(std::move(输入),输出)
	# 参数
	Array&& 输入，MATLAB UTF16 字符行向量、字符串数组或字符向量元胞数组。函数返回后，输入数组将不再可用。
	同类迭代器<std::string> T& 输出迭代器，应接受std::string输入，确保有足够大的内存分配。函数返回后，输出迭代器将被指向输出序列的末尾。
	*/
	template<同类迭代器<std::string> T>
		requires 非const<T>
	void 万能转码(Array&& 输入, T& 输出);
	/*
	将 MATLAB UTF16 字符行向量、字符串数组或字符向量元胞数组拷出到接受 UTF8 std::string 的迭代器
	语法：Mex工具::万能转码(std::move(输入),输出)
	# 参数
	Array&& 输入，MATLAB UTF16 字符行向量、字符串数组或字符向量元胞数组。函数返回后，输入数组将不再可用。
	同类迭代器<std::string> const T&& 输出迭代器，应接受std::string输入，确保有足够大的内存分配。
	*/
	template<同类迭代器<std::string> T>
	inline void 万能转码(Array&& 输入, T&& 输出);
	/*
	从C++迭代器拷贝到MATLAB简单数组。
	*/
	template<MATLAB简单数组 T输出, 隐读迭代器<数组类型转元素<T输出>> T输入>
	inline T输出 万能转码(T输入& 输入, const ArrayDimensions& 各维尺寸);
	/*
	从C++迭代器拷贝到MATLAB简单数组。
	*/
	template<MATLAB简单数组 T输出, 隐读迭代器<数组类型转元素<T输出>> T输入>
	inline T输出 万能转码(T输入&& 输入, const ArrayDimensions& 各维尺寸);
	/*
	从C++迭代器输出到MATLAB数组。
	语法：Mex工具::万能转码<T输出>(输入,各维尺寸)
	# 参数
	typename T输出，MATLAB数组类型
	非const T输入& 输入，可读迭代器，输出值应当可显式转换为目标MATLAB类型。例如，任何指针类型都可以显式转换为任何整数类型。函数返回后，迭代器将向后移动【输出数组的元素个数】个位置。
	const ArrayDimensions& 各维尺寸，输出数组各维尺寸。
	# 返回值
	T输出，指定类型的MATLAB数组
	*/
	template<typename T输出, 非const T输入>
	inline T输出 万能转码(T输入& 输入, const ArrayDimensions& 各维尺寸);
	/*
	从C++迭代器输出到MATLAB数组。
	语法：Mex工具::万能转码<T输出>(输入,各维尺寸)
	# 参数
	typename T输出，MATLAB数组类型
	T输入&& 输入，可读迭代器，输出值应当可显式转换为目标MATLAB类型。例如，任何指针类型都可以显式转换为任何整数类型。
	const ArrayDimensions& 各维尺寸，输出数组各维尺寸。
	# 返回值
	T输出，指定类型的MATLAB数组
	*/
	template<typename T输出, typename T输入>
	inline T输出 万能转码(T输入&& 输入, const ArrayDimensions& 各维尺寸);
	/*
	从C++迭代器输出到对应类型的MATLAB简单数组，不执行类型转换。
	语法：Mex工具::万能转码(输入,各维尺寸)
	# 示例
	int a[] = { 1,2,3 };
	万能转码(a, { 3,1 });
	万能转码(L"123", { 1,3 }); //错误，wchar_t不是合法的MATLAB元素类型
	万能转码((char16_t*)L"123", { 1,3 }); //正确，char16_t是合法的MATLAB元素类型
	# 参数
	T输入& 输入，可读迭代器，输出值应当是合法的MATLAB简单元素类型。如果T输入支持加法运算，函数返回后，迭代器将向后移动【输出数组的元素个数】个位置。
	const ArrayDimensions& 各维尺寸，输出MATLAB数组各维尺寸
	# 返回值
	TypedArray<取迭代器值类型<T输入>>，从迭代器输出类型的MATLAB数组
	*/
	template<迭代器 T输入, MATLAB简单元素 T输出 = 取迭代器值类型<T输入>>
	inline TypedArray<T输出> 万能转码(T输入& 输入, const ArrayDimensions& 各维尺寸);
	/*
	从C++迭代器输出到对应类型的MATLAB简单数组，不执行类型转换。
	语法：Mex工具::万能转码(输入,各维尺寸)
	# 示例
	int a[] = { 1,2,3 };
	万能转码(a, { 3,1 });
	万能转码(L"123", { 1,3 }); //错误，wchar_t不是合法的MATLAB元素类型
	万能转码((char16_t*)L"123", { 1,3 }); //正确，char16_t是合法的MATLAB元素类型
	# 参数
	T输入&& 输入，可读迭代器，输出值应当是合法的MATLAB简单元素类型。如果T输入支持加法运算，函数返回后，迭代器将向后移动【输出数组的元素个数】个位置。
	const ArrayDimensions& 各维尺寸，输出MATLAB数组各维尺寸
	# 返回值
	TypedArray<取迭代器值类型<T输入>>，从迭代器输出类型的MATLAB数组
	*/
	template<迭代器 T输入, MATLAB简单元素 T输出 = 取迭代器值类型<T输入>>
	inline TypedArray<T输出> 万能转码(T输入&& 输入, const ArrayDimensions& 各维尺寸);
	/*
	从C++迭代器输出到对应类型的MATLAB复杂数组，不执行类型转换，执行移动构造
	语法：Mex工具::万能转码(输入,各维尺寸)
	# 参数
	非const T输入& 输入，可读迭代器，输出值应当是合法的MATLAB复杂元素类型。函数返回后，原来位置的那些复杂元素将不再可用，迭代器将向后移动【输出数组的元素个数】个位置。
	const ArrayDimensions& 各维尺寸，输出MATLAB数组各维尺寸
	# 返回值
	TypedArray<取迭代器值类型<T输入>>，从迭代器输出类型的MATLAB数组
	*/
	template<迭代器 T输入, MATLAB复杂元素 T输出 = 取迭代器值类型<T输入>>
	TypedArray<T输出> 万能转码(T输入& 输入, const ArrayDimensions& 各维尺寸);
	/*
	从C++迭代器输出到对应类型的MATLAB复杂数组，不执行类型转换，执行移动构造
	语法：Mex工具::万能转码(输入,各维尺寸)
	# 参数
	T输入&& 输入，可读迭代器，输出值应当是合法的MATLAB复杂元素类型。函数返回后，原来位置的那些复杂元素将不再可用。
	const ArrayDimensions& 各维尺寸，输出MATLAB数组各维尺寸
	# 返回值
	TypedArray<取迭代器值类型<T输入>>，从迭代器输出类型的MATLAB数组
	*/
	template<迭代器 T输入, MATLAB复杂元素 T输出 = 取迭代器值类型<T输入>>
	inline TypedArray<T输出> 万能转码(T输入&& 输入, const ArrayDimensions& 各维尺寸);
	/*
	从C++枚举类型的迭代器输出到其对应基础类型的MATLAB数组。
	语法：Mex工具::万能转码(输入,各维尺寸)
	# 参数
	非const T输入& 输入，输出枚举类型的迭代器。函数返回后，迭代器将向后移动【输出数组的元素个数】个位置。
	const ArrayDimensions& 各维尺寸，输出MATLAB数组的各维尺寸
	# 返回
	TypedArray<std::underlying_type_t<取迭代器值类型<T输入>>>，从输入迭代器输出的枚举的基础类型的MATLAB数组
	*/
	template<迭代器 T输入, CanCreateArray T输出 = std::underlying_type_t<取迭代器值类型<T输入>>>
	TypedArray<T输出> 万能转码(T输入& 输入, const ArrayDimensions& 各维尺寸);
	/*
	从C++枚举类型的迭代器输出到其对应基础类型的MATLAB数组。
	语法：Mex工具::万能转码(输入,各维尺寸)
	# 参数
	T输入&& 输入，输出枚举类型的迭代器。
	const ArrayDimensions& 各维尺寸，输出MATLAB数组的各维尺寸
	# 返回
	TypedArray<std::underlying_type_t<取迭代器值类型<T输入>>>，从输入迭代器输出的枚举的基础类型的MATLAB数组
	*/
	template<迭代器 T输入, CanCreateArray T输出 = std::underlying_type_t<取迭代器值类型<T输入>>>
	inline TypedArray<T输出> 万能转码(T输入&& 输入, const ArrayDimensions& 各维尺寸);
	/*
	从 C++  UTF8 std::string 迭代器输出为 MATLAB UTF16 StringArray
	语法：Mex工具::万能转码(输入,各维尺寸)
	# 参数
	同类迭代器<std::string> T& 输入，可读std::string迭代器。函数返回后，迭代器将向后移动【输出数组的元素个数】个位置。
	const ArrayDimensions& 各维尺寸，输出MATLAB字符串数组的各维尺寸
	# 返回值
	StringArray，MATLAB UTF16 字符串数组
	*/
	template<只能是<StringArray>T输出 = StringArray, 同类迭代器<std::string> T>
	StringArray 万能转码(T& 输入, const ArrayDimensions& 各维尺寸);
	/*
	从 C++  UTF8 std::string 迭代器输出为 MATLAB UTF16 StringArray
	语法：Mex工具::万能转码(输入,各维尺寸)
	# 参数
	T&& 输入，可读std::string迭代器。函数返回后，迭代器将向后移动【输出数组的元素个数】个位置。
	const ArrayDimensions& 各维尺寸，输出MATLAB字符串数组的各维尺寸
	# 返回值
	StringArray，MATLAB UTF16 字符串数组
	*/
	template<只能是<StringArray>T输出 = StringArray, 同类迭代器<std::string> T>
	inline StringArray 万能转码(T&& 输入, const ArrayDimensions& 各维尺寸);
	/*
	使用动态类型创建一个动态类型缓冲，执行内存拷贝然后打包为 MATLAB Array
	泛型编程的一个常见情境，就是我们不关心用户提供的运行时数据类型，只是单纯进行内存拷贝。但是MATLAB官方提供的 MEX API 不允许动态类型的内存拷贝，因此这里提供一个动态类型缓冲区，可以接受一个运行时类型枚举，创建一个动态类型缓冲，完成数据拷贝后再打包成无类型的 MATLAB Array。
	*/
	struct 动态类型缓冲
	{
		void* const 指针;
		const size_t 字节数;
		/*
		创建动态类型缓冲。
		语法：Mex工具::动态类型缓冲::创建(类型,元素数)
		# 参数
		ArrayType 类型，动态类型枚举
		size_t 元素数
		# 返回值
		std::unique_ptr<动态类型缓冲>，请通过指针操作对象，因为内部实现是被子类继承的，直接取值会导致对象被截断。
		*/
		static std::unique_ptr<动态类型缓冲>创建(ArrayType 类型, size_t 元素数);
		virtual ~动态类型缓冲() {}
		//打包后本对象变为不可用，所有数据封装在 MATLAB Array 中
		virtual Array 打包(ArrayDimensions 各维尺寸)noexcept = 0;
		virtual Array 打包()noexcept = 0;
	};
}
```
某些计算机/编译器可能存在中文编码错误问题。你需要设置中文编码为UTF-8。对于 Windows 11，可以在任务栏搜索intl.cpl，转到【管理\非Unicode程序的语言\更改系统区域设置】，勾选【Beta版：使用 Unicode UTF-8 提供全球语言支持】