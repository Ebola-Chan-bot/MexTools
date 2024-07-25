#pragma once
namespace Mex工具
{
	template<>matlab::data::CharArray 万能转码<matlab::data::CharArray>(matlab::data::Array&& 输入);
	template<>matlab::data::MATLABString 万能转码<matlab::data::MATLABString>(matlab::data::Array&& 输入);
	template<>matlab::data::String 万能转码<matlab::data::String>(matlab::data::Array&& 输入);
	template<>std::string 万能转码<std::string>(matlab::data::Array&& 输入);
	template<>matlab::data::StringArray 万能转码<matlab::data::StringArray>(matlab::data::Array&& 输入);
	template<>std::wstring 万能转码<std::wstring>(matlab::data::Array&& 输入);
}