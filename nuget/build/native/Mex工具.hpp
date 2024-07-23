#pragma once
#include <magic_enum.hpp>
#include <mex.hpp>
//Mex工具全局异常枚举
enum class MexTools
{
	No_exceptions,
	Type_conversion_failed,
	Memory_copy_failed,
	Enumerations_uncopyable,
	Unsupported_type,
	Unsupported_API,
	Array_cannot_convert_to_CharArray,
	Array_cannot_convert_to_String,
	Array_cannot_convert_to_std_string,
	Array_cannot_convert_to_StringArray,
	Array_cannot_convert_to_MATLABString,
	Array_cannot_copy_to_char,
	Array_cannot_copy_to_wchar_t,
	Cannot_get_pointer_of_SparseArray,
	Unexpected_SEH_exception,
	Unexpected_CPP_exception
};
namespace Mex工具
{
	//异常处理

/*将任意枚举类型当作异常抛给MATLAB。枚举类型名和字面文本将同时作为MException的identifier和message，因此只能使用英文、数字和下划线。
MATLAB只能正确捕获std::exception及其派生类。此方法将枚举类型的异常转换为matlab::engine::MATLABException抛出，符合MATLAB捕获要求。
用户只应对std::exception及其派生类直接使用throw。对于其它异常类型，应使用此方法或任何其它方法将异常类型转换为std::exception及其派生类，或者自行catch并处理。如果违反这个规则，异常信息将会丢失，MATLAB只能接收到`Mex异常::Unexpected_CPP_exception`。
和throw一样，此方法将违反noexcept约定。异常不能从noexcept方法中向外传递，而是导致MATLAB进程崩溃。
*/
	template<typename T>
	[[noreturn]] void EnumThrow(T 异常)
	{
		std::ostringstream 异常信息流;
		for (const char* 字符指针 = typeid(T).name(); const char 字符 = *字符指针; 字符指针 += 字符 == ':' ? 2 : 1)
			异常信息流 << 字符;
		异常信息流 << ':' << magic_enum::enum_name(异常);
		const std::string 异常信息 = 异常信息流.str();
		throw matlab::engine::MATLABException(异常信息, std::u16string(异常信息.cbegin(), 异常信息.cend()));
	}
	/*将任意枚举类型当作异常抛给MATLAB。枚举类型名和字面文本将作为MException的identifier，因此只能使用英文、数字和下划线。消息必须是UTF16字符串，可以包含任意字符。
	MATLAB只能正确捕获std::exception及其派生类。此方法将枚举类型的异常转换为matlab::engine::MATLABException抛出，符合MATLAB捕获要求。
	用户只应对std::exception及其派生类直接使用throw。对于其它异常类型，应使用此方法或任何其它方法将异常类型转换为std::exception及其派生类，或者自行catch并处理。如果违反这个规则，异常信息将会丢失，MATLAB只能接收到`Mex异常::Unexpected_CPP_exception`。
	和throw一样，此方法将违反noexcept约定。异常不能从noexcept方法中向外传递，而是导致MATLAB进程崩溃。
	可选将标识符添加到消息。标识符会添加到消息开头。
	*/
	template<bool 将标识符添加到消息 = true,typename T>
	[[noreturn]] void EnumThrow(T 标识符, const std::u16string& 消息)
	{
		std::ostringstream 标识符流;
		for (const char* 字符指针 = typeid(T).name(); const char 字符 = *字符指针; 字符指针 += 字符 == ':' ? 2 : 1)
			标识符流 << 字符;
		标识符流 << ':' << magic_enum::enum_name(标识符);
		if constexpr (将标识符添加到消息)
		{
			std::u16string 合并消息;
			const std::string 标识符文本 = 标识符流.str();
			合并消息.resize_and_overwrite(标识符文本.size() + 消息.size() + 1, [&标识符文本, &消息](char16_t* 合并消息指针, size_t 尺寸)
				{
					char16_t* 指针 = std::copy(标识符文本.begin(), 标识符文本.end(), 合并消息指针);
					*指针++ = u'：';
					return std::copy(消息.begin(), 消息.end(), 指针) - 合并消息指针;
				});
			throw matlab::engine::MATLABException(标识符文本, 合并消息);
		}
		else
			throw matlab::engine::MATLABException(标识符流.str(), 消息);
	}
	/*将任意枚举类型当作异常抛给MATLAB。枚举类型名和字面文本将作为MException的identifier，因此只能使用英文、数字和下划线。消息必须是C样式0结尾UTF8字符指针，可以包含任意字符。
	MATLAB只能正确捕获std::exception及其派生类。此方法将枚举类型的异常转换为matlab::engine::MATLABException抛出，符合MATLAB捕获要求。
	用户只应对std::exception及其派生类直接使用throw。对于其它异常类型，应使用此方法或任何其它方法将异常类型转换为std::exception及其派生类，或者自行catch并处理。如果违反这个规则，异常信息将会丢失，MATLAB只能接收到`Mex异常::Unexpected_CPP_exception`。
	和throw一样，此方法将违反noexcept约定。异常不能从noexcept方法中向外传递，而是导致MATLAB进程崩溃。
	可选将标识符添加到消息。标识符会添加到消息开头。
	*/
	template<bool 将标识符添加到消息 = true, typename T>
	[[noreturn]] void EnumThrow(T 标识符, const char* 消息)
	{
		std::ostringstream 标识符流;
		for (const char* 字符指针 = typeid(T).name(); const char 字符 = *字符指针; 字符指针 += 字符 == ':' ? 2 : 1)
			标识符流 << 字符;
		标识符流 << ':' << magic_enum::enum_name(标识符);
		if constexpr (将标识符添加到消息)
		{
			const std::string 标识符文本 = 标识符流.str();
			标识符流 << "：" << 消息;
			throw matlab::engine::MATLABException(标识符文本, 万能转码<String>(标识符流.str());
		}
		else
			throw matlab::engine::MATLABException(标识符流.str(), 万能转码<String>(消息));
	}
	//检查GetLastError()，如果有错误则抛出MATLAB异常。如果没有错误则不执行任何操作。可选使用指定的 MException identifier
	void CheckLastError(const std::string& identifier = "MexTools:Win32Exception");
	//检查GetLastError()，如果有错误则抛出MATLAB异常。如果没有错误则不执行任何操作。可选使用枚举值作为 MException identifier，
	template<typename T>
	inline void CheckLastError(T identifier)
	{
		CheckLastError(magic_enum::enum_name(identifier));
	}
}
#include "Mex工具.前置.hpp"
namespace Mex工具
{
	//公开的命名空间不能 using namespace，会导致用户 using 本命名空间以后级联 using

