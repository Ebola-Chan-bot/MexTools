export module Mex工具:实现;
import "MexFunction.hpp";
import std;
namespace Mex工具
{
	using namespace matlab::data;
	template<ArrayType T>struct 动态类型转静态_s { using type = void; };
	template<>struct 动态类型转静态_s<ArrayType::LOGICAL> { using type = bool; };
	template<>struct 动态类型转静态_s<ArrayType::CHAR> { using type = CHAR16_T; };
	template<>struct 动态类型转静态_s<ArrayType::MATLAB_STRING> { using type = MATLABString; };
	template<>struct 动态类型转静态_s<ArrayType::DOUBLE> { using type = double; };
	template<>struct 动态类型转静态_s<ArrayType::SINGLE> { using type = float; };
	template<>struct 动态类型转静态_s<ArrayType::INT8> { using type = int8_t; };
	template<>struct 动态类型转静态_s<ArrayType::INT16> { using type = int16_t; };
	template<>struct 动态类型转静态_s<ArrayType::INT32> { using type = int32_t; };
	template<>struct 动态类型转静态_s<ArrayType::INT64> { using type = int64_t; };
	template<>struct 动态类型转静态_s<ArrayType::UINT8> { using type = uint8_t; };
	template<>struct 动态类型转静态_s<ArrayType::UINT16> { using type = uint16_t; };
	template<>struct 动态类型转静态_s<ArrayType::UINT32> { using type = uint32_t; };
	template<>struct 动态类型转静态_s<ArrayType::UINT64> { using type = uint64_t; };
	template<>struct 动态类型转静态_s<ArrayType::COMPLEX_DOUBLE> { using type = std::complex<double>; };
	template<>struct 动态类型转静态_s<ArrayType::COMPLEX_SINGLE> { using type = std::complex<float>; };
	template<>struct 动态类型转静态_s<ArrayType::COMPLEX_INT8> { using type = std::complex<int8_t>; };
	template<>struct 动态类型转静态_s<ArrayType::COMPLEX_INT16> { using type = std::complex<int16_t>; };
	template<>struct 动态类型转静态_s<ArrayType::COMPLEX_INT32> { using type = std::complex<int32_t>; };
	template<>struct 动态类型转静态_s<ArrayType::COMPLEX_INT64> { using type = std::complex<int64_t>; };
	template<>struct 动态类型转静态_s<ArrayType::COMPLEX_UINT8> { using type = std::complex<uint8_t>; };
	template<>struct 动态类型转静态_s<ArrayType::COMPLEX_UINT16> { using type = std::complex<uint16_t>; };
	template<>struct 动态类型转静态_s<ArrayType::COMPLEX_UINT32> { using type = std::complex<uint32_t>; };
	template<>struct 动态类型转静态_s<ArrayType::COMPLEX_UINT64> { using type = std::complex<uint64_t>; };
	template<>struct 动态类型转静态_s<ArrayType::CELL> { using type = Array; };
	template<>struct 动态类型转静态_s<ArrayType::STRUCT> { using type = Struct; };
	template<>struct 动态类型转静态_s<ArrayType::ENUM> { using type = Enumeration; };
	template<>struct 动态类型转静态_s<ArrayType::OBJECT> { using type = Object; };
	template<>struct 动态类型转静态_s<ArrayType::SPARSE_LOGICAL> { using type = bool; };
	template<>struct 动态类型转静态_s<ArrayType::SPARSE_DOUBLE> { using type = double; };
	template<>struct 动态类型转静态_s<ArrayType::SPARSE_COMPLEX_DOUBLE> { using type = std::complex<double>; };
	template<typename T>
	constexpr uint8_t 类型字节数_v = sizeof(T);
	template<>
	constexpr uint8_t 类型字节数_v<void> = 0;
	template<typename T>
	struct 类型字节数_s;
	template<std::underlying_type_t<ArrayType>...T>
	struct 类型字节数_s<std::integer_sequence<std::underlying_type_t<ArrayType>, T...>>
	{
		static constexpr uint8_t value[] = { 类型字节数_v<typename 动态类型转静态_s<(ArrayType)T>::type>... };
	};
	
	export extern std::map<void*, std::move_only_function<void(void*)const>>自动析构表;

