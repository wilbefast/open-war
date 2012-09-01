/**
 * @author william
 * @created 20-Aug-2012
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

import com.jme3.material.Material;
import com.jme3.math.Vector2f;
import com.jme3.renderer.RenderManager;
import com.jme3.renderer.ViewPort;
import com.jme3.scene.Node;
import com.jme3.scene.Spatial;
import com.jme3.scene.control.AbstractControl;
import com.jme3.scene.control.Control;
import com.jme3.terrain.geomipmap.TerrainLodControl;
import com.jme3.terrain.geomipmap.TerrainQuad;
import com.jme3.terrain.heightmap.AbstractHeightMap;
import com.jme3.terrain.heightmap.ImageBasedHeightMap;
import com.jme3.texture.Texture;
import com.jme3.texture.Texture.WrapMode;

public class Terrain extends AbstractControl implements Cloneable
{
  /* 
   * ---------------------------------------------------------------------------
   * CLASS-NAMESPACE VARIABLES
   * ---------------------------------------------------------------------------
   */
  private static Node node;


  /* 
   * ---------------------------------------------------------------------------
   * CLASS-NAMESPACE FUNCTIONS
   * ---------------------------------------------------------------------------
   */
  public static Node getNode()
  {
    if(node == null)
      node = new Node();
    return node;
  }
  
  public static float getHeight(Vector2f position)
  {
    // Search for a TerrainQuad that corresponds to the specified position
    for(Spatial spatial : Terrain.node.getChildren())
    {
      Terrain terrain = spatial.getControl(Terrain.class);
      if(terrain != null)
      {
        float result = terrain.quad.getHeight(position);
        if(result != Float.NaN)
          return result;
      }
    }
    // Return NaN if no TerrainQuad underlies the requested position
    return Float.NaN;
  }


  /* 
   * ---------------------------------------------------------------------------
   * ATTRIBUTES
   * ---------------------------------------------------------------------------
   */
  private TerrainQuad quad;
  private Material material;

  /* 
   * ---------------------------------------------------------------------------
   * METHODS
   * ---------------------------------------------------------------------------
   */


  /* -------------------- creation -------------------- */
  
  /**
   * Empty constructor for serialisation
   */
  public Terrain()
  {
  } 
  
  public Terrain(MonkeyWar war)
  {
    /** 1. Create terrain material and load four textures into it. */
    material = new Material(war.getAssetManager(),
                               "Common/MatDefs/Terrain/Terrain.j3md");

    /** 1.1) Add ALPHA map (for red-blue-green coded splat textures) */
    material.setTexture("Alpha", war.getAssetManager().loadTexture(
            "Textures/Terrain/splat/alphamap.png"));

    /** 1.2) Add GRASS texture into the red layer (Tex1). */
    Texture grass = war.getAssetManager().loadTexture(
            "Textures/Terrain/splat/grass.jpg");
    grass.setWrap(WrapMode.Repeat);
    material.setTexture("Tex1", grass);
    material.setFloat("Tex1Scale", 64f);

    /** 1.3) Add DIRT texture into the green layer (Tex2) */
    Texture dirt = war.getAssetManager().loadTexture(
            "Textures/Terrain/splat/dirt.jpg");
    dirt.setWrap(WrapMode.Repeat);
    material.setTexture("Tex2", dirt);
    material.setFloat("Tex2Scale", 32f);

    /** 1.4) Add ROAD texture into the blue layer (Tex3) */
    Texture rock = war.getAssetManager().loadTexture(
            "Textures/Terrain/splat/road.jpg");
    rock.setWrap(WrapMode.Repeat);
    material.setTexture("Tex3", rock);
    material.setFloat("Tex3Scale", 128f);

    /** 2. Create the height map */
    AbstractHeightMap heightmap = null;
    Texture heightMapImage = war.getAssetManager().loadTexture(
            "Textures/Terrain/splat/mountains512.png");
    heightmap = new ImageBasedHeightMap(heightMapImage.getImage());
    heightmap.load();

    /** 3. We have prepared material and heightmap. 
     * Now we create the actual terrain:
     * 3.1) Create a TerrainQuad and name it "my terrain".
     * 3.2) A good value for terrain tiles is 64x64 -- so we supply 64+1=65.
     * 3.3) We prepared a heightmap of size 512x512 -- so we supply 512+1=513.
     * 3.4) As LOD step scale we supply Vector3f(1,1,1).
     * 3.5) We supply the prepared heightmap itself.
     */
    int patchSize = 65;
    quad = new TerrainQuad("my terrain", patchSize, 513, heightmap.
            getHeightMap());

    /** 4. We give the terrain its material, position & scale it, and attach it. */
    quad.setMaterial(material);
    quad.setLocalScale(2f, 1f, 2f);
    Terrain.node.attachChild(quad);
    
    /** 5. The LOD (level of detail) depends on were the camera is: */
    TerrainLodControl lod = new TerrainLodControl(quad, war.getCamera());
    quad.addControl(lod);
    
    /** ATTACH THE TERRAIN CONTROL TO THE TERRAIN */
    quad.addControl(this);
  }

  /* -------------------- query -------------------- */


  /* -------------------- modification -------------------- */


  /* -------------------- overrides -------------------- */


  /* -------------------- implementations -------------------- */

  @Override
  protected void controlUpdate(float tpf)
  {
    // do nothing
  }

  @Override
  protected void controlRender(RenderManager rm, ViewPort vp)
  {
    // do nothing
  }

  public Control cloneForSpatial(Spatial spatial)
  {
    Terrain clone = new Terrain();
    clone.setSpatial(spatial);
    return clone;
  }
}
