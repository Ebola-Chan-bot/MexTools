#include "Mex工具.hpp"
#include <magic_enum.hpp>
#include <mexAdapter.hpp>
#include <Windows.h>
import std;
#pragma comment(lib,"libMatlabDataArray.lib")
#pragma comment(lib,"libmex.lib")
using namespace matlab::data;
namespace Mex工具
{
	ArrayFactory 数组工厂;
	std::shared_ptr<matlab::engine::MATLABEngine> MATLAB引擎;
	std::map<void*, std::move_only_function<void(void*)const>>自动析构表;
	template<>
	CharArray 万能转码<CharArray>(Array&& 输入)
	{
		switch (输入.getType())
		{
		case ArrayType::CHAR:
			return std::move(输入);
		case ArrayType::MATLAB_STRING:
			return 数组工厂.createCharArray(((const Array&)输入)[0].operator String());
		case ArrayType::CELL:
			return ((const Array&)输入)[0];
		default:
			return 数组工厂.createCharArray(MATLAB引擎->feval(MATLAB转换函数<MATLABString>, std::move(输入))[0].operator String());
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
			return ((const Array&)输入)[0].operator String();
		case ArrayType::CELL:
			return ((const Array&)输入)[0].operator CharArray().toUTF16();
		default:
			return MATLAB引擎->feval(MATLAB转换函数<MATLABString>, std::move(输入))[0].operator String();
		}
	}
	template<>
	String 万能转码<String>(Array&& 输入)
	{
		switch (输入.getType())
		{
		case ArrayType::CHAR:
			return CharArray(std::move(输入)).toUTF16();
		case ArrayType::MATLAB_STRING:
			return ((const Array&)输入)[0];
		case ArrayType::CELL:
			return ((const Array&)输入)[0].operator CharArray().toUTF16();
		default:
			return MATLAB引擎->feval(MATLAB转换函数<MATLABString>, std::move(输入))[0];
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
			输出.resize_and_overwrite((长度 + 1) * 3, [宽指针=(wchar_t*)CharArray(std::move(输入)).release().get(), 长度](char* 指针, size_t 尺寸)
				{
					return WideCharToMultiByte(CP_UTF8, 0, 宽指针, 长度, 指针, 尺寸, nullptr, nullptr) - 1;
				});
		}
		break;
		case ArrayType::MATLAB_STRING:
		{
			const String 字符串 = ((const Array&)输入)[0];
			输出.resize_and_overwrite((字符串.size() + 1) * 3, [&字符串](char* 指针, size_t 尺寸)
				{
					return WideCharToMultiByte(CP_UTF8, 0, (wchar_t*)字符串.data(), 字符串.size(), 指针, 尺寸, nullptr, nullptr) - 1;
				});
		}
		break;
		case ArrayType::CELL:
		{
			CharArray 字符数组= ((const Array&)输入)[0];
			const int 长度 = 字符数组.getNumberOfElements();
			输出.resize_and_overwrite((长度 + 1) * 3, [宽指针=(wchar_t*)字符数组.release().get(), 长度](char* 指针, size_t 尺寸)
				{
					return WideCharToMultiByte(CP_UTF8, 0, 宽指针, 长度, 指针, 尺寸, nullptr, nullptr) - 1;
				});
		}
		break;
		default:
			const String 字符串 = MATLAB引擎->feval(MATLAB转换函数<MATLABString>, std::move(输入))[0];
			输出.resize_and_overwrite((字符串.size() + 1) * 3, [&字符串](char* 指针, size_t 尺寸)
				{
					return WideCharToMultiByte(CP_UTF8, 0, (wchar_t*)字符串.data(), 字符串.size(), 指针, 尺寸, nullptr, nullptr) - 1;
				});
			break;
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
			StringArray 输出 = 数组工厂.createArray<MATLABString>(输入.getDimensions());
			const size_t 元素个数 = 输入.getNumberOfElements();
			for (size_t a = 0; a < 元素个数; ++a)
				输出[a] = ((const Array&)输入)[a].operator CharArray().toUTF16();
			return 输出;
		}
		default:
			return MATLAB引擎->feval(MATLAB转换函数<MATLABString>, std::move(输入));
		}
	}
	template<>
	std::wstring 万能转码<std::wstring>(Array&& 输入)
	{
		switch (输入.getType())
		{
		case ArrayType::CHAR:
		{
			const size_t 长度 = 输入.getNumberOfElements();
			return std::wstring((wchar_t*)CharArray(std::move(输入)).release().get(), 长度);
		}
		case ArrayType::MATLAB_STRING:
		{
			const String 字符串 = ((const Array&)输入)[0];
			return std::wstring((wchar_t*)字符串.data(), 字符串.size());
		}
		case ArrayType::CELL:
		{
			CharArray 字符数组 = ((const Array&)输入)[0];
			const size_t 长度 = 字符数组.getNumberOfElements();
			return std::wstring((wchar_t*)字符数组.release().get(), 长度);
		}
		default:
		{
			const String 字符串 = MATLAB引擎->feval(MATLAB转换函数<MATLABString>, std::move(输入))[0];
			return std::wstring((wchar_t*)字符串.data(), 字符串.size());
		}
		}
	}
	void 自动析构(void* 指针, std::move_only_function<void(void*)const>&& 删除器)noexcept
	{
		自动析构表[指针] = std::move(删除器);
	}
	bool 手动析构(void* 指针)noexcept
	{
		return 自动析构表.erase(指针);
	}
	bool 对象存在(void* 指针)noexcept
	{
		return 自动析构表.contains(指针);
	}
	namespace 内部
	{
		CharArray 标量转换<CharArray>::转换(const char* 输入, size_t 长度)
		{
			buffer_ptr_t<char16_t> 缓冲 = Mex工具::数组工厂.createBuffer<char16_t>(长度 + 1);
			长度 = MultiByteToWideChar(CP_UTF8, 0, 输入, 长度, (wchar_t*)缓冲.get(), 长度 + 1) - 1;
			return Mex工具::数组工厂.createArrayFromBuffer({ 1,长度 }, std::move(缓冲));
		}
		String 标量转换<String>::转换(const char* 输入, size_t 长度)
		{
			String 返回;
			返回.resize_and_overwrite(长度 + 1, [输入, 长度](char16_t* 指针, size_t 尺寸)
				{
					return MultiByteToWideChar(CP_UTF8, 0, 输入, 长度, (wchar_t*)指针, 尺寸) - 1;
				});
			return 返回;
		}
		int WCTMB(const wchar_t* 宽字符串, int 宽字符数, char* 字节缓冲, int 缓冲长度)
		{
			return WideCharToMultiByte(CP_UTF8, 0, 宽字符串, 宽字符数, 字节缓冲, 缓冲长度, nullptr, nullptr) - 1;
		}
	}
}
using namespace Mex工具;
using namespace matlab::mex;
static void SEH安全(ArgumentList& outputs, ArgumentList& inputs)
{
	__try
	{
		执行(outputs, inputs);
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		throw Mex异常::Unexpected_SEH_exception;
	}
}
MexFunction::MexFunction()
{
	MATLAB引擎 = getEngine();
	初始化();
}
void MexFunction::operator()(ArgumentList outputs, ArgumentList inputs)
{
	try
	{
		try
		{
			SEH安全(outputs, inputs);
		}
		catch (Mex异常)
		{
			throw;
		}
		catch (const std::exception&)
		{
			//标准异常由MATLAB负责捕获
			throw;
		}
		catch (...)
		{
			throw Mex异常::Unexpected_CPP_exception;
		}
	}
	catch (Mex异常 e)
	{
		const std::string 异常文本(magic_enum::enum_name(e));
		throw matlab::engine::MATLABException(异常文本, 万能转码<String>(异常文本));
	}
}
MexFunction::~MexFunction()
{
	清理();
	for (const auto& a : 自动析构表)
		a.second(a.first);
}