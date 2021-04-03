package gui;

import java.util.Vector;

import cpp.CppInterface;
import cpp.CppResult;
import cpp.task.CppDoActionGetBoard;
import cpp.task.CppGetMoves;
import gui.action.ActionButtonPressType;
import gui.action.ActionData;
import gui.action.ActionType;
import gui.action.MouseActionData;
import gui.node.base.GuiElement;
import gui.node.base.GuiOwner;
import gui.render.RenderColor;
import gui.render.RenderEngine;
import gui.render.RenderImage;
import gui.render.RenderingContext;

public class GuiChessCanvas extends GuiElement {
	
	GuiChessRoot rootOwner;
	private static String imagePaths[] = {
			"res/BKing.png",
			"res/BQueen.png",
			"res/BRook.png",
			"res/BBishop.png",
			"res/BKnight.png",
			"res/BPawn.png",
			"",
			"res/WPawn.png",
			"res/WKnight.png",
			"res/WBishop.png",
			"res/WRook.png",
			"res/WQueen.png",
			"res/WKing.png"
	};
	
	private RenderImage images[];
	private boolean bFlipped = false;
	
	public int pieces[][] = new int[8][8];
	
	int rankFrom = -1;
	int fileFrom = -1;
	
	Vector<Integer> ranks = new Vector<Integer>();
	Vector<Integer> files = new Vector<Integer>();

	public GuiChessCanvas(GuiOwner owner, GuiFrame size)
	{
		super(owner, size);
		
		if (owner instanceof GuiChessRoot)
		{
			rootOwner = (GuiChessRoot)owner;
		}
	}
	
	@Override
	public void Render(RenderingContext renContext)
	{
		RenderEngine renderer = renContext.renderer;
		
		if (images == null)
		{
			images = new RenderImage[imagePaths.length];
			for (int i = 0; i < imagePaths.length; ++i)
			{
				if (imagePaths[i].length() > 0)
				{
					images[i] = renderer.LoadImage(imagePaths[i], GuiFrame.FRAME_FILL());
				}
			}
		}
		
		GuiFrame area = renContext.frame;
		float squareSize = 0.125f;
		for (int row = 0; row < 8; ++row)
		{
			for (int file = 0; file < 8; ++file)
			{
				int rowFlipProcessed = bFlipped ? row : 7 - row;
				GuiFrame square = area.SubFrame(new GuiFrame(file * squareSize, rowFlipProcessed * squareSize,
						(file + 1) * squareSize, (rowFlipProcessed + 1) * squareSize));
				RenderColor color = ((row + file) % 2 == 0) ? RenderColor.COL_GRAY : RenderColor.COL_WHITE;
				renderer.SetColor(color);
				renderer.FillBox(square.IntLeft(), square.IntTop(), square.IntWidth(), square.IntHeight());
				
				if (pieces[row][file] != 0)
				{
					renderer.DrawImage(images[6 + pieces[row][file]], square.IntLeft(), square.IntTop(),
							square.IntWidth(), square.IntHeight());
				}
			}
		}

		RenderColor moveColor = new RenderColor(127, 0, 0, 255);
		renderer.SetColor(moveColor);
		for (int move = 0; move < ranks.size(); ++move)
		{
			int row = ranks.elementAt(move);
			int file = files.elementAt(move);
			int rowFlipProcessed = bFlipped ? row : 7 - row;
			GuiFrame square = area.SubFrame(new GuiFrame(file * squareSize, rowFlipProcessed * squareSize,
					(file + 1) * squareSize, (rowFlipProcessed + 1) * squareSize));
			float moveSize = 0.15f;
			float halfSize = 0.5f * moveSize;
			GuiFrame moveMark = square.SubFrame(new GuiFrame(0.5f - halfSize, 0.5f - halfSize,
					0.5f + halfSize, 0.5f + halfSize));
			renderer.FillEllipse(moveMark.IntLeft(), moveMark.IntTop(), moveMark.IntWidth(), moveMark.IntHeight());
		}
	}
	
	@Override
	public void TakeTick(float step)
	{
		super.TakeTick(step);

		CppInterface cpp = rootOwner.cppInterface;
		if (cpp.Task == null && cpp.Result != null)
		{
			CppResult result = cpp.Result;
			if (result.token.equals("moves"))
			{
				cpp.Result = null;
				
				ranks.clear();
				files.clear();
				
				String message = result.message;
				for (int read = 0; read < message.length(); read += 2)
				{
					ranks.add(message.charAt(read) - '0');
					files.add(message.charAt(read + 1) - '0');
				}
			}
		}
	}
	
	@Override
	public void HandleAction(ActionData action)
	{
		if (action.type == ActionType.MOUSE && action.pressType == ActionButtonPressType.DOWN)
		{
			CppInterface cpp = rootOwner.cppInterface;
			MouseActionData mouse = (MouseActionData)action;
			int rankPressed = (int)(8 * mouse.point.y);
			int filePressed = (int)(8 * mouse.point.x);
			if (!bFlipped)
			{
				rankPressed = 7 - rankPressed;
			}

			if (rankFrom == -1)
			{
				rankFrom = rankPressed;
				fileFrom = filePressed;
				cpp.Task = new CppGetMoves(rankFrom, fileFrom);
				return;
			}
			
			if (rankFrom == rankPressed && fileFrom == filePressed)
			{
				rankFrom = -1;
				fileFrom = -1;
				ranks.clear();
				files.clear();
				return;
			}

			if (cpp.Task != null || cpp.Result != null)
			{
				System.err.println("Interface busy!");
				return;
			}

			for (int move = 0; move < ranks.size(); ++move)
			{
				if (ranks.elementAt(move) == rankPressed && files.elementAt(move) == filePressed)
				{
					cpp.Task = new CppDoActionGetBoard("move",
							String.format("%d%d%d%d", rankFrom, fileFrom, rankPressed, filePressed));
					rootOwner.bWhitesTurn = !rootOwner.bWhitesTurn;
					ranks.clear();
					files.clear();
					rankFrom = -1;
					fileFrom = -1;
					return;
				}
			}

			rankFrom = rankPressed;
			fileFrom = filePressed;
			ranks.clear();
			files.clear();
			cpp.Task = new CppGetMoves(rankFrom, fileFrom);
			return;
		}
	}
	
}
