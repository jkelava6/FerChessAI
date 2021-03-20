package gui;

import cpp.CppInterface;
import cpp.CppResult;
import cpp.task.CppDoActionGetBoard;
import gui.node.base.GuiGroup;
import gui.node.base.GuiOwner;

public class GuiChessRoot extends GuiGroup
{
	CppInterface cppInterface;
	Thread cppThread;
	GuiChessCanvas canvas;
	boolean bWhitesTurn = true;
	boolean bWhiteAI = false;
	boolean bBlackAI = true;

	public GuiChessRoot(GuiOwner owner, GuiFrame size)
	{
		super(owner, size);
		
		cppInterface = new CppInterface();
		cppInterface.bInitOnRun = true;
		cppThread = new Thread(cppInterface);
		cppThread.start();
		
		canvas = new GuiChessCanvas(this, GuiFrame.FRAME_FILL());
		super.AddElement(canvas);
		
		cppInterface.Task = new CppDoActionGetBoard("start", "");
	}

	@Override
	public void TakeTick(float step)
	{
		super.TakeTick(step);
		
		if (cppInterface.Result != null && cppInterface.Task == null)
		{
			CppResult result = cppInterface.Result;
			
			if (result.token.equals("board"))
			{
				cppInterface.Result = null;
				for (int row = 0; row < 8; ++row)
				{
					for (int file = 0; file < 8; ++file)
					{
						char pieceChar = result.message.charAt(row * 8 + file);
						int pieceInt = (int)pieceChar - (int)'a' - 6;
						canvas.pieces[row][file] = pieceInt;
					}
				}
			}
		}
		
		if (cppInterface.Task == null && cppInterface.Result == null)
		{
			if (bWhitesTurn && bWhiteAI || !bWhitesTurn && bBlackAI)
			{
				cppInterface.Task = new CppDoActionGetBoard("ai", "");
				bWhitesTurn = !bWhitesTurn;
			}
		}
	}
	

}
