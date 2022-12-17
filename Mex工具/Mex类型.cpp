#include "Mex类型.h"
void mexHandleException(void (*callbackErrHandler)(const char*, const char*)) {
	try {
		throw;
	}
	catch (const matlab::engine::MATLABException& ex) {
		callbackErrHandler(ex.what(), ex.getMessageID().c_str());
	}
	catch (const matlab::engine::Exception& ex) {
		callbackErrHandler(ex.what(), "");
	}
	catch (const matlab::Exception& ex) {
		callbackErrHandler(ex.what(), "");
	}
	catch (const std::exception& ex) {
		callbackErrHandler(ex.what(), "");
	}
	catch (...) {
		callbackErrHandler("Unknown exception thrown.", "");
	}
}
void mexFunction() {}
void mexDestructorUtil(matlab::mex::Function* t) {
	delete t;
}
void implToArray(int na, void* va[], std::vector<matlab::data::Array>& pa) {
	assert(na == static_cast<int>(pa.capacity()));

	for (int i = 0; i < na; i++) {
		matlab::data::impl::ArrayImpl* impl = reinterpret_cast<matlab::data::impl::ArrayImpl*>(va[i]);
		pa.push_back(matlab::data::detail::Access::createObj<matlab::data::Array>(impl));
	}
}
void implDeleter(matlab::data::impl::ArrayImpl** impl) {
	if (impl != nullptr)
		free(impl);
}
void arrayToImplOutput(int nlhs, std::vector<matlab::data::Array>& edi_plhs, void (*callbackOutput)(int, void**)) {
	assert(nlhs == static_cast<int>(edi_plhs.size()));
	std::unique_ptr<matlab::data::impl::ArrayImpl*, void(*)(matlab::data::impl::ArrayImpl**)> vlhsPtr(new matlab::data::impl::ArrayImpl * [nlhs], [](matlab::data::impl::ArrayImpl** ptr) {
		delete[] ptr;
		});
	void** vlhs = (void**)vlhsPtr.get();
	arrayToImpl(nlhs, vlhs, edi_plhs);
	callbackOutput(nlhs, vlhs);
}
void arrayToImpl(int na, void* va[], const std::vector<matlab::data::Array>& pa) {
	for (int i = 0; i < na; i++) {
		va[i] = matlab::data::detail::Access::getImpl<matlab::data::impl::ArrayImpl>(pa[i]);
	}
}
void matlab::engine::MATLABEngine::setVariable(const std::u16string& varName,
	const matlab::data::Array& var,
	matlab::engine::WorkspaceType workspaceType) {
	const char* workspace = (workspaceType == matlab::engine::WorkspaceType::BASE) ? "base" : "global";
	void* mexcept = nullptr;

	void* varImpl = matlab::data::detail::Access::getImpl<matlab::data::impl::ArrayImpl>(var);

	int errID = 0;
	mexApiSetVariable(pImpl, varImpl, workspace, varName.c_str(), &errID, &mexcept);

	throwIfError(errID, mexcept);
}
void matlab::engine::MATLABEngine::setVariable(const std::string& varName,
	const matlab::data::Array& var,
	matlab::engine::WorkspaceType workspaceType) {
	setVariable(std::u16string(varName.begin(), varName.end()), var, workspaceType);
}
void matlab::engine::MATLABEngine::setProperty(matlab::data::Array& object,
	size_t index,
	const std::u16string& propertyName,
	const matlab::data::Array& value) {
	void* mexcept = nullptr;
	void* objImpl = matlab::data::detail::Access::getImpl<matlab::data::impl::ArrayImpl>(object);
	void* propertyImpl = matlab::data::detail::Access::getImpl<matlab::data::impl::ArrayImpl>(value);

	int errID = 0;
	void* impl = mexApiSetProperty(pImpl, objImpl, index, propertyName.c_str(), propertyImpl, &errID, &mexcept);

	throwIfError(errID, mexcept);

	object = matlab::data::detail::Access::createObj<matlab::data::Array>(reinterpret_cast<matlab::data::impl::ArrayImpl*>(impl));
}