	//标准MEX函数签名。你可以定义多个具有如此签名的函数，然后由执行函数负责分发。
#define Mex工具API(API名称) void API名称(matlab::mex::ArgumentList& 输出, matlab::mex::ArgumentList& 输入)

	//用户必须定义以下函数

	/*全局初始化，在MATLAB首次载入MEX时调用。用户应当在此函数中进行全局变量初始化、持久资源分配等不应在每次调用时重复进行的操作。全局变量也可以在本函数外初始化，但这样做不能保证初始化顺序，仅适用于不依赖其它全局变量的情况。在此方法中进行具有严格顺序依赖要求的全局变量初始化。
	初始化约定为noexcept。不应在初始化阶段抛出任何异常。
	*/
	void 初始化()noexcept;
	//执行调用，MATLAB每次调用MEX文件函数时调用此方法。用户应当在此函数中处理输入参数，充分利用初始化阶段分配的持久资源，然后将结果写入输出参数
	Mex工具API(执行);
	/*全局清理，在MATLAB卸载MEX（包括 clear mex 以及MATLAB会话退出）时调用。用户应当在此函数中释放全局变量、持久资源等
	清理约定为noexcept。不应在清理阶段抛出任何异常。
	*/
	void 清理()noexcept;
	//标准MEX函数指针类型，便于构建跳转表，将参数分发到单个MEX文件中的多个函数。例如，可以在执行函数中构建一个该类型的数组，然后根据输入参数选择调用哪个函数。
	using API = decltype(执行)*;

	//类型相关模板

