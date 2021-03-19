package cpp;

public class CppResult {
	public String token;
	public String message;
	
	public CppResult()
	{
		token = "token";
		message = "message";
	}
	
	public CppResult(String rcvToken, String rcvMessage)
	{
		token = rcvToken;
		message = rcvMessage;
	}
}
