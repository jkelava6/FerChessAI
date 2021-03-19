package gui;
import app.Launcher;

public class ChessWinLauncher {
	
	public static void main(String[] args)
	{
		Launcher.Launch("FerChessAI", new GuiChessRoot(null, GuiFrame.FRAME_FILL()), true, false);
	}

}
