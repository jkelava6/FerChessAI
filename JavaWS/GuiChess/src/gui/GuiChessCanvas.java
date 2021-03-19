package gui;

import gui.node.base.GuiElement;
import gui.node.base.GuiOwner;
import gui.render.RenderColor;
import gui.render.RenderEngine;
import gui.render.RenderingContext;

public class GuiChessCanvas extends GuiElement {

	public GuiChessCanvas(GuiOwner owner, GuiFrame size) {
		super(owner, size);
	}
	
	@Override
	public void Render(RenderingContext renContext)
	{
		RenderEngine renderer = renContext.renderer;
		GuiFrame area = renContext.frame;
		float squareSize = 0.125f;
		for (int row = 0; row < 8; ++row)
		{
			for (int file = 0; file < 8; ++file)
			{
				GuiFrame square = area.SubFrame(new GuiFrame(row * squareSize, file * squareSize,
						(row + 1) * squareSize, (file + 1) * squareSize));
				RenderColor color = ((row + file) % 2 == 0) ? RenderColor.COL_BLACK : RenderColor.COL_WHITE;
				renderer.SetColor(color);
				renderer.FillBox(square.IntLeft(), square.IntTop(), square.IntWidth(), square.IntHeight());
			}
		}
	}
	
}
