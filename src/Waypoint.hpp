#ifndef WAYPOINT_HPP_INCLUDED
#define WAYPOINT_HPP_INCLUDED

#include <OgreSceneManager.h>
#include <OgreEntity.h>

class Waypoint
{
  /* CONSTANTS */
public:
  static Waypoint one, two, three;
  static void attach_all(Ogre::SceneManager* scene_manager);

  /* ATTRIBUTES */
private:
  // scene graph identifiers
  Ogre::Entity *entity;
  Ogre::SceneNode *node;

  /* METHODS */
public:
  // creation, destruction
  Waypoint();
  void attach(Ogre::SceneManager* scene_manager, const char* name,
              Ogre::Vector3 position);
  // query
  Ogre::Vector3 const& getPosition() const;
};

#endif // WAYPOINT_HPP_INCLUDED
