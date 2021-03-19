import app.Launcher;
import gui.GuiChessRoot;
import gui.GuiFrame;

public class ChessWinLauncher {
	
	public static void main(String[] args)
	{
		Launcher.Launch("FerChessAI", new GuiChessRoot(null, GuiFrame.FRAME_FILL()), true, false);
	}

}
