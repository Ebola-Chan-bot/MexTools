#include "Mex实现.h"
#include "cppmex/detail/mexErrorDispatch.hpp"
#include "cppmex/detail/mexExceptionImpl.hpp"
void* mexCreateMexFunction(void (*callbackErrHandler)(const char*, const char*)) {
	try {
		matlab::mex::Function* mexFunc = mexCreatorUtil<MexFunction>();
		return mexFunc;
	}
	catch (...) {
		mexHandleException(callbackErrHandler);
		return nullptr;
	}
}
void mexDestroyMexFunction(void* mexFunc,
	void (*callbackErrHandler)(const char*, const char*)) {
	matlab::mex::Function* mexFunction = reinterpret_cast<matlab::mex::Function*>(mexFunc);
	try {
		mexDestructorUtil(mexFunction);
	}
	catch (...) {
		mexHandleException(callbackErrHandler);
		return;
	}
}
void mexFunctionAdapter(int nlhs_,
	int nlhs,
	int nrhs,
	void* vrhs[],
	void* mFun,
	void (*callbackOutput)(int, void**),
	void (*callbackErrHandler)(const char*, const char*)) {

	matlab::mex::Function* mexFunction = reinterpret_cast<matlab::mex::Function*>(mFun);

	std::vector<matlab::data::Array> edi_prhs;
	edi_prhs.reserve(nrhs);
	implToArray(nrhs, vrhs, edi_prhs);

	std::vector<matlab::data::Array> edi_plhs(nlhs);
	matlab::mex::ArgumentList outputs(edi_plhs.begin(), edi_plhs.end(), nlhs_);
	matlab::mex::ArgumentList inputs(edi_prhs.begin(), edi_prhs.end(), nrhs);

	try {
		(*mexFunction)(outputs, inputs);
	}
	catch (...) {
		mexHandleException(callbackErrHandler);
		return;
	}

	arrayToImplOutput(nlhs, edi_plhs, callbackOutput);
}
matlab::mex::Function::Function() {
	functionImpl = mexGetFunctionImpl();
}
void throwIfError(int errID, void* mexcept) {
	matlab::mex::detail::ErrorType errorID(static_cast<matlab::mex::detail::ErrorType>(errID));
	switch (errorID) {
	case matlab::mex::detail::ErrorType::NoException:
		break;
	case matlab::mex::detail::ErrorType::RuntimeError: {
		matlab::data::impl::ArrayImpl* impl = reinterpret_cast<matlab::data::impl::ArrayImpl*>(mexcept);
		matlab::data::Array exArr(matlab::data::detail::Access::createObj<matlab::data::Array>(impl));
		matlab::data::StructArray sArr(exArr);
		matlab::data::TypedArray<int> errStatus = sArr[0][std::string("status")];
		int rErrID_ = errStatus[0];
		matlab::mex::detail::ErrorType rErrorID(static_cast<matlab::mex::detail::ErrorType>(rErrID_));
		switch (rErrorID) {
		case matlab::mex::detail::ErrorType::SyntaxError: {
			matlab::engine::MATLABSyntaxException exp = matlab::engine::detail::createMATLABSyntaxException(sArr);
			throw exp;
		}
		case matlab::mex::detail::ErrorType::ExecutionError: {
			matlab::engine::MATLABExecutionException exp = matlab::engine::detail::createMATLABExecutionException(sArr);
			throw exp;
		}
		case matlab::mex::detail::ErrorType::EngineError: {
			matlab::engine::MATLABException exp = matlab::engine::detail::createMATLABException(sArr);
			throw exp;
		}
		case matlab::mex::detail::ErrorType::InterruptedError: {
			std::string msg = "MATLAB command was interrupted.";
			throw matlab::engine::InterruptedException("MATLAB:mex:CppMexException", matlab::engine::convertUTF8StringToUTF16String(msg));
		}
		default:
			throw matlab::engine::MATLABException("MATLAB:mex:CppMexException", matlab::engine::convertUTF8StringToUTF16String("Runtime Error"));
		}
	}
	case matlab::mex::detail::ErrorType::ThreadError: {
		const std::string msg = "Synchronous version of MATLAB Engine functions must be called from the thread that called the MEX function.";
		throw matlab::engine::MATLABException("MATLAB:mex:CppMexThreadMismatch", matlab::engine::convertUTF8StringToUTF16String(msg));
	}
	case matlab::mex::detail::ErrorType::OutOfMemory: {
		std::string outOfMemoryError = "Not enough memory available to support the request.";
		throw matlab::OutOfMemoryException(outOfMemoryError);
	}
	case matlab::mex::detail::ErrorType::CancelError: {
		std::string msg = "MATLAB command was cancelled.";
		throw matlab::engine::CancelledException("MATLAB:mex:CppMexException", matlab::engine::convertUTF8StringToUTF16String(msg));
	}
	case matlab::mex::detail::ErrorType::SystemError: {
		std::string msg = "Unexpected exception caught in feval.";
		throw matlab::engine::MATLABException("MATLAB:mex:CppMexException", matlab::engine::convertUTF8StringToUTF16String(msg));
	}
	default:
		throw matlab::engine::MATLABException("MATLAB:mex:CppMexException", matlab::engine::convertUTF8StringToUTF16String("Error"));
	}
}
void matlab::mex::Function::mexLock() {
	int errID = 0;
	cppmex_mexLock_with_error_check(functionImpl, &errID);
	throwIfError(errID, nullptr);
}
void matlab::mex::Function::mexUnlock() {
	int errID = 0;
	cppmex_mexUnlock_with_error_check(functionImpl, &errID);
	throwIfError(errID, nullptr);
}
matlab::mex::Function::~Function() NOEXCEPT_FALSE {
	mexDestroyFunctionImpl(functionImpl);
}
std::shared_ptr<matlab::engine::MATLABEngine> matlab::mex::Function::getEngine() {
	void* engine = cppmex_getEngine(functionImpl);
	return std::make_shared<matlab::engine::MATLABEngine>(engine);
}
std::u16string matlab::mex::Function::getFunctionName() const {
	const char16_t* fn = cppmex_getFunctionName(functionImpl);

	if (!fn) {
		std::string outOfMemoryError = "Not enough memory available to support the request.";
		throw matlab::engine::Exception(outOfMemoryError);
	}

	char16_t* fName = const_cast<char16_t*>(fn);
	std::u16string fNameStr = std::u16string(fName);
	mexReleaseMemory(nullptr, fName);
	return fNameStr;
}