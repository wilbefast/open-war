package tutorials;

import com.jme3.app.SimpleApplication;
import com.jme3.material.Material;
import com.jme3.math.ColorRGBA;
import com.jme3.math.Vector3f;
import com.jme3.renderer.RenderManager;
import com.jme3.scene.Geometry;
import com.jme3.scene.shape.Box;
import com.jme3.scene.Node;

public class HelloNode extends SimpleApplication
{
  @Override
  public void simpleInitApp()
  {
    /** Local variables */
    Material mat;
    Geometry geom;
    Box box;
    Node node;
    
    /** Build a simple scene graph */
    node = new Node("pivot");
    rootNode.attachChild(node);
    
    /** Rotate the pivot node: all attached spatials will be rotated */
    node.rotate(.4f,.4f,0f);
    
    /** Create a blue box at coordinates (1,-1,1) */
    box = new Box(new Vector3f(1, -1, 1), 1, 1, 1);
    geom = new Geometry("Box", box);
    mat = new Material(assetManager, "Common/MatDefs/Misc/Unshaded.j3md");
    mat.setColor("Color", ColorRGBA.Blue);
    geom.setMaterial(mat);
    node.attachChild(geom);

    /** Create a red box straight above the blue one at (1,3,1) */
    box = new Box(new Vector3f(1, 3, 1), 1, 1, 1);
    geom = new Geometry("Box", box);
    mat = new Material(assetManager, "Common/MatDefs/Misc/Unshaded.j3md");
    mat.setColor("Color", ColorRGBA.Red);
    geom.setMaterial(mat);
    node.attachChild(geom);
  }

  @Override
  public void simpleUpdate(float tpf)
  {
    //TODO: add update code
  }

  @Override
  public void simpleRender(RenderManager rm)
  {
    //TODO: add render code
  }
}
