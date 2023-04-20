classdef MatlabInterface<uint8
	%这是一个调用 C++ MEX 文件函数的示例接口类
	%这是一个枚举类，每个枚举就是MEX文件函数中定义的API。使用语法：
	% ```
	% [varargout{:}]=MatlabInterface.APIName.Call(varargin{:});
	% ```
	%支持任意多个输入或输出值
	enumeration
		Function1(0)
		Function2(1)
	end
	methods
		function varargout=Call(obj,varargin)
			[Error,varargout{:}]=MexW64Sample(uint8(obj),varargin{:});
			%第1个返回值为错误代码，你可以自定义更复杂的异常处理流程
			if Error
				error('MexW64Sample出错，错误代码：%u',Error);
			end
		end
	end
end