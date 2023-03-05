#include "Mex工具.h"
#include<Windows.h>
namespace Mex工具
{
	template<>
	String 万能转码<String>(Array&& 输入)
	{
		switch (输入.getType())
		{
		case ArrayType::CHAR:
			return CharArray(std::move(输入)).toUTF16();
		case ArrayType::MATLAB_STRING:
			return (StringArray(std::move(输入))[0]);
		[[unlikely]] default:
			throw InvalidArrayTypeException("输入不是字符串");
		}
	}
	template String 万能转码<String>(Array&& 输入);
	template<>
	CharArray 万能转码<CharArray>(Array&& 输入)
	{
		switch (输入.getType())
		{
		case ArrayType::CHAR:
			return std::move(输入);
		case ArrayType::MATLAB_STRING:
			return 数组工厂.createCharArray(String(StringArray(std::move(输入))[0]));
		[[unlikely]] default:
			throw InvalidArrayTypeException("输入不是字符串");
		}
	}
	template CharArray 万能转码<CharArray>(Array&& 输入);
	void 异常输出补全(ArgumentList& outputs)
	{
		std::vector<Array>::iterator 头指针 = outputs.begin();
		const std::vector<Array>::iterator 尾指针 = outputs.end();
		while (++头指针 < 尾指针)
			*头指针 = 数组工厂.createEmptyArray();
	}
	template<>
	std::string 万能转码<std::string>(Array&& 输入)
	{
		const String 字符串 = 万能转码<String>(std::move(输入));
		std::string 返回;
		返回.resize_and_overwrite((字符串.size() + 1) * 3, [&字符串](char* 指针, size_t 尺寸)
			{
				return WideCharToMultiByte(CP_UTF8, 0, (wchar_t*)字符串.c_str(), -1, 指针, 尺寸, nullptr, nullptr) - 1;
			});
		return 返回;
	}
	template std::string 万能转码<std::string>(Array&& 输入);
	template<>
	std::unique_ptr<std::string[]> 万能转码<std::unique_ptr<std::string[]>>(Array&& 输入)
	{
		std::unique_ptr<std::string[]> 输出数组;
		switch (输入.getType())
		{
		case ArrayType::CELL:
		{
			输出数组 = std::make_unique<std::string[]>(输入.getNumberOfElements());
			std::string* 头指针 = 输出数组.get();
			for (Array&& 元胞 : CellArray(std::move(输入)))
			{
				const String 字符串 = CharArray(std::move(元胞)).toUTF16();
				(头指针++)->resize_and_overwrite((字符串.size() + 1) * 3, [&字符串](char* 指针, size_t 尺寸)
					{
						return WideCharToMultiByte(CP_UTF8, 0, (wchar_t*)字符串.c_str(), -1, 指针, 尺寸, nullptr, nullptr) - 1;
					});
			}
		}
		break;
		case ArrayType::CHAR:
		{
			输出数组 = std::make_unique<std::string[]>(1);
			const String 字符串 = CharArray(std::move(输入)).toUTF16();
			输出数组[0].resize_and_overwrite((字符串.size() + 1) * 3, [&字符串](char* 指针, size_t 尺寸)
				{
					return WideCharToMultiByte(CP_UTF8, 0, (wchar_t*)字符串.c_str(), -1, 指针, 尺寸, nullptr, nullptr) - 1;
				});
		}
		break;
		case ArrayType::MATLAB_STRING:
		{
			输出数组 = std::make_unique<std::string[]>(输入.getNumberOfElements());
			std::string* 头指针 = 输出数组.get();
			for (String&& 字符串 : StringArray(std::move(输入)))
				(头指针++)->resize_and_overwrite((字符串.size() + 1) * 3, [&字符串](char* 指针, size_t 尺寸)
					{
						return WideCharToMultiByte(CP_UTF8, 0, (wchar_t*)字符串.c_str(), -1, 指针, 尺寸, nullptr, nullptr) - 1;
					});
		}
		break;
		[[unlikely]] default:
			throw InvalidArrayTypeException("输入不是字符串");
		}
		return 输出数组;
	}
	template std::unique_ptr<std::string[]> 万能转码<std::unique_ptr<std::string[]>>(Array&& 输入);
	template<typename T>
	class 有类型缓冲 :public 无类型缓冲
	{
		buffer_ptr_t<T>缓冲;
		const uint64_t 元素数;
		有类型缓冲(buffer_ptr_t<T>&& 缓冲, uint64_t 元素数) :无类型缓冲(缓冲.get(), 元素数 * sizeof(T)), 缓冲(std::move(缓冲)), 元素数(元素数) {}
		Array 打包()noexcept override
		{
			return 数组工厂.createArrayFromBuffer({ 元素数,1 }, std::move(缓冲));
		}
	public:
		virtual ~有类型缓冲() {}
		static std::unique_ptr<无类型缓冲>返回(uint64_t 元素数)
		{
			return std::unique_ptr<无类型缓冲>(new 有类型缓冲<T>(数组工厂.createBuffer<T>(元素数), 元素数));
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
	template<typename T>
	class 有类型缓冲<SparseArray<T>> :public 无类型缓冲
	{
		buffer_ptr_t<T>缓冲;
		const uint64_t 元素数;
		有类型缓冲(buffer_ptr_t<T>&& 缓冲, uint64_t 元素数) :无类型缓冲(缓冲.get(), 元素数 * sizeof(T)), 缓冲(std::move(缓冲)), 元素数(元素数) {}
		Array 打包()noexcept override
		{
			struct DataRow
			{
				T Data;
				uint64_t Row;
			};
			std::vector<DataRow> SparseVector;
			for (uint64_t a = 0; a < 元素数; ++a)
				if (不为零(缓冲[a])) [[unlikely]]
					SparseVector.emplace_back(缓冲[a], a);
			const uint64_t nnz = SparseVector.size();
			buffer_ptr_t<T>data = 数组工厂.createBuffer<T>(nnz);
			buffer_ptr_t<size_t>rows = 数组工厂.createBuffer<size_t>(nnz);
			buffer_ptr_t<size_t>cols = 数组工厂.createBuffer<size_t>(nnz);
			for (uint64_t a = 0; a < nnz; ++a)
			{
				data[a] = SparseVector[a].Data;
				rows[a] = SparseVector[a].Row;
			}
			std::fill_n(cols.get(), nnz, 1);
			return 数组工厂.createSparseArray({ 元素数,1 }, nnz, std::move(data), std::move(rows), std::move(cols));
		}
	public:
		virtual ~有类型缓冲() {}
		static std::unique_ptr<无类型缓冲>返回(uint64_t 元素数)
		{
			return std::unique_ptr<无类型缓冲>(new 有类型缓冲<SparseArray<T>>(数组工厂.createBuffer<T>(元素数), 元素数));
		}
	};
	template<>
	struct 有类型缓冲<nullptr_t>
	{
		static std::unique_ptr<无类型缓冲>返回(uint64_t 元素数)
		{
			throw 不支持的类型;
		}
	};
	//不能inline，会导致无法链接
	std::unique_ptr<无类型缓冲>无类型缓冲::创建(ArrayType 类型, uint64_t 元素数)
	{
		return 动态类型选择模板<有类型缓冲>(类型)(元素数);
	}
	template<typename T>
	class 类型包装器 :public 无类型数组
	{
		TypedArray<T>数组;
		类型包装器(Array&& 数组) :无类型数组(数组.getNumberOfElements() * sizeof(T)), 数组(std::move(数组)) {}
		void 拷贝(void* 输出)const override
		{
			try
			{
				std::copy(数组.cbegin(), 数组.cend(), (T*)输出);
			}
			catch (...)
			{
				throw 内存拷贝失败;
			}
		}
	public:
		virtual ~类型包装器() {}
		static std::unique_ptr<无类型数组>返回(Array&& 数组)
		{
			return std::unique_ptr<无类型数组>(new 类型包装器<T>(std::move(数组)));
		}
	};
	template<typename T>
	class 类型包装器<SparseArray<T>> :public 无类型数组
	{
		SparseArray<T>数组;
		类型包装器(Array&& 数组) :无类型数组(数组.getNumberOfElements() * sizeof(T)), 数组(std::move(数组)) {}
		void 拷贝(void* 输出)const override
		{
			try
			{
				std::copy(数组.cbegin(), 数组.cend(), (T*)输出);
			}
			catch (...)
			{
				throw 内存拷贝失败;
			}
		}
	public:
		virtual ~类型包装器() {}
		static std::unique_ptr<无类型数组>返回(Array&& 数组)
		{
			return std::unique_ptr<无类型数组>(new 类型包装器<SparseArray<T>>(std::move(数组)));
		}
	};
	template<>
	struct 类型包装器<nullptr_t>
	{
		static std::unique_ptr<无类型数组>返回(Array&& 数组)
		{
			throw 不支持的类型;
		}
	};
	template<>
	inline void 类型包装器<Enumeration>::拷贝(void* 输出)const
	{
		throw 枚举类型不能拷贝;
	}
	//不能inline，会导致无法链接
	std::unique_ptr<无类型数组> 无类型数组::创建(Array&& 数组)
	{
		return 动态类型选择模板<类型包装器>(数组.getType())(std::move(数组));
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
	template String 万能转码<String>(const char*);
	StringArray 万能转码(const std::string* UTF8, size_t 个数)
	{
		StringArray 返回 = 数组工厂.createArray<MATLABString>({ 个数 });
		for (size_t a = 0; a < 个数; ++a)
		{
			String 输出;
			const std::string& 输入 = UTF8[a];
			输出.resize_and_overwrite(输入.size() + 1, [输入](char16_t* 指针, size_t 尺寸)
				{
					return MultiByteToWideChar(CP_UTF8, 0, 输入.c_str(), -1, (wchar_t*)指针, 尺寸) - 1;
				});
			返回[a] = std::move(输出);
		}
		return 返回;
	}
	StringArray 万能转码(const char* const* UTF8, size_t 个数)
	{
		StringArray 返回 = 数组工厂.createArray<MATLABString>({ 个数 });
		for (size_t a = 0; a < 个数; ++a)
		{
			String 输出;
			const char* const 输入 = UTF8[a];
			输出.resize_and_overwrite(strlen(输入) + 1, [输入](char16_t* 指针, size_t 尺寸)
				{
					return MultiByteToWideChar(CP_UTF8, 0, 输入, -1, (wchar_t*)指针, 尺寸) - 1;
				});
			返回[a] = std::move(输出);
		}
		return 返回;
	}
}