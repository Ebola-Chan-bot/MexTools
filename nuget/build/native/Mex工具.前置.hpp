#pragma once
struct MexFunction :matlab::mex::Function
{
	MexFunction();
	void operator()(matlab::mex::ArgumentList outputs, matlab::mex::ArgumentList inputs)override;
	virtual ~MexFunction();
};
namespace Mex工具
{
	extern matlab::data::ArrayFactory 数组工厂;
	extern std::shared_ptr<matlab::engine::MATLABEngine> MATLAB引擎;
	template<typename T>
	[[noreturn]] void EnumThrow(T 异常);
	namespace 内部 {
		using namespace matlab::data;
		int WCTMB(const wchar_t* 宽字符串, int 宽字符数, char* 字节缓冲, int 缓冲长度);
		template<typename T, typename 目标>
		concept 可以显式转换到 = requires(目标 & 输出, T && 输入) { 输出 = (目标)std::move(输入); };
		template<typename T, typename 目标>
		concept 必须显式转换到 = !std::convertible_to<T, 目标>&& 可以显式转换到<T, 目标>;
		template<typename T, typename 目标>
		concept 必须MATLAB转换到 = !(std::convertible_to<T, 目标> && 可以显式转换到<T, 目标>);
		template<typename T>
		const std::u16string MATLAB转换函数 = u"";
		template<>
		const std::u16string MATLAB转换函数<bool> = u"logical";
		template<>
		const std::u16string MATLAB转换函数<char16_t> = u"char";
		template<>
		const std::u16string MATLAB转换函数<MATLABString> = u"string";
		template<>
		const std::u16string MATLAB转换函数<double> = u"double";
		template<>
		const std::u16string MATLAB转换函数<float> = u"single";
		template<>
		const std::u16string MATLAB转换函数<int8_t> = u"int8";
		template<>
		const std::u16string MATLAB转换函数<int16_t> = u"int16";
		template<>
		const std::u16string MATLAB转换函数<int32_t> = u"int32";
		template<>
		const std::u16string MATLAB转换函数<int64_t> = u"int64";
		template<>
		const std::u16string MATLAB转换函数<uint8_t> = u"uint8";
		template<>
		const std::u16string MATLAB转换函数<uint16_t> = u"uint16";
		template<>
		const std::u16string MATLAB转换函数<uint32_t> = u"uint32";
		template<>
		const std::u16string MATLAB转换函数<uint64_t> = u"uint64";
		template<typename T>
		const std::u16string MATLAB转换函数<std::complex<T>> = u"complex";
		template<typename T>
		const std::u16string MATLAB转换函数<SparseArray<T>> = u"sparse";
		template <class _Ty, class... _Types>
		_INLINE_VAR constexpr bool _Is_any_of_v = // true if and only if _Ty is in _Types
#if _HAS_CXX17
		(std::is_same_v<std::remove_cvref_t<_Ty>, _Types> || ...);
#else // ^^^ _HAS_CXX17 / !_HAS_CXX17 vvv
			std::disjunction_v<std::is_same<std::remove_cvref_t<_Ty>, _Types>...>;
#endif // ^^^ !_HAS_CXX17 ^^^
		template<typename T>
		concept 字符串类型 = _Is_any_of_v<T, CharArray, MATLABString, String, StringArray, std::string, std::wstring, CellArray, char, char16_t, wchar_t, std::string_view, std::wstring_view, std::u16string_view, char*, wchar_t*, char16_t*>;
		template<typename T>
		concept 可以宽字符输出 = requires(std::wostringstream 输出, T && 输入) { 输出 << std::move(输入); };
		template<typename T>
		concept 可以窄字符输出 = requires(std::ostringstream 输出, T && 输入) { 输出 << std::move(输入); };
		template<typename T>
		concept 宽字符输出 = !字符串类型<T> && 可以宽字符输出<T>;
		template<typename T>
		concept 窄字符输出 = !(字符串类型<T> || 可以宽字符输出<T>) && 可以窄字符输出<T>;
		template<typename T>
		concept 需要MATLAB转换字符串 = !(字符串类型<T> || 可以宽字符输出<T> || 可以窄字符输出<T>);
		template<typename T>
		using 取迭代器值类型 = std::remove_cvref_t<decltype(*std::declval<T>())>;
		template<typename T, typename...值类型>
		concept 迭代器值类型是 = _Is_any_of_v<取迭代器值类型<T>, 值类型...>;
		template<typename T>struct 数组类型转元素_s;
		template<typename T>struct 数组类型转元素_s<TypedArray<T>> { using type = T; };
		template<typename T>struct 数组类型转元素_s<SparseArray<T>> { using type = T; };
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
		template<typename T>constexpr size_t 类型字节数_v = sizeof(T);
		template<>constexpr size_t 类型字节数_v<void> = 0;
		template<typename T>struct 类型字节数_s;
		template<std::underlying_type_t<ArrayType>...T>
		struct 类型字节数_s<std::integer_sequence<std::underlying_type_t<ArrayType>, T...>>
		{
			static constexpr size_t value[] = { 类型字节数_v<typename 动态类型转静态_s<(ArrayType)T>::type>... };
		};

