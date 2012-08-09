#include "Application.hpp"

using namespace Ogre;

/// CREATION

//------------------------------------------------------------------------------
Application::Application() :
BaseApplication(),
soldiers()
{
}
//------------------------------------------------------------------------------
Application::~Application()
{
}

//------------------------------------------------------------------------------
void Application::createScene(void)
{
	// Set ambient light
	mSceneMgr->setAmbientLight(Ogre::ColourValue(0.5, 0.5, 0.5));
	mSceneMgr->setSkyDome(true, "Examples/CloudySky", 5, 8);

	// World geometry
	mSceneMgr->setWorldGeometry("terrain.cfg");

  // Create the waypoints
  Waypoint::attach_all(mSceneMgr);

  // Create the two soldiers
  Soldier* s = new Soldier();
	s->attach(mSceneMgr, "Robot1", Vector3(150.0f, 0.0f, 25.0f));
	soldiers[0] = s;
	s = new Soldier();
	s->attach(mSceneMgr, "Robot2", Vector3(-200.0f, 0.0f, 300.0f));
	soldiers[1] = s;

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
}
//------------------------------------------------------------------------------
bool Application::frameRenderingQueued(const Ogre::FrameEvent &evt)
{
  soldiers[0]->update(evt.timeSinceLastFrame);
  soldiers[1]->update(evt.timeSinceLastFrame);

	return BaseApplication::frameRenderingQueued(evt);
}
//------------------------------------------------------------------------------
bool Application::mouseMoved(const OIS::MouseEvent &arg)
{
  if(!BaseApplication::mouseMoved(arg))
    return false;

  return true;
}
//------------------------------------------------------------------------------
bool Application::mousePressed(const OIS::MouseEvent &arg, OIS::MouseButtonID id)
{
  return true;
}
//------------------------------------------------------------------------------
bool Application::mouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id)
{
  return true;
}
//------------------------------------------------------------------------------
