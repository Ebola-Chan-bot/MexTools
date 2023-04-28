#pragma once
#include<Mex实现.h>
using namespace matlab::mex;
class MexFunction :public Function
{
public:
	//用户定义此方法作为MEX文件函数体
	void operator()(ArgumentList& outputs, ArgumentList& inputs)override;
	//还必须定义析构方法，将在 clear mex 时调用。如果没有资源释放可以定义为空方法。
	~MexFunction()override;
};