package cpp.task;

import cpp.CppInterface;
import cpp.CppResult;
import cpp.CppTask;

public class CppReadToken implements CppTask {

	private String token;
	
	public CppReadToken(String readToken)
	{
		token = readToken;
	}
	
	@Override
	public CppResult Resolve(CppInterface cppInterface)
	{
		CppResult result = new CppResult();
		result.token = token;
		result.message = cppInterface.ReadToken(token);
		return result;
	}

}
