#include"pch.h"
#include"MexW64Sample.h"
#include<Mex工具.h>
using namespace Mex工具;
API声明(功能1)
{
	//每个功能写在一个API声明块里，针对该功能的输入从inputs[1]开始，输出从outputs[1]开始
	outputs[1] = std::move(inputs[1]);
	char*  a = (char*)malloc(数组字节数(inputs[2]));
	万能转码(std::move(inputs[2]), std::move(a));
	free(a);
}
API声明(功能2)
{
	outputs[1] = 万能转码(std::move(inputs[1]));
	LARGE_INTEGER A{ .QuadPart = 1 };
	CellArray C = 万能转码(std::move(inputs[1]));
	for (Reference<Array> a : C)
	{
		a = inputs[1];
	}
	std::unique_ptr<CellArray>U;
}
void A1(SparseArray<bool>输入)
{
	int* 输出 = nullptr;
	*(输出 + 1) = (int&)std::move(输入);
}
void A2(void*const&输入)
{
	int a[] = { 1,2,3 };
	万能转码(a, { 3,1 });
	char16_t* b = (char16_t* const)L"123";
	万能转码(b, { 1,3 }); //正确，char16_t是合法的MATLAB元素类型
}
void A3(std::string* 输出)
{
	取迭代器值类型<decltype(输出)>;
	万能转码(输出, { 1 });
	void* a = nullptr;
	char* b = nullptr;
	万能转码<CharArray>(std::move(b), { 1 });
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
MexFunction::~MexFunction()
{
	//MATLAB端调用 clear mex 时将调用此析构函数，执行资源释放。如果没有要释放的资源，可以留空。
}