void matlab::engine::MATLABEngine::setProperty(matlab::data::Array& object,
	size_t index,
	const std::string& propertyName,
	const matlab::data::Array& value) {
	setProperty(object, index, std::u16string(propertyName.begin(), propertyName.end()), value);
}

void matlab::engine::MATLABEngine::setProperty(matlab::data::Array& object,
	const std::u16string& propertyName,
	const matlab::data::Array& value) {
	setProperty(object, 0, propertyName, value);
}

void matlab::engine::MATLABEngine::setProperty(matlab::data::Array& object,
	const std::string& propertyName,
	const matlab::data::Array& value) {
	setProperty(object, 0, std::u16string(propertyName.begin(), propertyName.end()), value);
}
void matlab::engine::MATLABEngine::eval(const std::u16string& str,
	const std::shared_ptr<matlab::engine::StreamBuffer>& output,
	const std::shared_ptr<matlab::engine::StreamBuffer>& error) {
	matlab::data::ArrayFactory factory;
	auto mStr = factory.createScalar(str);

	feval(matlab::engine::convertUTF8StringToUTF16String("eval"), 0, std::vector<matlab::data::Array>({ mStr }), output, error);
}

/*** matlab::engine::MATLABEngine ***/
std::vector<matlab::data::Array> matlab::engine::MATLABEngine::feval(const std::u16string& function,
	const int nlhs,
	const std::vector<matlab::data::Array>& args,
	const std::shared_ptr<matlab::engine::StreamBuffer>& output,
	const std::shared_ptr<matlab::engine::StreamBuffer>& error) {
	int nrhs = static_cast<int>(args.size());

	matlab::data::impl::ArrayImpl** plhs_arr_impl = (nlhs == 0) ? nullptr : (matlab::data::impl::ArrayImpl**)malloc(sizeof(matlab::data::impl::ArrayImpl*) * nlhs);
	std::unique_ptr<matlab::data::impl::ArrayImpl*, void(*)(matlab::data::impl::ArrayImpl**)> plhs_impl_guard(plhs_arr_impl, &implDeleter);

	matlab::data::impl::ArrayImpl** args_arr_impl = (nrhs == 0) ? nullptr : (matlab::data::impl::ArrayImpl**)malloc(sizeof(matlab::data::impl::ArrayImpl*) * nrhs);
	std::unique_ptr<matlab::data::impl::ArrayImpl*, void(*)(matlab::data::impl::ArrayImpl**)> args_impl_guard(args_arr_impl, &implDeleter);

	void** plhs_impl = reinterpret_cast<void**>(plhs_arr_impl);
	void** args_impl = reinterpret_cast<void**>(args_arr_impl);

	arrayToImpl(nrhs, args_impl, args);

	void* mexcept = nullptr;

	std::string function_utf8 = matlab::engine::convertUTF16StringToUTF8String(function);

	void* output_ = output ? new std::shared_ptr<StreamBuffer>(std::move(output)) : nullptr;
	void* error_ = error ? new std::shared_ptr<StreamBuffer>(std::move(error)) : nullptr;

	int errID = 0;
	mexApiFeval(pImpl, nlhs, plhs_impl, nrhs, args_impl, function_utf8.c_str(), &errID, &mexcept, output_, error_, &writeToStreamBuffer, &deleteStreamBufferImpl);

	throwIfError(errID, mexcept);

	std::vector<matlab::data::Array> plhs;
	plhs.reserve(nlhs);
	implToArray(nlhs, plhs_impl, plhs);

	return plhs;
}

std::vector<matlab::data::Array> matlab::engine::MATLABEngine::feval(const std::string& function,
	const int nlhs,
	const std::vector<matlab::data::Array>& args,
	const std::shared_ptr<matlab::engine::StreamBuffer>& output,
	const std::shared_ptr<matlab::engine::StreamBuffer>& error) {
	return feval(std::u16string(function.begin(), function.end()), nlhs, args, output, error);
}

matlab::data::Array matlab::engine::MATLABEngine::feval(const std::u16string& function,
	const std::vector<matlab::data::Array>& args,
	const std::shared_ptr<matlab::engine::StreamBuffer>& output,
	const std::shared_ptr<matlab::engine::StreamBuffer>& error) {

	std::vector<matlab::data::Array> out_vec = feval(function, 1, args, output, error);
	return out_vec.at(0);
}

