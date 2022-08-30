#include "pch.h"
#include <Mex工具.h>
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
	返回.resize_and_overwrite((字符串.size()+1) * 3, [&字符串](char* 指针, size_t 尺寸)
		{
			return WideCharToMultiByte(CP_UTF8, 0, (wchar_t*)字符串.c_str(), -1, 指针, 尺寸, nullptr, nullptr)-1;
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
			(头指针++)->resize_and_overwrite((字符串.size()+1) * 3, [&字符串](char* 指针, size_t 尺寸)
				{
					return WideCharToMultiByte(CP_UTF8, 0, (wchar_t*)字符串.c_str(), -1, 指针, 尺寸, nullptr, nullptr)-1;
				});
		}
	}
	break;
	case ArrayType::CHAR:
	{
		输出数组 = std::make_unique<std::string[]>(1);
		const String 字符串 = CharArray(std::move(输入)).toUTF16();
		输出数组[0].resize_and_overwrite((字符串.size()+1) * 3, [&字符串](char* 指针, size_t 尺寸)
			{
				return WideCharToMultiByte(CP_UTF8, 0, (wchar_t*)字符串.c_str(), -1, 指针, 尺寸, nullptr, nullptr)-1;
			}); 
	}
	break;
	case ArrayType::MATLAB_STRING:
	{
		输出数组 = std::make_unique<std::string[]>(输入.getNumberOfElements());
		std::string* 头指针 = 输出数组.get();
		for (String&& 字符串 : StringArray(std::move(输入)))
			(头指针++)->resize_and_overwrite((字符串.size()+1) * 3, [&字符串](char* 指针, size_t 尺寸)
				{
					return WideCharToMultiByte(CP_UTF8, 0, (wchar_t*)字符串.c_str(), -1, 指针, 尺寸, nullptr, nullptr)-1;
				});
	}
	break;
	[[unlikely]]default:
		throw InvalidArrayTypeException("输入不是字符串");
	}
	return 输出数组;
}
template std::unique_ptr<std::string[]> 万能转码<std::unique_ptr<std::string[]>>(Array&& 输入);
template<>
String 万能转码<String>(const char* 输入)
{
	String 返回;
	const size_t 长度 = strlen(输入)+1;
	返回.resize_and_overwrite(长度, [输入](char16_t* 指针, size_t 尺寸)
		{
			return MultiByteToWideChar(CP_UTF8, 0, 输入, -1, (wchar_t*)指针, 尺寸)-1;
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
		输出.resize_and_overwrite(输入.size()+1, [输入](char16_t* 指针, size_t 尺寸)
			{
				return MultiByteToWideChar(CP_UTF8, 0, 输入.c_str(), -1, (wchar_t*)指针, 尺寸)-1;
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
		输出.resize_and_overwrite(strlen(输入)+1, [输入](char16_t* 指针, size_t 尺寸)
			{
				return MultiByteToWideChar(CP_UTF8, 0, 输入, -1, (wchar_t*)指针, 尺寸) - 1;
			});
		返回[a] = std::move(输出);
	}
	return 返回;
}