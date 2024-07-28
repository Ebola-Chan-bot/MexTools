#pragma once
namespace Mex工具
{
	template<>matlab::data::CharArray 万能转码<matlab::data::CharArray>(matlab::data::Array&& 输入);
	template<>matlab::data::MATLABString 万能转码<matlab::data::MATLABString>(matlab::data::Array&& 输入);
	template<>matlab::data::String 万能转码<matlab::data::String>(matlab::data::Array&& 输入);
	template<>std::string 万能转码<std::string>(matlab::data::Array&& 输入);
	template<>matlab::data::StringArray 万能转码<matlab::data::StringArray>(matlab::data::Array&& 输入);
	template<>std::wstring 万能转码<std::wstring>(matlab::data::Array&& 输入);
	template<typename T>
	[[noreturn]] void EnumThrow(T 异常)
	{
		std::ostringstream 异常信息流;
		内部::枚举转标识符(异常信息流, 异常);
		const std::string 异常信息 = 异常信息流.str();
		throw matlab::engine::MATLABException(异常信息, std::u16string(异常信息.cbegin(), 异常信息.cend()));
	}
	template<typename T>
	void EnumWarning(T 异常)
	{
		std::basic_ostringstream<char16_t> 异常信息流;
		内部::枚举转标识符(异常信息流, 异常);
		const matlab::data::String 异常信息 = 异常信息流.str();
		MATLAB引擎->feval("warning", 异常信息, 异常信息);
	}
	inline constexpr size_t 类型字节数(matlab::data::ArrayType 类型)
	{
		return 内部::类型字节数_s<std::make_integer_sequence<std::underlying_type_t<matlab::data::ArrayType>, 32>>::value[(size_t)类型];
	}
	inline size_t 数组字节数(const matlab::data::Array& 输入)
	{
		return 类型字节数(输入.getType()) * 输入.getNumberOfElements();
	}
	template<template<typename> typename 模板>
	auto 动态类型选择模板(matlab::data::ArrayType 类型)
	{
		using namespace matlab::data;
		switch (类型)
		{
		case ArrayType::LOGICAL:
			return 模板<bool>::value;
		case ArrayType::CHAR:
			return 模板<CHAR16_T>::value;
		case ArrayType::MATLAB_STRING:
			return 模板<MATLABString>::value;
		case ArrayType::DOUBLE:
			return 模板<double>::value;
		case ArrayType::SINGLE:
			return 模板<float>::value;
		case ArrayType::INT8:
			return 模板<int8_t>::value;
		case ArrayType::UINT8:
			return 模板<uint8_t>::value;
		case ArrayType::INT16:
			return 模板<int16_t>::value;
		case ArrayType::UINT16:
			return 模板<uint16_t>::value;
		case ArrayType::INT32:
			return 模板<int32_t>::value;
		case ArrayType::UINT32:
			return 模板<uint32_t>::value;
		case ArrayType::INT64:
			return 模板<int64_t>::value;
		case ArrayType::UINT64:
			return 模板<uint64_t>::value;
		case ArrayType::COMPLEX_DOUBLE:
			return 模板<std::complex<double>>::value;
		case ArrayType::COMPLEX_SINGLE:
			return 模板<std::complex<float>>::value;
		case ArrayType::COMPLEX_INT8:
			return 模板<std::complex<int8_t>>::value;
		case ArrayType::COMPLEX_UINT8:
			return 模板<std::complex<uint8_t>>::value;
		case ArrayType::COMPLEX_INT16:
			return 模板<std::complex<int16_t>>::value;
		case ArrayType::COMPLEX_UINT16:
			return 模板<std::complex<uint16_t>>::value;
		case ArrayType::COMPLEX_INT32:
			return 模板<std::complex<int32_t>>::value;
		case ArrayType::COMPLEX_UINT32:
			return 模板<std::complex<uint32_t>>::value;
		case ArrayType::COMPLEX_INT64:
			return 模板<std::complex<int64_t>>::value;
		case ArrayType::COMPLEX_UINT64:
			return 模板<std::complex<uint64_t>>::value;
		case ArrayType::CELL:
			return 模板<Array>::value;
		case ArrayType::STRUCT:
			return 模板<Struct>::value;
		case ArrayType::OBJECT:
		case ArrayType::VALUE_OBJECT:
		case ArrayType::HANDLE_OBJECT_REF:
			return 模板<Object>::value;
		case ArrayType::ENUM:
			return 模板<EnumArray>::value;
		case ArrayType::SPARSE_LOGICAL:
			return 模板<SparseArray<bool>>::value;
		case ArrayType::SPARSE_DOUBLE:
			return 模板<SparseArray<double>>::value;
		case ArrayType::SPARSE_COMPLEX_DOUBLE:
			return 模板<SparseArray<std::complex<double>>>::value;
		default:
			EnumThrow(MexTools::Unsupported_type);
		}
	}
	template<template<typename> typename 模板, typename...T>
	auto 动态类型选择模板(matlab::data::ArrayType 类型, T&&...函数参数)
	{
		using namespace matlab::data;
		switch (类型)
		{
		case ArrayType::LOGICAL:
			return 模板<bool>::value(std::forward<T>(函数参数)...);
		case ArrayType::CHAR:
			return 模板<CHAR16_T>::value(std::forward<T>(函数参数)...);
		case ArrayType::MATLAB_STRING:
			return 模板<MATLABString>::value(std::forward<T>(函数参数)...);
		case ArrayType::DOUBLE:
			return 模板<double>::value(std::forward<T>(函数参数)...);
		case ArrayType::SINGLE:
			return 模板<float>::value(std::forward<T>(函数参数)...);
		case ArrayType::INT8:
			return 模板<int8_t>::value(std::forward<T>(函数参数)...);
		case ArrayType::UINT8:
			return 模板<uint8_t>::value(std::forward<T>(函数参数)...);
		case ArrayType::INT16:
			return 模板<int16_t>::value(std::forward<T>(函数参数)...);
		case ArrayType::UINT16:
			return 模板<uint16_t>::value(std::forward<T>(函数参数)...);
		case ArrayType::INT32:
			return 模板<int32_t>::value(std::forward<T>(函数参数)...);
		case ArrayType::UINT32:
			return 模板<uint32_t>::value(std::forward<T>(函数参数)...);
		case ArrayType::INT64:
			return 模板<int64_t>::value(std::forward<T>(函数参数)...);
		case ArrayType::UINT64:
			return 模板<uint64_t>::value(std::forward<T>(函数参数)...);
		case ArrayType::COMPLEX_DOUBLE:
			return 模板<std::complex<double>>::value(std::forward<T>(函数参数)...);
		case ArrayType::COMPLEX_SINGLE:
			return 模板<std::complex<float>>::value(std::forward<T>(函数参数)...);
		case ArrayType::COMPLEX_INT8:
			return 模板<std::complex<int8_t>>::value(std::forward<T>(函数参数)...);
		case ArrayType::COMPLEX_UINT8:
			return 模板<std::complex<uint8_t>>::value(std::forward<T>(函数参数)...);
		case ArrayType::COMPLEX_INT16:
			return 模板<std::complex<int16_t>>::value(std::forward<T>(函数参数)...);
		case ArrayType::COMPLEX_UINT16:
			return 模板<std::complex<uint16_t>>::value(std::forward<T>(函数参数)...);
		case ArrayType::COMPLEX_INT32:
			return 模板<std::complex<int32_t>>::value(std::forward<T>(函数参数)...);
		case ArrayType::COMPLEX_UINT32:
			return 模板<std::complex<uint32_t>>::value(std::forward<T>(函数参数)...);
		case ArrayType::COMPLEX_INT64:
			return 模板<std::complex<int64_t>>::value(std::forward<T>(函数参数)...);
		case ArrayType::COMPLEX_UINT64:
			return 模板<std::complex<uint64_t>>::value(std::forward<T>(函数参数)...);
		case ArrayType::CELL:
			return 模板<Array>::value(std::forward<T>(函数参数)...);
		case ArrayType::STRUCT:
			return 模板<Struct>::value(std::forward<T>(函数参数)...);
		case ArrayType::OBJECT:
		case ArrayType::VALUE_OBJECT:
		case ArrayType::HANDLE_OBJECT_REF:
			return 模板<Object>::value(std::forward<T>(函数参数)...);
		case ArrayType::ENUM:
			return 模板<Enumeration>::value(std::forward<T>(函数参数)...);
		case ArrayType::SPARSE_LOGICAL:
			return 模板<SparseArray<bool>>::value(std::forward<T>(函数参数)...);
		case ArrayType::SPARSE_DOUBLE:
			return 模板<SparseArray<double>>::value(std::forward<T>(函数参数)...);
		case ArrayType::SPARSE_COMPLEX_DOUBLE:
			return 模板<SparseArray<std::complex<double>>>::value(std::forward<T>(函数参数)...);
		default:
			EnumThrow(MexTools::Unsupported_type);
		}
	}
	template<typename 输出>
	inline 输出 万能转码(matlab::data::Array&& 输入)
	{
		return matlab::data::apply_visitor(std::move(输入), 内部::标量转换<输出>());
	}
	template<typename 输出, typename T>
	inline 输出 万能转码(T&& 输入)
	{
		return 内部::标量转换<输出>::转换(std::forward<T>(输入));
	}
	template<typename T>
		requires requires(T&& 输入) { 数组工厂.createScalar<内部::数值标准化_t<T>>(std::move(输入)); }
	inline matlab::data::TypedArray<内部::数值标准化_t<T>>万能转码(T&& 输入)
	{
		return 数组工厂.createScalar<内部::数值标准化_t<T>>(std::move(输入));
	}
	inline matlab::data::TypedArray<size_t>万能转码(const void* 输入)
	{
		return 数组工厂.createScalar<size_t>((size_t)输入);
	}
	template<typename 迭代器>
	inline void 万能转码(matlab::data::Array&& 输入, 迭代器&& 输出)
	{
		matlab::data::apply_visitor(std::move(输入), 内部::迭代MC<迭代器&&>(std::forward<迭代器>(输出)));
	}
	template<typename 输出类型, typename 迭代器>
	inline 输出类型 万能转码(迭代器&& 输入, matlab::data::ArrayDimensions&& 各维尺寸)
	{
		return 内部::迭代CM<数组类型转元素<输出类型>>::转换(std::forward<迭代器>(输入), std::move(各维尺寸));
	}
	template<typename 迭代器>
	inline matlab::data::TypedArray<内部::取迭代器值类型<迭代器>>万能转码(迭代器&& 输入, matlab::data::ArrayDimensions&& 各维尺寸)
	{
		return 内部::迭代CM<内部::取迭代器值类型<迭代器>>::转换(std::forward<迭代器>(输入), std::move(各维尺寸));
	}
	template<typename 迭代器>
		requires std::is_pointer_v<内部::取迭代器值类型<迭代器>>
	inline matlab::data::TypedArray<size_t>万能转码(迭代器&& 输入, matlab::data::ArrayDimensions&& 各维尺寸)
	{
		return 内部::迭代CM<size_t>::转换(std::forward<迭代器>(输入), std::move(各维尺寸));
	}
	template<typename 迭代器>
	inline matlab::data::Array 万能转码(matlab::data::ArrayType 元素类型, 迭代器&& 输入, matlab::data::ArrayDimensions&& 各维尺寸)
	{
		return 动态类型选择模板<内部::动态CM>(元素类型, std::forward<迭代器>(输入), std::move(各维尺寸));
	}
	template<typename 输出类型>
	inline 输出类型 万能转码(数组类型转元素<输出类型>* 输入, matlab::data::ArrayDimensions&& 各维尺寸, matlab::data::buffer_deleter_t 自定义删除器)
	{
		return 数组工厂.createArrayFromBuffer(std::move(各维尺寸), matlab::data::buffer_ptr_t<数组类型转元素<输出类型>>(输入, 自定义删除器));
	}
	inline matlab::data::Array 万能转码(matlab::data::ArrayType 元素类型, void* 输入, matlab::data::ArrayDimensions&& 各维尺寸, matlab::data::buffer_deleter_t 自定义删除器)
	{
		return 动态类型选择模板<内部::指针转动态数组>(元素类型, std::move(各维尺寸), 输入, 自定义删除器);
	}
	template<typename T>
	inline void 自动析构(T* 对象指针)noexcept
	{
		自动析构(对象指针, [](void* 对象指针) {delete (T*)对象指针; });
	}
	template<bool 将标识符添加到消息, typename 标识符类型, typename...消息类型>
	[[noreturn]] void EnumThrow(标识符类型 标识符, 消息类型...消息)
	{
		std::ostringstream 标识符流;
		内部::枚举转标识符(标识符流, 标识符);
		if constexpr (将标识符添加到消息 && sizeof...(消息类型))
		{
			const std::string 标识符文本 = 标识符流.str();
			std::basic_ostringstream<char16_t> 消息流(万能转码<matlab::data::String>(标识符文本), std::ios_base::ate);
			((消息流 << u"：") << ... << 消息);
			throw matlab::engine::MATLABException(标识符文本, 消息流.str());
		}
		else
		{
			std::basic_ostringstream<char16_t>消息流;
			(消息流 << ... << 消息);
			throw matlab::engine::MATLABException(标识符流.str(), 消息流.str());
		}
	}
	template<bool 将标识符添加到消息, typename 标识符类型, 内部::可写入UTF8流...消息类型>
	[[noreturn]] void EnumThrow(标识符类型 标识符, 消息类型...消息)
	{
		std::ostringstream 标识符流;
		内部::枚举转标识符(标识符流, 标识符);
		if constexpr (将标识符添加到消息 && sizeof...(消息类型))
		{
			const std::string 标识符文本 = 标识符流.str();
			((标识符流 << "：") << ... << 消息);//折叠表达式要求括号
			throw matlab::engine::MATLABException(标识符文本, 万能转码<matlab::data::String>(标识符流.str()));
		}
		else
		{
			std::ostringstream 消息流;
			(消息流 << ... << 消息);
			throw matlab::engine::MATLABException(标识符流.str(), 万能转码<matlab::data::String>(消息流.str()));
		}
	}
	template<bool 将标识符添加到消息, typename 标识符类型, typename...消息类型>
	void EnumWarning(标识符类型 标识符, 消息类型...消息)
	{
		if constexpr (将标识符添加到消息 && sizeof...(消息类型))
		{
			std::basic_ostringstream<char16_t> 标识符流;
			内部::枚举转标识符(标识符流, 标识符);
			const matlab::data::String 标识符文本 = 标识符流.str();
			((标识符流 << u"：") << ... << 消息);//折叠表达式要求括号
			MATLAB引擎->feval("warning", 标识符文本, 标识符流.str());
		}
		else
		{
			std::ostringstream 标识符流;
			内部::枚举转标识符(标识符流, 标识符);
			std::basic_ostringstream<char16_t> 消息流;
			(消息流 << ... << 消息);
			MATLAB引擎->feval("warning", 标识符流.str(), 消息流.str());
		}
	}
	template<bool 将标识符添加到消息, typename 标识符类型, 内部::可写入UTF8流...消息类型>
	void EnumWarning(标识符类型 标识符, 消息类型...消息)
	{
		std::ostringstream 标识符流;
		内部::枚举转标识符(标识符流, 标识符);
		if constexpr (将标识符添加到消息 && sizeof...(消息类型))
		{
			const std::string 标识符文本 = 标识符流.str();
			((标识符流 << "：") << ... << 消息);//折叠表达式要求括号
			MATLAB引擎->feval("warning", 标识符文本, 标识符流.str());
		}
		else
		{
			std::ostringstream 消息流;
			(消息流 << ... << 消息);
			MATLAB引擎->feval("warning", 标识符流.str(), 消息流.str());
		}
	}
	template<bool 将标识符添加到消息, typename 标识符类型, typename...消息类型>
	[[noreturn]] inline void ThrowLastError(标识符类型 identifier, 消息类型...消息)
	{
		EnumThrow<将标识符添加到消息>(identifier, WindowsErrorMessage().get(), 消息...);
	}
}