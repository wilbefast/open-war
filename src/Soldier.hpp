#ifndef SOLDIER_HPP_INCLUDED
#define SOLDIER_HPP_INCLUDED

#include <OgreSceneManager.h>
#include <OgreEntity.h>

#include "Waypoint.hpp"

class Soldier
{
  /* CONSTANTS */
private:
  static const Ogre::Real WALK_SPEED;

  /* NESTING */
private:
  enum State
  {
    IDLING, WALKING
  };

  /* ATTRIBUTES */
private:
  // current state
  State state;
  // direction, destination and movement
  Ogre::Real distance_left;
  Ogre::Vector3 direction;
  Ogre::Vector3 destination;
  std::deque<Waypoint const*> waypoints;
  // animation
  Ogre::AnimationState *animation;
  // scene graph identifiers
  Ogre::Entity *entity;
  Ogre::SceneNode *node;

  /* METHODS */
public:
  // creation, destruction
  Soldier();
  void attach(Ogre::SceneManager* scene_manager, const char* name,
              Ogre::Vector3 position);
  // movement
  void nextWaypoint();
  // update
  void update(Ogre::Real amount);
};

#endif // SOLDIER_HPP_INCLUDED
