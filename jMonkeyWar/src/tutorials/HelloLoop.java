package tutorials;
 
import com.jme3.app.SimpleApplication;
import com.jme3.material.Material;
import com.jme3.math.ColorRGBA;
import com.jme3.math.Vector3f;
import com.jme3.scene.Geometry;
import com.jme3.scene.shape.Box;
 
public class HelloLoop extends SimpleApplication 
{
  /// ATTRIBUTES
  protected Geometry player;
  protected ColorRGBA colour;
  protected float scale = 1.0f;

  
  /// METHODS
  @Override
  public void simpleInitApp() 
  {
    // Create the player
    Box b = new Box(Vector3f.ZERO, 1, 1, 1);
    player = new Geometry("blue cube", b);
    rootNode.attachChild(player);
    
    // Create the material
    Material mat = new Material(assetManager,
      "Common/MatDefs/Misc/Unshaded.j3md");
    colour = ColorRGBA.randomColor();
    mat.setColor("Color", colour);
    player.setMaterial(mat);
    
  }

  /* This is the update loop */
  @Override
  public void simpleUpdate(float delta) 
  {
    // make the player rotate
    player.rotate(delta, 2*delta, 3*delta); 
    
    // change colour
    colour.interpolate(ColorRGBA.randomColor(), Math.min(1.0f, delta));
    
    // change size
    scale += (scale > 1.0f ? -1 : 1) * delta * 0.5f;
    player.scale(scale);
  }
}