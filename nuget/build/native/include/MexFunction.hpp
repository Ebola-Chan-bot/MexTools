#pragma once
#include<mexAdapter.hpp>
struct MexFunction :matlab::mex::Function
{
	MexFunction();
	void operator()(matlab::mex::ArgumentList outputs, matlab::mex::ArgumentList inputs)override;
	virtual ~MexFunction();
};