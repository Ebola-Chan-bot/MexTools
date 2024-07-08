#pragma once
#include<mexAdapter.hpp>
struct MexFunction :matlab::mex::Function
{
	MexFunction();
	void operator()(matlab::mex::ArgumentList outputs, matlab::mex::ArgumentList inputs)override;
	virtual ~MexFunction();
};
using namespace matlab::data;
void TE(CellArray 数组)
{
	String 字符串 = CharArray(数组[0]).toUTF16();
}