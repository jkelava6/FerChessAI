package gui;

import cpp.CppInterface;
import cpp.task.CppReadToken;
import gui.node.base.GuiGroup;
import gui.node.base.GuiOwner;

public class GuiChessRoot extends GuiGroup
{
	CppInterface cppInterface;
	Thread cppThread;

	public GuiChessRoot(GuiOwner owner, GuiFrame size)
	{
		super(owner, size);
		
		cppInterface = new CppInterface();
		cppInterface.bInitOnRun = true;
		cppThread = new Thread(cppInterface);
		cppThread.start();
		
		super.AddElement(new GuiChessCanvas(this, GuiFrame.FRAME_FILL()));
		
		cppInterface.Task = new CppReadToken("board");
	}

	@Override
	public void TakeTick(float step)
	{
		super.TakeTick(step);
		
		if (cppInterface.Result != null && cppInterface.Task == null)
		{
			cppInterface.Result = null;
		}
	}
	

}
