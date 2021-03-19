package cpp;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.InputStreamReader;
import java.io.OutputStreamWriter;

public class CppInterface implements Runnable {
	
	private boolean bActive = true;
	public CppTask Task;
	public CppResult Result;
	public boolean bInitOnRun = false;
	
	String fileExecutable = "FerChessAI.exe";
	boolean bIsInit = false;
	BufferedReader input;
	BufferedWriter output;
	Process cppAgent;
	
	@Override
	public void finalize()
	{
		if (cppAgent != null)
		{
			cppAgent.destroy();
		}
	}
	
	public void Init()
	{
		if (bIsInit)
		{
			return;
		}
		
		try {
			String command = fileExecutable;
			ProcessBuilder builder = new ProcessBuilder(
			"cmd.exe", "/c", command);
			builder.redirectErrorStream(true);
			cppAgent = builder.start();
			
			input = new BufferedReader(new InputStreamReader(cppAgent.getInputStream()));
			output = new BufferedWriter(new OutputStreamWriter(cppAgent.getOutputStream()));
			
			bIsInit = true;
		} catch (Exception e) {
			e.printStackTrace();
		}
	}

	@Override
	public void run()
	{
		if (bInitOnRun)
		{
			Init();
		}
		bActive = true;
		while (bActive)
		{
			if (Task != null && Result == null)
			{
				Result = Task.Resolve(this);
				Task = null;
			}
			
			try {
				Thread.sleep(10);
			} catch (InterruptedException e) {
			}
		}
	}
	
	public String ReadToken(String token)
	{
		token = "<" + token + ">";
		StringBuilder message = new StringBuilder();
		try {
			final int tokenLength = token.length();
			final String endSequence = "</>";
			final int endSeqLen = endSequence.length();

			int tokenProgress = 0;
			int endProgress = 0;

			while (true)
			{
				int ReadCode = input.read();
				if (ReadCode == -1)
				{
					Thread.sleep(10);
					continue;
				}
				char ReadChar = (char)ReadCode;

				if (tokenProgress < tokenLength)
				{
					if (ReadChar == token.charAt(tokenProgress))
					{
						++tokenProgress;
					}
					else
					{
						tokenProgress = 0;
					}
					continue;
				}

				if (endProgress < endSeqLen)
				{
					if (ReadChar != endSequence.charAt(endProgress))
					{
						endProgress = 0;
					}
					if (ReadChar == endSequence.charAt(endProgress))
					{
						++endProgress;
						continue;
					}
					if (endProgress == endSeqLen)
					{
						break;
					}
				}

				message.append(ReadChar);
			}
		} catch (Exception e) {
			e.printStackTrace();
		}
		
		return message.toString();
	}
	
	public void WriteToken(String token, String message)
	{
		try {
			output.write(String.format("<%s>%s</>", token, message));
		} catch (Exception e) {
			e.printStackTrace();
		}
	}

}
