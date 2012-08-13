/*
Open War: an open-source Total War clone, written in C++ using Ogre3D.
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

#ifndef APPLICATION_HPP_INCLUDED
#define APPLICATION_HPP_INCLUDED

#include <list>

#include <CEGUI/CEGUI.h>
#include <CEGUI/RendererModules/Ogre/CEGUIOgreRenderer.h>

#include <OGRE/Terrain/OgreTerrain.h>
#include <OGRE/Terrain/OgreTerrainGroup.h>

#include "BaseApplication.h"
#include "Soldier.hpp"

class Application : public BaseApplication
{
  /// ATTRIBUTES
private:
  SoldierMap soldiers;
  Ogre::RaySceneQuery *ray_query;       // The ray scene query pointer
  bool r_mouse, l_mouse;		            // True if the mouse buttons are down
  Ogre::Vector3 cursor_pos;             // Position of the cursor in the world
  CEGUI::Renderer *gui_renderer;		    // CEGUI renderer
  // terrain
  Ogre::TerrainGlobalOptions* mTerrainGlobals;
  Ogre::TerrainGroup* mTerrainGroup;
  bool mTerrainsImported;

  /// METHODS
public:
  // creation, destruction
  Application();
  virtual ~Application();
  // query
  Ogre::RaySceneQueryResult getUnderCursor(OIS::MouseState mouse_state);
  Ogre::RaySceneQueryResult getBelowPosition(Ogre::Vector3 position);
  bool getTerrainCollision(Ogre::RaySceneQueryResult in, Ogre::Vector3* out = NULL);
  Soldier* getSoldierCollision(Ogre::RaySceneQueryResult in);

  /// SUBROUTINES
protected:
  // creation
  virtual void createScene();
  // frame listener
  virtual void createFrameListener();
  virtual bool frameRenderingQueued(const Ogre::FrameEvent &evt);
  // mouse listener
  virtual bool mouseMoved(const OIS::MouseEvent &evt);
  virtual bool mousePressed(const OIS::MouseEvent &evt,OIS::MouseButtonID id);
  virtual bool mouseReleased(const OIS::MouseEvent &evt,OIS::MouseButtonID id);
  // terrain
  void defineTerrain(long x, long y);
  void initBlendMaps(Ogre::Terrain* terrain);
  void configureTerrainDefaults(Ogre::Light* light);
};

#endif // APPLICATION_HPP_INCLUDED