	//此变量模板将静态的数组元素类型转换为matlab::data::ArrayType枚举值
	template<typename T>
	constexpr matlab::data::ArrayType 静态类型转动态 = matlab::data::GetArrayType<T>::type;
	//此别名模板将matlab::data::ArrayType枚举值转换为静态的数组元素类型
	template<matlab::data::ArrayType T>
	using 动态类型转静态 = typename 内部::动态类型转静态_s<T>::type;
	//此别名模板将TypedArray或SparseArray类型转换为其元素类型
	template<typename T>
	using 数组类型转元素 = typename 内部::数组类型转元素_s<T>::type;
	//将matlab::data::ArrayType枚举值转换为对应的C++类型的字节数
	inline constexpr size_t 类型字节数(matlab::data::ArrayType 类型)
	{
		return 内部::类型字节数_s<std::make_integer_sequence<std::underlying_type_t<matlab::data::ArrayType>, 32>>::value[(size_t)类型];
	}
	//获取任意类型MATLAB数组的字节数，即元素个数×单个元素的字节数。稀疏数组的字节数包含其0元素所占字节。
	inline size_t 数组字节数(const matlab::data::Array& 输入)
	{
		return 类型字节数(输入.getType()) * 输入.getNumberOfElements();
	}
	/*根据动态类型选择类模板，返回其名为value的静态成员变量。
	此函数根据输入的动态类型，将对应的静态类型作为输入类模板的参数。然后，返回模板实例化类型的 static value 成员变量。但是，该成员变量的类型必须对所有可能的动态类型保持相同，不允许类型转换。
	输入类模板通常应当提供一个泛化实现，对不支持的动态类型采取默认行为（如报错等），除非你的类模板支持所有MATLAB类型（即本函数体中列出的所有case）。然后对支持的类型提供特化或偏特化实现。不需要考虑ArrayType枚举项未列出的情况：本函数会自动处理并报错。
	例如，一种可能的计算动态类型的数组字节数的实现（尽管本库并非如此实现）：
	```
	template<typename T>struct 类型字节数{static constexpr size_t value = sizeof(T);};
	template<typename T>struct 类型字节数<SparseArray<T>>{static constexpr size_t value = sizeof(T);};
	inline size_t 数组字节数(const matlab::data::Array& 输入){return 动态类型选择模板<类型字节数>(输入.getType());}
	```
	*/
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
	/*根据动态类型选择类模板，返回其名为value的静态成员函数。
	此函数根据输入的动态类型，将对应的静态类型作为输入类模板的参数。然后，返回模板实例化类型的 static value 成员函数。但是，该函数的签名必须对所有可能的动态类型保持相同。然后，将输入给本函数的后续参数完美转发给那个函数。
	*/
	template<template<typename> typename 模板,typename...T>
	auto 动态类型选择模板(matlab::data::ArrayType 类型,T&&...函数参数)
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

	//有用的全局变量。这些变量会在用户定义的初始化之前被自动初始化，但在进入用户初始化之前这些变量可能尚未初始化，因此用户不应依赖这些变量进行全局变量初始化。在进入用户初始化以后可以使用，但通常不应该修改这些变量。

	extern matlab::data::ArrayFactory 数组工厂;
	extern std::shared_ptr<matlab::engine::MATLABEngine> MATLAB引擎;

	//万能转码

