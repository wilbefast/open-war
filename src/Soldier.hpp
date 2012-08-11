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

#ifndef SOLDIER_HPP_INCLUDED
#define SOLDIER_HPP_INCLUDED

#include <map>
#include <string>

class Soldier;
typedef std::map<std::string, Soldier*> SoldierMap;
typedef SoldierMap::iterator SoldierIter;

#include <OgreSceneManager.h>
#include <OgreEntity.h>

#include "Waypoint.hpp"

class Soldier
{
  /// CLASS VARIABLES
private:
  static unsigned int count;

  /// CONSTANTS
private:
  static const Ogre::Real WALK_SPEED;

  /// NESTING
private:
  enum State
  {
    IDLING, WALKING
  };

  /// ATTRIBUTES
private:
  // current state
  State state;
  // control
  bool selected;
  // direction, destination and movement
  Ogre::Real distance_left;
  Ogre::Vector3 direction;
  Ogre::Vector3 destination;
  WaypointList waypoints;
  // animation
  Ogre::AnimationState *animation;
  // scene graph identifiers
  Ogre::Entity *entity;
  Ogre::SceneNode *node;

  /// METHODS
public:
  // creation, destruction
  Soldier();
  virtual ~Soldier();
  void attach(SoldierMap&, Ogre::SceneManager*, Ogre::Vector3 position);
  // movement
  void nextWaypoint();
  // update
  void update(Ogre::Real d_time);
  // control
  void setSelected(bool _selected);
  void addWaypoint(Waypoint new_waypoint);
  // query
  bool isSelected() const;
};

#endif // SOLDIER_HPP_INCLUDED