matlab::data::Array matlab::engine::MATLABEngine::feval(const std::string& function,
	const std::vector<matlab::data::Array>& args,
	const std::shared_ptr<matlab::engine::StreamBuffer>& output,
	const std::shared_ptr<matlab::engine::StreamBuffer>& error) {
	return feval(std::u16string(function.begin(), function.end()), args, output, error);
}

matlab::data::Array matlab::engine::MATLABEngine::feval(const std::u16string& function,
	const matlab::data::Array& arg,
	const std::shared_ptr<matlab::engine::StreamBuffer>& output,
	const std::shared_ptr<matlab::engine::StreamBuffer>& error) {
	return feval(function, std::vector<matlab::data::Array>({ arg }), output, error);
}

matlab::data::Array matlab::engine::MATLABEngine::feval(const std::string& function,
	const matlab::data::Array& arg,
	const std::shared_ptr<matlab::engine::StreamBuffer>& output,
	const std::shared_ptr<matlab::engine::StreamBuffer>& error) {
	return feval(std::u16string(function.begin(), function.end()), std::vector<matlab::data::Array>({ arg }), output, error);
}

matlab::data::Array matlab::engine::MATLABEngine::getVariable(const std::string& varName,
	matlab::engine::WorkspaceType workspaceType) {
	return getVariable(std::u16string(varName.begin(), varName.end()), workspaceType);
}





matlab::data::Array matlab::engine::MATLABEngine::getProperty(const matlab::data::Array& object,
	size_t index,
	const std::u16string& propertyName) {
	void* mexcept = nullptr;
	void* objImpl = matlab::data::detail::Access::getImpl<matlab::data::impl::ArrayImpl>(object);

	int errID = 0;
	void* impl = mexApiGetProperty(pImpl, objImpl, index, propertyName.c_str(), &errID, &mexcept);

	throwIfError(errID, mexcept);

	return matlab::data::detail::Access::createObj<matlab::data::Array>(reinterpret_cast<matlab::data::impl::ArrayImpl*>(impl));
}

matlab::data::Array matlab::engine::MATLABEngine::getProperty(const matlab::data::Array& object,
	size_t index,
	const std::string& propertyName) {
	return getProperty(object, index, std::u16string(propertyName.begin(), propertyName.end()));
}

matlab::data::Array matlab::engine::MATLABEngine::getProperty(const matlab::data::Array& object,
	const std::u16string& propertyName) {
	return getProperty(object, 0, propertyName);
}

matlab::data::Array matlab::engine::MATLABEngine::getProperty(const matlab::data::Array& object,
	const std::string& propertyName) {
	return getProperty(object, 0, std::u16string(propertyName.begin(), propertyName.end()));
}
matlab::engine::FutureResult<void> matlab::engine::MATLABEngine::evalAsync(const std::u16string& str,
	const std::shared_ptr<matlab::engine::StreamBuffer>& output,
	const std::shared_ptr<matlab::engine::StreamBuffer>& error) {
	std::promise<void>* p = new std::promise<void>();
	std::future<void> f = p->get_future();
	void* output_ = output ? new std::shared_ptr<StreamBuffer>(std::move(output)) : nullptr;
	void* error_ = error ? new std::shared_ptr<StreamBuffer>(std::move(error)) : nullptr;
	uintptr_t handle = mexApiEvalAsync(pImpl, str.c_str(), &set_eval_promise_data, &set_eval_promise_exception, p, output_, error_, &writeToStreamBuffer, &deleteStreamBufferImpl);
	return matlab::engine::FutureResult<void>(std::move(f), std::make_shared<matlab::engine::TaskReference>(handle, mexApiCancelFevalWithCompletion));
}

