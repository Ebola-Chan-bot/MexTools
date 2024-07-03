#include "Mex实现.h"
#include "Mex工具.h"
#include<excpt.h>
namespace Mex工具 {
	matlab::engine::MATLABEngine& MATLAB引擎;
	namespace 内部
	{
		std::map<void*, std::move_only_function<void(void*)const>>自动析构表;
	}
}
static void SEH安全(ArgumentList& outputs, ArgumentList& inputs)
{
	__try
	{
		执行(outputs, inputs);
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		throw Mex工具::Mex异常::意外的SEH异常;
	}
}
static void Cpp安全(ArgumentList& outputs, ArgumentList& inputs)
{
	try
	{
		SEH安全(outputs, inputs);
	}
	catch (Mex工具::Mex异常 e)
	{
		if (e == Mex工具::Mex异常::意外的SEH异常)
			Mex工具::MATLAB引擎.feval<void>("error", String(u"MexTool:SEHException"), String(u"发生了意外的SEH异常"));
		else
		{
			const std::wstring 错误消息 = (std::wostringstream(L"发生了意外的Mex异常，错误代码：") << (uint16_t)e).str();
			Mex工具::MATLAB引擎.feval<void>("error", String(u"MexTool:MexException"), String((char16_t*)错误消息.c_str()));
		}
	}
	catch (...)
	{
		Mex工具::MATLAB引擎.feval<void>("error", String(u"MexTool:CppException"), String(u"发生了意外的C++异常"));
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
		SEH安全(outputs, inputs);
	}
	virtual ~MexFunction()
	{
		清理();
		for (const auto& a : Mex工具::内部::自动析构表)
			a.second(a.first);
	}
};
//未使用，仅用于确保这些函数被静态库导出
constexpr volatile void* 导出列表[] = { mexCreateMexFunction, mexDestroyMexFunction ,mexFunctionAdapter };