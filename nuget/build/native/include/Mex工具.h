#pragma once
#include<Mex类型.h>
#include<array>
#include<map>
#include<Windows.h>
namespace Mex工具
{
	enum class Mex异常 :uint8_t
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
		此Array不能拷出为wchar_t,
		稀疏数组不能取得指针,
		不能从空数组取得标量,
		意外的SEH异常,
	};
	//这里使用static而不是extern，因为从其它编译单元链接的变量不一定能在DllMain阶段完成初始化，会造成意外错误。
	static matlab::data::ArrayFactory 数组工厂;
	//可以作为MATLAB数组元素的类型
	template<typename T>
	concept MATLAB元素类型 = requires{matlab::data::GetArrayType<std::remove_cvref_t<T>>::type; };
	template<MATLAB元素类型 T>
	constexpr bool 稀疏 = false;
	template<MATLAB元素类型 T>
	constexpr bool 稀疏<matlab::data::SparseArray<T>> = true;
	template<typename T>
	concept MATLAB复杂元素 = std::_Is_any_of_v<std::remove_cvref_t<T>, matlab::data::Array, matlab::data::Struct, matlab::data::MATLABString, matlab::data::Object> && !稀疏<T>;
	template<typename T>
	concept MATLAB简单元素 = MATLAB元素类型<T> && !(MATLAB复杂元素<T> || 稀疏<T> || std::is_same_v<T, matlab::data::Enumeration>);
	template<typename T>
	concept 迭代器 = requires(T 器) { *(器 + 1); };
	template<迭代器 T>
	class 取迭代器值类型_s
	{
		static const T 器;
	public:
		using 值类型 = std::remove_cvref_t<decltype(*(器 + 1))>;
	};
	//请勿使用decltype(*)语法，会带引用——任意类型都可以转换为任意引用类型，造成意外结果
	template<typename T>
	using 取迭代器值类型 = 取迭代器值类型_s<T>::值类型;
	template<typename T输出, typename T输入>
	concept 隐写迭代器 = requires(T输入 输入, T输出 输出) { *(输出 + 1) = 输入; };
	template<typename T输出, typename T输入>
	concept 可写迭代器 = requires(T输入 输入, T输出 输出) { *(输出 + 1) = (取迭代器值类型<T输出>)输入; };
	template<typename T输出, typename T输入>
	concept 显写迭代器 = 可写迭代器<T输出, T输入> && !隐写迭代器<T输出, T输入>;
	template<typename T输入, typename T输出>
	concept 隐读迭代器 = requires(T输入 输入, T输出 & 输出) { 输出 = *(输入 + 1); };
	template<typename T输入, typename T输出>
	concept 可读迭代器 = requires(T输入 输入) { (T输出)* (输入 + 1); };
	template<typename T输入, typename T输出>
	concept 显读迭代器 = 可读迭代器<T输入, T输出> && !隐读迭代器<T输入, T输出>;
	template<typename T输入, typename T输出>
	concept 同类迭代器 = std::is_same_v<取迭代器值类型<T输入>, std::remove_cvref_t<T输出>>;
	template <typename T>
	concept 非const = !std::is_const_v<T>;
	template<typename T1, typename T2>
	concept 只能是 = std::is_same_v<T1, T2>;
	namespace 内部
	{
		using namespace matlab::data;
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
		转换结构，用作【动态类型选择模板】的模板参数。此转换结构提供一个一般实现和两个特化实现，都有一个【static 输出 返回(const Array& 数组)】成员。注意返回成员的类型跟模板参数【输出】有关，因为输出属于后续模板参数，而【动态类型选择模板】仅要求第一个模板参数不能与返回成员类型有关。因此这里的返回成员可以跟输出类型有关，而不能跟输入类型有关。
		对一般的输入输出，我们认为它不是有效的类型转换，应当抛异常。
		*/
		template<typename 目标类型>
		struct 标量转换
		{
			template<typename T>
			目标类型 operator()(const T&输入)
			{
				return (目标类型)输入[0];
			}
			template<typename T>
				requires requires(目标类型& 输出, const T& 输入) { 输出 = 输入[0] }
			{
				return 输入[0];
			}
		};
		//输出迭代器必须用引用返回，不然不能满足动态类型选择模板的要求
		template<typename T输入, typename T输出>
		struct 数组拷贝
		{
			static void 返回(const Array& 输入, T输出& 输出)
			{
				throw Mex异常::类型转换失败;
			}
		};
		template<typename T>
		inline void 选择性赋值(const T& 输入, const T& 输出) {}
		template<typename T>
			requires requires(const T& 输入, T& 输出) { 输出 = 输入; }
		inline void 选择性赋值(const T& 输入, T& 输出)
		{
			输出 = 输入;
		}
		template<MATLAB简单元素 T输入, 隐写迭代器<T输入> T输出>
		struct 数组拷贝<T输入, T输出&>
		{
			static void 返回(const Array& 输入, T输出& 输出)
			{
				const TypedArray<T输入>类型化(输入);
				选择性赋值(std::copy(类型化.cbegin(), 类型化.cend(), 输出), 输出);
			}
		};
		template<typename T输入, typename T输出>
			requires MATLAB复杂元素<T输入>&& 可写迭代器<T输出, T输入> || 显写迭代器<T输出, T输入>
		struct 数组拷贝<T输入, T输出&>
		{
			static void 返回(const Array& 输入, T输出& 输出)
			{
				const TypedArray<T输入>类型化(输入);
				for (const T输入& a : 类型化)
					*(输出++) = (取迭代器值类型<T输出>)a;
			}
		};
		template<typename T输入, typename T输出>
			requires MATLAB复杂元素<T输入>&& 可写迭代器<T输出, T输入> || 显写迭代器<T输出, T输入>
		struct 数组拷贝<T输入, const T输出&>
		{
			static void 返回(const Array& 输入, const T输出& 输出)
			{
				T输出 拷贝 = 输出;
				数组拷贝<T输入, T输出&>::返回(输入, 拷贝);
			}
		};
		template<typename T被加数, typename T加数>
		inline void 选择性求和(const T被加数& 被加数, const T加数& 加数) {}
		template<非const T被加数, typename T加数>
			requires requires(T被加数& 被加数, const T加数& 加数) { 被加数 += 加数; }
		inline void 选择性求和(T被加数& 被加数, const T加数& 加数)
		{
			被加数 += 加数;
		}
		template<typename T输入, typename T输出>
			requires (!std::is_void_v<取迭代器值类型<T输出>>&& requires(T输出& 输出, T输入 a) { (输出 + 2)[0] = (取迭代器值类型<T输出>)a; })
		struct 数组拷贝<SparseArray<T输入>, T输出>
		{
			static void 返回(const Array& 输入, T输出& 输出)
			{
				const SparseArray<T输入>稀疏(输入);
				const TypedIterator<const T输入>迭代尾 = 稀疏.cend();
				const size_t 高度 = 稀疏.getDimensions().front();
				std::fill_n(输出, 稀疏.getNumberOfElements(), 0);
				for (TypedIterator<const T输入>a = 稀疏.cbegin(); a < 迭代尾; ++a)
				{
					const SparseIndex 索引 = 稀疏.getIndex(a);
					输出[索引.first + 索引.second * 高度] = (取迭代器值类型<T输出>) * a;
				}
				选择性求和(输出, 稀疏.getNumberOfElements());
			}
		};
		template<typename T>
		struct 去Sparse_s
		{
			using 元素 = T;
		};
		template<typename T>
		struct 去Sparse_s<SparseArray<T>>
		{
			using 元素 = T;
		};
		template<typename T>
		using 去Sparse = 去Sparse_s<T>::元素;
		//void*特化用来处理用户不关心数组具体类型的情况
		template<typename T输入, typename T输出>
			requires (!std::is_void_v<T输入>&& std::is_same_v<std::remove_cvref_t<T输出>, void*>)
		struct 数组拷贝<T输入, T输出&>
		{
			static void 返回(const Array& 输入, T输出& 输出)
			{
				using 转指针 = std::_Copy_cv<T输出, 去Sparse<T输入>*&>;
				数组拷贝<T输入, 转指针>::返回(输入, (转指针)输出);
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
				throw Mex异常::不支持的类型;
			}
		};
		template<typename T>
			requires std::is_base_of_v<Array, T>
		struct 数组类型转元素_s {};
		template<MATLAB元素类型 T>
		struct 数组类型转元素_s<TypedArray<T>>
		{
			using 元素类型 = std::remove_cvref_t<T>;
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
		template<MATLAB复杂元素 T输出, 显读迭代器<T输出> T输入>
			requires 非const<T输入>
		void 通用迭代拷贝(TypedArray<T输出>& 输出, T输入& 输入)
		{
			for (Reference<T输出>a : 输出)
				a = (T输出)std::move(*(输入++));
		}
		template<MATLAB简单元素 T输出, 显读迭代器<T输出> T输入>
			requires 非const<T输入>
		void 通用迭代拷贝(TypedArray<T输出>& 输出, T输入& 输入)
		{
			for (T输出& a : 输出)
				a = (T输出) * (输入++);
		}
		extern std::map<void*, std::move_only_function<void(void*)const>>自动析构表;
	}
	//此using在将动态类型枚举ArrayType转为静态类型
	template<matlab::data::ArrayType T>
	using 动态类型转静态 = 内部::动态类型转静态<T>::类型;
	constexpr uint8_t 类型总数 = 32;
	//此数组存储了每种动态类型枚举对应的静态类型的尺寸。使用方法示例：类型尺寸[(int)inputs[1].getType()]
	constexpr std::array<uint8_t, 类型总数>类型尺寸 = 内部::类型尺寸<std::make_integer_sequence<int, 类型总数>>;
	template<typename T>
	concept MATLAB数组类型 = requires{typename 内部::数组类型转元素_s<T>::元素类型; };
	template<MATLAB数组类型 T>
	using 数组类型转元素 = 内部::数组类型转元素_s<T>::元素类型;
	template<typename T>
	concept MATLAB复杂数组 = MATLAB复杂元素<数组类型转元素<T>>;
	template<typename T>
	concept MATLAB简单数组 = MATLAB简单元素<数组类型转元素<T>>;
	template<typename T>
	concept CanCreateScalar = MATLAB元素类型<T> && requires(T 输入)
	{
		数组工厂.createScalar(输入);
	};
	template<typename T>
	concept CanCreateArray = MATLAB元素类型<T> && requires(matlab::data::ArrayDimensions 各维尺寸)
	{
		数组工厂.createArray<T>(各维尺寸);
	};

	//增强功能，可以使用如下三个宏定义在一个MEX文件函数中定义多个API

#define API声明(函数名) void 函数名(matlab::mex::ArgumentList& outputs,matlab::mex::ArgumentList& inputs)
#define API索引 constexpr void (*(API[]))(matlab::mex::ArgumentList&, matlab::mex::ArgumentList&) =
#define API调用 const uint8_t 选项=万能转码<uint8_t>(std::move(inputs[0]));if(选项<std::extent_v<decltype(API)>)API[选项](outputs, inputs);else throw Mex异常::不支持的API;
	/*
	出错时，将后续返回值设为空数组
	为了将C++异常传递给MATLAB，我们通常需要将MEX文件函数的第一个返回值保留作为错误代码
	*/
	void 异常输出补全(matlab::mex::ArgumentList& outputs);

	/*
	将MATLAB简单类型标量转换为C++类型，自动执行必要的显式转换，还支持MATLAB稀疏矩阵。
	语法：Mex工具::万能转码<T>(输入)
	此函数支持以下转换：
	- 整数和浮点类型互转，遵循C++转换规则
	- MATLAB数值类型转复数（std::complex）
	- 整数转任意类型指针
	# 参数
	typename T，转换目标类型，支持所有可从MATLAB简单元素类型显式转换而来的类型
	const Array& 输入，MATLAB标量常量引用。
	*/
	template<typename T>
		requires (!std::_Is_any_of_v<T, matlab::data::CellArray,std::string>)
	inline T 万能转码(const matlab::data::Array& 输入)
	{
		if (输入.isEmpty())
			throw Mex异常::不能从空数组取得标量;
		return 动态类型选择模板<内部::转换结构, T>(输入.getType())(输入);
	}
	/*
	将MATLAB字符行向量、字符串或字符行向量元胞标量转换为 UTF16 String（std::u16string）
	语法：Mex工具::万能转码<String>(输入)
	# 参数
	const Array& 输入，MATLAB标量常量引用。
	*/
	template<>
	extern matlab::data::String 万能转码<matlab::data::String>(const matlab::data::Array& 输入);
	/*
	将MATLAB字符行向量、字符串或字符行向量元胞标量转换为 UTF16 MATLABString
	语法：Mex工具::万能转码<MATLABString>(输入)
	参数：const Array& 输入，MATLAB标量常量引用。
	*/
	template<>
	extern matlab::data::MATLABString 万能转码<matlab::data::MATLABString>(const matlab::data::Array& 输入);
	/*
	将MATLAB字符行向量、字符串或字符行向量元胞标量转换为 UTF16 CharArray
	语法：Mex工具::万能转码<CharArray>(输入)
	参数：const Array& 输入，MATLAB标量常量引用。
	*/
	template<>
	extern matlab::data::CharArray 万能转码<matlab::data::CharArray>(const matlab::data::Array& 输入);
	/*
	将MATLAB字符行向量、字符串或字符行向量元胞标量转换为 UTF8 std::string，使用 Win32 WideCharToMultiByte 执行UTF16到UTF8的转码
	语法：Mex工具::万能转码(std::move(输入))
	参数：Array&& 输入，MATLAB标量右值引用。函数返回后，输入数组将变成不可用。
	*/
	std::string 万能转码(matlab::data::Array&& 输入);
	/*
	将MATLAB字符行向量、字符串数组或字符行向量元胞数组转换为 UTF16 StringArray。字符行向量将转换为字符串标量，字符串数组和字符行向量元胞数组将转换为各维尺寸相同的字符串数组。
	语法：Mex工具::万能转码<StringArray>(输入)
	参数：const Array& 输入，MATLAB标量常量引用。
	*/
	template<>
	extern matlab::data::StringArray 万能转码<matlab::data::StringArray>(const matlab::data::Array& 输入);
	/*
	将C++类型转换为对应的MATLAB标量，执行必要的显式转换
	语法：Mex工具::万能转码<T输出>(输入);
	# 参数
	typename T输出，目标MATLAB数组类型。
	const T输入& 输入，可显式转换为MATLAB支持类型的输入值。例如，所有指针类型都能显式转换为MATLAB整数标量。
	# 返回
	T输出，MATLAB简单标量
	*/
	template<typename T输出, typename T输入>
		requires requires(const T输入& 输入) { 数组工厂.createScalar(数组类型转元素<T输出>(输入)); }
	inline T输出 万能转码(const T输入& 输入)
	{
		return 数组工厂.createScalar(数组类型转元素<T输出>(输入));
	}
	/*
	将C++简单类型转换为对应的MATLAB简单标量，不执行转换
	语法：Mex工具::万能转码(输入);
	参数：T 输入，MATLAB元素类型的输入值。
	返回：MATLAB简单类型标量
	*/
	template<MATLAB简单元素 T>
	inline matlab::data::TypedArray<T>万能转码(T 输入)
	{
		return 数组工厂.createScalar(输入);
	}
	/*
	将C++复杂类型转换为对应的MATLAB复杂标量，执行移动构造
	语法：Mex工具::万能转码(输入);
	参数：const T& 输入，MATLAB复杂类型的输入值
	返回：MATLAB复杂类型标量
	*/
	template<MATLAB复杂元素 T>
		requires CanCreateScalar<T>
	inline matlab::data::TypedArray<T>万能转码(const T& 输入)
	{
		return 数组工厂.createScalar(输入);
	}
	/*
	将任意指针类型转换为 MATLAB size_t 标量
	语法：Mex工具::万能转码(输入);
	参数：const void* 输入，任意指针
	返回：TypedArray<size_t>，与指针数值相等的 MATLAB size_t 标量
	*/
	inline matlab::data::TypedArray<size_t>万能转码(const void* 输入)
	{
		return 数组工厂.createScalar((size_t)输入);
	}
	/*
	将任意MATLAB数组包装成元胞标量
	语法：Mex工具::万能转码(输入);
	参数：const Array& 输入，要包装的MATLAB数组。
	返回：CellArray，包装了输入数组的元胞标量
	*/
	template<typename T = matlab::data::CellArray>
		requires std::is_same_v<T, matlab::data::CellArray>
	inline matlab::data::CellArray 万能转码(const matlab::data::Array& 输入)
	{
		matlab::data::CellArray 输出 = 数组工厂.createCellArray({ 1 });
		输出[0] = 输入;
		return 输出;
	}
	/*
	将枚举类型转换成其基础类型的MATLAB标量
	语法：Mex工具::万能转码(输入)
	参数：T 输入，要转换的枚举类型
	返回：TypedArray<std::underlying_type_t<T>>，此枚举类基础类型的MATLAB标量
	*/
	template<typename T>
		requires CanCreateScalar<std::underlying_type_t<T>>
	matlab::data::TypedArray<std::underlying_type_t<T>> 万能转码(T 输入)
	{
		return 数组工厂.createScalar((std::underlying_type_t<T>)输入);
	}
	template<typename T>
	concept MATLAB字符串 = std::_Is_any_of_v<T, matlab::data::CharArray, matlab::data::String, matlab::data::MATLABString, matlab::data::StringArray>;
	template<MATLAB字符串 T>
	inline T 万能转码(const char*);
	/*
	将C样式UTF8字符串转换为 MATLAB UTF16 CharArray
	语法：Mex工具::万能转码<CharArray>(输入)
	参数：const char* 输入，0结尾的C样式UTF8字符串
	返回：CharArray MATLAB UTF16 字符行向量，使用 Win32 MultiByteToWideChar 执行转码
	*/
	template<>
	inline matlab::data::CharArray 万能转码<matlab::data::CharArray>(const char* 输入)
	{
		size_t 长度 = strlen(输入) + 1;
		matlab::data::buffer_ptr_t<char16_t>缓冲 = 数组工厂.createBuffer<char16_t>(长度);
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
	extern matlab::data::String 万能转码<matlab::data::String>(const char* 输入);
	/*
	将C样式UTF8字符串转换为 UTF16 MATLABString
	语法：Mex工具::万能转码<MATLABString>(输入)
	参数：const char* 输入，0结尾的C样式UTF8字符串
	返回：MATLABString，MATLAB UTF16 字符串元素，使用 Win32 MultiByteToWideChar 执行转码
	*/
	template<>
	inline matlab::data::MATLABString 万能转码<matlab::data::MATLABString>(const char* 输入)
	{
		return 万能转码<matlab::data::String>(输入);
	}
	/*
	将C样式UTF8字符串转换为 MATLAB UTF16 StringArray
	语法：Mex工具::万能转码<StringArray>(输入)
	参数：const char* 输入，0结尾的C样式UTF8字符串
	返回：StringArray MATLAB UTF16 字符串数组，使用 Win32 MultiByteToWideChar 执行转码
	*/
	template<>
	inline matlab::data::StringArray 万能转码<matlab::data::StringArray>(const char* 输入)
	{
		return 数组工厂.createScalar(万能转码<matlab::data::MATLABString>(输入));
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
	inline matlab::data::CharArray 万能转码<matlab::data::CharArray>(const wchar_t* 输入)
	{
		const size_t 字符个数 = wcslen(输入);
		matlab::data::buffer_ptr_t<char16_t>缓冲 = 数组工厂.createBuffer<char16_t>(字符个数);
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
	inline matlab::data::String 万能转码<matlab::data::String>(const wchar_t* 输入)
	{
		return (char16_t*)输入;
	}
	/*
	将C样式 UTF16 wchar_t 字符串转换为 MATLABString。C++ wchar_t 和 MATLAB char16_t 本质上是同一种类型，不需要转码
	语法：Mex工具::万能转码<MATLABString>(输入)
	参数：const wchar_t* 输入，0结尾的C样式 UTF16 wchar_t 字符串
	返回：MATLABString UTF16 字符串元素
	*/
	template<>
	inline matlab::data::MATLABString 万能转码<matlab::data::MATLABString>(const wchar_t* 输入)
	{
		return 万能转码<matlab::data::String>(输入);
	}
	/*
	将C样式 UTF16 wchar_t 字符串转换为 MATLAB StringArray。C++ wchar_t 和 MATLAB char16_t 本质上是同一种类型，不需要转码
	语法：Mex工具::万能转码<StringArray>(输入)
	参数：const wchar_t* 输入，0结尾的C样式 UTF16 wchar_t 字符串
	返回：StringArray MATLAB UTF16 字符串数组
	*/
	template<>
	inline matlab::data::StringArray 万能转码<matlab::data::StringArray>(const wchar_t* 输入)
	{
		return 数组工厂.createScalar(万能转码<matlab::data::MATLABString>(输入));
	}
	//获取MATLAB数组的字节数，即元素字节数×元素个数
	inline size_t 数组字节数(const matlab::data::Array& 数组)
	{
		matlab::data::apply_visitor
		return 动态类型选择模板<内部::数组字节数>(数组.getType())(数组);
	}
	/*
	将MATLAB简单数组拷贝到C++迭代器或指针。这里的指针指的是定义了[]索引, ++自增和*解引用运算符的任何类型；迭代器类似于指针，但不需要定义[]索引。从满数组，可以拷贝到迭代器；从稀疏矩阵，可以拷贝到指针。输出的数据类型必须能够从输入类型隐式转换。一个反例是，整数类型不能隐式转换为指针类型，因此接受指针的迭代器不能在此处使用。从任意数组可以拷贝到void*，不会进行任何类型转换或解析，直接拷贝原始字节。
	语法：Mex工具::万能转码(输入,输出);
	# 参数
	const Array& 输入，MATLAB数组。
	T& 输出，输出迭代器或指针。对于稀疏矩阵，只能输出到指针（定义[]索引操作）；对于满矩阵，可以输出到迭代器。无论哪种，都可以输出到void*。如果T不是const，函数返回后，输出迭代器将被指向输出序列的末尾。
	*/
	template<非const T>
	inline void 万能转码(const matlab::data::Array& 输入, T& 输出)
	{
		动态类型选择模板<内部::数组拷贝, T&>(输入.getType())(输入, 输出);
	}
	/*
	将MATLAB简单数组拷贝到C++迭代器或指针。这里的指针指的是定义了[]索引, ++自增和*解引用运算符的任何类型；迭代器类似于指针，但不需要定义[]索引。从满数组，可以拷贝到迭代器；从稀疏矩阵，可以拷贝到指针。输出的数据类型必须能够从输入类型隐式转换。一个反例是，整数类型不能隐式转换为指针类型，因此接受指针的迭代器不能在此处使用。从任意数组可以拷贝到void*，不会进行任何类型转换或解析，直接拷贝原始字节。
	语法：Mex工具::万能转码(输入,输出);
	# 参数
	const Array& 输入，MATLAB数组。
	T&& 输出，输出迭代器或指针。对于稀疏矩阵，只能输出到指针（定义[]索引操作）；对于满矩阵，可以输出到迭代器。无论哪种，都可以输出到void*。如果T不是const，函数返回后，输出迭代器将被指向输出序列的末尾。
	*/
	template<typename T>
		requires (!(同类迭代器<T, wchar_t> || 同类迭代器<T, std::string> || 同类迭代器<T, char>))
	inline void 万能转码(const matlab::data::Array& 输入, T&& 输出)
	{
		动态类型选择模板<内部::数组拷贝, const T&>(输入.getType())(输入, 输出);
	}
	/*
	将 MATLAB UTF16 字符行向量、字符串标量或字符向量元胞标量拷出为C样式0结尾的 UTF8 char*
	语法：Mex工具::万能转码(std::move(输入),输出)
	# 参数
	Array&& 输入，MATLAB UTF16 字符行向量、字符串标量或字符向量元胞标量。此函数返回后，输入数组将变成不可用。
	char*const& 输出，应确保有足够大的内存分配，输出的C样式字符串将以0结尾。
	# 返回值
	int，输出的UTF8字节数，包括尾随0
	*/
	int 万能转码(matlab::data::Array&& 输入, char* const& 输出);
	/*
	将 MATLAB UTF16 字符行向量、字符串标量或字符向量元胞标量拷出为C样式0结尾的 UTF8 char*
	语法：Mex工具::万能转码(std::move(输入),输出)
	# 参数
	Array&& 输入，MATLAB UTF16 字符行向量、字符串标量或字符向量元胞标量。此函数返回后，输入数组将变成不可用。
	char*& 输出，应确保有足够大的内存分配，输出的C样式字符串将以0结尾。函数返回后，此指针将指向输出序列的末尾。
	# 返回值
	int，输出的UTF8字节数，包括尾随0
	*/
	inline int 万能转码(matlab::data::Array&& 输入, char*& 输出)
	{
		const int 字数 = 万能转码(std::move(输入), (char* const&)输出);
		输出 += 字数;
		return 字数;
	}
	/*
	将MATLAB字符行向量、字符串标量或字符向量元胞标量拷出到接受wchar_t的迭代器，例如wchar_t*
	语法：Mex工具::万能转码(输入,输出)
	# 参数
	const Array& 输入，MATLAB UTF16 字符行向量、字符串标量或字符向量元胞标量。
	同类迭代器<wchar_t> T& 输出迭代器，应接受wchar_t输入，确保有足够大的内存分配。输出的字符串不会在结尾加0。如果T不是const，函数返回后，输出迭代器将被指向输出序列的末尾。
	*/
	template<同类迭代器<wchar_t> T>
	void 万能转码(const matlab::data::Array& 输入, T& 输出)
	{
		switch (输入.getType())
		{
		case matlab::data::ArrayType::CHAR:
		{
			const matlab::data::CharArray 字符串(输入);
			内部::选择性赋值(std::copy(字符串.cbegin(), 字符串.cend(), 输出), 输出);
		}
		break;
		case matlab::data::ArrayType::MATLAB_STRING:
		{
			const matlab::data::String 字符串 = 输入[0];
			内部::选择性赋值(std::copy(字符串.cbegin(), 字符串.cend(), 输出), 输出);
		}
		break;
		case matlab::data::ArrayType::CELL:
		{
			const matlab::data::CharArray 字符串 = 输入[0];
			内部::选择性赋值(std::copy(字符串.cbegin(), 字符串.cend(), 输出), 输出);
		}
		break;
		default:
			throw Mex异常::此Array不能拷出为wchar_t;
		}
	}
	/*
	将MATLAB字符行向量、字符串标量或字符向量元胞标量拷出到接受wchar_t的迭代器，例如wchar_t*
	语法：Mex工具::万能转码(输入,输出)
	# 参数
	const Array& 输入，MATLAB UTF16 字符行向量、字符串标量或字符向量元胞标量。
	同类迭代器<wchar_t> T&& 输出迭代器，应接受wchar_t输入，确保有足够大的内存分配。输出的字符串不会在结尾加0。如果T不是const，函数返回后，输出迭代器将被指向输出序列的末尾。
	*/
	template<同类迭代器<wchar_t> T>
	inline void 万能转码(const matlab::data::Array& 输入, T&& 输出)
	{
		万能转码(输入, (const T&)输出);
	}
	/*
	将 MATLAB UTF16 字符行向量、字符串数组或字符向量元胞数组拷出到接受 UTF8 std::string 的迭代器
	语法：Mex工具::万能转码(std::move(输入),输出)
	# 参数
	Array&& 输入，MATLAB UTF16 字符行向量、字符串数组或字符向量元胞数组。函数返回后，输入数组将变成不可用。
	同类迭代器<std::string> T& 输出迭代器，应接受std::string输入，确保有足够大的内存分配。函数返回后，输出迭代器将被指向输出序列的末尾。
	*/
	template<同类迭代器<std::string> T>
		requires 非const<T>
	void 万能转码(Array&& 输入, T& 输出)
	{
		switch (输入.getType())
		{
		case ArrayType::CHAR:
		{
			const int 长度 = 输入.getNumberOfElements();
			const buffer_ptr_t<char16_t>缓冲 = CharArray(std::move(输入)).release();
			输出++->resize_and_overwrite(长度 * 3 + 1, [&缓冲, 长度](char* 指针, size_t 尺寸)
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
			throw Mex异常::此Array不能转换为stdstring;
		}
	}
	/*
	将 MATLAB UTF16 字符行向量、字符串数组或字符向量元胞数组拷出到接受 UTF8 std::string 的迭代器
	语法：Mex工具::万能转码(std::move(输入),输出)
	# 参数
	Array&& 输入，MATLAB UTF16 字符行向量、字符串数组或字符向量元胞数组。函数返回后，输入数组将变成不可用。
	同类迭代器<std::string> const T&& 输出迭代器，应接受std::string输入，确保有足够大的内存分配。
	*/
	template<同类迭代器<std::string> T>
	inline void 万能转码(Array&& 输入, T&& 输出)
	{
		万能转码(std::move(输入), 输出);
	}
	/*
	从C++迭代器拷贝到MATLAB简单数组。
	语法：Mex工具::万能转码<T输出>(输入,各维尺寸)
	# 示例
	```
	万能转码<CharArray>(L"输出字符串", { 1,5 }); //自动转换wchar_t为char16_t
	int a[] = { 1,2,3 };
	万能转码<TypedArray<double>>(a, { 3,1 }); //自动转换int为double
	```
	# 参数
	MATLAB简单数组 T输出，MATLAB数组类型
	隐读迭代器<数组类型转元素<T输出>> T输入& 输入，可读迭代器，输出值应当可隐式转换为目标MATLAB元素类型。如果T输入支持加法运算，函数返回后，迭代器将向后移动【输出数组的元素个数】个位置。
	const ArrayDimensions& 各维尺寸，输出数组各维尺寸。
	# 返回值
	T输出，指定类型的MATLAB数组
	*/
	template<MATLAB简单数组 T输出, 隐读迭代器<数组类型转元素<T输出>> T输入>
	inline T输出 万能转码(T输入& 输入, const ArrayDimensions& 各维尺寸)
	{
		T输出 输出 = 数组工厂.createArray<数组类型转元素<T输出>>(各维尺寸);
		const size_t 元素个数 = 输出.getNumberOfElements();
		std::copy_n(输入, 元素个数, 输出.begin());
		内部::选择性求和(输入, 元素个数);
		return 输出;
	}
	/*
	从C++迭代器拷贝到MATLAB简单数组。
	语法：Mex工具::万能转码<T输出>(输入,各维尺寸)
	# 示例
	```
	万能转码<CharArray>(L"输出字符串", { 1,5 }); //自动转换wchar_t为char16_t
	int a[] = { 1,2,3 };
	万能转码<TypedArray<double>>(a, { 3,1 }); //自动转换int为double
	```
	# 参数
	MATLAB简单数组 T输出，MATLAB数组类型
	隐读迭代器<数组类型转元素<T输出>> T输入&& 输入，可读迭代器，输出值应当可隐式转换为目标MATLAB元素类型。如果T输入支持加法运算，函数返回后，迭代器将向后移动【输出数组的元素个数】个位置。
	const ArrayDimensions& 各维尺寸，输出数组各维尺寸。
	# 返回值
	T输出，指定类型的MATLAB数组
	*/
	template<MATLAB简单数组 T输出, 隐读迭代器<数组类型转元素<T输出>> T输入>
	inline T输出 万能转码(T输入&& 输入, const ArrayDimensions& 各维尺寸)
	{
		return 万能转码<T输出>((const T输入&)输入, 各维尺寸);
	}
	/*
	从C++迭代器输出到MATLAB数组。
	语法：Mex工具::万能转码<T输出>(输入,各维尺寸)
	# 示例
	```
	void* a = nullptr;
	void** b = &a;
	万能转码<CharArray>(b, { 1 });
	```
	# 参数
	typename T输出，MATLAB数组类型
	非const T输入& 输入，可读迭代器，输出值应当可显式转换为目标MATLAB类型。例如，任何指针类型都可以显式转换为任何整数类型。函数返回后，迭代器将向后移动【输出数组的元素个数】个位置。
	const ArrayDimensions& 各维尺寸，输出数组各维尺寸。
	# 返回值
	T输出，指定类型的MATLAB数组
	*/
	template<typename T输出, 非const T输入>
		requires MATLAB复杂数组<T输出>&& CanCreateArray<数组类型转元素<T输出>> || 显读迭代器<T输入, 数组类型转元素<T输出>>
	inline T输出 万能转码(T输入& 输入, const ArrayDimensions& 各维尺寸)
	{
		T输出 输出 = 数组工厂.createArray<数组类型转元素<T输出>>(各维尺寸);
		内部::通用迭代拷贝(输出, 输入);
		return 输出;
	}
	/*
	从C++迭代器输出到MATLAB数组。
	语法：Mex工具::万能转码<T输出>(输入,各维尺寸)
	# 示例
	```
	void* a = nullptr;
	万能转码<CharArray>(&a, { 1 });
	```
	# 参数
	typename T输出，MATLAB数组类型
	T输入&& 输入，可读迭代器，输出值应当可显式转换为目标MATLAB类型。例如，任何指针类型都可以显式转换为任何整数类型。
	const ArrayDimensions& 各维尺寸，输出数组各维尺寸。
	# 返回值
	T输出，指定类型的MATLAB数组
	*/
	template<typename T输出, typename T输入>
		requires MATLAB复杂数组<T输出>&& CanCreateArray<T输出> || 显读迭代器<T输入, 数组类型转元素<T输出>>
	inline T输出 万能转码(T输入&& 输入, const ArrayDimensions& 各维尺寸)
	{
		return 万能转码<T输出>(输入, 各维尺寸);
	}
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
	inline TypedArray<T输出> 万能转码(T输入& 输入, const ArrayDimensions& 各维尺寸)
	{
		TypedArray<T输出>输出 = 数组工厂.createArray<T输出>(各维尺寸);
		const size_t 元素个数 = 输出.getNumberOfElements();
		std::copy_n(输入, 元素个数, 输出.begin());
		内部::选择性求和(输入, 元素个数);
		return 输出;
	}
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
	inline TypedArray<T输出> 万能转码(T输入&& 输入, const ArrayDimensions& 各维尺寸)
	{
		return 万能转码((const T输入&)输入, 各维尺寸);
	}
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
		requires CanCreateArray<T输出>&& 非const<T输入>
	TypedArray<T输出> 万能转码(T输入& 输入, const ArrayDimensions& 各维尺寸)
	{
		TypedArray<T输出> 输出 = 数组工厂.createArray<T输出>(各维尺寸);
		for (Reference<T输出>a : 输出)
			a = (T输出)std::move(*(输入++));
		return 输出;
	}
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
		requires CanCreateArray<T输出>
	inline TypedArray<T输出> 万能转码(T输入&& 输入, const ArrayDimensions& 各维尺寸)
	{
		万能转码(输入, 各维尺寸);
	}
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
		requires 非const<T输入>
	TypedArray<T输出> 万能转码(T输入& 输入, const ArrayDimensions& 各维尺寸)
	{
		TypedArray<T输出>输出 = 数组工厂.createArray<T输出>(各维尺寸);
		for (T输出& a : 输出)
			a = (T输出) * (输入++);
		return 输出;
	}
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
	inline TypedArray<T输出> 万能转码(T输入&& 输入, const ArrayDimensions& 各维尺寸)
	{
		return 万能转码(输入, 各维尺寸);
	}
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
		requires 非const<T>
	StringArray 万能转码(T& 输入, const ArrayDimensions& 各维尺寸)
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
		return 输出;
	}
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
	inline StringArray 万能转码(T&& 输入, const ArrayDimensions& 各维尺寸)
	{
		return 万能转码(输入, 各维尺寸);
	}
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
		/*
		通过无类型指针访问MATLAB数组。此方法仅适用于POD类型的MATLAB数组。
		语法：Mex工具::动态类型缓冲::读取(MATLAB数组)
		# 参数
		Array&& MATLAB数组，要访问的MATLAB数组。此函数返回后，原数组变为不可用。
		# 返回值
		std::unique_ptr<动态类型缓冲>，请通过指针操作对象，因为内部实现是被子类继承的，直接取值会导致对象被截断。
		*/
		static std::unique_ptr<动态类型缓冲>读取(Array&& MATLAB数组);
		virtual ~动态类型缓冲() {}
		//打包后本对象变为不可用，所有数据封装在 MATLAB Array 中
		virtual Array 打包(ArrayDimensions 各维尺寸)noexcept = 0;
		virtual Array 打包()noexcept = 0;
	protected:
		动态类型缓冲(void* 指针, size_t 字节数) :指针(指针), 字节数(字节数) {}
	};
	//将对象指针加入自动析构表。clear mex 时此指针将被自动delete。只能对new创建的对象指针使用此方法。
	template<typename T>
	inline void 自动析构(T* 对象指针)noexcept
	{
		内部::自动析构表[对象指针] = [](void* 指针) {delete (T*)指针; };
	}
	//将对象指针加入自动析构表。clear mex 时此对象将被自动析构。使用指定的删除器。
	template<typename T>
	inline void 自动析构(T* 对象指针, std::move_only_function<void(void*)>&& 删除器)noexcept
	{
		内部::自动析构表[对象指针] = std::move(删除器);
	}
	//此方法用于提示指定对象已被自动析构，从而避免自动析构表重复析构。此方法不负责析构对象本身，对象本身仍由调用方负责析构。
	inline bool 手动析构(void* 对象指针)noexcept
	{
		return 内部::自动析构表.erase(对象指针);
	}
	//检查对象指针是否存在于自动析构表中。如不存在，此指针可能是无效的，或者创建时未加入自动析构表。
	inline bool 对象存在(void* 对象指针)noexcept
	{
		return 内部::自动析构表.contains(对象指针);
	}
	//使用此对象调用MATLAB方法。此对象由MEX工具定义，用户可在初始化方法内以及那之后使用，初始化之前不应使用该全局对象。
	extern matlab::engine::MATLABEngine& MATLAB引擎;
}