package cpp.task;

import cpp.CppInterface;
import cpp.CppResult;
import cpp.CppTask;

public class CppWriteToken implements CppTask {

	private String token;
	private String message;
	
	public CppWriteToken(String writeToken, String writeMessage)
	{
		token = writeToken;
		message = writeMessage;
	}
	
	@Override
	public CppResult Resolve(CppInterface cppInterface)
	{
		cppInterface.WriteToken(token, message);
		return new CppResult();
	}

}
