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
  /// CONSTANTS
private:
  static const Ogre::Real MIN_Y, MAX_Y;

  /// ATTRIBUTES
private:
  bool rotate, max_zoom_in, max_zoom_out;
  Ogre::Camera* camera;
  Ogre::Real top_speed, rotate_speed;
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
  void stopPan();
  void stopZoom();
  void stayAbove(Ogre::Real target_y, Ogre::Real d_time);
  void stayBelow(Ogre::Real target_y, Ogre::Real d_time);
  virtual void injectKeyDown(const OIS::KeyEvent& evt);
  virtual void injectKeyUp(const OIS::KeyEvent& evt);
  virtual void injectMouseMove(const OIS::MouseEvent& evt);
  virtual void injectMouseUp(const OIS::MouseEvent& evt, OIS::MouseButtonID id);
  virtual void injectMouseDown(const OIS::MouseEvent& evt, OIS::MouseButtonID id);

  /// SUBROUTINES
private:
  void move(Ogre::Real d_time);
  void stayOnSide(Ogre::Real target_y, Ogre::Real d_time, int side);
};

#endif // OVERHEADCAMERA_HPP_INCLUDED