		//万能转码实现

		template<typename T>
		concept 可以MATLAB元素转换 = requires(T && 输入) { MATLAB引擎->feval<String>(MATLAB转换函数<void>, std::move(输入)); };
		template<typename T>
		concept 可以MATLAB数组转换 = requires(T && 输入) { MATLAB引擎->feval(MATLAB转换函数<void>, std::move(输入)); };
		template<typename T>
		concept 需要MATLAB元素转字符串 = 需要MATLAB转换字符串<T> && 可以MATLAB元素转换<T>;
		template<typename T>
		concept 需要MATLAB数组转字符串 = 需要MATLAB转换字符串<T> && 可以MATLAB数组转换<T>;
		template<typename T, typename 目标>
		concept 必须MATLAB元素转换到 = 必须MATLAB转换到<T, 目标>&& 可以MATLAB元素转换<T>;
		template<typename T, typename 目标>
		concept 必须MATLAB数组转换到 = 必须MATLAB转换到<T, 目标>&& 可以MATLAB数组转换<T>;

		template<typename 输出>
		struct 标量转换
		{
			template<template<typename>typename 是否稀疏, std::convertible_to<输出> 元素类型>
			输出 operator()(const 是否稀疏<元素类型>& 输入)
			{
				return 输入[0].operator 元素类型();
			}
			template<template<typename>typename 是否稀疏, 必须显式转换到<输出> 元素类型>
			输出 operator()(const 是否稀疏<元素类型>& 输入)
			{
				return (输出)输入[0].operator 元素类型();
			}
			template<template<typename>typename 是否稀疏, 必须MATLAB转换到<输出> 元素类型>
			输出 operator()(const 是否稀疏<元素类型>& 输入)
			{
				return MATLAB引擎->feval(MATLAB转换函数<输出>, std::move(输入))[0];
			}
		};

		template<typename 输出>
		struct 标量转换<TypedArray<输出>>
		{
			template<std::convertible_to<输出> T>static TypedArray<输出>转换(T&& 输入) { return 数组工厂.createScalar<输出>(std::move(输入)); }
			template<必须显式转换到<输出> T>static TypedArray<输出>转换(T&& 输入) { return 数组工厂.createScalar<输出>((输出)std::move(输入)); }
			template<必须MATLAB元素转换到<输出>T>
			static TypedArray<输出>转换(T&& 输入)
			{
				return MATLAB引擎->feval(MATLAB转换函数<输出>, 数组工厂.createScalar(std::move(输入)));
			}
			template<必须MATLAB数组转换到<输出>T>
			static TypedArray<输出>转换(T&& 输入)
			{
				return MATLAB引擎->feval(MATLAB转换函数<输出>, std::move(输入));
			}
		};

