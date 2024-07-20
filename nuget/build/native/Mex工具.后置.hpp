#pragma once
namespace Mex工具
{
	using namespace matlab::data;
	template<>CharArray 万能转码<CharArray>(Array&& 输入);
	template<>MATLABString 万能转码<MATLABString>(Array&& 输入);
	template<>String 万能转码<String>(Array&& 输入);
	template<>std::string 万能转码<std::string>(Array&& 输入);
	template<>StringArray 万能转码<StringArray>(Array&& 输入);
	template<>std::wstring 万能转码<std::wstring>(Array&& 输入);
}