	/*一对一转换。支持以下所有转换：
	将输入MATLAB类型视为标量，转换为C++类型。如果类型不完全匹配，将优先执行隐式转换；如果不能隐式转换，再尝试显式转换；如果还不行，还会尝试调用MATLAB引擎转换。
	将所有能被转换为string的MATLAB类型视为标量，转换为CharArray、MATLABString、String（i.e. std::u16string）、StringArray、std::string或std::wstring。自动执行必要的UTF16到UTF8转换。
	输入右值引用，这意味着转换后输入数组可能不再可用。
	*/
	template<typename 输出>
	inline 输出 万能转码(matlab::data::Array&& 输入)
	{
		return matlab::data::apply_visitor(std::move(输入), 内部::标量转换<输出>());
	}
	/*一对一转换。支持以下所有转换：
	将输入C++类型转换为MATLAB标量。如果类型不完全匹配，将优先执行隐式转换；如果不能隐式转换，再尝试显式转换；如果还不行，还会尝试调用MATLAB引擎转换。
	将所有能被MATLAB转换为StringArray的Array类型视为标量，或所有被std::ostringstream::operator<<或std::wostringstream::operator<<支持的类型，转换为CharArray、MATLABString、String（i.e. std::u16string）或StringArray。自动执行必要的UTF8到UTF16转换。
	最好使用std::move输入右值引用。某些转换支持常量引用，某些不支持，可以根据是否能通过编译来判定。
	*/
	template<typename 输出, typename T>
	inline 输出 万能转码(T&& 输入)
	{
		return 内部::标量转换<输出>::转换(std::forward<T>(输入));
	}
	//将MATLAB标量元素类型转换为标量数组类型
	template<typename T>
		requires requires(T&& 输入) { 数组工厂.createScalar<内部::数值标准化_t<T>>(std::move(输入)); }
	inline matlab::data::TypedArray<内部::数值标准化_t<T>>万能转码(T&& 输入)
	{
		return 数组工厂.createScalar<内部::数值标准化_t<T>>(std::move(输入));
	}
	//将C++指针转换为 MATLAB uint64 标量
	inline matlab::data::TypedArray<size_t>万能转码(const void* 输入)
	{
		return 数组工厂.createScalar<size_t>((size_t)输入);
	}
	/*将MATLAB数组拷出到迭代器。如果类型不匹配，将优先执行隐式转换；如果不能隐式转换，再尝试显式转换；如果还不行，还会尝试调用MATLAB引擎转换。
	特别地，所有能被转换为string的MATLAB类型数组可以被拷出到接受CharArray、MATLABString、String（i.e.std::u16string）、std::string或std::wstring的迭代器。自动执行必要的UTF16到UTF8转换。
	特别地，如果迭代器是void*，将被强制转换为指向数组值类型的指针使用。
	特别地，SparseArray将被填充为满数组。
	输入matlab::data::Array右值引用，这意味着转换后输入对象可能不再可用。
	函数执行后，迭代器将指向拷出的最后一个元素的下一个位置。如果输入稀疏数组，迭代器必须支持使用operator[]的随机写入方式。
	*/
	template<typename 迭代器>
	inline void 万能转码(matlab::data::Array&& 输入, 迭代器& 输出)
	{
		matlab::data::apply_visitor(std::move(输入), 内部::迭代MC(输出));
	}
	/*从迭代器创建具有指定维度的MATLAB满数组。如果类型不匹配，将优先执行隐式转换；如果不能隐式转换，再尝试显式转换。
	特别地，如果输出类型是StringArray，迭代器对应的值类型可以是所有被std::ostringstream::operator<<或std::wostringstream::operator<<支持的类型；如果不支持，还可以是任何能被MATLAB转换为string的MATLAB元素对象（如枚举、分类数组或任何实现了string方法的对象等）
	特别地，如果迭代器是void*，将被强制转换为指向数组值类型的指针使用。
	输入matlab::data::ArrayDimensions右值引用，这意味着转换后输入对象可能不再可用。
	函数执行后，迭代器将指向最后一个元素的下一个位置。
	*/
	template<typename 输出类型, typename 迭代器>
	inline 输出类型 万能转码(迭代器& 输入, matlab::data::ArrayDimensions&& 各维尺寸)
	{
		return 内部::迭代CM<数组类型转元素<输出类型>>::value(输入, std::move(各维尺寸));
	}
	//从迭代器创建具有迭代器值类型的MATLAB数组。输入matlab::data::ArrayDimensions右值引用，这意味着转换后输入对象可能不再可用。函数执行后，迭代器将指向最后一个元素的下一个位置。
	template<typename 迭代器>
	inline matlab::data::TypedArray<内部::取迭代器值类型<迭代器>>万能转码(迭代器&输入, matlab::data::ArrayDimensions&& 各维尺寸)
	{
		return 内部::迭代CM<内部::取迭代器值类型<迭代器>>::value(输入, std::move(各维尺寸));
	}
	//从指针迭代器（指迭代器解引用得到指针，迭代器本身不一定是指针）创建具有uint64类型的MATLAB数组。输入matlab::data::ArrayDimensions右值引用，这意味着转换后输入对象可能不再可用。函数执行后，迭代器将指向最后一个元素的下一个位置。
	template<typename 迭代器>
	requires std::is_pointer_v<内部::取迭代器值类型<迭代器>>
	inline matlab::data::TypedArray<size_t>万能转码(迭代器& 输入, matlab::data::ArrayDimensions&& 各维尺寸)
	{
		return 内部::迭代CM<size_t>::value(输入, std::move(各维尺寸));
	}
	/*从迭代器创建具有动态类型的MATLAB数组。如果类型不匹配，将优先执行隐式转换；如果不能隐式转换，再尝试显式转换。
	特别地，如果输出类型是ArrayType::MATLAB_STRING，迭代器对应的值类型可以是所有被std::ostringstream::operator<<或std::wostringstream::operator<<支持的类型；如果不支持，还可以是任何能被MATLAB转换为string的MATLAB元素对象（如枚举、分类数组或任何实现了string方法的对象等）
	特别地，如果迭代器是void*，将被强制转换为指向数组值类型的指针使用。
	输入matlab::data::ArrayDimensions右值引用，这意味着转换后输入对象可能不再可用。
	函数执行后，迭代器将指向最后一个元素的下一个位置。
	*/
	template<typename 迭代器>
	inline matlab::data::Array 万能转码(matlab::data::ArrayType 元素类型, 迭代器& 输入, matlab::data::ArrayDimensions&& 各维尺寸)
	{
		return 动态类型选择模板<内部::动态CM>(元素类型, 输入, std::move(各维尺寸));
	}
	//将给定指针直接作为指定MATLAB满数组的基础数据缓冲区。数据类型必须完全匹配。必须额外指定删除器。从R2024b开始支持，之前的版本不支持。
	template<typename 输出类型>
	inline 输出类型 万能转码(数组类型转元素<输出类型>* 输入, matlab::data::ArrayDimensions&& 各维尺寸, matlab::data::buffer_deleter_t 自定义删除器)
	{
		return 数组工厂.createArrayFromBuffer(std::move(各维尺寸), matlab::data::buffer_ptr_t<数组类型转元素<输出类型>>(输入, 自定义删除器));
	}
	//将给定指针直接作为动态类型MATLAB满数组的基础数据缓冲区。数据类型必须完全匹配。必须额外指定删除器。从R2024b开始支持，之前的版本不支持。
	inline matlab::data::Array 万能转码(matlab::data::ArrayType 元素类型, void* 输入, matlab::data::ArrayDimensions&& 各维尺寸, matlab::data::buffer_deleter_t 自定义删除器)
	{
		return 动态类型选择模板<内部::指针转动态数组>(元素类型, std::move(各维尺寸), 输入, 自定义删除器);
	}