		template<>
		class 标量转换<CharArray>
		{
			static CharArray 转换(const char* 输入, size_t 长度);
			static CharArray 转换(const char16_t* 输入, size_t 长度)
			{
				matlab::data::buffer_ptr_t<char16_t>缓冲 = 数组工厂.createBuffer<char16_t>(长度);
				std::copy_n(输入, 长度, 缓冲.get());
				return 数组工厂.createArrayFromBuffer({ 1,长度 }, std::move(缓冲));
			}
		public:
			static CharArray 转换(CharArray&& 输入) { return std::move(输入); }
			static CharArray 转换(MATLABString&& 输入) { return 数组工厂.createCharArray(std::move(输入)); }
			static CharArray 转换(const CellArray& 输入) { return 输入[0]; }
			static CharArray 转换(const StringArray& 输入) { return 数组工厂.createCharArray(输入[0].operator String()); }
			static CharArray 转换(char 输入) { return 数组工厂.createScalar<char16_t>(输入); }
			static CharArray 转换(const char* 输入) { return 转换(输入, strlen(输入)); }
			static CharArray 转换(const std::string& 输入) { return 转换(输入.data(), 输入.size()); }
			static CharArray 转换(const std::string_view& 输入) { return 转换(输入.data(), 输入.size()); }
			static CharArray 转换(wchar_t 输入) { return 数组工厂.createScalar<char16_t>(输入); }
			static CharArray 转换(const wchar_t* 输入) { return 转换((char16_t*)输入, wcslen(输入)); }
			static CharArray 转换(const std::wstring& 输入) { return 转换((char16_t*)输入.data(), 输入.size()); }
			static CharArray 转换(const std::wstring_view& 输入) { return 转换((char16_t*)输入.data(), 输入.size()); }
			static CharArray 转换(char16_t 输入) { return 数组工厂.createScalar<char16_t>(输入); }
			static CharArray 转换(const char16_t* 输入) { return 转换(输入, wcslen((wchar_t*)输入)); }
			static CharArray 转换(String&& 输入) { return 数组工厂.createCharArray(std::move(输入)); }
			static CharArray 转换(const std::u16string_view& 输入) { return 转换(输入.data(), 输入.size()); }
			template<宽字符输出 T>
			static CharArray 转换(T&& 输入)
			{
				std::wostringstream 流;
				流 << std::move(输入);
				return 转换(流.str());
			}
			template<窄字符输出 T>
			static CharArray 转换(T&& 输入)
			{
				std::stringstream 流;
				流 << std::move(输入);
				return 转换(流.str());
			}
			template<需要MATLAB元素转字符串 T>
			static CharArray 转换(T&& 输入)
			{
				return 数组工厂.createCharArray(MATLAB引擎->feval<String>(MATLAB转换函数<MATLABString>, std::move(输入)));
			}
			template<需要MATLAB数组转字符串 T>
			static CharArray 转换(T&& 输入)
			{
				return 数组工厂.createCharArray(MATLAB引擎->feval(MATLAB转换函数<MATLABString>, std::move(输入))[0].operator String());
			}
		};
		template<>
		class 标量转换<String>
		{
			static String 转换(const char* 输入, size_t 长度);
		public:
			static String 转换(const CharArray& 输入) { return 输入.toUTF16(); }
			static String 转换(MATLABString&& 输入) { return std::move(输入); }
			static String 转换(const CellArray& 输入) { return 输入[0].operator CharArray().toUTF16(); }
			static String 转换(const StringArray& 输入) { return 输入[0]; }
			static String 转换(char 输入) { return String(1, 输入); }
			static String 转换(const char* 输入) { return 转换(输入, strlen(输入)); }
			static String 转换(const std::string& 输入) { return 转换(输入.data(), 输入.size()); }
			static String 转换(const std::string_view& 输入) { return 转换(输入.data(), 输入.size()); }
			static String 转换(wchar_t 输入) { return String(1, 输入); }
			static String 转换(const wchar_t* 输入) { return (char16_t*)输入; }
			static String 转换(const std::wstring& 输入) { return String((char16_t*)输入.data(), 输入.size()); }
			static String 转换(const std::wstring_view& 输入) { return String((char16_t*)输入.data(), 输入.size()); }
			static String 转换(char16_t 输入) { return String(1, 输入); }
			static String 转换(const char16_t* 输入) { return 输入; }
			static String 转换(String&& 输入) { return std::move(输入); }
			static String 转换(const std::u16string_view& 输入) { return String(输入); }//String不能从u16string_view移动构造
			template<宽字符输出 T>
			static String 转换(T&& 输入)
			{
				std::wostringstream 流;
				流 << std::move(输入);
				return 转换(流.str());
			}
			template<窄字符输出 T>
			static String 转换(T&& 输入)
			{
				std::stringstream 流;
				流 << std::move(输入);
				return 转换(流.str());
			}
			template<需要MATLAB元素转字符串 T>
			static String 转换(T&& 输入)
			{
				return MATLAB引擎->feval<String>(MATLAB转换函数<MATLABString>, std::move(输入));
			}
			template<需要MATLAB数组转字符串 T>
			static String 转换(T&& 输入)
			{
				return MATLAB引擎->feval(MATLAB转换函数<MATLABString>, std::move(输入))[0];
			}

		};
		template<>
		struct 标量转换<MATLABString>
		{
			static MATLABString 转换(MATLABString&& 输入) { return std::move(输入); }
			static MATLABString 转换(String&& 输入) { return std::move(输入); }
			template<需要MATLAB元素转字符串 T>
			static MATLABString 转换(T&& 输入)
			{
				return MATLAB引擎->feval<MATLABString>(MATLAB转换函数<MATLABString>, std::move(输入));
			}
			template<需要MATLAB数组转字符串 T>
			static MATLABString 转换(T&& 输入)
			{
				return MATLAB引擎->feval(MATLAB转换函数<MATLABString>, std::move(输入))[0];
			}
			template<typename T>
				requires (!(_Is_any_of_v<T, MATLABString, String> || 需要MATLAB转换字符串<T>))
			static MATLABString 转换(T&& 输入)
			{
				return 标量转换<String>::转换(std::move(输入));
			}
		};
		template<>
		struct 标量转换<StringArray>
		{
			static StringArray 转换(StringArray&& 输入) { return std::move(输入); }
			template<需要MATLAB数组转字符串 T>
			static StringArray 转换(T&& 输入)
			{
				return MATLAB引擎->feval(MATLAB转换函数<MATLABString>, std::move(输入));
			}
			template<typename T>
				requires(!(std::is_same_v<std::remove_cvref_t<T>, StringArray> || 需要MATLAB数组转字符串<T>))
			static StringArray 转换(T&& 输入)
			{
				return 数组工厂.createScalar(标量转换<String>::转换(std::move(输入)));
			}
		};
		template<typename 迭代器>
		class 迭代MC
		{
			迭代器& 输出;
			using 值类型 = 取迭代器值类型<迭代器>;
		public:
			迭代MC(迭代器& 输出) :输出(输出) {}
			template<std::convertible_to<值类型> T>
			void operator()(const TypedArray<T>& 输入)
			{
				输出 = std::copy(输入.cbegin(), 输入.cend(), 输出);
			}
			template<必须显式转换到<值类型>T>
			void operator()(const TypedArray<T>& 输入)
			{
				for (T a : 输入)
					*(输出++) = (值类型)a;
			}
			template<必须MATLAB元素转换到<值类型>T>
			void operator()(TypedArray<T>&& 输入)
			{
				operator()((TypedArray<值类型>)MATLAB引擎->feval(MATLAB转换函数<值类型>, std::move(输入)));
			}
			template<std::convertible_to<值类型> T>
			void operator()(const matlab::data::SparseArray<T>& 输入)
			{
				const TypedIterator<const T>迭代尾 = 输入.cend();
				const size_t 高度 = 输入.getDimensions().front();
				std::fill_n(输出, 输入.getNumberOfElements(), 0);
				for (TypedIterator<const T>a = 输入.cbegin(); a < 迭代尾; ++a)
				{
					const SparseIndex 索引 = 输入.getIndex(a);
					输出[索引.first + 索引.second * 高度] = *a;
				}
				输出 += 输入.getNumberOfElements();
			}
			template<必须显式转换到<值类型>T>
			void operator()(const matlab::data::SparseArray<T>& 输入)
			{
				const TypedIterator<const T>迭代尾 = 输入.cend();
				const size_t 高度 = 输入.getDimensions().front();
				std::fill_n(输出, 输入.getNumberOfElements(), 0);
				for (TypedIterator<const T>a = 输入.cbegin(); a < 迭代尾; ++a)
				{
					const SparseIndex 索引 = 输入.getIndex(a);
					输出[索引.first + 索引.second * 高度] = (值类型)*a;
				}
				输出 += 输入.getNumberOfElements();
			}
			template<必须MATLAB元素转换到<值类型>T>
			void operator()(matlab::data::SparseArray<T>&& 输入)
			{
				operator()((matlab::data::SparseArray<值类型>)MATLAB引擎->feval(MATLAB转换函数<值类型>, std::move(输入)));
			}
		};
		template<typename 迭代器>
		class 迭代MC<const 迭代器>
		{
			迭代器 输出;
			using 值类型 = 取迭代器值类型<迭代器>;
		public:
			迭代MC(const 迭代器& 输出) :输出(输出) {}
			template<std::convertible_to<值类型> T>
			void operator()(const TypedArray<T>& 输入)
			{
				std::copy(输入.cbegin(), 输入.cend(), 输出);
			}
			template<必须显式转换到<值类型>T>
			void operator()(const TypedArray<T>& 输入)
			{
				for (T a : 输入)
					*(输出++) = (值类型)a;
			}
			template<必须MATLAB元素转换到<值类型>T>
			void operator()(TypedArray<T>&& 输入)
			{
				operator()((TypedArray<值类型>)MATLAB引擎->feval(MATLAB转换函数<值类型>, std::move(输入)));
			}
			template<std::convertible_to<值类型> T>
			void operator()(const matlab::data::SparseArray<T>& 输入)
			{
				const TypedIterator<const T>迭代尾 = 输入.cend();
				const size_t 高度 = 输入.getDimensions().front();
				std::fill_n(输出, 输入.getNumberOfElements(), 0);
				for (TypedIterator<const T>a = 输入.cbegin(); a < 迭代尾; ++a)
				{
					const SparseIndex 索引 = 输入.getIndex(a);
					输出[索引.first + 索引.second * 高度] = *a;
				}
			}
			template<必须显式转换到<值类型>T>
			void operator()(const matlab::data::SparseArray<T>& 输入)
			{
				const TypedIterator<const T>迭代尾 = 输入.cend();
				const size_t 高度 = 输入.getDimensions().front();
				std::fill_n(输出, 输入.getNumberOfElements(), 0);
				for (TypedIterator<const T>a = 输入.cbegin(); a < 迭代尾; ++a)
				{
					const SparseIndex 索引 = 输入.getIndex(a);
					输出[索引.first + 索引.second * 高度] = (值类型)*a;
				}
			}
			template<必须MATLAB元素转换到<值类型>T>
			void operator()(matlab::data::SparseArray<T>&& 输入)
			{
				operator()((matlab::data::SparseArray<值类型>)MATLAB引擎->feval(MATLAB转换函数<值类型>, std::move(输入)));
			}
		};
		template<>
		class 迭代MC<void*>
		{
			void*& 输出;
		public:
			迭代MC(void*& 输出) :输出(输出) {}
			template<template<typename>typename 是否稀疏, typename T>
			void operator()(const 是否稀疏<T>& 输入)
			{
				迭代MC<T*>((T*&)输出)(输入);
			}
		};
		template<>
		class 迭代MC<void*const>
		{
			void* 输出;
		public:
			迭代MC(void*const& 输出) :输出(输出) {}
			template<template<typename>typename 是否稀疏, typename T>
			void operator()(const 是否稀疏<T>& 输入)
			{
				迭代MC<T*>((T*&)输出)(输入);
			}
		};
		template<迭代器值类型是<CharArray>迭代器>
		class 迭代MC<迭代器>
		{
			迭代器& 输出;
		public:
			迭代MC(迭代器& 输出) :输出(输出) {}
			void operator()(CharArray&& 输入)
			{
				*(输出++) = std::move(输入);
			}
			void operator()(const CellArray& 输入)
			{
				输出 = std::copy(输入.cbegin(), 输入.cend(), 输出);
			}
			void operator()(const StringArray& 输入)
			{
				for (String a : 输入)
					*(输出++) = 数组工厂.createCharArray(std::move(a));
			}
			template<typename T>
				requires (!_Is_any_of_v<T, CharArray, CellArray, StringArray>)
			void operator()(T&& 输入)
			{
				operator()(StringArray(MATLAB引擎->feval(MATLAB转换函数<MATLABString>, std::move(输入))));
			}
		};
		template<迭代器值类型是<CharArray>迭代器>
		class 迭代MC<const 迭代器>
		{
			迭代器 输出;
		public:
			迭代MC(const 迭代器& 输出) :输出(输出) {}
			void operator()(CharArray&& 输入)
			{
				*输出 = std::move(输入);
			}
			void operator()(const CellArray& 输入)
			{
				std::copy(输入.cbegin(), 输入.cend(), 输出);
			}
			void operator()(const StringArray& 输入)
			{
				for (String a : 输入)
					*(输出++) = 数组工厂.createCharArray(std::move(a));
			}
			template<typename T>
				requires (!_Is_any_of_v<T, CharArray, CellArray, StringArray>)
			void operator()(T&& 输入)
			{
				operator()(StringArray(MATLAB引擎->feval(MATLAB转换函数<MATLABString>, std::move(输入))));
			}
		};
		template<迭代器值类型是<String, MATLABString>迭代器>
		class 迭代MC<迭代器>
		{
			迭代器& 输出;
		public:
			迭代MC(迭代器& 输出) :输出(输出) {}
			void operator()(const CharArray& 输入)
			{
				输出++->resize_and_overwrite(输入.getNumberOfElements(), [&输入](char16_t* 指针, size_t 尺寸)
					{
						std::copy(输入.cbegin(), 输入.cend(), 指针);
						return 输入.getNumberOfElements();
					});
			}
			void operator()(const CellArray& 输入)
			{
				for (CharArray a : 输入)
					输出++->resize_and_overwrite(a.getNumberOfElements(), [&a](char16_t* 指针, size_t 尺寸)
						{
							std::copy(a.cbegin(), a.cend(), 指针);
							return a.getNumberOfElements();
						});
			}
			void operator()(const StringArray& 输入)
			{
				输出 = std::copy(输入.cbegin(), 输入.cend(), 输出);
			}
			template<typename T>
				requires (!_Is_any_of_v<T, CharArray, CellArray, StringArray>)
			void operator()(T&& 输入)
			{
				operator()(StringArray(MATLAB引擎->feval(MATLAB转换函数<MATLABString>, std::move(输入))));
			}
		};
		template<迭代器值类型是<String, MATLABString>迭代器>
		class 迭代MC<const 迭代器>
		{
			迭代器 输出;
		public:
			迭代MC(const 迭代器& 输出) :输出(输出) {}
			void operator()(const CharArray& 输入)
			{
				输出->resize_and_overwrite(输入.getNumberOfElements(), [&输入](char16_t* 指针, size_t 尺寸)
					{
						std::copy(输入.cbegin(), 输入.cend(), 指针);
						return 输入.getNumberOfElements();
					});
			}
			void operator()(const CellArray& 输入)
			{
				for (CharArray a : 输入)
					输出++->resize_and_overwrite(a.getNumberOfElements(), [&a](char16_t* 指针, size_t 尺寸)
						{
							std::copy(a.cbegin(), a.cend(), 指针);
							return a.getNumberOfElements();
						});
			}
			void operator()(const StringArray& 输入)
			{
				std::copy(输入.cbegin(), 输入.cend(), 输出);
			}
			template<typename T>
				requires (!_Is_any_of_v<T, CharArray, CellArray, StringArray>)
			void operator()(T&& 输入)
			{
				operator()(StringArray(MATLAB引擎->feval(MATLAB转换函数<MATLABString>, std::move(输入))));
			}
		};
		template<迭代器值类型是<std::string>迭代器>
		class 迭代MC<迭代器>
		{
			迭代器& 输出;
		public:
			迭代MC(迭代器& 输出) :输出(输出) {}
			void operator()(CharArray&& 输入)
			{
				const int 长度 = 输入.getNumberOfElements();
				输出++->resize_and_overwrite((长度 + 1) * 3, [宽指针 = (wchar_t*)CharArray(std::move(输入)).release().get(), 长度](char* 指针, size_t 尺寸)
					{
						return WCTMB(宽指针, 长度, 指针, 尺寸) - 1;
					});
			}
			void operator()(const CellArray& 输入)
			{
				for (CharArray& a : 输入)
				{
					const int 长度 = a.getNumberOfElements();
					输出++->resize_and_overwrite((长度 + 1) * 3, [宽指针 = (wchar_t*)a.release().get(), 长度](char* 指针, size_t 尺寸)
						{
							return WCTMB(宽指针, 长度, 指针, 尺寸) - 1;
						});
				}
			}
			void operator()(const StringArray& 输入)
			{
				for (const String& 字符串 : 输入)
				{
					输出++->resize_and_overwrite((字符串.size() + 1) * 3, [&字符串](char* 指针, size_t 尺寸)
						{
							return WCTMB((wchar_t*)字符串.data(), 字符串.size(), 指针, 尺寸) - 1;
						});
				}
			}
			template<typename T>
				requires (!_Is_any_of_v<T, CharArray, CellArray, StringArray>)
			void operator()(T&& 输入)
			{
				operator()(StringArray(MATLAB引擎->feval(MATLAB转换函数<MATLABString>, std::move(输入))));
			}
		};
		template<迭代器值类型是<std::string>迭代器>
		class 迭代MC<const 迭代器>
		{
			迭代器 输出;
		public:
			迭代MC(const 迭代器& 输出) :输出(输出) {}
			void operator()(CharArray&& 输入)
			{
				const int 长度 = 输入.getNumberOfElements();
				输出->resize_and_overwrite((长度 + 1) * 3, [宽指针 = (wchar_t*)CharArray(std::move(输入)).release().get(), 长度](char* 指针, size_t 尺寸)
					{
						return WCTMB(宽指针, 长度, 指针, 尺寸) - 1;
					});
			}
			void operator()(const CellArray& 输入)
			{
				for (CharArray& a : 输入)
				{
					const int 长度 = a.getNumberOfElements();
					输出++->resize_and_overwrite((长度 + 1) * 3, [宽指针 = (wchar_t*)a.release().get(), 长度](char* 指针, size_t 尺寸)
						{
							return WCTMB(宽指针, 长度, 指针, 尺寸) - 1;
						});
				}
			}
			void operator()(const StringArray& 输入)
			{
				for (const String& 字符串 : 输入)
				{
					输出++->resize_and_overwrite((字符串.size() + 1) * 3, [&字符串](char* 指针, size_t 尺寸)
						{
							return WCTMB((wchar_t*)字符串.data(), 字符串.size(), 指针, 尺寸) - 1;
						});
				}
			}
			template<typename T>
				requires (!_Is_any_of_v<T, CharArray, CellArray, StringArray>)
			void operator()(T&& 输入)
			{
				operator()(StringArray(MATLAB引擎->feval(MATLAB转换函数<MATLABString>, std::move(输入))));
			}
		};
		template<迭代器值类型是<std::wstring>迭代器>
		class 迭代MC<迭代器>
		{
			迭代器& 输出;
		public:
			迭代MC(迭代器& 输出) :输出(输出) {}
			void operator()(const CharArray& 输入)
			{
				输出++->resize_and_overwrite(输入.getNumberOfElements(), [&输入](wchar_t* 指针, size_t 尺寸)
					{
						std::copy(输入.cbegin(), 输入.cend(), 指针);
						return 输入.getNumberOfElements();
					});
			}
			void operator()(const CellArray& 输入)
			{
				for (CharArray a : 输入)
					输出++->resize_and_overwrite(a.getNumberOfElements(), [&a](wchar_t* 指针, size_t 尺寸)
						{
							std::copy(a.cbegin(), a.cend(), 指针);
							return a.getNumberOfElements();
						});
			}
			void operator()(const StringArray& 输入)
			{
				for (String a : 输入)
					输出++->resize_and_overwrite(a.size(), [&a](wchar_t* 指针, size_t 尺寸)
						{
							std::copy(a.cbegin(), a.cend(), 指针);
							return a.size();
						});
			}
			template<typename T>
				requires (!_Is_any_of_v<T, CharArray, CellArray, StringArray>)
			void operator()(T&& 输入)
			{
				operator()(StringArray(MATLAB引擎->feval(MATLAB转换函数<MATLABString>, std::move(输入))));
			}
		};
		template<迭代器值类型是<std::wstring>迭代器>
		class 迭代MC<const 迭代器>
		{
			迭代器 输出;
		public:
			迭代MC(const 迭代器& 输出) :输出(输出) {}
			void operator()(const CharArray& 输入)
			{
				输出->resize_and_overwrite(输入.getNumberOfElements(), [&输入](wchar_t* 指针, size_t 尺寸)
					{
						std::copy(输入.cbegin(), 输入.cend(), 指针);
						return 输入.getNumberOfElements();
					});
			}
			void operator()(const CellArray& 输入)
			{
				for (CharArray a : 输入)
					输出++->resize_and_overwrite(a.getNumberOfElements(), [&a](wchar_t* 指针, size_t 尺寸)
						{
							std::copy(a.cbegin(), a.cend(), 指针);
							return a.getNumberOfElements();
						});
			}
			void operator()(const StringArray& 输入)
			{
				for (String a : 输入)
					输出++->resize_and_overwrite(a.size(), [&a](wchar_t* 指针, size_t 尺寸)
						{
							std::copy(a.cbegin(), a.cend(), 指针);
							return a.size();
						});
			}
			template<typename T>
				requires (!_Is_any_of_v<T, CharArray, CellArray, StringArray>)
			void operator()(T&& 输入)
			{
				operator()(StringArray(MATLAB引擎->feval(MATLAB转换函数<MATLABString>, std::move(输入))));
			}
		};
		template<typename T>
		struct 迭代CM
		{
			template<typename 迭代器>
			static TypedArray<T>转换(迭代器& 输入, ArrayDimensions&& 各维尺寸)
			{
				TypedArray<T> 输出 = 数组工厂.createArray<T>(std::move(各维尺寸));
				const 迭代器 输入尾 = 输入 + 输出.getNumberOfElements();
				std::transform(输入, 输入尾, 输出.begin(), [](取迭代器值类型<迭代器>&& a) {return (T)std::move(a); });
				输入 = 输入尾;
				return 输出;
			}
			template<typename 迭代器>
			static TypedArray<T>转换(const 迭代器& 输入, ArrayDimensions&& 各维尺寸)
			{
				TypedArray<T> 输出 = 数组工厂.createArray<T>(std::move(各维尺寸));
				std::transform(输入, 输入 + 输出.getNumberOfElements(), 输出.begin(), [](取迭代器值类型<迭代器>&& a) {return (T)std::move(a); });
				return 输出;
			}
			template<typename 迭代器>
				requires std::convertible_to<取迭代器值类型<迭代器>, T>
			static TypedArray<T>转换(迭代器& 输入, ArrayDimensions&& 各维尺寸)
			{
				TypedArray<T> 输出 = 数组工厂.createArray<T>(std::move(各维尺寸));
				const 迭代器 输入尾 = 输入 + 输出.getNumberOfElements();
				std::copy(输入, 输入尾, 输出.begin());
				输入 = 输入尾;
				return 输出;
			}
			template<typename 迭代器>
				requires std::convertible_to<取迭代器值类型<迭代器>, T>
			static TypedArray<T>转换(const 迭代器& 输入, ArrayDimensions&& 各维尺寸)
			{
				TypedArray<T> 输出 = 数组工厂.createArray<T>(std::move(各维尺寸));
				std::copy(输入, 输入 + 输出.getNumberOfElements(), 输出.begin());
				return 输出;
			}
			static TypedArray<T>转换(void*& 输入, ArrayDimensions&& 各维尺寸)
			{
				return 转换((T*&)输入, std::move(各维尺寸));
			}
			static TypedArray<T>转换(void*const& 输入, ArrayDimensions&& 各维尺寸)
			{
				return 转换((T*)输入, std::move(各维尺寸));
			}
		};
		template<>
		struct 迭代CM<MATLABString>
		{
			template<迭代器值类型是<String, MATLABString>迭代器>
			static StringArray 转换(迭代器& 输入, ArrayDimensions&& 各维尺寸)
			{
				StringArray 输出 = 数组工厂.createArray<MATLABString>(std::move(各维尺寸));
				const 迭代器 输入尾 = 输入 + 输出.getNumberOfElements();
				std::copy(输入, 输入尾, 输出.begin());
				输入 = 输入尾;
				return 输出;
			}
			template<typename 迭代器>
			static StringArray 转换(迭代器& 输入, ArrayDimensions&& 各维尺寸)
			{
				StringArray 输出 = 数组工厂.createArray<MATLABString>(std::move(各维尺寸));
				const 迭代器 输入尾 = 输入 + 输出.getNumberOfElements();
				std::transform(输入, 输入尾, 输出.begin(), 标量转换<MATLABString>::转换<取迭代器值类型<迭代器>>);
				输入 = 输入尾;
				return 输出;
			}
			static StringArray 转换(void*& 输入, ArrayDimensions&& 各维尺寸)
			{
				return 转换((MATLABString*&)输入, std::move(各维尺寸));
			}
			template<迭代器值类型是<String, MATLABString>迭代器>
			static StringArray 转换(const 迭代器& 输入, ArrayDimensions&& 各维尺寸)
			{
				StringArray 输出 = 数组工厂.createArray<MATLABString>(std::move(各维尺寸));
				std::copy(输入, 输入 + 输出.getNumberOfElements(), 输出.begin());
				return 输出;
			}
			template<typename 迭代器>
			static StringArray 转换(const 迭代器& 输入, ArrayDimensions&& 各维尺寸)
			{
				StringArray 输出 = 数组工厂.createArray<MATLABString>(std::move(各维尺寸));
				std::transform(输入, 输入 + 输出.getNumberOfElements(), 输出.begin(), 标量转换<MATLABString>::转换<取迭代器值类型<迭代器>>);
				return 输出;
			}
			static StringArray 转换(void*const& 输入, ArrayDimensions&& 各维尺寸)
			{
				return 转换((MATLABString*)输入, std::move(各维尺寸));
			}
		};
		template<typename T>
		struct 动态CM
		{
			template<typename 迭代器>
			[[noreturn]] static Array value(迭代器& 输入, ArrayDimensions&& 各维尺寸)
			{
				EnumThrow(MexTools::Unsupported_type);
			}
		};
		template<typename T>
			requires requires {数组工厂.createArray<T>({ 1 }); }
		struct 动态CM<T>
		{
			template<typename 迭代器>
			static Array value(迭代器&& 输入, ArrayDimensions&& 各维尺寸)
			{
				return 迭代CM<T>::转换(std::forward<迭代器>(输入), std::move(各维尺寸));
			}
		};
		template<typename T>
		struct 指针转动态数组
		{
			[[noreturn]]static Array value(ArrayDimensions&& 各维尺寸, void* 指针, buffer_deleter_t 自定义删除器)
			{
				EnumThrow(MexTools::Unsupported_type);
			}
		};
		template<typename T>
			requires requires{std::declval<matlab::data::buffer_ptr_t<T>>(); }
		struct 指针转动态数组<T>
		{
			static Array value(ArrayDimensions&& 各维尺寸, void* 指针, buffer_deleter_t 自定义删除器)
			{
				return 数组工厂.createArrayFromBuffer(std::move(各维尺寸), buffer_ptr_t<T>((T*)指针, 自定义删除器));
			}
		};
		template<typename T>
		struct 数值标准化
		{
			using type = T;
		};
		template<>
		struct 数值标准化<__int8>
		{
			using type = int8_t;
		};
		template<>
		struct 数值标准化<unsigned __int8>
		{
			using type = uint8_t;
		};
		template<>
		struct 数值标准化<__int16>
		{
			using type = int16_t;
		};
		template<>
		struct 数值标准化<unsigned __int16>
		{
			using type = uint16_t;
		};
		template<>
		struct 数值标准化<__int32>
		{
			using type = int32_t;
		};
		template<>
		struct 数值标准化<unsigned __int32>
		{
			using type = uint32_t;
		};
		template<>
		struct 数值标准化<__int64>
		{
			using type = int64_t;
		};
		template<>
		struct 数值标准化<unsigned __int64>
		{
			using type = uint64_t;
		};
		template<typename T>
		using 数值标准化_t = typename 数值标准化<std::remove_cvref_t<T>>::type;
		template<typename T>
		concept 可写入UTF8流 = requires(std::ostringstream 流, T 消息) { 流 << 消息; };
		template<typename T>
		concept 可写入UTF16流 =requires(std::basic_ostringstream<char16_t> 流, T 消息) { 流 << 消息; };
		template<bool 标识符添加到消息>
		struct EnumThrow_s;
		template<>
		struct EnumThrow_s<true>
		{
			template<typename 枚举类型,
			[[noreturn]]static void Throw
		};
	}
}