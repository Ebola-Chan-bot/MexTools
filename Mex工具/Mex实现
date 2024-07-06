#include "Mex实现.h"
#include "Mex工具.h"
#include<excpt.h>
#include<magic_enum.hpp>
using namespace matlab::engine;
namespace Mex工具 {
	MATLABEngine& MATLAB引擎;
	namespace 内部
	{
		std::map<void*, std::move_only_function<void(void*)const>>自动析构表;
	}
}
using namespace matlab::mex;
static void SEH安全(ArgumentList& outputs, ArgumentList& inputs)
{
	__try
	{
		执行(outputs, inputs);
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		throw Mex工具::Mex异常::Unexpected_SEH_exception;
	}
}
struct MexFunction :Function
{
	MexFunction()
	{
		Mex工具::MATLAB引擎 = *getEngine();
		初始化();
	}
	void operator()(ArgumentList outputs, ArgumentList inputs)override
	{
		try
		{
			try
			{
				SEH安全(outputs, inputs);
			}
			catch (Mex工具::Mex异常 e)
			{
				throw;
			}
			catch (const std::exception& e)
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
			const std::string_view 异常文本 = magic_enum::enum_name(e);
			throw MATLABException(异常文本, Mex工具::万能转码<matlab::data::String>(异常文本));
		}
	}
	virtual ~MexFunction()
	{
		清理();
		for (const auto& a : Mex工具::内部::自动析构表)
			a.second(a.first);
	}
};
//未使用，仅用于确保这些函数被静态库导出
static constexpr volatile void* 导出列表[] = { mexCreateMexFunction, mexDestroyMexFunction ,mexFunctionAdapter };