matlab::engine::FutureResult<matlab::data::Array> matlab::engine::MATLABEngine::getVariableAsync(const std::u16string& varName,
	WorkspaceType workspaceType) {
	std::promise<matlab::data::Array>* p = new std::promise<matlab::data::Array>();
	std::future<matlab::data::Array> f = p->get_future();
	const char* workspace = (workspaceType == matlab::engine::WorkspaceType::BASE) ? "base" : "global";
	uintptr_t handle = mexApiGetVariableAsync(pImpl, workspace, varName.c_str(), &set_feval_promise_data, &set_feval_promise_exception, p);
	return matlab::engine::FutureResult<matlab::data::Array>(std::move(f), std::make_shared<matlab::engine::TaskReference>(handle, mexApiCancelFevalWithCompletion));
}

matlab::engine::FutureResult<matlab::data::Array> matlab::engine::MATLABEngine::getVariableAsync(const std::string& varName,
	WorkspaceType workspaceType) {
	return getVariableAsync(std::u16string(varName.begin(), varName.end()), workspaceType);
}

matlab::engine::FutureResult<void> matlab::engine::MATLABEngine::setVariableAsync(const std::u16string& varName,
	const matlab::data::Array& var,
	WorkspaceType workspaceType) {
	std::promise<void>* p = new std::promise<void>();
	std::future<void> f = p->get_future();
	const char* workspace = (workspaceType == matlab::engine::WorkspaceType::BASE) ? "base" : "global";
	matlab::data::impl::ArrayImpl* varImpl = matlab::data::detail::Access::getImpl<matlab::data::impl::ArrayImpl>(var);
	uintptr_t handle = mexApiSetVariableAsync(pImpl, varImpl, workspace, varName.c_str(), &set_feval_promise_data, &set_feval_promise_exception, p);
	return matlab::engine::FutureResult<void>(std::move(f), std::make_shared<matlab::engine::TaskReference>(handle, mexApiCancelFevalWithCompletion));
}

matlab::engine::FutureResult<void> matlab::engine::MATLABEngine::setVariableAsync(const std::string& varName,
	const matlab::data::Array& var,
	WorkspaceType workspaceType) {
	return setVariableAsync(std::u16string(varName.begin(), varName.end()), var, workspaceType);
}

matlab::engine::FutureResult<matlab::data::Array> matlab::engine::MATLABEngine::getPropertyAsync(const matlab::data::Array& object,
	size_t index,
	const std::u16string& propertyName) {
	std::promise<matlab::data::Array>* p = new std::promise<matlab::data::Array>();
	std::future<matlab::data::Array> f = p->get_future();
	matlab::data::impl::ArrayImpl* objectImpl = matlab::data::detail::Access::getImpl<matlab::data::impl::ArrayImpl>(object);
	uintptr_t handle = mexApiGetPropertyAsync(pImpl, objectImpl, index, propertyName.c_str(), &set_feval_promise_data, &set_feval_promise_exception, p);
	return matlab::engine::FutureResult<matlab::data::Array>(std::move(f), std::make_shared<matlab::engine::TaskReference>(handle, mexApiCancelFevalWithCompletion));
}

matlab::engine::FutureResult<matlab::data::Array> matlab::engine::MATLABEngine::getPropertyAsync(const matlab::data::Array& object,
	size_t index,
	const std::string& propertyName) {
	return getPropertyAsync(object, index, std::u16string(propertyName.begin(), propertyName.end()));
}

matlab::engine::FutureResult<matlab::data::Array> matlab::engine::MATLABEngine::getPropertyAsync(const matlab::data::Array& object,
	const std::u16string& propertyName) {
	return getPropertyAsync(object, 0, propertyName);
}

matlab::engine::FutureResult<matlab::data::Array> matlab::engine::MATLABEngine::getPropertyAsync(const matlab::data::Array& object,
	const std::string& propertyName) {
	return getPropertyAsync(object, std::u16string(propertyName.begin(), propertyName.end()));
}

