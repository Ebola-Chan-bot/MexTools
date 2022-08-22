#pragma once
#include <cppmex/detail/mexFunctionAdapterImpl.hpp>
#include <Mex类型.h>
//静态库函数必须“被使用”才能导出。dllexport和静态方法都不算被使用，一般方法会出现重定义错误，因此这里采用λ表达式赋给函数指针的方式
static void (*导出列表)() = []()
{
	mexCreateMexFunction(nullptr);
	mexDestroyMexFunction(nullptr, nullptr);
	mexFunctionAdapter(0, 0, 0, nullptr, nullptr, nullptr, nullptr);
};
using namespace matlab::mex;
class MexFunction :public Function
{
public:
	//用户实现此方法作为MEX文件函数体
	void operator()(ArgumentList& outputs, ArgumentList& inputs)override;
};