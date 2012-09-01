package tutorials;

import com.jme3.app.SimpleApplication;
import com.jme3.font.BitmapText;
import com.jme3.light.DirectionalLight;
import com.jme3.material.Material;
import com.jme3.math.FastMath;
import com.jme3.math.Vector3f;
import com.jme3.scene.Geometry;
import com.jme3.scene.Spatial;
import com.jme3.scene.shape.Box;
 
public class HelloAssets extends SimpleApplication 
{
  @Override
  public void simpleInitApp() 
  {
    /** Local variables */
    Material mat;
    
    /** Create a teapot */
    Spatial teapot = assetManager.loadModel("Models/Teapot/Teapot.obj");
    mat = new Material(assetManager, "Common/MatDefs/Misc/ShowNormals.j3md");
    teapot.setMaterial(mat);
    rootNode.attachChild(teapot);
    
    flyCam.setMoveSpeed(5.0f);

    /** Create a wall */
    Box box = new Box(Vector3f.ZERO, 2.5f,2.5f,1.0f);
    Spatial wall = new Geometry("Box", box );
    mat = new Material(assetManager, "Common/MatDefs/Misc/Unshaded.j3md");
    mat.setTexture("ColorMap",
         assetManager.loadTexture("Textures/Terrain/BrickWall/BrickWall.jpg"));
    wall.setMaterial(mat);
    wall.setLocalTranslation(2.0f,-2.5f,0.0f);
    rootNode.attachChild(wall);
    
    /** Clear GUI */
    guiNode.detachAllChildren();

    /** Display a line of text with a default font */
    guiFont = assetManager.loadFont("Interface/Fonts/Default.fnt");
    BitmapText helloText = new BitmapText(guiFont, false);
    helloText.setSize(guiFont.getCharSet().getRenderedSize());
    helloText.setText("Hello World");
    helloText.setLocalTranslation(300, helloText.getLineHeight(), 0);
    guiNode.attachChild(helloText);

    /** Load a model from test_data (OgreXML + material + texture) */
    Spatial knight = assetManager.loadModel("Models/knight/knight.mesh.xml");
    knight.scale(1.2f);
    knight.rotate(0.0f, -FastMath.HALF_PI, 0.0f); 
    knight.setLocalTranslation(-2.0f, 0.0f, -2.0f);
    rootNode.attachChild(knight);
    
    /** We must add a light to make the model visible */
    DirectionalLight sun = new DirectionalLight();
    sun.setDirection(new Vector3f(-0.1f, -0.7f, -1.0f));
    rootNode.addLight(sun);

  }
}
