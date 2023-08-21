#pragma once
//实现MEX文件函数必须包含本头文件
#include <cppmex/detail/mexFunctionAdapterImpl.hpp>
#include <Mex类型.h>
//静态库函数必须“被使用”才能导出。dllexport和静态方法都不算被使用，一般方法会出现重定义错误，因此这里采用λ表达式赋给函数指针的方式
namespace Mex工具
{
	namespace 内部
	{
		static void (*导出列表)() = []()
			{
				mexCreateMexFunction(nullptr);
				mexDestroyMexFunction(nullptr, nullptr);
				mexFunctionAdapter(0, 0, 0, nullptr, nullptr, nullptr, nullptr);
			};
	}
}
//用户必须定义的初始化函数
matlab::mex::Function* 创建Mex函数();
//用户必须定义的终结化函数
void 销毁Mex函数(matlab::mex::Function* 函数指针);
/*
matlab::mex::Function是MEX文件函数必须实现的接口。用户应当定义一个子类继承之，并重写其operator()。例：
```C++
using namespace matlab::mex;
struct MexFunction :public Function //必须命名为MexFunction，public继承Function
{
	MexFunction(); //此方法可选，用于初始化，不能有任何参数输入
	void operator()(ArgumentList& outputs, ArgumentList& inputs)override; //必须定义此方法
	virtual ~MexFunction(); //此方法可选，用于 clear mex 时释放资源，必须虚析构
};
Function* 创建Mex函数()
{
	return new MexFunction();
}
void 销毁Mex函数(Function* 函数指针)
{
	delete 函数指针;
}
```
*/