#pragma once
#include"Mex类型.h"
#include<array>
#include<Windows.h>
using namespace matlab::data;
using namespace matlab::mex;
namespace Mex工具
{
	enum Mex异常 :uint8_t
	{
		无异常,
		类型转换失败,
		内存拷贝失败,
		枚举类型不能拷贝,
		不支持的类型,
		不支持的API,
		此Array不能转换为CharArray,
		此Array不能转换为String,
		此Array不能转换为stdstring,
		此Array不能转换为StringArray,
		此Array不能转换为MATLABString,
		此Array不能拷出为char,
		此Array不能拷出为wchar_t
	};
	//这里使用static而不是extern，因为从其它编译单元链接的变量不一定能在DllMain阶段完成初始化，会造成意外错误。
	static ArrayFactory 数组工厂;
	//可以作为MATLAB数组元素的类型
	template<typename T>
	concept MATLAB元素类型 = requires{GetArrayType<T>::type; };
	template<typename T>
	concept MATLAB简单元素 = MATLAB元素类型<T> && !std::_Is_any_of_v<T, Array, Struct, MATLABString, Object, Enumeration>;
	/*
* 根据动态类型选择类模板并返回成员
* MATLAB数组使用一个枚举值指示数组的动态类型。当你试图编写一个泛型函数时，一般来说需要用一个冗长的switch语句将其正确转换为C++静态类型，然后调用对应的函数模板。本函数封装此过程，根据MATLAB动态类型，实例化对应的模板类，取其返回成员。调用方可按需求设计一个模板类来包装类型特定的方法。本文件中的万能转码函数就用此方法实现标量转换，实现方法亦可作为参考示例。
* @param 模板，必须至少接受一个类型参数作为第一个模板参数，至少包含一个名为“返回”的静态成员，此成员的类型必须与运行时输入的MATLAB类型（即第一个模板参数）无关；如果是静态函数，则函数的所有输入参数和返回值类型必须与运行时输入的MATLAB类型无关。模板的第一个参数应当为所有MATLAB对应类型提供“返回”静态成员，还应当为nullptr_t类型也提供“返回”静态成员——这用于对不支持的MATLAB类型进行异常处理。例如，你可以为nullptr_t类型特化的返回成员提供一个异常输出。
* @param Ts，将要传递给模板的后续模板参数。如果你提供的模板不具有或具有可选的后续模板参数，则此处的后续模板参数也是可选的。
* @param 类型，运行时得到的MATLAB动态类型枚举
* @return 返回模板类名为“返回”的成员
*/
	template<template <typename T, typename...Ts>typename 模板, typename...Ts>
	constexpr auto 动态类型选择模板(ArrayType 类型)
	{
		switch (类型)
		{
		case ArrayType::LOGICAL:
			return 模板<bool, Ts...>::返回;
		case ArrayType::CHAR:
			return 模板<CHAR16_T, Ts...>::返回;
		case ArrayType::MATLAB_STRING:
			return 模板<MATLABString, Ts...>::返回;
		case ArrayType::DOUBLE:
			return 模板<double, Ts...>::返回;
		case ArrayType::SINGLE:
			return 模板<float, Ts...>::返回;
		case ArrayType::INT8:
			return 模板<int8_t, Ts...>::返回;
		case ArrayType::INT16:
			return 模板<int16_t, Ts...>::返回;
		case ArrayType::INT32:
			return 模板<int32_t, Ts...>::返回;
		case ArrayType::INT64:
			return 模板<int64_t, Ts...>::返回;
		case ArrayType::UINT8:
			return 模板<uint8_t, Ts...>::返回;
		case ArrayType::UINT16:
			return 模板<uint16_t, Ts...>::返回;
		case ArrayType::UINT32:
			return 模板<uint32_t, Ts...>::返回;
		case ArrayType::UINT64:
			return 模板<uint64_t, Ts...>::返回;
		case ArrayType::COMPLEX_DOUBLE:
			return 模板<std::complex<double>, Ts...>::返回;
		case ArrayType::COMPLEX_SINGLE:
			return 模板<std::complex<float>, Ts...>::返回;
		case ArrayType::COMPLEX_INT8:
			return 模板<std::complex<int8_t>, Ts...>::返回;
		case ArrayType::COMPLEX_INT16:
			return 模板<std::complex<int16_t>, Ts...>::返回;
		case ArrayType::COMPLEX_INT32:
			return 模板<std::complex<int32_t>, Ts...>::返回;
		case ArrayType::COMPLEX_INT64:
			return 模板<std::complex<int64_t>, Ts...>::返回;
		case ArrayType::COMPLEX_UINT8:
			return 模板<std::complex<uint8_t>, Ts...>::返回;
		case ArrayType::COMPLEX_UINT16:
			return 模板<std::complex<uint16_t>, Ts...>::返回;
		case ArrayType::COMPLEX_UINT32:
			return 模板<std::complex<uint32_t>, Ts...>::返回;
		case ArrayType::COMPLEX_UINT64:
			return 模板<std::complex<uint64_t>, Ts...>::返回;
		case ArrayType::CELL:
			return 模板<Array, Ts...>::返回;
		case ArrayType::STRUCT:
			return 模板<Struct, Ts...>::返回;
		case ArrayType::ENUM:
			return 模板<Enumeration, Ts...>::返回;
		case ArrayType::OBJECT:
			return 模板<Object, Ts...>::返回;
		case ArrayType::SPARSE_LOGICAL:
			return 模板<SparseArray<bool>, Ts...>::返回;
		case ArrayType::SPARSE_DOUBLE:
			return 模板<SparseArray<double>, Ts...>::返回;
		case ArrayType::SPARSE_COMPLEX_DOUBLE:
			return 模板<SparseArray<std::complex<double>>, Ts...>::返回;
		default:
			return 模板<void, Ts...>::返回;
		}
	}
	namespace 内部
	{
		template<ArrayType T>struct 动态类型转静态 { using 类型 = void; };
		template<>struct 动态类型转静态<ArrayType::LOGICAL> { using 类型 = bool; };
		template<>struct 动态类型转静态<ArrayType::CHAR> { using 类型 = CHAR16_T; };
		template<>struct 动态类型转静态<ArrayType::MATLAB_STRING> { using 类型 = MATLABString; };
		template<>struct 动态类型转静态<ArrayType::DOUBLE> { using 类型 = double; };
		template<>struct 动态类型转静态<ArrayType::SINGLE> { using 类型 = float; };
		template<>struct 动态类型转静态<ArrayType::INT8> { using 类型 = int8_t; };
		template<>struct 动态类型转静态<ArrayType::INT16> { using 类型 = int16_t; };
		template<>struct 动态类型转静态<ArrayType::INT32> { using 类型 = int32_t; };
		template<>struct 动态类型转静态<ArrayType::INT64> { using 类型 = int64_t; };
		template<>struct 动态类型转静态<ArrayType::UINT8> { using 类型 = uint8_t; };
		template<>struct 动态类型转静态<ArrayType::UINT16> { using 类型 = uint16_t; };
		template<>struct 动态类型转静态<ArrayType::UINT32> { using 类型 = uint32_t; };
		template<>struct 动态类型转静态<ArrayType::UINT64> { using 类型 = uint64_t; };
		template<>struct 动态类型转静态<ArrayType::COMPLEX_DOUBLE> { using 类型 = std::complex<double>; };
		template<>struct 动态类型转静态<ArrayType::COMPLEX_SINGLE> { using 类型 = std::complex<float>; };
		template<>struct 动态类型转静态<ArrayType::COMPLEX_INT8> { using 类型 = std::complex<int8_t>; };
		template<>struct 动态类型转静态<ArrayType::COMPLEX_INT16> { using 类型 = std::complex<int16_t>; };
		template<>struct 动态类型转静态<ArrayType::COMPLEX_INT32> { using 类型 = std::complex<int32_t>; };
		template<>struct 动态类型转静态<ArrayType::COMPLEX_INT64> { using 类型 = std::complex<int64_t>; };
		template<>struct 动态类型转静态<ArrayType::COMPLEX_UINT8> { using 类型 = std::complex<uint8_t>; };
		template<>struct 动态类型转静态<ArrayType::COMPLEX_UINT16> { using 类型 = std::complex<uint16_t>; };
		template<>struct 动态类型转静态<ArrayType::COMPLEX_UINT32> { using 类型 = std::complex<uint32_t>; };
		template<>struct 动态类型转静态<ArrayType::COMPLEX_UINT64> { using 类型 = std::complex<uint64_t>; };
		template<>struct 动态类型转静态<ArrayType::CELL> { using 类型 = Array; };
		template<>struct 动态类型转静态<ArrayType::STRUCT> { using 类型 = Struct; };
		template<>struct 动态类型转静态<ArrayType::ENUM> { using 类型 = Enumeration; };
		template<>struct 动态类型转静态<ArrayType::OBJECT> { using 类型 = Object; };
		template<>struct 动态类型转静态<ArrayType::SPARSE_LOGICAL> { using 类型 = SparseArray<bool>; };
		template<>struct 动态类型转静态<ArrayType::SPARSE_DOUBLE> { using 类型 = SparseArray<double>; };
		template<>struct 动态类型转静态<ArrayType::SPARSE_COMPLEX_DOUBLE> { using 类型 = SparseArray<std::complex<double>>; };
		template<typename T>
		constexpr std::array<uint8_t, 0>类型尺寸;
		template<int...T>
		constexpr std::array<uint8_t, sizeof...(T)> 类型尺寸<std::integer_sequence<int, T...>> = { sizeof(动态类型转静态<ArrayType(T)>)... };
		/*
		转换结构，用作【动态类型选择模板】的模板参数。此转换结构提供一个一般实现和两个特化实现，都有一个【static 输出 返回(Array&& 数组)】成员。注意返回成员的类型跟模板参数【输出】有关，因为输出属于后续模板参数，而【动态类型选择模板】仅要求第一个模板参数不能与返回成员类型有关。因此这里的返回成员可以跟输出类型有关，而不能跟输入类型有关。
		对一般的输入输出，我们认为它不是有效的类型转换，应当抛异常。
		*/
		template<typename 输入, typename 输出>
		struct 转换结构
		{
			static 输出 返回(Array&& 数组)
			{
				throw 类型转换失败;
			}
		};
		//满足std::is_convertible_v约束的特化类型，可以执行正确的类型转换
		template<typename T输入, typename T输出>
		concept 可转换 = requires(T输入 输入)
		{
			T输出(输入);
		};
		template<MATLAB简单元素 T输入,typename T输出>
		requires 可转换<T输入,T输出>
		struct 转换结构<T输入, T输出>
		{
			static T输出 返回(Array&& 数组)
			{
				return T输出(TypedArray<T输入>(数组)[0]);
			}
		};
		//对于MATLAB稀疏矩阵需要特殊处理，输入类型虽然是SparseArray，但输出的应当是元素的类型，而不是数组类型。
		template<MATLAB简单元素 T输入,typename T输出>
		requires 可转换<T输入,T输出>
		struct 转换结构<SparseArray<T输入>, T输出>
		{
			static T输出 返回(Array&& 数组)
			{
				return T输出(SparseArray<T输入>(数组)[0]);
			}
		};
		template<typename T输入, typename T输出>
		struct 数组拷贝
		{
			static void 返回(Array&& 输入, T输出 输出)
			{
				throw 类型转换失败;
			}
		};
		template<MATLAB简单元素 T输入,typename T输出>
			requires requires(const TypedArray<T输入>& 输入, T输出 输出) { std::copy(输入.cbegin(), 输入.cend(), 输出); }
		struct 数组拷贝<T输入, T输出>
		{
			static void 返回(Array&& 输入, T输出 输出)
			{
				const TypedArray<T输入>类型化(std::move(输入));
				std::copy(类型化.cbegin(), 类型化.cend(), 输出);
			}
		};
		//void*特化用来处理用户不关心数组具体类型的情况
		template<typename T输入>
			requires (!std::is_void_v<T输入>)
		struct 数组拷贝<T输入, void*>
		{
			static void 返回(Array&& 输入, void* 输出)
			{
				数组拷贝<T输入, T输入*>::返回(std::move(输入), (T输入*)输出);
			}
		};
		template<MATLAB简单元素 T输入, typename T输出>
			requires requires(T输出 输出, TypedIterator<const T输入>a)
		{
			std::fill_n(输出, 0, 0);
			输出[0] = *a;
		}
		struct 数组拷贝<SparseArray<T输入>, T输出>
		{
			static void 返回(Array&& 输入, T输出 输出)
			{
				const SparseArray<T输入>稀疏(std::move(输入));
				const TypedIterator<const T输入>迭代尾 = 稀疏.cend();
				const size_t 高度 = 稀疏.getDimensions().front();
				std::fill_n(输出, 稀疏.getNumberOfElements(), 0);
				for (TypedIterator<const T输入>a = 稀疏.cbegin(); a < 迭代尾; ++a)
				{
					const SparseIndex 索引 = 稀疏.getIndex(a);
					输出[索引.first + 索引.second * 高度] = *a;
				}
			}
		};
		template<typename T输入>
		struct 数组拷贝<SparseArray<T输入>, void*>
		{
			static void 返回(Array&& 输入, void* 输出)
			{
				数组拷贝<SparseArray<T输入>, T输入*>::返回(std::move(输入), (T输入*)输出);
			}
		};
		template<typename T>
		struct 数组字节数
		{
			static size_t 返回(const Array& 输入)
			{
				return 输入.getNumberOfElements() * sizeof(T);
			}
		};
		template<typename T>
		struct 数组字节数<SparseArray<T>> :数组字节数<T> {};
		template<>
		struct 数组字节数<void>
		{
			static size_t 返回(const Array& 输入)
			{
				throw 不支持的类型;
			}
		};
		template<typename T>
			requires std::is_base_of_v<Array, T>
		struct 数组类型转元素_s {};
		template<MATLAB元素类型 T>
		struct 数组类型转元素_s<TypedArray<T>>
		{
			using 元素类型 = T;
		};
		template<>
		struct 数组类型转元素_s<CharArray>
		{
			using 元素类型 = char16_t;
		};
		template<>
		struct 数组类型转元素_s<StructArray>
		{
			using 元素类型 = Struct;
		};
	}
	//此using在将动态类型枚举ArrayType转为静态类型
	template<ArrayType T>
	using 动态类型转静态 = 内部::动态类型转静态<T>::类型;
	constexpr uint8_t 类型总数 = 32;
	//此数组存储了每种动态类型枚举对应的静态类型的尺寸。使用方法示例：类型尺寸[(int)inputs[1].getType()]
	constexpr std::array<uint8_t, 类型总数>类型尺寸 = 内部::类型尺寸<std::make_integer_sequence<int, 类型总数>>;
	template<typename T>
	concept MATLAB数组类型 = requires{typename 内部::数组类型转元素_s<T>::元素类型; };
	template<MATLAB数组类型 T>
	using 数组类型转元素 = 内部::数组类型转元素_s<T>::元素类型;

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
	requires (!std::is_same_v<T,CellArray>)
	inline T 万能转码(Array&& 输入)
	{
		return 动态类型选择模板<内部::转换结构, T>(输入.getType())(std::move(输入));
	}
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
	template<typename T输出,typename T输入>
		requires requires(T输入&& 输入){ 数组工厂.createScalar(数组类型转元素<T输出>(std::move(输入))); }
	inline T输出 万能转码(T输入&& 输入)
	{
		return 数组工厂.createScalar(数组类型转元素<T输出>(std::move(输入)));
	}
	/*
	将C++类型转换为对应的MATLAB标量，不执行转换
	语法：Mex工具::万能转码(std::move(输入));
	参数：T&& 输入，MATLAB元素类型的输入值。对于简单类型，可以省略std::move。对于复杂类型，原输入值将不再可用。
	返回：MATLAB标量
	*/
	template<typename T>
	requires requires(T&&输入){
		GetArrayType<T>::type;
		数组工厂.createScalar(std::move(输入));
	}
	inline TypedArray<T>万能转码(T&& 输入)
	{
		return 数组工厂.createScalar(std::move(输入));
	}
	/*
	将任意指针类型转换为 MATLAB size_t 标量
	语法：Mex工具::万能转码(输入);
	参数：const void* 输入，任意指针
	返回：TypedArray<size_t>，与指针数值相等的 MATLAB size_t 标量
	*/
	inline TypedArray<size_t>万能转码(const void* 输入)
	{
		return 数组工厂.createScalar((size_t)输入);
	}
	/*
	将任意MATLAB数组包装成元胞标量
	语法：Mex工具::万能转码(std::move(输入));
	参数：Array&& 输入，要包装的MATLAB数组。包入后，原输入数组将不再可用。
	返回：CellArray，包装了输入数组的元胞标量
	*/
	template<typename T=CellArray>
	requires std::is_same_v<T,CellArray>
	inline CellArray 万能转码(Array&& 输入)
	{
		CellArray 输出 = 数组工厂.createCellArray({ 1 });
		输出[0] = std::move(输入);
		return 输出;
	}
	/*
	将枚举类型转换成其基础类型的MATLAB标量
	语法：Mex工具::万能转码(输入)
	参数：T 输入，要转换的枚举类型
	返回：TypedArray<std::underlying_type_t<T>>，此枚举类基础类型的MATLAB标量
	*/
	template<typename T>
		requires requires(std::underlying_type_t<T> 输入)
	{
		GetArrayType<std::underlying_type_t<T>>::type;
		数组工厂.createScalar(输入);
	}
	TypedArray<std::underlying_type_t<T>> 万能转码(T 输入)
	{
		return 数组工厂.createScalar((std::underlying_type_t<T>)输入);
	}
	template<typename T>
	concept MATLAB字符串 = std::_Is_any_of_v<T, CharArray, String, MATLABString, StringArray>;
	template<MATLAB字符串 T>
	inline T 万能转码(const char*);
	/*
	将C样式UTF8字符串转换为 MATLAB UTF16 CharArray
	语法：Mex工具::万能转码<CharArray>(输入)
	参数：const char* 输入，0结尾的C样式UTF8字符串
	返回：CharArray MATLAB UTF16 字符行向量，使用 Win32 MultiByteToWideChar 执行转码
	*/
	template<>
	inline CharArray 万能转码<CharArray>(const char* 输入)
	{
		size_t 长度 = strlen(输入) + 1;
		buffer_ptr_t<char16_t>缓冲 = 数组工厂.createBuffer<char16_t>(长度);
		长度 = MultiByteToWideChar(CP_UTF8, 0, 输入, -1, (wchar_t*)缓冲.get(), 长度) - 1;
		return 数组工厂.createArrayFromBuffer({ 1,长度 }, std::move(缓冲));
	}
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
	inline MATLABString 万能转码<MATLABString>(const char* 输入)
	{
		return 万能转码<String>(输入);
	}
	/*
	将C样式UTF8字符串转换为 MATLAB UTF16 StringArray
	语法：Mex工具::万能转码<StringArray>(输入)
	参数：const char* 输入，0结尾的C样式UTF8字符串
	返回：StringArray MATLAB UTF16 字符串数组，使用 Win32 MultiByteToWideChar 执行转码
	*/
	template<>
	inline StringArray 万能转码<StringArray>(const char* 输入)
	{
		return 数组工厂.createScalar(万能转码<MATLABString>(输入));
	}
	template<MATLAB字符串 T>
	inline T 万能转码(const wchar_t*);
	/*
	将C样式 UTF16 wchar_t 字符串转换为 MATLAB CharArray。C++ wchar_t 和 MATLAB char16_t 本质上是同一种类型，不需要转码
	语法：Mex工具::万能转码<CharArray>(输入)
	参数：const wchar_t* 输入，0结尾的C样式 UTF16 wchar_t 字符串
	返回：CharArray MATLAB UTF16 字符行向量
	*/
	template<>
	inline CharArray 万能转码<CharArray>(const wchar_t* 输入)
	{
		const size_t 字符个数 = wcslen(输入);
		buffer_ptr_t<char16_t>缓冲 = 数组工厂.createBuffer<char16_t>(字符个数);
		std::copy_n(输入, 字符个数, 缓冲.get());
		return 数组工厂.createArrayFromBuffer({ 1,字符个数 }, std::move(缓冲));
	}
	/*
	将C样式 UTF16 wchar_t 字符串转换为 MATLAB String。C++ wchar_t 和 MATLAB char16_t 本质上是同一种类型，不需要转码
	语法：Mex工具::万能转码<String>(输入)
	参数：const wchar_t* 输入，0结尾的C样式 UTF16 wchar_t 字符串
	返回：String MATLAB UTF16 字符串元素
	*/
	template<>
	inline String 万能转码<String>(const wchar_t* 输入)
	{
		return String((char16_t*)输入);
	}
	/*
	将C样式 UTF16 wchar_t 字符串转换为 MATLABString。C++ wchar_t 和 MATLAB char16_t 本质上是同一种类型，不需要转码
	语法：Mex工具::万能转码<MATLABString>(输入)
	参数：const wchar_t* 输入，0结尾的C样式 UTF16 wchar_t 字符串
	返回：MATLABString UTF16 字符串元素
	*/
	template<>
	inline MATLABString 万能转码<MATLABString>(const wchar_t* 输入)
	{
		return 万能转码<String>(输入);
	}
	/*
	将C样式 UTF16 wchar_t 字符串转换为 MATLAB StringArray。C++ wchar_t 和 MATLAB char16_t 本质上是同一种类型，不需要转码
	语法：Mex工具::万能转码<StringArray>(输入)
	参数：const wchar_t* 输入，0结尾的C样式 UTF16 wchar_t 字符串
	返回：StringArray MATLAB UTF16 字符串数组
	*/
	template<>
	inline StringArray 万能转码<StringArray>(const wchar_t* 输入)
	{
		return 数组工厂.createScalar(万能转码<MATLABString>(输入));
	}
	//获取MATLAB数组的字节数，即元素字节数×元素个数
	inline size_t 数组字节数(const Array& 数组)
	{
		return 动态类型选择模板<内部::数组字节数>(数组.getType())(数组);
	}
	/*
	将MATLAB简单数组拷贝到C++迭代器或指针。这里的指针指的是定义了[]索引, ++自增和*解引用运算符的任何类型；迭代器类似于指针，但不需要定义[]索引。从满数组，可以拷贝到迭代器；从稀疏矩阵，可以拷贝到指针。输出的数据类型必须能够从输入类型隐式转换。一个反例是，整数类型不能隐式转换为指针类型，因此接受指针的迭代器不能在此处使用。从任意数组可以拷贝到void*，不会进行任何类型转换或解析，直接拷贝原始字节。
	语法：Mex工具::万能转码(std::move(输入),输出);
	# 参数
	Array&& 输入，MATLAB数组右值引用。函数返回后，输入数组将不再可用。
	T 输出，输出迭代器或指针。对于稀疏矩阵，只能输出到指针（定义[]索引操作）；对于满矩阵，可以输出到迭代器。无论哪种，都可以输出到void*。
	*/
	template<typename T>
	inline void 万能转码(Array&& 输入, T 输出)
	{
		动态类型选择模板<内部::数组拷贝, T>(输入.getType())(std::move(输入), 输出);
	}
	/*
	将 MATLAB UTF16 字符行向量、字符串标量或字符向量元胞标量拷出为C样式0结尾的 UTF8 char*
	语法：Mex工具::万能转码(std::move(输入),输出)
	# 参数
	Array&& 输入，MATLAB UTF16 字符行向量、字符串标量或字符向量元胞标量。函数返回后，输入数组将不再可用。
	char* 输出，应确保有足够大的内存分配，输出的C样式字符串将以0结尾
	# 返回值
	int，输出的UTF8字节数，包括尾随0
	*/
	inline int 万能转码(Array&& 输入, char* 输出);
	/*
	将MATLAB字符行向量、字符串标量或字符向量元胞标量拷出到接受wchar_t的迭代器，例如wchar_t*
	语法：Mex工具::万能转码(std::move(输入),输出)
	# 参数
	Array&& 输入，MATLAB UTF16 字符行向量、字符串标量或字符向量元胞标量。函数返回后，输入数组将不再可用。
	T 输出迭代器，应接受wchar_t输入，确保有足够大的内存分配。输出的字符串不会在结尾加0。
	*/
	template<typename T>
		requires requires(T 输出, wchar_t 输入) { static_cast<decltype(*++输出)>(输入); }
	void 万能转码(Array&& 输入, T 输出)
	{
		switch (输入.getType())
		{
		case ArrayType::CHAR:
		{
			const CharArray 字符向量(std::move(输入));
			std::copy(字符向量.cbegin(), 字符向量.cend(), 输出);
		}
		break;
		case ArrayType::MATLAB_STRING:
		{
			const String 字符串(StringArray(std::move(输入))[0]);
			std::copy(字符串.cbegin(), 字符串.cend(), 输出);
		}
		break;
		case ArrayType::CELL:
		{
			const CharArray 字符向量(Array(CellArray(std::move(输入))[0]));
			std::copy(字符向量.cbegin(), 字符向量.cend(), 输出);
		}
		break;
		default:
			throw 此Array不能拷出为wchar_t;
		}
	}
	/*
	将 MATLAB UTF16 字符行向量、字符串数组或字符向量元胞数组拷出到接受 UTF8 std::string 的迭代器
	语法：Mex工具::万能转码(std::move(输入),输出)
	# 参数
	Array&& 输入，MATLAB UTF16 字符行向量、字符串数组或字符向量元胞数组。函数返回后，输入数组将不再可用。
	T 输出迭代器，应接受std::string输入，确保有足够大的内存分配。
	*/
	template<typename T>
		requires requires(T 输出, std::string 输入) { static_cast<decltype(*++输出)>(输入); }
	void 万能转码(Array&& 输入, T 输出)
	{
		switch (输入.getType())
		{
		case ArrayType::CHAR:
		{
			const int 长度 = 输入.getNumberOfElements();
			const buffer_ptr_t<char16_t>缓冲 = CharArray(std::move(输入)).release();
			输出->resize_and_overwrite(长度 * 3 + 1, [&缓冲, 长度](char* 指针, size_t 尺寸)
				{
					return WideCharToMultiByte(CP_UTF8, 0, (wchar_t*)缓冲.get(), 长度, 指针, 尺寸, nullptr, nullptr) - 1;
				});
		}
		break;
		case ArrayType::MATLAB_STRING:
			for (const String& 字符串 : StringArray(std::move(输入)))
				输出++->resize_and_overwrite(字符串.size() * 3 + 1, [&字符串](char* 指针, size_t 尺寸)
					{
						return WideCharToMultiByte(CP_UTF8, 0, (wchar_t*)字符串.c_str(), -1, 指针, 尺寸, nullptr, nullptr) - 1;
					});
			break;
		case ArrayType::CELL:
			for (Array&& a : CellArray(std::move(输入)))
			{
				const int 长度 = a.getNumberOfElements();
				const buffer_ptr_t<char16_t>缓冲 = CharArray(std::move(a)).release();
				输出++->resize_and_overwrite((长度 + 1) * 3, [&缓冲, 长度](char* 指针, size_t 尺寸)
					{
						return WideCharToMultiByte(CP_UTF8, 0, (wchar_t*)缓冲.get(), 长度, 指针, 尺寸, nullptr, nullptr) - 1;
					});
			}
			break;
		default:
			throw 此Array不能转换为stdstring;
		}
	}
	/*
	从C++迭代器输出到MATLAB数组。迭代器的输出值应当可显式转换为目标MATLAB类型。如果迭代器输出值是复杂类型，将执行移动构造，输出数组后迭代器内的原对象将不再可用。
	语法：Mex工具::万能转码<T输出>(输入,各维尺寸)
	# 示例
	```
	万能转码<CharArray>(L"输出字符串", { 1,5 }); //自动转换wchar_t为char16_t
	int a[] = { 1,2,3 };
	万能转码<TypedArray<double>>(a, { 3,1 }); //自动转换int为double
	```
	# 参数
	typename T输出，MATLAB数组类型
	T输入 输入，可读迭代器，输出值应当可显式转换为目标MATLAB类型
	ArrayDimensions 各维尺寸，输出数组各维尺寸。
	# 返回值
	T输出，指定类型的MATLAB数组
	*/
	template<typename T输出, typename T输入>
		requires requires(T输入 输入, T输出& 输出) 
	{ 
		输出 = 数组工厂.createArray<数组类型转元素<T输出>>({ 1 });
		*输出.begin() = (数组类型转元素<T输出>)std::move(*++输入);
	}
	T输出 万能转码(T输入 输入, ArrayDimensions 各维尺寸)
	{
		using T = 数组类型转元素<T输出>;
		T输出 输出 = 数组工厂.createArray<T>(各维尺寸);
		for (T& a : 输出)
			a = (T)std::move(*(输入++));
		return 输出;
	}
	/*
	从C++迭代器输出到对应类型的MATLAB数组，不执行类型转换。如果迭代器输出值是复杂类型，将执行移动构造，输出数组后迭代器内的原对象将不再可用。
	语法：Mex工具::万能转码(输入,各维尺寸)
	# 示例
	int a[] = { 1,2,3 };
	万能转码(a, { 3,1 });
	万能转码(L"123", { 1,3 }); //错误，wchar_t不是合法的MATLAB元素类型
	万能转码((char16_t*)L"123", { 1,3 }); //正确，char16_t是合法的MATLAB元素类型
	# 参数
	T输入 输入，可读迭代器，输出值应当是合法的MATLAB元素类型
	ArrayDimensions 各维尺寸，输出MATLAB数组各维尺寸
	# 返回值
	TypedArray<std::remove_cvref_t<decltype(*输入)>>，从迭代器输出类型的MATLAB数组
	*/
	template<typename T输入>
		requires requires(T输入 输入)
	{
		GetArrayType<std::remove_cvref_t<decltype(*++输入)>>::type;
		*数组工厂.createArray<std::remove_cvref_t<decltype(*++输入)>>({ 1 }).begin() = *++输入;
	}
	auto 万能转码(T输入 输入, ArrayDimensions 各维尺寸)
	{
		using T输出 = std::remove_cvref_t<decltype(*输入)>;
		TypedArray<T输出>输出 = 数组工厂.createArray<T输出>(各维尺寸);
		for (T输出& a : 输出)
			a = std::move(*(输入++));
		return 输出;
	}
	/*
	从C++枚举类型的迭代器输出到其对应基础类型的MATLAB数组。
	语法：Mex工具::万能转码(输入,各维尺寸)
	# 参数
	T输入 输入，输出枚举类型的迭代器
	ArrayDimensions 各维尺寸，输出MATLAB数组的各维尺寸
	# 返回
	TypedArray<std::underlying_type_t<decltype(*输入)>>，从输入迭代器输出的枚举的基础类型的MATLAB数组
	*/
	template<typename T输入>
		requires requires(T输入 输入)
	{
		GetArrayType<std::underlying_type_t<decltype(*++输入)>>::type;
		数组工厂.createArray<std::underlying_type_t<decltype(*输入)>>({ 1 });
	}
	auto 万能转码(T输入 输入, ArrayDimensions 各维尺寸)
	{
		using T输出 = std::underlying_type_t<decltype(*输入)>;
		TypedArray<T输出>输出 = 数组工厂.createArray<T输出>(各维尺寸);
		for (T输出& a : 输出)
			a = (T输出) * (输入++);
		return 输出;
	}
	/*
	从 C++  UTF8 std::string 迭代器输出为 MATLAB UTF16 StringArray
	语法：Mex工具::万能转码(输入,各维尺寸)
	# 参数
	T 输入，可读std::string迭代器
	ArrayDimensions 各维尺寸，输出MATLAB字符串数组的各维尺寸
	# 返回值
	StringArray，MATLAB UTF16 字符串数组
	*/
	template<typename T>
	requires requires(T 输入)
	{
		输入->size();
		MultiByteToWideChar(CP_UTF8, 0, 输入->c_str(), -1, nullptr, 0) - 1;
		输入++;
	}
	StringArray 万能转码(T 输入, ArrayDimensions 各维尺寸)
	{
		StringArray 输出 = 数组工厂.createArray<MATLABString>(各维尺寸);
		const size_t 个数 = 输出.getNumberOfElements();
		for (size_t a = 0; a < 个数; ++a)
		{
			String 字符串;
			字符串.resize_and_overwrite(输入->size() + 1, [&输入](char16_t* 指针, size_t 尺寸)
				{
					return MultiByteToWideChar(CP_UTF8, 0, 输入->c_str(), -1, (wchar_t*)指针, 尺寸) - 1;
				});
			输出[a] = std::move(字符串);
			输入++;
		}
	}
}