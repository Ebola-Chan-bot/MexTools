export module Mex工具;
import :实现;
export import :隐式导出;
namespace Mex工具
{
	//用户必须定义以下函数

	//全局初始化，在MATLAB首次载入MEX时调用。用户应当在此函数中进行全局变量初始化、持久资源分配等不应在每次调用时重复进行的操作。全局变量也可以在本函数外初始化，但这样做不能保证初始化顺序，仅适用于不依赖其它全局变量的情况。在此方法中进行具有严格顺序依赖要求的全局变量初始化。
	export void 初始化();
	//执行调用，MATLAB每次调用MEX文件函数时调用此方法。用户应当在此函数中处理输入参数，充分利用初始化阶段分配的持久资源，然后将结果写入输出参数
	export void 执行(matlab::mex::ArgumentList& 输出, matlab::mex::ArgumentList& 输入);
	//全局清理，在MATLAB卸载MEX（包括 clear mex 以及MATLAB会话退出）时调用。用户应当在此函数中释放全局变量、持久资源等
	export void 清理();

	//类型相关模板

	//此变量模板将静态的数组元素类型转换为ArrayType枚举值
	export template<typename T>
		extern constexpr ArrayType 静态类型转动态 = GetArrayType<T>::type;
	//此别名模板将ArrayType枚举值转换为静态的数组元素类型
	export template<ArrayType T>
		using 动态类型转静态 = typename 动态类型转静态_s<T>::type;
	//此别名模板将TypedArray或SparseArray类型转换为其元素类型
	export template<typename T>
		using 数组类型转元素 = typename 数组类型转元素_s<T>::type;
	//将ArrayType枚举值转换为对应的C++类型的字节数，需要将ArrayType转换为size_t后作为该数组的索引以得到字节数。使用示例：Mex工具::类型字节数[(size_t)ArrayType::DOUBLE];
	export constexpr const uint8_t(&类型字节数)[] = 类型字节数_s<std::make_integer_sequence<std::underlying_type_t<ArrayType>, 32>>::value;
	//获取任意类型MATLAB数组的字节数，即元素个数×单个元素的字节数。稀疏数组的字节数包含其0元素所占字节。
	export inline size_t 数组字节数(const Array& 输入)
	{
		return 类型字节数[(size_t)输入.getType()] * 输入.getNumberOfElements();
	}

	//有用的全局变量

	export extern ArrayFactory 数组工厂;
	export extern std::shared_ptr<matlab::engine::MATLABEngine> MATLAB引擎;

	//万能转码

	/*一对一转换。支持以下所有转换：
	将输入MATLAB类型视为标量，转换为C++类型。如果类型不完全匹配，将优先执行隐式转换；如果不能隐式转换，再尝试显式转换。
	将所有能被转换为string的MATLAB类型视为标量，转换为CharArray、MATLABString、String（i.e. std::u16string）、StringArray、std::string或std::wstring。自动执行必要的UTF16到UTF8转换。
	输入右值引用，这意味着转换后输入数组可能不再可用。
	*/
	export template<typename 输出>
		inline 输出 万能转码(Array&& 输入)
	{
		return apply_visitor(std::move(输入), 标量转换<输出>());
	}
	/*一对一转换。支持以下所有转换：
	将输入C++类型转换为MATLAB标量。如果类型不完全匹配，将优先执行隐式转换；如果不能隐式转换，再尝试显式转换。
	将所有能被MATLAB转换为StringArray的Array类型视为标量，或所有被std::ostring::operator<<或std::wostream::operator<<支持的类型，转换为CharArray、MATLABString、String（i.e. std::u16string）或StringArray。自动执行必要的UTF8到UTF16转换。
	输入右值引用，这意味着转换后输入对象可能不再可用。
	*/
	export template<typename 输出, typename T>
		inline 输出 万能转码(T&& 输入)
	{
		return 万能转码<数组类型转元素<输出>>(std::move(输入));
	}
	/*将MATLAB数组拷出到迭代器。如果类型不匹配，将优先执行隐式转换；如果不能隐式转换，再尝试显式转换。
	特别地，所有能被转换为string的MATLAB类型数组可以被拷出到接受CharArray、MATLABString、String（i.e.std::u16string）、std::string或std::wstring的迭代器。自动执行必要的UTF16到UTF8转换。
	特别地，如果迭代器是void*，将把MATLAB数组的底层字节直接拷出到目标指针，无论其是否为POD类型。
	特别地，SparseArray将被展开为满数组。
	输入Array右值引用，这意味着转换后输入对象可能不再可用。
	函数执行后，迭代器将指向拷出的最后一个元素的下一个位置。
	*/
	export template<typename 迭代器>
		void 万能转码(Array&&, 迭代器&);
	/*从迭代器创建具有指定维度的MATLAB满数组。如果类型不匹配，将优先执行隐式转换；如果不能隐式转换，再尝试显式转换。
	特别地，如果输出类型是StringArray，迭代器对应的值类型可以是任何能被MATLAB转换为StringArray的类型，或所有被std::ostring::operator<<或std::wostream::operator<<支持的类型。
	特别地，如果迭代器是void*，将被解释为指向输出类型的元素类型的指针。
	输入ArrayDimensions右值引用，这意味着转换后输入对象可能不再可用。
	函数执行后，迭代器将指向最后一个元素的下一个位置。
	*/
	export template<typename 迭代器, typename 输出类型 = TypedArray<std::remove_cvref_t<decltype(*(std::declval<迭代器>() + 1))>>>
		输出类型 万能转码(迭代器&, ArrayDimensions&&);

	//自动析构

	//将对象指针加入自动析构表。clear mex 时此指针将被自动delete。只能对new创建的对象指针使用此方法。
	export template<typename T>
		inline void 自动析构(T* 对象指针)noexcept
	{
		自动析构表[对象指针] = [](void* 指针) {delete (T*)指针; };
	}
	//将对象指针加入自动析构表。clear mex 时此对象将被自动析构。使用指定的删除器。
	export template<typename T>
		inline void 自动析构(T* 对象指针, std::move_only_function<void(void*)>&& 删除器)noexcept
	{
		自动析构表[对象指针] = std::move(删除器);
	}
	//此方法用于提示指定对象已被自动析构，从而避免自动析构表重复析构。此方法不负责析构对象本身，对象本身仍由调用方负责析构。
	export inline bool 手动析构(void* 对象指针)noexcept
	{
		return 自动析构表.erase(对象指针);
	}
	//检查对象指针是否存在于自动析构表中。如不存在，此指针可能是无效的，或者创建时未加入自动析构表。
	export inline bool 对象存在(void* 对象指针)noexcept
	{
		return 自动析构表.contains(对象指针);
	}

	//Mex工具可能会抛出的异常类型。除此之外，非本工具实现的代码还可能抛出std::exception及其派生类。
	export enum class Mex异常
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
}