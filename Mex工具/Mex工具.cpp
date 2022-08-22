#include "pch.h"
#include <Mex工具.h>
template<>
String 万能转码<String>(Array& 输入)
{
	switch (输入.getType())
	{
	case ArrayType::CHAR:
		return CharArray(std::move(输入)).toUTF16();
	case ArrayType::MATLAB_STRING:
		return (StringArray(std::move(输入))[0]);
	default:
		throw InvalidArrayTypeException("输入不是字符串");
	}
}
template String 万能转码<String>(Array& 输入);
template<>
CharArray 万能转码<CharArray>(Array& 输入)
{
	switch (输入.getType())
	{
	case ArrayType::CHAR:
		return std::move(输入);
	case ArrayType::MATLAB_STRING:
		return 数组工厂.createCharArray(String(StringArray(std::move(输入))[0]));
	default:
		throw InvalidArrayTypeException("输入不是字符串");
	}
}
template CharArray 万能转码<CharArray>(Array& 输入);
void 异常输出补全(ArgumentList& outputs)
{
	std::vector<Array>::iterator 输出头 = outputs.begin();
	const std::vector<Array>::iterator 输出尾 = outputs.end();
	while (++输出头 < 输出尾)
		*输出头 = 数组工厂.createEmptyArray();
}
template<>
std::string 万能转码<std::string>(Array& 输入)
{
	const String 字符串 = 万能转码<String>(输入);
	std::string 返回;
	返回.resize_and_overwrite(字符串.size() * 3, [&字符串](char* 指针, size_t 尺寸)
		{
			return WideCharToMultiByte(CP_UTF8, 0, (wchar_t*)字符串.c_str(), -1, 指针, 尺寸, nullptr, nullptr);
		});
	return 返回;
}
template std::string 万能转码<std::string>(Array& 输入);
template<>
String 万能转码<String>(const char* 输入)
{
	String 返回;
	const size_t 长度 = strlen(输入)+1;
	返回.resize_and_overwrite(长度, [输入](char16_t* 指针, size_t 尺寸)
		{
#ifdef _DEBUG
			尺寸 = MultiByteToWideChar(CP_UTF8, 0, 输入, -1, (wchar_t*)指针, 尺寸);
			return 尺寸;
#else
			return MultiByteToWideChar(CP_UTF8, 0, 输入, -1, (wchar_t*)指针, 尺寸);
#endif
		});
	return 返回;
}
template String 万能转码<String>(const char*);