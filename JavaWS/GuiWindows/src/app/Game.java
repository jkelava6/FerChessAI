package app;

import java.awt.Graphics;
import java.awt.event.KeyEvent;
import java.awt.event.KeyListener;
import java.awt.event.MouseEvent;
import java.awt.event.MouseListener;
import java.awt.image.BufferStrategy;

import javax.swing.JFrame;

import app.render.WindowsRenderer;
import gui.action.ActionButtonPressType;
import gui.action.ActionType;
import gui.action.KeyboardActionData;
import gui.action.MouseActionData;
import gui.node.base.GuiElement;
import gui.node.base.GuiPoint;
import gui.render.RenderingContext;

public class Game implements Runnable, MouseListener, KeyListener {
	
	private Display display;
	private boolean decorated;
	private boolean fixedSize;
	public int width;
	public int height;
	public String title;
	
	private boolean running = false;
	private Thread thread;
	
	private BufferStrategy bs;
	private Graphics g;
	
	private GuiElement rootElement;
	private long lastStep;
	
	public Game(String title, int width, int height, boolean decorated, boolean fixedSize) {
		this.width = width;
		this.height = height;
		this.title = title;
		this.decorated = decorated;
		this.fixedSize = fixedSize;
	}

	@Override
	public void run() {
		Init();
		GameLoop();
	}
	
	public synchronized void Start(GuiElement guiRoot) {
		if (running) {
			return;
		}
		running = true;
		rootElement = guiRoot;
		thread = new Thread(this);
		thread.start();
	}
	
	public synchronized void Stop() {
		while (running) {
			try {
				thread.join(10);
				running = false;
			} catch (InterruptedException e) {
				e.printStackTrace();
			}
		}
		
		display.Close();
	}
	
	private void Init() {
		display = new Display(title, width, height, decorated, fixedSize);
		display.getCanvas().addMouseListener(this);
		display.getCanvas().addKeyListener(this);
	}
	
	private void GameLoop() {
		lastStep = System.nanoTime();
		Render();
		while (running) {
			Physics();
			Render();
		}
	}
	
	private void Physics() {
		long currentTime = System.nanoTime();
		float step = (currentTime - lastStep) / 1e9f;
		lastStep = currentTime;
		rootElement.TakeTick(step);
	}
	
	private void Render() {
		bs = display.getCanvas().getBufferStrategy();
		if (bs == null) {
			display.getCanvas().createBufferStrategy(3);
			return;
		}
		g = bs.getDrawGraphics();
		
		// back ground color
		//g.setColor(Color.white);
		//g.fillRect(0, 0, width, height);
		// draw elements
		WindowsRenderer renderer = new WindowsRenderer(g);
		width = display.GetWidth();
		height = display.GetHeight();
		rootElement.Render(new RenderingContext(renderer, width, height));
		
		// end drawing
		bs.show();
		g.dispose();
	}
	
	public JFrame GetFrame() {
		return display.GetFrame();
	}

	@Override
	public void keyPressed(KeyEvent event) {
		if (rootElement != null) {
			rootElement.AcceptAction(new KeyboardActionData(ActionType.KEYBOARD, ActionButtonPressType.PRESS,
					KeyEvent.getKeyText(event.getKeyCode())));
		}
	}

	@Override
	public void keyReleased(KeyEvent event) {
		if (rootElement != null) {
			rootElement.AcceptAction(new KeyboardActionData(ActionType.KEYBOARD, ActionButtonPressType.UP,
					KeyEvent.getKeyText(event.getKeyCode())));
		}
	}

	@Override
	public void keyTyped(KeyEvent event) {
		if (rootElement != null) {
			rootElement.AcceptAction(new KeyboardActionData(ActionType.KEYBOARD, ActionButtonPressType.DOWN,
					KeyEvent.getKeyText(event.getKeyCode()),
					Character.valueOf(event.getKeyChar()).toString()));
		}
	}

	@Override
	public void mouseClicked(MouseEvent event) {
		if (rootElement != null) {
			GuiPoint where = new GuiPoint(event.getX() / (float)width, event.getY() / (float)height);
			rootElement.AcceptAction(new MouseActionData(ActionType.MOUSE, ActionButtonPressType.PRESS, where));
		}
	}

	@Override
	public void mouseEntered(MouseEvent event) {
	}

	@Override
	public void mouseExited(MouseEvent event) {
	}

	@Override
	public void mousePressed(MouseEvent event) {
		if (rootElement != null) {
			GuiPoint where = new GuiPoint(event.getX() / (float)width, event.getY() / (float)height);
			rootElement.AcceptAction(new MouseActionData(ActionType.MOUSE, ActionButtonPressType.DOWN, where));
		}
	}

	@Override
	public void mouseReleased(MouseEvent event) {
		if (rootElement != null) {
			GuiPoint where = new GuiPoint(event.getX() / (float)width, event.getY() / (float)height);
			rootElement.AcceptAction(new MouseActionData(ActionType.MOUSE, ActionButtonPressType.UP, where));
		}
	}

}
