#ifndef SOLDIER_HPP_INCLUDED
#define SOLDIER_HPP_INCLUDED

#include <list>

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
  std::list<Waypoint const*> waypoints;
  // animation
  Ogre::AnimationState *animation;
  // scene graph identifiers
  Ogre::Entity *entity;
  Ogre::SceneNode *node;

  /// METHODS
public:
  // creation, destruction
  Soldier();
  void attach(Ogre::SceneManager* scene_manager, Ogre::Vector3 position);
  // movement
  void nextWaypoint();
  // update
  void update(Ogre::Real d_time);
  // control
  void setSelected(bool _selected);
};

#endif // SOLDIER_HPP_INCLUDED
