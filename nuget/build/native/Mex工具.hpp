#pragma once
#include <magic_enum.hpp>
#include "Mex工具.前置.hpp"
namespace Mex工具
{
	using namespace matlab::data;
	using namespace 内部;

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

	//此变量模板将静态的数组元素类型转换为ArrayType枚举值
	 template<typename T>
	 constexpr ArrayType 静态类型转动态 = GetArrayType<T>::type;
	//此别名模板将ArrayType枚举值转换为静态的数组元素类型
	 template<ArrayType T>
		using 动态类型转静态 = typename 动态类型转静态_s<T>::type;
	//此别名模板将TypedArray或SparseArray类型转换为其元素类型
	 template<typename T>
		using 数组类型转元素 = typename 数组类型转元素_s<T>::type;
	//将ArrayType枚举值转换为对应的C++类型的字节数，需要将ArrayType转换为size_t后作为该数组的索引以得到字节数。使用示例：Mex工具::类型字节数[(size_t)ArrayType::DOUBLE];
	 constexpr const uint8_t(&类型字节数)[32] = 内部::类型字节数_s<std::make_integer_sequence<std::underlying_type_t<ArrayType>, 32>>::value;
	//获取任意类型MATLAB数组的字节数，即元素个数×单个元素的字节数。稀疏数组的字节数包含其0元素所占字节。
	 inline size_t 数组字节数(const Array& 输入)
	{
		return 类型字节数[(size_t)输入.getType()] * 输入.getNumberOfElements();
	}

	//有用的全局变量。这些变量会在用户定义的初始化之前被自动初始化，但在进入用户初始化之前这些变量可能尚未初始化，因此用户不应依赖这些变量进行全局变量初始化。在进入用户初始化以后可以使用，但通常不应该修改这些变量。

	 extern ArrayFactory 数组工厂;
	 extern std::shared_ptr<matlab::engine::MATLABEngine> MATLAB引擎;

	//万能转码

	/*一对一转换。支持以下所有转换：
	将输入MATLAB类型视为标量，转换为C++类型。如果类型不完全匹配，将优先执行隐式转换；如果不能隐式转换，再尝试显式转换；如果还不行，还会尝试调用MATLAB引擎转换。
	将所有能被转换为string的MATLAB类型视为标量，转换为CharArray、MATLABString、String（i.e. std::u16string）、StringArray、std::string或std::wstring。自动执行必要的UTF16到UTF8转换。
	输入右值引用，这意味着转换后输入数组可能不再可用。
	*/
	 template<typename 输出>
		inline 输出 万能转码(Array&& 输入)
	{
		return apply_visitor(std::move(输入), 标量转换<输出>());
	}
	/*一对一转换。支持以下所有转换：
	将输入C++类型转换为MATLAB标量。如果类型不完全匹配，将优先执行隐式转换；如果不能隐式转换，再尝试显式转换；如果还不行，还会尝试调用MATLAB引擎转换。
	将所有能被MATLAB转换为StringArray的Array类型视为标量，或所有被std::ostringstream::operator<<或std::wostringstream::operator<<支持的类型，转换为CharArray、MATLABString、String（i.e. std::u16string）或StringArray。自动执行必要的UTF8到UTF16转换。
	最好使用std::move输入右值引用。某些转换支持常量引用，某些不支持，可以根据是否能通过编译来判定。
	*/
	 template<typename 输出, typename T>
		inline 输出 万能转码(T&& 输入)
	{
		return 标量转换<输出>::转换(std::forward<T>(输入));
	}
	/*将MATLAB数组拷出到迭代器。如果类型不匹配，将优先执行隐式转换；如果不能隐式转换，再尝试显式转换；如果还不行，还会尝试调用MATLAB引擎转换。
	特别地，所有能被转换为string的MATLAB类型数组可以被拷出到接受CharArray、MATLABString、String（i.e.std::u16string）、std::string或std::wstring的迭代器。自动执行必要的UTF16到UTF8转换。
	特别地，如果迭代器是void*，将被强制转换为指向数组值类型的指针使用。
	特别地，SparseArray将被填充为满数组。
	输入Array右值引用，这意味着转换后输入对象可能不再可用。
	函数执行后，迭代器将指向拷出的最后一个元素的下一个位置。如果输入稀疏数组，迭代器必须支持使用operator[]的随机写入方式。
	*/
	 template<typename 迭代器>
	 inline void 万能转码(Array&& 输入, 迭代器& 输出)
	 {
		 apply_visitor(std::move(输入), 迭代MC(输出));
	 }
	/*从迭代器创建具有指定维度的MATLAB满数组。如果类型不匹配，将优先执行隐式转换；如果不能隐式转换，再尝试显式转换。
	特别地，如果输出类型是StringArray，迭代器对应的值类型可以是所有被std::ostringstream::operator<<或std::wostringstream::operator<<支持的类型；如果不支持，还可以是任何能被MATLAB转换为string的MATLAB元素对象（如枚举、分类数组或任何实现了string方法的对象等）
	特别地，如果迭代器是void*，将被强制转换为指向数组值类型的指针使用。
	输入ArrayDimensions右值引用，这意味着转换后输入对象可能不再可用。
	函数执行后，迭代器将指向最后一个元素的下一个位置。
	*/
	 template<typename 输出类型, typename 迭代器>
	 inline 输出类型 万能转码(迭代器& 输入, ArrayDimensions&& 各维尺寸)
	 {
		 return 迭代CM<输出类型>::转换(输入, std::move(各维尺寸));
	 }
	//将给定指针直接作为指定MATLAB满数组的基础数据缓冲区。数据类型必须完全匹配。必须额外指定删除器。从R2024b开始支持，之前的版本不支持。
	 template<typename 输出类型>
	 inline 输出类型 万能转码(数组类型转元素<输出类型>* 输入, ArrayDimensions&& 各维尺寸, buffer_deleter_t 自定义删除器)
	 {
		 return 数组工厂.createArrayFromBuffer(std::move(各维尺寸), buffer_ptr_t<数组类型转元素<输出类型>>(输入, 自定义删除器));
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

	 //Mex工具可能会抛出的异常类型。除此之外，非本工具实现的代码还可能抛出std::exception及其派生类。
	 enum class Mex异常
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

	 /*将任意枚举类型当作异常抛给MATLAB。枚举类型名和字面文本将同时作为MException的identifier和message，因此只能使用英文、数字和下划线。
	 MATLAB只能正确捕获std::exception及其派生类。此方法将枚举类型的异常转换为matlab::engine::MATLABException抛出，符合MATLAB捕获要求。
	 用户只应对std::exception及其派生类直接使用throw。对于其它异常类型，应使用此方法或任何其它方法将异常类型转换为std::exception及其派生类，或者自行catch并处理。如果违反这个规则，异常信息将会丢失，MATLAB只能接收到`Mex异常::Unexpected_CPP_exception`。
	 和throw一样，此方法将违反noexcept约定。异常不能从noexcept方法中向外传递，而是导致MATLAB进程崩溃。
	 */
	 template<typename T>
	 [[noreturn]]inline void EnumThrow(T 异常)
	 {
		 std::ostringstream 异常信息流;
		 for (const char* 字符指针 = typeid(T).name(); const char 字符 = *字符指针; 字符指针 += 字符 == ':' ? 2 : 1)
			 异常信息流 << 字符;
		 const std::string 异常信息 = (异常信息流 << ':' << magic_enum::enum_name(异常)).str();
		 throw matlab::engine::MATLABException(异常信息, 万能转码<String>(异常信息));
	 }
}
#include"Mex工具.后置.hpp"