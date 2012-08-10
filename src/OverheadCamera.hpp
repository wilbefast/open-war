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
  bool rotate;
  Ogre::Camera* camera;
  Ogre::Real top_pan_speed, rotate_speed;
  Ogre::Vector3 pan_speed, zoom_speed, input, zoom_direction;

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
  virtual void stopPan();
  virtual void stopZoom();
  virtual void injectKeyDown(const OIS::KeyEvent& evt);
  virtual void injectKeyUp(const OIS::KeyEvent& evt);
  virtual void injectMouseMove(const OIS::MouseEvent& evt);
  virtual void injectMouseUp(const OIS::MouseEvent& evt, OIS::MouseButtonID id);
  virtual void injectMouseDown(const OIS::MouseEvent& evt, OIS::MouseButtonID id);

  /// SUBROUTINES
private:
  void move(Ogre::Real d_time);
};

#endif // OVERHEADCAMERA_HPP_INCLUDED
