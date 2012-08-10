#include <iostream>

#include "Application.hpp"

using namespace Ogre;
using namespace std;

/// CREATION
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

  // Create the waypoints
  Waypoint::attach_all(mSceneMgr);

  // Create the two soldiers
  Soldier* s = new Soldier();
	s->attach(mSceneMgr, Vector3(150.0f, 0.0f, 25.0f));
	soldiers.push_back(s);
	s = new Soldier();
	s->attach(mSceneMgr, Vector3(-200.0f, 0.0f, 300.0f));
	soldiers.push_back(s);

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

  // Check for collisions with terrain only if the camera has moved
  static Ogre::Vector3 cam_pos = mCamera->getPosition();
  if(cam_pos != mCamera->getPosition())
  {
    // Get the new camera position
    cam_pos = mCamera->getPosition();

    // Prepare and perform a scene query ray-cast
    Ogre::Ray cam_ray(Ogre::Vector3(cam_pos.x, 5000.0f, cam_pos.z),
                      Ogre::Vector3::NEGATIVE_UNIT_Y);
    ray_query->setRay(cam_ray);
    Ogre::RaySceneQueryResult &result = ray_query->execute();
    Ogre::RaySceneQueryResult::iterator itr = result.begin();

    // Make sure the camera stays above the terrain
    if (itr != result.end() && itr->worldFragment)
    {
      Ogre::Real terrain_h = itr->worldFragment->singleIntersection.y;
      if ((terrain_h + 10.0f) > cam_pos.y)
      {
        mCamera->setPosition( cam_pos.x, terrain_h + 10.0f, cam_pos.z );
        mCameraMan->stopZoom();
      }
    }
  }

  // Update the game objects
  for(std::list<Soldier*>::iterator i = soldiers.begin(); i != soldiers.end(); i++)
    (*i)->update(evt.timeSinceLastFrame);

	return true;
}
//------------------------------------------------------------------------------
bool Application::mouseMoved(const OIS::MouseEvent &evt)
{
  // Base application logic
  if(!BaseApplication::mouseMoved(evt))
    return false;

  // Update CEGUI with the mouse motion
  CEGUI::System::getSingleton().injectMouseMove(evt.state.X.rel, evt.state.Y.rel);

  return true;
}
//------------------------------------------------------------------------------
bool Application::mousePressed(const OIS::MouseEvent &evt, OIS::MouseButtonID id)
{
  // Left mouse button down
  if (id == OIS::MB_Left)
  {
    // Set mouse state
    l_mouse = true;

    // Create a new Soldier
    Soldier* new_soldier = new Soldier();
    new_soldier->attach(mSceneMgr, cursor_pos);
  }

  // Right mouse button down
  else if (id == OIS::MB_Right)
    r_mouse = true;

  // consume event
  return true;
}
//------------------------------------------------------------------------------
bool Application::mouseReleased(const OIS::MouseEvent &evt, OIS::MouseButtonID id)
{
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
