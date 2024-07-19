#pragma once
#include<mexAdapter.hpp>
struct MexFunction :matlab::mex::Function
{
	MexFunction();
	void operator()(matlab::mex::ArgumentList outputs, matlab::mex::ArgumentList inputs)override;
	virtual ~MexFunction();
};
using namespace matlab::data;
void TE(const char16_t*迭代器,CharArray&数组)
{
	std::copy_n(迭代器,100,数组.begin());
}