matlab::engine::FutureResult<void> matlab::engine::MATLABEngine::setPropertyAsync(matlab::data::Array& object,
	size_t index,
	const std::u16string& propertyName,
	const matlab::data::Array& value) {
	std::promise<matlab::data::Array>* p = new std::promise<matlab::data::Array>();
	std::future<matlab::data::Array> f = p->get_future();
	matlab::data::impl::ArrayImpl* objectImpl = matlab::data::detail::Access::getImpl<matlab::data::impl::ArrayImpl>(object);
	matlab::data::impl::ArrayImpl* propImpl = matlab::data::detail::Access::getImpl<matlab::data::impl::ArrayImpl>(value);
	uintptr_t handle = mexApiSetPropertyAsync(pImpl, objectImpl, index, propertyName.c_str(), propImpl, &set_feval_promise_data, &set_feval_promise_exception, p);
	auto copyableF = std::make_shared<std::future<matlab::data::Array>>(std::move(f));
	auto convertToResultType = [&object, copyableF]() {
		matlab::data::Array vec = copyableF->get();
		object = std::move(vec);
	};
	std::future<void> future = std::async(std::launch::deferred, std::move(convertToResultType));
	return FutureResult<void>(std::move(future), std::make_shared<matlab::engine::TaskReference>(handle, mexApiCancelFevalWithCompletion));
}

matlab::engine::FutureResult<void> matlab::engine::MATLABEngine::setPropertyAsync(matlab::data::Array& object,
	size_t index,
	const std::string& propertyName,
	const matlab::data::Array& value) {
	return setPropertyAsync(object, index, std::u16string(propertyName.begin(), propertyName.end()), value);
}

matlab::engine::FutureResult<void> matlab::engine::MATLABEngine::setPropertyAsync(matlab::data::Array& object,
	const std::u16string& propertyName,
	const matlab::data::Array& value) {
	return setPropertyAsync(object, 0, propertyName, value);
}

matlab::engine::FutureResult<void> matlab::engine::MATLABEngine::setPropertyAsync(matlab::data::Array& object,
	const std::string& propertyName,
	const matlab::data::Array& value) {
	return setPropertyAsync(object, std::u16string(propertyName.begin(), propertyName.end()), value);
}
matlab::engine::FutureResult<std::vector<matlab::data::Array>> matlab::engine::MATLABEngine::fevalAsync(const std::u16string& function,
	const int nlhs,
	const std::vector<matlab::data::Array>& args,
	const std::shared_ptr<matlab::engine::StreamBuffer>& output,
	const std::shared_ptr<matlab::engine::StreamBuffer>& error) {

	int nrhs = static_cast<int>(args.size());

	matlab::data::impl::ArrayImpl** args_arr_impl = (nrhs == 0) ? nullptr : (matlab::data::impl::ArrayImpl**)malloc(sizeof(matlab::data::impl::ArrayImpl*) * nrhs);
	std::unique_ptr<matlab::data::impl::ArrayImpl*, void(*)(matlab::data::impl::ArrayImpl**)> args_impl_guard(args_arr_impl, &implDeleter);

	void** args_impl = reinterpret_cast<void**>(args_arr_impl);

	arrayToImpl(nrhs, args_impl, args);

	std::promise<std::vector<matlab::data::Array> >* p = new std::promise<std::vector<matlab::data::Array> >();
	std::future<std::vector<matlab::data::Array> > f = p->get_future();

	std::string function_utf8 = matlab::engine::convertUTF16StringToUTF8String(function);

	void* output_ = output ? new std::shared_ptr<StreamBuffer>(std::move(output)) : nullptr;
	void* error_ = error ? new std::shared_ptr<StreamBuffer>(std::move(error)) : nullptr;

	uintptr_t handle = mexApiFevalAsync(pImpl, nlhs, nrhs, args_impl, function_utf8.c_str(), false, &set_feval_promise_data, &set_feval_promise_exception, p, output_, error_, &writeToStreamBuffer, &deleteStreamBufferImpl);

	return matlab::engine::FutureResult<std::vector<matlab::data::Array>>(std::move(f), std::make_shared<matlab::engine::TaskReference>(handle, mexApiCancelFevalWithCompletion));
}


matlab::engine::FutureResult<std::vector<matlab::data::Array>> matlab::engine::MATLABEngine::fevalAsync(const std::string& function,
	const int nlhs,
	const std::vector<matlab::data::Array>& args,
	const std::shared_ptr<matlab::engine::StreamBuffer>& output,
	const std::shared_ptr<matlab::engine::StreamBuffer>& error) {
	return fevalAsync(std::u16string(function.begin(), function.end()), nlhs, args, output, error);
}


