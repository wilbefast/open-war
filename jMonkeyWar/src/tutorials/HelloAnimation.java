package tutorials;

import com.jme3.animation.AnimChannel;
import com.jme3.animation.AnimControl;
import com.jme3.animation.AnimEventListener;
import com.jme3.animation.LoopMode;
import com.jme3.app.SimpleApplication;
import com.jme3.input.KeyInput;
import com.jme3.input.controls.ActionListener;
import com.jme3.input.controls.KeyTrigger;
import com.jme3.light.DirectionalLight;
import com.jme3.math.ColorRGBA;
import com.jme3.math.Vector3f;
import com.jme3.scene.Node;

import com.jme3.scene.debug.SkeletonDebugger;
import com.jme3.material.Material;

/** Sample 7 - how to load an OgreXML model and play an animation,
 * using channels, a controller, and an AnimEventListener. */
public class HelloAnimation extends SimpleApplication
        implements AnimEventListener
{
  /// ATTRIBUTES
  private AnimChannel channel;
  private AnimControl control;
  Node player;
  private ActionListener actionListener;

  /// METHODS  @Override
  public void simpleInitApp()
  {
    viewPort.setBackgroundColor(ColorRGBA.LightGray);
    initKeys();
    DirectionalLight dl = new DirectionalLight();
    dl.setDirection(new Vector3f(-0.1f, -1f, -1).normalizeLocal());
    rootNode.addLight(dl);
    player = (Node) assetManager.loadModel("Models/Oto/Oto.mesh.xml");
    player.setLocalScale(0.5f);
    rootNode.attachChild(player);
    control = player.getControl(AnimControl.class);
    control.addListener(this);
    channel = control.createChannel();
    channel.setAnim("stand");
    
    SkeletonDebugger skeletonDebug = 
    new SkeletonDebugger("skeleton", control.getSkeleton());
    Material mat = new Material(assetManager, "Common/MatDefs/Misc/Unshaded.j3md");
    mat.setColor("Color", ColorRGBA.Green);
    mat.getAdditionalRenderState().setDepthTest(false);
    skeletonDebug.setMaterial(mat);
    player.attachChild(skeletonDebug);
  }

  public void onAnimCycleDone(AnimControl control, AnimChannel channel,
                              String animName)
  {
    channel.setAnim("stand", 0.10f);
    channel.setLoopMode(LoopMode.Cycle);
  }

  public void onAnimChange(AnimControl control, AnimChannel channel,
                           String animName)
  {
    // unused
  }

  /** Custom Keybinding: Map named actions to inputs. */
  private void initKeys()
  {
    // set up action listener
    actionListener = new ActionListener()
    {
      public void onAction(String name, boolean keyPressed, float tpf)
      {
        if(!keyPressed)
        {
          if (name.equals("Walk"))
          {
            channel.setAnim("Walk", 0.50f);
            channel.setLoopMode(LoopMode.Cycle);
          }
          
          if (name.equals("Push"))
          {
            channel.setAnim("push", 0.50f);
            channel.setLoopMode(LoopMode.Cycle);
          }
          
          if (name.equals("Pull"))
          {
            channel.setAnim("pull", 0.50f);
            channel.setLoopMode(LoopMode.DontLoop);
          }
           if (name.equals("Dodge"))
          {
            channel.setAnim("Dodge", 0.50f);
            channel.setLoopMode(LoopMode.Cycle);
          }
        }
      }
    };
    
    // create mappings
    inputManager.addMapping("Walk", new KeyTrigger(KeyInput.KEY_SPACE));
    inputManager.addMapping("Push", new KeyTrigger(KeyInput.KEY_O));
    inputManager.addMapping("Pull", new KeyTrigger(KeyInput.KEY_L));
    inputManager.addMapping("Dodge", new KeyTrigger(KeyInput.KEY_P));
    inputManager.addListener(actionListener, "Walk");
    inputManager.addListener(actionListener, "Push");
    inputManager.addListener(actionListener, "Pull");
    inputManager.addListener(actionListener, "Dodge");
  }
}
