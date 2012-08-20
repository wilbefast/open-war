package tutorials;

import com.jme3.app.SimpleApplication;
import com.jme3.material.Material;
import com.jme3.math.Vector3f;
import com.jme3.scene.Geometry;
import com.jme3.scene.shape.Box;
import com.jme3.math.ColorRGBA;
import com.jme3.input.KeyInput;
import com.jme3.input.MouseInput;
import com.jme3.input.controls.ActionListener;
import com.jme3.input.controls.AnalogListener;
import com.jme3.input.controls.KeyTrigger;
import com.jme3.input.controls.MouseAxisTrigger;
import com.jme3.input.controls.MouseButtonTrigger;


public class HelloInput extends SimpleApplication
{
  /// ATTRIBUTES
  protected Geometry player;
  Boolean isRunning = true;
  private ActionListener actionListener;
  private AnalogListener analogListener;

  /// METHODS
  @Override
  public void simpleInitApp()
  {
    Box b = new Box(Vector3f.ZERO, 1, 1, 1);
    player = new Geometry("Player", b);
    Material mat = new Material(assetManager,
                                "Common/MatDefs/Misc/Unshaded.j3md");
    mat.setColor("Color", ColorRGBA.Blue);
    player.setMaterial(mat);
    rootNode.attachChild(player);
    initKeys(); // load my custom keybinding
  }

  private void initKeys()
  {
        
    // Set up ActionListener
    actionListener = new ActionListener()
    {

      public void onAction(String name, boolean keyPressed, float tpf)
      {
        if (name.equals("Pause") && !keyPressed)
        {
          isRunning = !isRunning;
        }
      }
    };
    
    // Set up AnalogueListener
    analogListener = new AnalogListener()
    {

      public void onAnalog(String name, float value, float tpf)
      {
        if (isRunning)
        {
          if (name.equals("Rotate"))
          {
            player.rotate(0, value * speed, 0);
          }
          if (name.equals("Up"))
          {
            Vector3f v = player.getLocalTranslation();
            player.setLocalTranslation(v.x, v.y + value * speed, v.z);
          }
          if (name.equals("Down"))
          {
            Vector3f v = player.getLocalTranslation();
            player.setLocalTranslation(v.x, v.y - value * speed, v.z);
          }
          if (name.equals("Right"))
          {
            Vector3f v = player.getLocalTranslation();
            player.setLocalTranslation(v.x + value * speed, v.y, v.z);
          }
          if (name.equals("Left"))
          {
            Vector3f v = player.getLocalTranslation();
            player.setLocalTranslation(v.x - value * speed, v.y, v.z);
          }
        }
        else
        {
          System.out.println("Press P to unpause.");
        }
      }
    };
    
    // You can map one or several inputs to one named action
    inputManager.addMapping("Pause", new KeyTrigger(KeyInput.KEY_P));
    inputManager.addMapping("Left", new KeyTrigger(KeyInput.KEY_J));
    inputManager.addMapping("Right", new KeyTrigger(KeyInput.KEY_K));
    inputManager.addMapping("Rotate", new KeyTrigger(KeyInput.KEY_SPACE),
                            new MouseButtonTrigger(MouseInput.BUTTON_LEFT));
    inputManager.addMapping("Up", new KeyTrigger(KeyInput.KEY_G));
    inputManager.addMapping("Down", new KeyTrigger(KeyInput.KEY_H));
    inputManager.addMapping("Up", new MouseAxisTrigger(MouseInput.AXIS_WHEEL, false));
    inputManager.addMapping("Down", new MouseAxisTrigger(MouseInput.AXIS_WHEEL, true));
    // Add the names to the action listener.
    inputManager.addListener(actionListener, new String[]
            { "Pause" });
    inputManager.addListener(analogListener, new String[]
            { "Up", "Down", "Left", "Right", "Rotate" });
  }
}