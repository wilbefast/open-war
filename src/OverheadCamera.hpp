#ifndef OVERHEADCAMERA_HPP_INCLUDED
#define OVERHEADCAMERA_HPP_INCLUDED

#include "Ogre.h"
#include <limits>

#include <OISEvents.h>
#include <OISInputManager.h>
#include <OISKeyboard.h>
#include <OISMouse.h>

class OverheadCamera
{
  /// ATTRIBUTES
private:
  Ogre::Camera* camera;
  Ogre::Real top_speed;
  Ogre::Vector3 speed;
  Ogre::Vector2 input;

  /// METHODS
public:
  // creation
  OverheadCamera(Ogre::Camera* _camera);
  virtual ~OverheadCamera();
  // modification
  void setCamera(Ogre::Camera* _camera);
  void setTopSpeed(Ogre::Real _top_speed);
  // query
  Ogre::Camera* getCamera();
  Ogre::Real getTopSpeed() const;
  // update
  virtual bool frameRenderingQueued(const Ogre::FrameEvent& evt);
  // control
  virtual void injectStop();
  virtual void injectKeyDown(const OIS::KeyEvent& evt);
  virtual void injectKeyUp(const OIS::KeyEvent& evt);
};

#endif // OVERHEADCAMERA_HPP_INCLUDED
