package cpp.task;

import cpp.CppInterface;
import cpp.CppResult;
import cpp.CppTask;

public class CppGetMoves implements CppTask {
	
	private int rank;
	private int file;
	
	public CppGetMoves(int moveRank, int moveFile)
	{
		rank = moveRank;
		file = moveFile;
	}

	@Override
	public CppResult Resolve(CppInterface cppInterface)
	{
		cppInterface.WriteToken("search", String.format("%d%d", rank, file));
		return new CppResult("moves", cppInterface.ReadToken("moves"));
	}

}