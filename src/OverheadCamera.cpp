#include "OverheadCamera.hpp"

#include <CEGUI/CEGUI.h> // for CEGUI::MouseCursor
#include <iostream>

#define SIGN(x) ((x>0)?1:((x<0)?-1:0))
#define ABS(x) ((x>0)?x:-x)

using namespace Ogre;
using namespace std;

/// CREATION, DESTRUCTION

OverheadCamera::OverheadCamera(Camera* _camera) :
camera(_camera),
top_pan_speed(250),
top_zoom_speed(600),
pan_speed(Vector3::ZERO),
zoom_speed(Vector3::ZERO),
input(Vector3::ZERO),
zoom_direction(Vector3::ZERO)
{
}

OverheadCamera::~OverheadCamera()
{
}

/// UPDATE

bool OverheadCamera::frameRenderingQueued(const FrameEvent& evt)
{
  // pan the camera based on keyboard input
  pan(evt);

  // zoom the camera towards the cursor
  zoom(evt);

  // event consumed, no error
  return true;
}

/// CONTROL

void OverheadCamera::manualStop()
{
  pan_speed = Vector3::ZERO;
  zoom_speed = Vector3::ZERO;
  input = Vector3::ZERO;
}

void OverheadCamera::injectKeyDown(const OIS::KeyEvent& evt)
{
  // left and right
  if (evt.key == OIS::KC_LEFT)
    input.x = (input.x > 0) ? 0 : -1;
  else if (evt.key == OIS::KC_RIGHT)
    input.x = (input.x < 0) ? 0 : 1;
  // up and down
  else if (evt.key == OIS::KC_UP)
    input.y = (input.y > 0) ? 0 : -1;
  else if (evt.key == OIS::KC_DOWN)
    input.y = (input.y < 0) ? 0 : 1;
}

void OverheadCamera::injectKeyUp(const OIS::KeyEvent& evt)
{
  // left and right
  if (evt.key == OIS::KC_LEFT && input.x < 0)
    input.x = 0;
  else if (evt.key == OIS::KC_RIGHT && input.x > 0)
    input.x = 0;
  // up and down
  else if (evt.key == OIS::KC_UP && input.y < 0)
    input.y = 0;
  else if (evt.key == OIS::KC_DOWN && input.y > 0)
    input.y = 0;
}

void OverheadCamera::injectMouseMove(const OIS::MouseEvent& evt)
{
  // Calculate the direction towards the cursor
  CEGUI::Point mouse_pos = CEGUI::MouseCursor::getSingleton().getPosition();
  Ray mouse_ray =
    camera->getCameraToViewportRay(mouse_pos.d_x/float(evt.state.width),
                                   mouse_pos.d_y/float(evt.state.height));
  zoom_direction = mouse_ray.getDirection();

  // Store the mouse-wheel movement
  input.z = evt.state.Z.rel;

  // Update CEGUI with the mouse motion
  CEGUI::System::getSingleton().injectMouseMove(evt.state.X.rel, evt.state.Y.rel);
}

void OverheadCamera::injectMouseUp(const OIS::MouseEvent& evt, OIS::MouseButtonID id)
{
  // todo
}

void OverheadCamera::injectMouseDown(const OIS::MouseEvent& evt, OIS::MouseButtonID id)
{
  // todo
}

/// SUBROUTINES

void OverheadCamera::pan(const FrameEvent& evt)
{
  // acceleration vector to be built based on keyboard input composite
  Vector3 delta = Vector3::ZERO,
  // prepare the axes along which the camera will move
          right = camera->getRight(),
          forwards = camera->getDirection();
  right.y = forwards.y = 0;

  // left and right (Z)
  if (input.x > 0)
    delta += right;
  else if (input.x < 0)
    delta -= right;

  // forwards and backwards (X)
  if (input.y > 0)
    delta -= forwards;
  else if (input.y < 0)
    delta += forwards;

  // if accelerating, try to reach top speed in a certain time
  if (delta.squaredLength() != 0)
  {
    delta.normalise();
    pan_speed += delta * top_pan_speed * evt.timeSinceLastFrame * 10;
  }
  // if not accelerating, try to stop in a certain time
  else
    pan_speed -= pan_speed * evt.timeSinceLastFrame * 10;

  // tiny value represents 0
  Ogre::Real tiny = std::numeric_limits<Real>::epsilon();

  // keep camera velocity below top speed
  if (pan_speed.squaredLength() > top_pan_speed*top_pan_speed)
  {
    pan_speed.normalise();
    pan_speed *= top_pan_speed;
  }
  // set camera velocity to 0 if below the tiny value
  else if (pan_speed.squaredLength() < tiny*tiny)
    pan_speed = Vector3::ZERO;

  // move the camera
  if (pan_speed != Vector3::ZERO)
    camera->move(pan_speed * evt.timeSinceLastFrame);
}

void OverheadCamera::zoom(const FrameEvent& evt)
{
  // tiny value represents 0
  Ogre::Real tiny = std::numeric_limits<Real>::epsilon();

  // if accelerating, try to reach top speed in a certain time
  if (ABS(input.z) > tiny)
  {
    zoom_direction.normalise();
    zoom_speed += zoom_direction * top_zoom_speed * evt.timeSinceLastFrame * 10 * SIGN(input.z);
    input.z *= 0.3;
  }
  // if not accelerating, try to stop in a certain time
  else
  {
    input.z = 0;
    zoom_speed -= zoom_speed * evt.timeSinceLastFrame * 10;
  }

  // keep camera velocity below top speed
  if (zoom_speed.squaredLength() > top_zoom_speed*top_zoom_speed)
  {
    zoom_speed.normalise();
    zoom_speed *= top_zoom_speed;
  }
  // set camera velocity to 0 if below the tiny value
  else if (zoom_speed.squaredLength() < tiny*tiny)
    zoom_speed = Vector3::ZERO;

  // move the camera
  if (zoom_speed != Vector3::ZERO)
    camera->move(zoom_speed * evt.timeSinceLastFrame);
}
