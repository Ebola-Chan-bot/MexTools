module;
#include<magic_enum.hpp>
#include<excpt.h>
module Mex工具;
using namespace matlab::data;
namespace Mex工具
{
	ArrayFactory 数组工厂;
	std::shared_ptr<matlab::engine::MATLABEngine> MATLAB引擎;
	std::map<void*, std::move_only_function<void(void*)const>>自动析构表;
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
		throw Mex工具::Mex异常::Unexpected_SEH_exception;
	}
}
MexFunction::MexFunction()
{
	Mex工具::MATLAB引擎 = getEngine();
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
		catch (Mex工具::Mex异常)
		{
			throw;
		}
		catch (const std::exception&)
		{
			throw;
		}
		catch (...)
		{
			throw Mex工具::Mex异常::Unexpected_CPP_exception;
		}
	}
	catch (Mex工具::Mex异常 e)
	{
		const std::string 异常文本(magic_enum::enum_name(e));
		throw matlab::engine::MATLABException(异常文本, Mex工具::万能转码<String>(异常文本));
	}
}
MexFunction::~MexFunction()
{
	清理();
	for (const auto& a : Mex工具::自动析构表)
		a.second(a.first);
}
static constexpr volatile void* 导出列表[] = { mexCreateMexFunction, mexDestroyMexFunction ,mexFunctionAdapter };