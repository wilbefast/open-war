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

#include "OverheadCamera.hpp"

#include <CEGUI/CEGUI.h> // for CEGUI::MouseCursor
#include <iostream>

#define SIGN(x) ((x>0)?1:((x<0)?-1:0))
#define ABS(x) ((x>0)?x:-x)
#define SQ(x) (x*x)

using namespace Ogre;
using namespace std;

/// CONSTANTS

const Real OverheadCamera::MIN_Y = 50.0f,
           OverheadCamera::MAX_Y = 1500.0f;

/// CREATION, DESTRUCTION

OverheadCamera::OverheadCamera(Camera* _camera) :
rotate(false),
max_zoom_in(false),
max_zoom_out(false),
camera(_camera),
top_speed(700.0f),
rotate_speed(0.1f),
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

void OverheadCamera::stayAbove(Real target_y, Real d_time)
{
  return stayOnSide(target_y, d_time, 1);
}

void OverheadCamera::stayBelow(Real target_y, Real d_time)
{
  return stayOnSide(target_y, d_time, -1);
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
    /*Ogre::Real dist = (camera->getPosition() - cursor_pos).length();
    camera->yaw(Ogre::Degree(-evt.state.X.rel * 0.25f));
    camera->pitch(Ogre::Degree(-evt.state.Y.rel * 0.25f));
    camera->moveRelative(Ogre::Vector3(0, 0, dist));*/
  }

  // Calculate the direction towards the cursor
  CEGUI::Point mouse_pos = CEGUI::MouseCursor::getSingleton().getPosition();
  Ray mouse_ray =
    camera->getCameraToViewportRay(mouse_pos.d_x/float(evt.state.width),
                                   mouse_pos.d_y/float(evt.state.height));

  // Cap maximum zoom
  bool zoom_in = evt.state.Z.rel > 0,
       zoom_out = evt.state.Z.rel < 0;
  if(zoom_in)
    max_zoom_out = false;
  else if(zoom_out)
    max_zoom_in = false;

  // Otherwise zoom towards cursor
  if((zoom_in && !max_zoom_in) || (zoom_out && !max_zoom_out))
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
  Real height_mod = (camera_pos.y - MIN_Y) / (MAX_Y - MIN_Y);

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
    pan_speed += delta * top_speed * d_time * 10;
  }
  // if not accelerating, try to stop in a certain time
  else if(pan_speed != Vector3::ZERO)
    pan_speed -= pan_speed * d_time * 10;

  if(zoom_speed != Vector3::ZERO)
    zoom_speed -= zoom_speed * d_time * 5;

  // set camera velocity to 0 if below the tiny value
  Ogre::Real tiny = std::numeric_limits<Real>::epsilon();
  if (zoom_speed.squaredLength() < SQ(tiny))
    zoom_speed = Vector3::ZERO;

  if (pan_speed.squaredLength() < SQ(tiny))
    pan_speed = Vector3::ZERO;
  // keep camera velocity below top speed
  else if (pan_speed.squaredLength() > SQ(top_speed))
  {
    pan_speed.normalise();
    pan_speed *= top_speed;
  }

  // stop zooming if below the minimum height
  stayAbove(MIN_Y, d_time);

  // stop zooming if above the maximum height
  stayBelow(1500, d_time);

  // move the camera
  if (pan_speed != Vector3::ZERO)
    camera->move(pan_speed * height_mod * d_time);
  if (zoom_speed != Vector3::ZERO)
    camera->move(zoom_speed * height_mod * d_time);
}

void OverheadCamera::stayOnSide(Real target_y, Real d_time, int side)
{
  // sign must be 1 or -1
  if(SQ(side) != 1)
    return;

  // cache
  Vector3 camera_pos = camera->getPosition();
  Real next_y = camera_pos.y + zoom_speed.y*d_time;

  // check for overflow
  if(side*next_y < side*target_y)
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

    if(side == 1)
      // prevent further attempts to zoom in until we zoom out again
      max_zoom_in = true;
    else
      // or vice versa
      max_zoom_out = true;
  }
}