	//自动析构

	//将对象指针加入自动析构表。clear mex 时此对象将被自动析构。使用指定的删除器。
	void 自动析构(void* 对象指针, std::move_only_function<void(void*)const>&& 删除器)noexcept;
	//将对象指针加入自动析构表。clear mex 时此指针将被自动delete。只能对new创建的对象指针使用此方法。
	template<typename T>
	inline void 自动析构(T* 对象指针)noexcept
	{
		自动析构(对象指针, [](void* 对象指针) {delete (T*)对象指针; });
	}
	//此方法用于提示指定对象已被自动析构，从而避免自动析构表重复析构。此方法不负责析构对象本身，对象本身仍由调用方负责析构。返回对象指针是否曾经存在于自动析构表中。
	bool 手动析构(void* 对象指针)noexcept;
	//检查对象指针是否存在于自动析构表中。如不存在，此指针可能是无效的，或者创建时未加入自动析构表。
	bool 对象存在(void* 对象指针)noexcept;

	//动态类型缓冲，可用于创建动态类型MATLAB数组。使用静态方法`创建`获取指针。创建后，向`get()`返回的指针写入数据，然后调用`创建数组`以创建MATLAB数组。创建数组后，缓冲将被释放，不再可用。
	struct 动态类型缓冲
	{
		static std::unique_ptr<动态类型缓冲> 创建(matlab::data::ArrayType 类型, size_t 元素数);
		virtual void* get()const noexcept = 0;
		virtual matlab::data::Array 创建数组(matlab::data::ArrayDimensions&& 各维尺寸)noexcept = 0;
		virtual ~动态类型缓冲() {}
	};
}
#include"Mex工具.后置.hpp"