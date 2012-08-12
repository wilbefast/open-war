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

#include <iostream>

#include "Application.hpp"

using namespace std;
using namespace Ogre;

/// CREATION, DESTRUCTION
//------------------------------------------------------------------------------
Application::Application() :
BaseApplication(),
soldiers(),
ray_query(NULL),
r_mouse(false), l_mouse(false),
cursor_pos(Vector3::ZERO),
gui_renderer()
{
}
//------------------------------------------------------------------------------
Application::~Application()
{
  // We created the query, and we are also responsible for deleting it.
  mSceneMgr->destroyQuery(ray_query);

  // Delete all the Soldiers
  for(SoldierIter i = soldiers.begin(); i != soldiers.end(); i++)
    delete i->second;
}

//------------------------------------------------------------------------------
void Application::createScene(void)
{
	// Set ambient light
	mSceneMgr->setAmbientLight(Ogre::ColourValue(0.5, 0.5, 0.5));

	// World geometry
	mSceneMgr->setWorldGeometry("terrain.cfg");

	// CEGUI setup
  gui_renderer = &CEGUI::OgreRenderer::bootstrapSystem();

  // Mouse
  CEGUI::SchemeManager::getSingleton().create((CEGUI::utf8*)"TaharezLook.scheme");
  CEGUI::MouseCursor::getSingleton().setImage("TaharezLook", "MouseArrow");

  // Set the camera to look at our handiwork
  mCamera->setPosition(90.0f, 280.0f, 535.0f);
  mCamera->pitch(Ogre::Degree(-30.0f));
  mCamera->yaw(Ogre::Degree(-15.0f));
}
//------------------------------------------------------------------------------

/// FRAME LISTENER
//------------------------------------------------------------------------------
void Application::createFrameListener(void)
{
  // Listen out from screen refreshes
	BaseApplication::createFrameListener();

  // Create RaySceneQuery
  ray_query = mSceneMgr->createRayQuery(Ogre::Ray());
}
//------------------------------------------------------------------------------
bool Application::frameRenderingQueued(const Ogre::FrameEvent &evt)
{
  // Process the base frame listener code.  Since we are going to be
  // manipulating the translate vector, we need this to happen first.
  if (!BaseApplication::frameRenderingQueued(evt))
   return false;

  // Check for collisions with terrain
  Vector3 collision;
	if(getTerrainCollision(getBelowPosition(mCamera->getPosition()), &collision))
    mCameraMan->stayAbove(collision.y + 20.0f, evt.timeSinceLastFrame);

  // Update the game objects
  for(SoldierIter i = soldiers.begin(); i != soldiers.end(); i++)
    i->second->update(evt.timeSinceLastFrame);

	return true;
}
//------------------------------------------------------------------------------

/// MOUSE LISTENER
//------------------------------------------------------------------------------
bool Application::mouseMoved(const OIS::MouseEvent &evt)
{
  // Base application logic
  if(!BaseApplication::mouseMoved(evt))
    return false;

  // Find point that cursor is pointing to
  getTerrainCollision(getUnderCursor(evt.state), &cursor_pos);


  // Update CEGUI with the mouse motion
  CEGUI::System::getSingleton().injectMouseMove(evt.state.X.rel, evt.state.Y.rel);

  return true;
}
//------------------------------------------------------------------------------
bool Application::mousePressed(const OIS::MouseEvent &evt, OIS::MouseButtonID id)
{
  // Base application logic
  if(!BaseApplication::mousePressed(evt, id))
    return false;


  // Left mouse button down
  if (id == OIS::MB_Left)
  {
    // Set mouse state
    l_mouse = true;

    // Select Soldiers under cursor
    Soldier* selection = getSoldierCollision(getUnderCursor(evt.state));
    if(selection)
      selection->setSelected(!selection->isSelected());
    else
      // Move Soldiers to empty area if nothing to select
      for(SoldierIter i = soldiers.begin(); i != soldiers.end(); i++)
        if(i->second->isSelected())
          i->second->addWaypoint(cursor_pos);
  }

  // Right mouse button down
  else if (id == OIS::MB_Right)
  {
    // Set mouse state
    r_mouse = true;

    // Create a new Soldier
    Soldier* new_soldier = new Soldier();
    new_soldier->attach(soldiers, mSceneMgr, cursor_pos);
  }

  // consume event
  return true;
}
//------------------------------------------------------------------------------
bool Application::mouseReleased(const OIS::MouseEvent &evt, OIS::MouseButtonID id)
{
  // Base application logic
  if(!BaseApplication::mouseReleased(evt, id))
    return false;

  // Left mouse button down
  if (id == OIS::MB_Left)
    l_mouse = false;

  // Right mouse button down
  else if (id == OIS::MB_Right)
    r_mouse = false;

  // consume event
  return true;
}
//------------------------------------------------------------------------------

/// UTILITY
//------------------------------------------------------------------------------
RaySceneQueryResult Application::getUnderCursor(OIS::MouseState mouse_state)
{
  // Calculate the ray implied by the cursor's position on the screen
  CEGUI::Point mouse_pos = CEGUI::MouseCursor::getSingleton().getPosition();
  Ray mouse_ray =
    mCamera->getCameraToViewportRay(mouse_pos.d_x/float(mouse_state.width),
                                    mouse_pos.d_y/float(mouse_state.height));

  // Execute the query and return the result
  ray_query->setRay(mouse_ray);
  return ray_query->execute();
}
//------------------------------------------------------------------------------
RaySceneQueryResult Application::getBelowPosition(Vector3 position)
{
  // Build and execute the query and return the result
  Ray down_ray = Ray(position, Vector3::NEGATIVE_UNIT_Y);
  ray_query->setRay(down_ray);
  return ray_query->execute();
}
//------------------------------------------------------------------------------
bool Application::getTerrainCollision(RaySceneQueryResult in, Vector3* out)
{
  // Get the first collision point
  for(RaySceneQueryResult::iterator i = in.begin(); i != in.end(); i++)
    // Ray intersects terrain geometry
    if(i->worldFragment)
    {
      // Query can be made with result = NULL, in which case don't write there!
      if(out)
        (*out) = i->worldFragment->singleIntersection;

      // Report whether a collision was found or not
      return true;
    }
  return false;
}
//------------------------------------------------------------------------------
Soldier* Application::getSoldierCollision(RaySceneQueryResult in)
{
  // Get the first Soldier collided with
  for(RaySceneQueryResult::iterator i = in.begin(); i != in.end(); i++)
    // Ray intersects movable Entity
    if(i->movable)
    {
      SoldierIter soldier_i = soldiers.find(i->movable);
      if(soldier_i != soldiers.end())
          return soldier_i->second;
    }
  return NULL;
}
