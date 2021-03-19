package gui.node.base;

public interface GuiOwner {
	
	public boolean RequestFocus(GuiElement child);
	
	public boolean CheckFocus(GuiElement child);
	
	public void ForceFocus(GuiElement element, boolean propagate);

}
