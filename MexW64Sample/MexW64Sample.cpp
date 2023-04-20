#include"pch.h"
#include<Mex实现.h>
#include<Mex工具.h>
using namespace Mex工具;
API声明(功能1)
{
	//每个功能写在一个API声明块里，针对该功能的输入从inputs[1]开始，输出从outputs[1]开始
	outputs[1] = std::move(inputs[1]);
}
API声明(功能2)
{
	outputs[1] = 万能转码(std::move(inputs[1]));
}
void MexFunction::operator()(ArgumentList& outputs, ArgumentList& inputs)
{
	API索引
	{
		//在索引中列出所有功能
		功能1,
		功能2,
	};
	try
	{
		//inputs[0]用来指示要调用哪个API
		API调用;
		outputs[0] = 万能转码(无异常);
	}
	catch (Mex异常 ex)
	{
		//outputs[0]保留作为错误代码输出
		outputs[0] = 万能转码(ex);
		异常输出补全(outputs);
	}
	//可以继续接更多catch块，处理自定义的其它异常类型
}