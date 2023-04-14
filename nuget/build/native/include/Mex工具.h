#pragma once
#include<Mex类型.h>
#include<array>
using namespace matlab::data;
using namespace matlab::mex;
namespace Mex工具
{
	enum Mex异常 :uint8_t
	{
		类型转换失败,
		内存拷贝失败,
		枚举类型不能拷贝,
		不支持的类型,
		不支持的API,
	};
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
			return 模板<nullptr_t, Ts...>::返回;
		}
	}
	namespace 内部
	{
		template<ArrayType T>struct 动态类型转静态 { using 类型 = nullptr_t; };
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
		template<typename 输入, typename 输出>
			requires std::is_convertible_v<输入, 输出>
		struct 转换结构<输入, 输出>
		{
			static 输出 返回(Array&& 数组)
			{
				return (输出)TypedArray<输入>(std::move(数组))[0];
			}
		};
		//对于MATLAB稀疏矩阵需要特殊处理，输入类型虽然是SparseArray，但输出的应当是元素的类型，而不是数组类型。
		template<typename 输入, typename 输出>
			requires std::is_convertible_v<输入, 输出>
		struct 转换结构<SparseArray<输入>, 输出>
		{
			static 输出 返回(Array&& 数组)
			{
				return (输出)SparseArray<输入>(std::move(数组))[0];
			}
		};
		template<typename T输入, typename T输出>
		struct 万能
		{
			static T输出 转码(T输入 输入);
		};
		template<typename T输出>
		struct 万能<Array&&, T输出>
		{
			static T输出 转码(Array&& 输入)
			{
				return 动态类型选择模板<转换结构, T输出>(输入.getType())(std::move(输入));
			}
		};
		template<typename T>
		struct 万能<Array&&, T*>
		{
			static T* 转码(Array&& 输入)
			{
				return (T*)万能<Array&&, size_t>::转码(std::move(输入));
			}
		};
		template<typename T输入, typename T输出>
		requires (!std::is_const_v<T输入>)
		struct 万能<T输入*, T输出> :万能<const T输入*, T输出> {};
		template<typename T>
		requires std::_Is_any_of_v<T,const char*,std::string>
		struct 万能<const T*, StringArray>
		{
			static StringArray 转码(const T* 输入, size_t 个数);
		};
		template<typename T>
		struct 万能<const T*, buffer_ptr_t<T>>
		{
			static buffer_ptr_t<T>转码(const T* 输入,size_t 个数)
			{
				buffer_ptr_t<T>缓冲 = 数组工厂.createBuffer<T>(个数);
				std::copy_n(输入, 个数, 缓冲.get());
				return 缓冲;
			}
		};
		template<typename T>
		struct 万能<T, TypedArray<T>>
		{
			static TypedArray<T>转码(T 输入)
			{
				return 数组工厂.createScalar(输入);
			}
		};
	}
	//此using在将动态类型枚举ArrayType转为静态类型
	template<ArrayType T>
	using 动态类型转静态 = 内部::动态类型转静态<T>::类型;
	constexpr uint8_t 类型总数 = 32;
	//此数组存储了每种动态类型枚举对应的静态类型的尺寸。使用方法示例：类型尺寸[(int)inputs[1].getType()]
	constexpr std::array<uint8_t, 类型总数>类型尺寸 = 内部::类型尺寸<std::make_integer_sequence<int, 类型总数>>;

	/*
	* 标量转换，支持所有类型MATLAB标量、元素和对应的C++类型互相转换。特别地，字符数组可以和字符串标量互相转换，所有基本数值类型都可以互相转换，所有指针类型可以转换为TypedArray<size_t>
	* @param T输出，转换目标类型，支持所有MATLAB类型和对应的C++类型。特别地，还支持 String, CharArray 和std::string
	* @param T输入，转换原本类型，支持所有MATLAB类型和对应的C++类型。特别地，还支持0结尾的C样式字符串，包括 const char* 和 const wchar_t*；还支持任意指针类型
	*/
	template<typename T输出,typename T输入>
	inline T输出 万能转码(T输入 输入)
	{
		return 内部::万能<T输入, T输出>::转码(输入);
	}
	//将C++指针转换为对应类型的MATLAB缓冲指针，或者将 const std::string* 和 const char*const* 转换为StringArray。必须指定数组元素个数。
	template<typename T输出,typename T输入>
	inline T输出 万能转码(const T输入* 输入, size_t 个数)
	{
		return 内部::万能<const T输入*, T输出>::转码(输入, 个数);
	}
	template<typename T>
	concept MATLAB类型 = requires{GetArrayType<T>::type; };
	//将C++指针转换为对应类型的MATLAB数组，必须指定各维长度
	template<MATLAB类型 T>
	inline TypedArray<T> 万能转码(const T* 输入, ArrayDimensions 各维长度)
	{
		size_t 个数 = 1;
		for (size_t 长度 : 各维长度)
			个数 *= 长度;
		return 数组工厂.createArrayFromBuffer(各维长度, 万能转码<buffer_ptr_t<T>>(输入, 个数));
	}
	//将多个C样式UTF8字符串转换为 UTF16 MATLAB 字符串数组，必须指定各维长度
	StringArray 万能转码(const char* const* 输入, ArrayDimensions 各维长度);
	//将多个UTF8字符串转换为 UTF16 MATLAB 字符串数组，必须指定各维长度
	StringArray 万能转码(const std::string* 输入, ArrayDimensions 各维长度);
	/*
	使用动态类型创建一个无类型缓冲，执行内存拷贝然后打包为 MATLAB Array
	泛型编程的一个常见情境，就是我们不关心用户提供的运行时数据类型，只是单纯进行内存拷贝。但是MATLAB官方提供的 MEX API 不允许无类型的内存拷贝，因此这里提供一个无类型缓冲区，可以接受一个运行时类型枚举，创建一个无类型缓冲，完成数据拷贝后再打包成无类型的 MATLAB Array。
	*/
	struct 无类型缓冲
	{
		void* const 指针;
		const uint64_t 字节数;
		//创建无类型缓冲需要提供一个运行时类型枚举和元素个数（不是字节数）。字节数等于元素数*类型尺寸[(int)类型]。
		static std::unique_ptr<无类型缓冲>创建(ArrayType 类型, uint64_t 元素数);
		virtual ~无类型缓冲() {}
		//打包后本对象变为不可用，所有数据封装在 MATLAB Array 中
		virtual Array 打包()noexcept = 0;
	protected:
		无类型缓冲(void* 指针, uint64_t 字节数) :指针(指针), 字节数(字节数) {}
	};
	/*
	从无类型的 MATLAB Array 拷贝数据
	泛型编程的一个常见情境，就是我们不关心用户提供的运行时数据类型，只是单纯进行内存拷贝。但是MATLAB官方提供的 MEX API 不允许无类型的内存拷贝。使用无类型数组可以将 MATLAB Array 包装起来，提供无类型内存拷贝的接口，你可以无视数据类型将任意MATLAB数组拷贝到指定内存。
	不同于无类型缓冲，这里不提供直接指向数组开头的内存指针。这是因为MATLAB数组的实现并不是简单的将数据连续排列在内存中，而有可能是按需动态生成的。
	*/
	struct 无类型数组
	{
		const uint64_t 字节数;
		virtual ~无类型数组() {}
		//从MATLAB数组取得无类型的数组
		static std::unique_ptr<无类型数组> 创建(Array&& 数组);
		//将数组中的内容无视数据类型直接拷贝到指针
		virtual void 拷贝(void* 输出)const = 0;

	protected:
		无类型数组(uint64_t 字节数) :字节数(字节数) {}
	};

	//增强功能，可以使用如下三个宏定义在一个MEX文件函数中定义多个API

#define API声明(函数名) void 函数名(ArgumentList& outputs,ArgumentList& inputs)
#define API索引 constexpr void (*(API[]))(ArgumentList&, ArgumentList&) =
#define API调用 const uint8_t 选项=万能转码<uint8_t>(std::move(inputs[0]));if(选项<std::extent_v<decltype(API)>)API[选项](outputs, inputs);else throw 不支持的API;
//增强功能，可以返回一个异常值并补齐额外的输出值
	void 异常输出补全(ArgumentList& outputs);
}