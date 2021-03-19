package cpp.task;

import cpp.CppInterface;
import cpp.CppResult;
import cpp.CppTask;

public class CppDoActionGetBoard implements CppTask {
	
	private String token;
	private String message;
	
	public CppDoActionGetBoard(String sendToken, String sendMessage)
	{
		token = sendToken;
		message = sendMessage;
	}

	@Override
	public CppResult Resolve(CppInterface cppInterface)
	{
		cppInterface.WriteToken(token, message);
		return new CppResult("board", cppInterface.ReadToken("board"));
	}

}
