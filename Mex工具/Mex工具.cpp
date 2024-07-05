#include "Mex工具.h"
using namespace matlab::mex;
using namespace matlab::data;
void Mex工具::异常输出补全(ArgumentList& outputs)
{
	std::vector<Array>::iterator 头指针 = outputs.begin();
	const std::vector<Array>::iterator 尾指针 = outputs.end();
	while (++头指针 < 尾指针)
		*头指针 = 数组工厂.createEmptyArray();
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
		((const Array&)输入)[0].operator CharArray().toUTF16();
	default:
		throw Mex异常::Array_cannot_convert_to_String;
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
		throw Mex异常::Array_cannot_convert_to_MATLABString;
	}
}
template<>
CharArray Mex工具::万能转码<CharArray>(Array&& 输入)
{
	switch (输入.getType())
	{
	case ArrayType::CHAR:
		return 输入;
	case ArrayType::MATLAB_STRING:
		return 数组工厂.createCharArray(((const Array&)输入)[0].operator String());
	case ArrayType::CELL:
		return ((const Array&)输入)[0];
	default:
		throw Mex异常::Array_cannot_convert_to_CharArray;
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
template<>
StringArray Mex工具::万能转码<StringArray>(Array&& 输入)
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
		return 输入;
	case ArrayType::CELL:
	{
		StringArray 输出 = 数组工厂.createArray<MATLABString>(输入.getDimensions());
		const size_t 元素个数 = 输入.getNumberOfElements();
		for (size_t a = 0; a < 元素个数; ++a)
			输出[a] = ((const Array&)输入)[a].operator CharArray().toUTF16();
		return 输出;
	}
	default:
		throw Mex异常::Array_cannot_convert_to_StringArray;
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
		const String 字符串 = std::move(输入[0]);
		return WideCharToMultiByte(CP_UTF8, 0, (LPCWCH)字符串.c_str(), -1, 输出, wcslen((wchar_t*)字符串.c_str()) * 3 + 1, nullptr, nullptr);
	}
	case ArrayType::CELL:
	{
		CharArray 字符数组 = std::move(输入[0]);
		const int 字数 = 字符数组.getNumberOfElements();
		const buffer_ptr_t<char16_t>缓冲 = 字符数组.release();
		return WideCharToMultiByte(CP_UTF8, 0, (LPCWCH)缓冲.get(), 字数, 输出, 字数 * 3 + 1, nullptr, nullptr);
	}
	default:
		throw Mex异常::此Array不能拷出为char;
	}
}
template<typename T>
struct 有类型缓冲 :public 动态类型缓冲
{
	有类型缓冲(buffer_ptr_t<T>&& 缓冲, uint64_t 元素数) :动态类型缓冲(缓冲.get(), 元素数 * sizeof(T)), 缓冲(std::move(缓冲)), 元素数(元素数) {}
	virtual ~有类型缓冲() {}
protected:
	buffer_ptr_t<T>缓冲;
	const uint64_t 元素数;
	Array 打包(ArrayDimensions 各维尺寸)noexcept override
	{
		return 数组工厂.createArrayFromBuffer(各维尺寸, std::move(缓冲));
	}
	Array 打包()noexcept override
	{
		return 打包({ 元素数,1 });
	}
};
template<typename T>
struct 有类型缓冲创建器
{
	static std::unique_ptr<动态类型缓冲>返回(uint64_t 元素数)
	{
		throw Mex异常::不支持的类型;
	}
};
template<typename T>
struct 有类型缓冲读取器
{
	static std::unique_ptr<动态类型缓冲>返回(Array&& 数组)
	{
		throw Mex异常::不支持的类型;
	}
};
template<MATLAB简单元素 T>
struct 有类型缓冲创建器<T>
{
	static std::unique_ptr<动态类型缓冲>返回(uint64_t 元素数)
	{
		return std::unique_ptr<动态类型缓冲>(new 有类型缓冲<T>(数组工厂.createBuffer<T>(元素数), 元素数));
	}
};
template<MATLAB简单元素 T>
struct 有类型缓冲读取器<T>
{
	static std::unique_ptr<动态类型缓冲>返回(Array&& 数组)
	{
		const uint64_t 元素数 = 数组.getNumberOfElements();
		return std::unique_ptr<动态类型缓冲>(new 有类型缓冲<T>(TypedArray<T>(std::move(数组)).release(), 元素数));
	}
};
//不能inline，会导致无法链接
std::unique_ptr<动态类型缓冲>动态类型缓冲::创建(ArrayType 类型, size_t 元素数)
{
	return 动态类型选择模板<有类型缓冲创建器>(类型)(元素数);
}
std::unique_ptr<动态类型缓冲>动态类型缓冲::读取(Array&& MATLAB数组)
{
	return 动态类型选择模板<有类型缓冲读取器>(MATLAB数组.getType())(std::move(MATLAB数组));
}