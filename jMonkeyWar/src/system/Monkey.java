/**
 * @author william
 * @created 19-Aug-2012
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

import com.jme3.animation.AnimChannel;
import com.jme3.animation.AnimControl;
import com.jme3.animation.LoopMode;
import com.jme3.math.Vector2f;
import com.jme3.math.Vector3f;
import com.jme3.renderer.RenderManager;
import com.jme3.renderer.ViewPort;
import com.jme3.scene.Node;
import com.jme3.scene.Spatial;
import com.jme3.scene.control.AbstractControl;
import com.jme3.scene.control.Control;
import java.util.LinkedList;
import java.util.Queue;

public class Monkey extends AbstractControl implements Cloneable
{
  /* 
   * ---------------------------------------------------------------------------
   * CLASS NAMESPACE CONSTANTS
   * ---------------------------------------------------------------------------
   */

  private static final float WALK_SPEED = 10.0f; 
  
  /* 
   * ---------------------------------------------------------------------------
   * CLASS NAMESPACE VARIABLES
   * ---------------------------------------------------------------------------
   */

  private static Node node;
  
  /* 
   * ---------------------------------------------------------------------------
   * CLASS NAMESPACE FUNCTIONS
   * ---------------------------------------------------------------------------
   */

  public static Node getNode()
  {
    if(node == null)
      node = new Node();
    return node;
  }
  
  public static void spawn(Vector3f position, MonkeyWar war)
  {
    // Load Spatial from file
    Spatial spatial = war.getAssetManager().loadModel("Models/Oto/Oto.mesh.xml");
    
    // Set up Spatial and attach to the scene graph
    node.attachChild(spatial);
    spatial.setLocalTranslation(position);
    spatial.setLocalScale(0.2f);
    // Create the Unit that will manage this Spatial and attach it
    Monkey spawn = new Monkey();
    spatial.addControl(spawn);
  }
  
  /* 
   * ---------------------------------------------------------------------------
   * NESTING
   * ---------------------------------------------------------------------------
   */

  private static enum State
  {
    IDLING, WALKING
  }
  
  /* 
   * ---------------------------------------------------------------------------
   * ATTRIBUTES
   * ---------------------------------------------------------------------------
   */

  // state
  private State state = State.IDLING;
  // orders
  private boolean selected = true; //false;
  private Queue<Vector3f> waypoints = new LinkedList<Vector3f>();
  private Vector3f destination;
  private Vector3f direction;
  private float distance_left = 0.0f;
  // animation
  private AnimChannel channel;
  
  /* 
   * ---------------------------------------------------------------------------
   * METHODS
   * ---------------------------------------------------------------------------
   */
  
  // creation
  /**
   * Empty constructor for serialisation
   */
  public Monkey()
  {
  } 
  
  /* -------------------- modification -------------------- */
  /**
   * Add a new destination to the Monkey's list of order
   * @param waypoint the new position to queue in the list of places to travel
   * to.
   */
  void addWaypoint(Vector3f waypoint)
  {
    if(selected)
      waypoints.add(waypoint);

  }
  
  /**
   * Select or deselect this Monkey.
   * @param _selected true if the Monkey is to be selected, false if it is to be
   * deselected.
   */
  public void setSelected(boolean _selected)
  {
    selected = _selected;
  }
  
  /* -------------------- query -------------------- */
  /**
   * Is this Monkey selected by the local player?
   * @return true if the Monkey is currently selected by the player, false if
   * not.
   */
  public boolean isSelected()
  {
    return selected;
  }

  /* -------------------- overrides -------------------- */
  
  /**
   * Change the Spatial object that this baby-sitter Monkey is applied to.
   * @param _spatial the new Spatial to be controlled.
   */
  @Override
  public void setSpatial(Spatial _spatial) 
  {
    // Standard stuff
    super.setSpatial(_spatial);
    // Set up animation
    channel = spatial.getControl(AnimControl.class).createChannel();
    channel.setAnim("stand");
    
  }
  
  
  /* -------------------- implementations -------------------- */

  @Override
  protected void controlUpdate(float delta)
  {
    // Try to get a new destination if currently idle
    if(state == State.IDLING)
      nextWaypoint();

    // Currently has a destination
    else
    {
      // Move an amount dependent on the time elapsed since last frame
      float move_amount = WALK_SPEED * delta;
      // Decrement the amount of distance left to move
      distance_left -= move_amount;
      // Check whether we have reached our destination
      if (distance_left <= 0.0f)
      {
        // Jump to target position if an overlap occurs
        spatial.setLocalTranslation(destination);
        // Start towards new location if there is one
        nextWaypoint();
      }
      else
        // Move towards the new waypoints
        spatial.move(direction.mult(move_amount));
    }
    
    // Always stay above terrain
    Vector3f pos = spatial.getLocalTranslation();
    float terrain_h = Terrain.getHeight(new Vector2f(pos.x, pos.z));
    spatial.setLocalTranslation(pos.x, terrain_h, pos.z);
  }

  @Override
  protected void controlRender(RenderManager renderer, ViewPort view)
  {
    // to nothing
  }

  public Control cloneForSpatial(Spatial spatial)
  {
    Monkey clone = new Monkey();
    clone.setSpatial(spatial);
    return clone;
  }
  
  
  /* -------------------- subroutines -------------------- */
 
  /**
   * Pop and start moving towards a new waypoint if there is one, setting 
   * animations accordingly. If not, stop and set the stand animation.
   */
  private void nextWaypoint()
  {
    if(waypoints.isEmpty())
    {
      if(state != State.IDLING)
      {
        // Step the idle animation
        channel.setAnim("stand", 0.10f);
        channel.setLoopMode(LoopMode.Cycle);

        // We are now idling again
        state = State.IDLING;
      }
    }
    else
    {
      // Get the next destination from the queue
      destination = waypoints.poll();
      distance_left = destination.distance(spatial.getWorldTranslation());
      spatial.lookAt(destination, Vector3f.UNIT_Y);
      
      // Reset the direction vector (should always be normalised)
      direction = destination.subtract(spatial.getWorldTranslation());
      direction.normalizeLocal();
      
      if(state != State.WALKING)
      {
        // Set walking animation
        channel.setAnim("Walk", 0.50f);
        channel.setLoopMode(LoopMode.Cycle);

        // We are now moving again
        state = State.WALKING;
      }
    }
  }

}