	//万能转码实现
	template<typename 输出>
	struct 标量转换
	{
		template<typename T>
		输出 operator()(const TypedArray<T>&输入)
		{
			return (输出)输入[0].operator T();
		}
		template<std::convertible_to<输出> T>
		输出 operator()(const TypedArray<T>&输入)
		{
			return 输入[0].operator T();
		}
		template<typename T>
		输出 operator()(const SparseArray<T>&输入)
		{
			return (输出)输入[0].operator T();
		}
		template<std::convertible_to<输出> T>
		输出 operator()(const SparseArray<T>& 输入)
		{
			return (输出)输入[0].operator T();
		}
	}; 
	extern ArrayFactory 数组工厂;
	extern std::shared_ptr<matlab::engine::MATLABEngine> MATLAB引擎; 
	template<typename 输出>
	输出 万能转码(Array&& 输入);
	template<>
	CharArray 万能转码<CharArray>(Array&& 输入)
	{
		switch (输入.getType())
		{
		case ArrayType::CHAR:
			return std::move(输入);
		case ArrayType::MATLAB_STRING:
			return 数组工厂.createCharArray(((const Array&)输入)[0].operator String());
		case ArrayType::CELL:
			return ((const Array&)输入)[0];
		default:
			return 数组工厂.createCharArray(MATLAB引擎->feval("string", std::move(输入))[0].operator String());
		}
	}
	template<>
	MATLABString Mex工具::万能转码<MATLABString>(Array&& 输入)
	{
		switch (输入.getType())
		{
		case ArrayType::CHAR:
			return CharArray(std::move(输入)).toUTF16();
		case ArrayType::MATLAB_STRING:
			return ((const Array&)输入)[0].operator String();
		case ArrayType::CELL:
			return ((const Array&)输入)[0].operator CharArray().toUTF16();
		default:
			return MATLAB引擎->feval("string", std::move(输入))[0].operator String();
		}
	}
	template<>
	String Mex工具::万能转码<String>(Array&& 输入)
	{
		switch (输入.getType())
		{
		case ArrayType::CHAR:
			return CharArray(std::move(输入)).toUTF16();
		case ArrayType::MATLAB_STRING:
			return ((const Array&)输入)[0];
		case ArrayType::CELL:
			return ((const Array&)输入)[0].operator CharArray().toUTF16();
		default:
			return MATLAB引擎->feval("string", std::move(输入))[0];
		}
	}
	template<>
	std::string Mex工具::万能转码<std::string>(Array&& 输入)
	{
		std::string 输出;
		switch (输入.getType())
		{
		case ArrayType::CHAR:
		{
			const int 长度 = 输入.getNumberOfElements();
			const buffer_ptr_t<char16_t>缓冲 = CharArray(std::move(输入)).release();
			输出.resize_and_overwrite((长度 + 1) * 3, [&缓冲, 长度](char* 指针, size_t 尺寸)
				{
					return WideCharToMultiByte(CP_UTF8, 0, (wchar_t*)缓冲.get(), 长度, 指针, 尺寸, nullptr, nullptr) - 1;
				});
		}
		break;
		case ArrayType::MATLAB_STRING:
		{
			const String 字符串 = ((const Array&)输入)[0];
			输出.resize_and_overwrite((字符串.size() + 1) * 3, [&字符串](char* 指针, size_t 尺寸)
				{
					return WideCharToMultiByte(CP_UTF8, 0, (wchar_t*)字符串.c_str(), -1, 指针, 尺寸, nullptr, nullptr) - 1;
				});
		}
		break;
		case ArrayType::CELL:
		{
			CharArray 字符数组 = ((const Array&)输入)[0];
			const int 长度 = 字符数组.getNumberOfElements();
			const buffer_ptr_t<char16_t>缓冲 = 字符数组.release();
			输出.resize_and_overwrite((长度 + 1) * 3, [&缓冲, 长度](char* 指针, size_t 尺寸)
				{
					return WideCharToMultiByte(CP_UTF8, 0, (wchar_t*)缓冲.get(), 长度, 指针, 尺寸, nullptr, nullptr) - 1;
				});
		}
		break;
		default:
			throw Mex异常::Array_cannot_convert_to_std_string;
		}
		return 输出;
	}
}