matlab::engine::FutureResult<matlab::data::Array> matlab::engine::MATLABEngine::fevalAsync(const std::u16string& function,
	const std::vector<matlab::data::Array>& args,
	const std::shared_ptr<matlab::engine::StreamBuffer>& output,
	const std::shared_ptr<matlab::engine::StreamBuffer>& error) {
	int nrhs = static_cast<int>(args.size());

	matlab::data::impl::ArrayImpl** args_arr_impl = (nrhs == 0) ? nullptr : (matlab::data::impl::ArrayImpl**)malloc(sizeof(matlab::data::impl::ArrayImpl*) * nrhs);
	std::unique_ptr<matlab::data::impl::ArrayImpl*, void(*)(matlab::data::impl::ArrayImpl**)> args_impl_guard(args_arr_impl, &implDeleter);

	void** args_impl = reinterpret_cast<void**>(args_arr_impl);

	arrayToImpl(nrhs, args_impl, args);

	std::promise<matlab::data::Array>* p = new std::promise<matlab::data::Array>();
	std::future<matlab::data::Array> f = p->get_future();

	std::string function_utf8 = matlab::engine::convertUTF16StringToUTF8String(function);

	void* output_ = output ? new std::shared_ptr<StreamBuffer>(std::move(output)) : nullptr;
	void* error_ = error ? new std::shared_ptr<StreamBuffer>(std::move(error)) : nullptr;

	uintptr_t handle = mexApiFevalAsync(pImpl, 1, nrhs, args_impl, function_utf8.c_str(), true, &set_feval_promise_data, &set_feval_promise_exception, p, output_, error_, &writeToStreamBuffer, &deleteStreamBufferImpl);

	return matlab::engine::FutureResult<matlab::data::Array>(std::move(f), std::make_shared<matlab::engine::TaskReference>(handle, mexApiCancelFevalWithCompletion));
}


matlab::engine::FutureResult<matlab::data::Array> matlab::engine::MATLABEngine::fevalAsync(const std::string& function,
	const std::vector<matlab::data::Array>& args,
	const std::shared_ptr<matlab::engine::StreamBuffer>& output,
	const std::shared_ptr<matlab::engine::StreamBuffer>& error) {
	return fevalAsync(std::u16string(function.begin(), function.end()), args, output, error);
}


matlab::engine::FutureResult<matlab::data::Array> matlab::engine::MATLABEngine::fevalAsync(const std::u16string& function,
	const matlab::data::Array& arg,
	const std::shared_ptr<matlab::engine::StreamBuffer>& output,
	const std::shared_ptr<matlab::engine::StreamBuffer>& error) {
	return fevalAsync(function, std::vector<matlab::data::Array>({ arg }), output, error);
}


matlab::engine::FutureResult<matlab::data::Array> matlab::engine::MATLABEngine::fevalAsync(const std::string& function,
	const matlab::data::Array& arg,
	const std::shared_ptr<matlab::engine::StreamBuffer>& output,
	const std::shared_ptr<matlab::engine::StreamBuffer>& error) {
	return fevalAsync(std::u16string(function.begin(), function.end()), arg, output, error);
}
matlab::data::Array getArray(void* v) {
	matlab::data::impl::ArrayImpl* impl = reinterpret_cast<matlab::data::impl::ArrayImpl*>(v);
	if (impl == nullptr)
		return matlab::data::Array();
	return matlab::data::detail::Access::createObj<matlab::data::Array>(impl);
}

matlab::data::Array matlab::engine::MATLABEngine::getVariable(const std::u16string& varName,
	matlab::engine::WorkspaceType workspaceType) {
	const char* workspace = (workspaceType == matlab::engine::WorkspaceType::BASE) ? "base" : "global";
	void* mexcept = nullptr;

	int errID = 0;
	void* res = mexApiGetVariable(pImpl, workspace, varName.c_str(), &errID, &mexcept);

	throwIfError(errID, mexcept);

	matlab::data::Array ret = getArray(res);
	return ret;
}