package gui;

import gui.node.base.GuiElement;
import gui.node.base.GuiOwner;
import gui.render.RenderColor;
import gui.render.RenderEngine;
import gui.render.RenderImage;
import gui.render.RenderingContext;

public class GuiChessCanvas extends GuiElement {
	
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
	
	public int pieces[][] = new int[8][8];

	public GuiChessCanvas(GuiOwner owner, GuiFrame size)
	{
		super(owner, size);
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
				GuiFrame square = area.SubFrame(new GuiFrame(file * squareSize, (7 - row) * squareSize,
						(file + 1) * squareSize, (8 - row) * squareSize));
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
	}
	
}
