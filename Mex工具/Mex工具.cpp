#include "Mex工具.h"
namespace Mex工具
{
	void 异常输出补全(ArgumentList& outputs)
	{
		std::vector<Array>::iterator 头指针 = outputs.begin();
		const std::vector<Array>::iterator 尾指针 = outputs.end();
		while (++头指针 < 尾指针)
			*头指针 = 数组工厂.createEmptyArray();
	}
	template<>
	String 万能转码<String>(Array&& 输入)
	{
		switch (输入.getType())
		{
		case ArrayType::CHAR:
			return CharArray(std::move(输入)).toUTF16();
		case ArrayType::MATLAB_STRING:
			return StringArray(std::move(输入))[0];
		case ArrayType::CELL:
			return CharArray(Array(CellArray(std::move(输入))[0])).toUTF16();
		default:
			throw 此Array不能转换为String;
		}
	}
	template<>
	MATLABString 万能转码<MATLABString>(Array&& 输入)
	{
		switch (输入.getType())
		{
		case ArrayType::CHAR:
			return CharArray(std::move(输入)).toUTF16();
		case ArrayType::MATLAB_STRING:
			return StringArray(std::move(输入))[0];
		case ArrayType::CELL:
			return CharArray(Array(CellArray(std::move(输入))[0])).toUTF16();
		default:
			throw 此Array不能转换为MATLABString;
		}
	}
	template<>
	CharArray 万能转码<CharArray>(Array&& 输入)
	{
		switch (输入.getType())
		{
		case ArrayType::CHAR:
			return std::move(输入);
		case ArrayType::MATLAB_STRING:
			return 数组工厂.createCharArray(String(StringArray(std::move(输入))[0]));
		case ArrayType::CELL:
			return CellArray(std::move(输入))[0];
		default:
			throw 此Array不能转换为CharArray;
		}
	}
	template<>
	std::string 万能转码<std::string>(Array&& 输入)
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
			const String 字符串 = StringArray(std::move(输入))[0];
			输出.resize_and_overwrite((字符串.size() + 1) * 3, [&字符串](char* 指针, size_t 尺寸)
				{
					return WideCharToMultiByte(CP_UTF8, 0, (wchar_t*)字符串.c_str(), -1, 指针, 尺寸, nullptr, nullptr) - 1;
				});
		}
		break;
		case ArrayType::CELL:
		{
			CharArray 字符数组(Array(CellArray(std::move(输入))[0]));
			const int 长度 = 字符数组.getNumberOfElements();
			const buffer_ptr_t<char16_t>缓冲 = 字符数组.release();
			输出.resize_and_overwrite((长度 + 1) * 3, [&缓冲, 长度](char* 指针, size_t 尺寸)
				{
					return WideCharToMultiByte(CP_UTF8, 0, (wchar_t*)缓冲.get(), 长度, 指针, 尺寸, nullptr, nullptr) - 1;
				});
		}
		break;
		default:
			throw 此Array不能转换为stdstring;
		}
		return 输出;
	}
	template<>
	StringArray 万能转码<StringArray>(Array&& 输入)
	{
		switch (输入.getType())
		{
		case ArrayType::CHAR:
		{
			StringArray 输出 = 数组工厂.createArray<MATLABString>({ 1 });
			输出[0] = CharArray(std::move(输入)).toUTF16();
			return 输出;
		}
		case ArrayType::MATLAB_STRING:
			return std::move(输入);
		case ArrayType::CELL:
		{
			CellArray 元胞(std::move(输入));
			StringArray 输出 = 数组工厂.createArray<MATLABString>(元胞.getDimensions());
			const size_t 元素个数 = 元胞.getNumberOfElements();
			for (size_t a = 0; a < 元素个数; ++a)
				输出[a] = CharArray(Array(std::move(元胞[a]))).toUTF16();
			return 输出;
		}
		default:
			throw 此Array不能转换为StringArray;
		}
	}
	template<>
	String 万能转码<String>(const char* 输入)
	{
		String 返回;
		const size_t 长度 = strlen(输入) + 1;
		返回.resize_and_overwrite(长度, [输入](char16_t* 指针, size_t 尺寸)
			{
				return MultiByteToWideChar(CP_UTF8, 0, 输入, -1, (wchar_t*)指针, 尺寸) - 1;
			});
		return 返回;
	}
	int 万能转码(Array&& 输入, char* const& 输出)
	{
		switch (输入.getType())
		{
		case ArrayType::CHAR:
		{
			const int 字数 = 输入.getNumberOfElements();
			const buffer_ptr_t<char16_t>缓冲 = CharArray(std::move(输入)).release();
			return WideCharToMultiByte(CP_UTF8, 0, (LPCWCH)缓冲.get(), 字数, 输出, 字数 * 3 + 1, nullptr, nullptr);
		}
		case ArrayType::MATLAB_STRING:
		{
			const String 字符串(StringArray(std::move(输入))[0]);
			return WideCharToMultiByte(CP_UTF8, 0, (LPCWCH)字符串.c_str(), -1, 输出, wcslen((wchar_t*)字符串.c_str()) * 3 + 1, nullptr, nullptr);
		}
		case ArrayType::CELL:
		{
			CharArray 字符数组(Array(CellArray(std::move(输入))[0]));
			const int 字数 = 字符数组.getNumberOfElements();
			const buffer_ptr_t<char16_t>缓冲 = std::move(字符数组).release();
			return WideCharToMultiByte(CP_UTF8, 0, (LPCWCH)缓冲.get(), 字数, 输出, 字数 * 3 + 1, nullptr, nullptr);
		}
		default:
			throw 此Array不能拷出为char;
		}
	}
	template<typename T>
	class 有类型缓冲 :public 动态类型缓冲
	{
		buffer_ptr_t<T>缓冲;
		const uint64_t 元素数;
		有类型缓冲(buffer_ptr_t<T>&& 缓冲, uint64_t 元素数) :动态类型缓冲(缓冲.get(), 元素数 * sizeof(T)), 缓冲(std::move(缓冲)), 元素数(元素数) {}
		Array 打包(ArrayDimensions 各维尺寸)noexcept override
		{
			return 数组工厂.createArrayFromBuffer(各维尺寸, std::move(缓冲));
		}
		Array 打包()noexcept override
		{
			return 打包({ 元素数,1 });
		}
	public:
		virtual ~有类型缓冲() {}
		static std::unique_ptr<动态类型缓冲>返回(uint64_t 元素数)
		{
			return std::unique_ptr<动态类型缓冲>(new 有类型缓冲<T>(数组工厂.createBuffer<T>(元素数), 元素数));
		}
	};
	template<typename T>
	inline bool 不为零(T 数值)
	{
		return 数值;
	}
	template<>
	inline bool 不为零<std::complex<double>>(std::complex<double>数值)
	{
		return 数值 != std::complex<double>(0);
	}
	using T = double;
	//template<typename T>
	template<>
	class 有类型缓冲<SparseArray<T>> :public 动态类型缓冲
	{
		buffer_ptr_t<T>缓冲;
		const size_t 元素数;
		有类型缓冲(buffer_ptr_t<T>&& 缓冲, size_t 元素数) :动态类型缓冲(缓冲.get(), 元素数 * sizeof(T)), 缓冲(std::move(缓冲)), 元素数(元素数) {}
		Array 打包(ArrayDimensions 各维尺寸)noexcept override
		{
			struct 非零项
			{
				T 值;
				size_t 位置;
			};
			std::vector<非零项> 值列表;
			for (size_t a = 0; a < 元素数; ++a)
				if (不为零(缓冲[a])) [[unlikely]]
					值列表.emplace_back(缓冲[a], a);
			const size_t 值个数 = 值列表.size();
			buffer_ptr_t<T>data = 数组工厂.createBuffer<T>(值个数);
			buffer_ptr_t<size_t>rows = 数组工厂.createBuffer<size_t>(值个数);
			buffer_ptr_t<size_t>cols = 数组工厂.createBuffer<size_t>(值个数);
			const size_t 行数 = 各维尺寸[0];
			for (size_t a = 0; a < 值个数; ++a)
			{
				const 非零项& 项 = 值列表[a];
				data[a] = 项.值;
				cols[a] = 项.位置 / 行数;
				rows[a] = 项.位置 % 行数;
			}
			return 数组工厂.createSparseArray(各维尺寸, 值个数, std::move(data), std::move(rows), std::move(cols));
		}
		Array 打包()noexcept override
		{
			return 打包({ 元素数,1 });
		}
	public:
		virtual ~有类型缓冲() {}
		static std::unique_ptr<动态类型缓冲>返回(size_t 元素数)
		{
			return std::unique_ptr<动态类型缓冲>(new 有类型缓冲<SparseArray<T>>(数组工厂.createBuffer<T>(元素数), 元素数));
		}
	};
	template<>
	struct 有类型缓冲<void>
	{
		static std::unique_ptr<动态类型缓冲>返回(size_t 元素数)
		{
			throw 不支持的类型;
		}
	};
	//不能inline，会导致无法链接
	std::unique_ptr<动态类型缓冲>动态类型缓冲::创建(ArrayType 类型, size_t 元素数)
	{
		return 动态类型选择模板<有类型缓冲>(类型)(元素数);
	}
}