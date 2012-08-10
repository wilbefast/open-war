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

  // Prepare to cast ray down from camera
  Vector3 cam_pos = mCamera->getPosition();
	Ray cam_ray(Vector3(cam_pos.x, 5000.0f, cam_pos.z), Vector3::NEGATIVE_UNIT_Y);
  // Check for collisions with terrain
  Vector3 collision;
	if(getTerrainCollision(cam_ray, &collision) && cam_pos.y < collision.y + 20.0f)
	{
    mCamera->setPosition(cam_pos.x, collision.y + 20.0f, cam_pos.z);
    mCameraMan->stopZoom();
	}

  // Update the game objects
  for(std::list<Soldier*>::iterator i = soldiers.begin(); i != soldiers.end(); i++)
    (*i)->update(evt.timeSinceLastFrame);

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
  CEGUI::Point mouse_pos = CEGUI::MouseCursor::getSingleton().getPosition();
  Ray mouse_ray =
    mCamera->getCameraToViewportRay(mouse_pos.d_x/float(evt.state.width),
                                    mouse_pos.d_y/float(evt.state.height));
  getTerrainCollision(mouse_ray, &cursor_pos);


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
  }

  // Right mouse button down
  else if (id == OIS::MB_Right)
  {
    // Set mouse state
    r_mouse = true;

    // Create a new Soldier
    Soldier* new_soldier = new Soldier();
    new_soldier->attach(mSceneMgr, cursor_pos);
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
bool Application::getTerrainCollision(Ray ray, Vector3* result)
{
  // Build and execute the query
  ray_query->setRay(ray);
  RaySceneQueryResult &query_result = ray_query->execute();
  RaySceneQueryResult::iterator itr = query_result.begin();

  // Get the first collision point
  if (itr != query_result.end() && itr->worldFragment)
  {
    // Query can be made with result = NULL, in which case don't write there!
    if(result)
      (*result) = itr->worldFragment->singleIntersection;

    // Report whether a collision was found or not
    return true;
  }
  else
    return false;
}
