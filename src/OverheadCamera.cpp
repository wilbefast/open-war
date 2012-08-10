#include "OverheadCamera.hpp"

#include <CEGUI/CEGUI.h> // for CEGUI::MouseCursor
#include <iostream>

#define SIGN(x) ((x>0)?1:((x<0)?-1:0))
#define ABS(x) ((x>0)?x:-x)

using namespace Ogre;
using namespace std;

/// CONSTANTS

const Real OverheadCamera::MIN_Y = 50.0f,
           OverheadCamera::PAN_Y_BONUS = 0.003f,
           OverheadCamera::ZOOM_Y_BONUS = 0.0045f,
           OverheadCamera::BONUS_MAX_Y = 800;

/// CREATION, DESTRUCTION

OverheadCamera::OverheadCamera(Camera* _camera) :
rotate(false),
camera(_camera),
top_speed(250),
rotate_speed(0.1),
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
  move(evt.timeSinceLastFrame);

  // event consumed, no error
  return true;
}

/// CONTROL

void OverheadCamera::stopPan()
{
  pan_speed = Vector3::ZERO;
  input = Vector3::ZERO;
}

void OverheadCamera::stopZoom()
{
  zoom_speed = Vector3::ZERO;
}

void OverheadCamera::stayAbove(unsigned int target_y, Real d_time)
{
  // cache
  Vector3 camera_pos = camera->getPosition();
  Real next_y = camera_pos.y + zoom_speed.y*d_time;

  // check for overflow
  if(next_y < target_y)
  {
    if(zoom_speed.y)
    {
      // camera should stop smoothly just where it would have touched MIN_Y
      Real fraction = (target_y - camera_pos.y) / (zoom_speed.y * d_time);
      zoom_speed *= fraction;
    }
    else
    {
      // if all else fails: jerky halt more or less where it should be
      stopZoom();
      camera->setPosition(camera_pos.x, target_y, camera_pos.z);
    }
  }
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
  // Rotate the camera based on mouse movement when middle button is held
  if(rotate)
  {
    camera->yaw(Ogre::Degree(-evt.state.X.rel * rotate_speed));
    camera->pitch(Ogre::Degree(-evt.state.Y.rel * rotate_speed));
  }

  // Calculate the direction towards the cursor
  CEGUI::Point mouse_pos = CEGUI::MouseCursor::getSingleton().getPosition();
  Ray mouse_ray =
    camera->getCameraToViewportRay(mouse_pos.d_x/float(evt.state.width),
                                   mouse_pos.d_y/float(evt.state.height));

  // Zoom towards the cursor
  if(evt.state.Z.rel < 0 || camera->getPosition().y > MIN_Y)
    zoom_speed += mouse_ray.getDirection() * evt.state.Z.rel * 10;
}

void OverheadCamera::injectMouseUp(const OIS::MouseEvent& evt, OIS::MouseButtonID id)
{
  if(id == OIS::MB_Middle)
    rotate = false;
}

void OverheadCamera::injectMouseDown(const OIS::MouseEvent& evt, OIS::MouseButtonID id)
{
  if(id == OIS::MB_Middle)
    rotate = true;
}

/// SUBROUTINES

void OverheadCamera::move(Real d_time)
{
  // speed depends on the height of the camera
  Vector3 camera_pos = camera->getPosition();
  Real pan_height_bonus =
    PAN_Y_BONUS * ((camera_pos.y > BONUS_MAX_Y) ? BONUS_MAX_Y : camera_pos.y);
  Real height_top_speed = top_speed * pan_height_bonus;
  Real zoom_height_bonus =
    ZOOM_Y_BONUS * ((camera_pos.y > BONUS_MAX_Y) ? BONUS_MAX_Y : camera_pos.y);

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
    pan_speed += delta * height_top_speed * d_time * 10;
  }
  // if not accelerating, try to stop in a certain time
  else if(pan_speed != Vector3::ZERO)
    pan_speed -= pan_speed * d_time * 10;

  if(zoom_speed != Vector3::ZERO)
    zoom_speed -= zoom_speed * d_time * 5;

  // set camera velocity to 0 if below the tiny value
  Ogre::Real tiny = std::numeric_limits<Real>::epsilon();
  if (zoom_speed.squaredLength() < tiny * tiny)
    zoom_speed = Vector3::ZERO;

  if (pan_speed.squaredLength() < tiny * tiny)
    pan_speed = Vector3::ZERO;
  // keep camera velocity below top speed
  else if (pan_speed.squaredLength() > height_top_speed * height_top_speed)
  {
    pan_speed.normalise();
    pan_speed *= height_top_speed;
  }

  // stop zooming if below the minimum height (Y)
  stayAbove(MIN_Y, d_time);

  // move the camera
  if (pan_speed != Vector3::ZERO)
    camera->move(pan_speed * d_time * pan_height_bonus);
  if (zoom_speed != Vector3::ZERO)
    camera->move(zoom_speed * d_time * zoom_height_bonus);
}
