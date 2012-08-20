/**
 * @author william
 * @created 17-Aug-2012
 */

/*
Open War: an open-source real-time tactics game.
Copyright (C) 2012 William James Dyce

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

package system;

import com.jme3.app.SimpleApplication;
import com.jme3.collision.CollisionResult;
import com.jme3.collision.CollisionResults;
import com.jme3.font.BitmapText;
import com.jme3.input.MouseInput;
import com.jme3.input.controls.ActionListener;
import com.jme3.input.controls.MouseButtonTrigger;
import com.jme3.light.DirectionalLight;
import com.jme3.math.ColorRGBA;
import com.jme3.math.Ray;
import com.jme3.math.Vector3f;
import com.jme3.renderer.RenderManager;
import com.jme3.scene.Spatial;


public class MonkeyWar extends SimpleApplication implements ActionListener
{
  /* 
   * ---------------------------------------------------------------------------
   * ATTRIBUTES
   * ---------------------------------------------------------------------------
   */

  /* 
   * ---------------------------------------------------------------------------
   * METHODS
   * ---------------------------------------------------------------------------
   */
  
  /* -------------------- modification -------------------- */
  
  
  /* -------------------- overrides -------------------- */
  
  @Override
  public void simpleInitApp()
  {
    // Set up basic view and control
    initGUI();
    initCamera();
    initInput();
    
    // Load the world
    loadWorld();
  }

  @Override
  public void simpleUpdate(float delta)
  {
    //TODO: add update code
  }

  @Override
  public void simpleRender(RenderManager renderer)
  {
    //TODO: add render code
  }
  
  
  /* -------------------- implementations -------------------- */
  
  public void onAction(String name, boolean isPressed, float delta)
  {
    if(name.equals("Command") && !isPressed)
      injectCommand();
    else if(name.equals("Select") && !isPressed)
      injectSelect();
  }

  /* -------------------- subroutines -------------------- */
  
  private void initGUI()
  {
    guiNode.detachAllChildren();
    guiFont = assetManager.loadFont("Interface/Fonts/Default.fnt");
    BitmapText ch = new BitmapText(guiFont, false);
    ch.setSize(guiFont.getCharSet().getRenderedSize() * 2);
    ch.setText("+"); // crosshairs
    ch.setLocalTranslation( // center
            settings.getWidth() / 2 - guiFont.getCharSet().getRenderedSize() / 3
                                      * 2,
            settings.getHeight() / 2 + ch.getLineHeight() / 2, 0);
    guiNode.attachChild(ch);
  }
  
  
  private void initCamera()
  {
    getCamera().setLocation(new Vector3f(0, 200, 0));
    getCamera().lookAt(Vector3f.ZERO, Vector3f.UNIT_Y);
    flyCam.setMoveSpeed(20);
  }
  
  private void loadWorld()
  {
    // Set up directional light (sun)
    viewPort.setBackgroundColor(ColorRGBA.LightGray);
    DirectionalLight sun = new DirectionalLight();
    sun.setDirection(new Vector3f(-0.1f, -1f, -1).normalizeLocal());
    rootNode.addLight(sun);
    
    // Create Monkeys
    rootNode.attachChild(Monkey.getNode());
    Monkey.spawn(new Vector3f(1, 0, 3), this);
    Monkey.spawn(new Vector3f(3, 0, 1), this);
    Monkey.spawn(new Vector3f(4, 0, 4), this);
    
    // Create Terrain
    rootNode.attachChild(Terrain.getNode());
    new Terrain(this);
  }
  
  private void initInput()
  {
    // Initialise control mappings
    inputManager.addMapping("Select",
                            new MouseButtonTrigger(MouseInput.BUTTON_LEFT));
    inputManager.addMapping("Command",
                            new MouseButtonTrigger(MouseInput.BUTTON_RIGHT));
    inputManager.addMapping("Orbit",
                            new MouseButtonTrigger(MouseInput.BUTTON_MIDDLE));
    inputManager.addListener(this, "Select");
    inputManager.addListener(this, "Command");
    inputManager.addListener(this, "Orbit");

  }
  
  private void injectCommand()
  {
    // Cast a ray from the camera
    CollisionResults results = new CollisionResults();
    Ray ray = new Ray(cam.getLocation(), cam.getDirection());
    // Search for collisions with Terrain
    Terrain.getNode().collideWith(ray, results);
    if(results.size() > 0)
    {
      // Attempt to find the Monkey corresponding to the click
      CollisionResult closest = results.getClosestCollision();
      // Issue a move command to all selected Monkeys
      for(Spatial spatial : Monkey.getNode().getChildren())
      {
        Monkey selected = spatial.getControl(Monkey.class);
        if(selected != null)
          selected.addWaypoint(closest.getContactPoint());
      }
    }
  }
  
  private void injectSelect()
  {
    // Cast a ray from the camera
    CollisionResults results = new CollisionResults();
    Ray ray = new Ray(cam.getLocation(), cam.getDirection());
    // Search for collisions with Monkeys
    Monkey.getNode().collideWith(ray, results);
    if(results.size() > 0)
    {
      // Attempt to find the Monkey corresponding to the click
      Monkey clicked = null;
      Spatial spatial = results.getClosestCollision().getGeometry();
      // NB - We don't know how many levels down the Geometry object is!
      while(spatial != null && clicked == null)
      {
        // Extract the Monkey from the Spatial if possible
        clicked = spatial.getControl(Monkey.class);
        spatial = spatial.getParent();
      }
      if(clicked != null)
        // Select the Unit that was clicked on
        clicked.setSelected(!clicked.isSelected());
    }
  }
}
