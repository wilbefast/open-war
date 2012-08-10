#include "Waypoint.hpp"

using namespace Ogre;

/* CLASS NAMESPACE */

Waypoint Waypoint::one, Waypoint::two, Waypoint::three;

void Waypoint::attach_all(SceneManager* scene_manager)
{
  Waypoint::one.attach(scene_manager, "wp1", Vector3(550.0f,  200.0f,  250.0f));
  Waypoint::two.attach(scene_manager, "wp2", Vector3(1000.0f,  200.0f, 500.0f));
  Waypoint::three.attach(scene_manager, "wp3", Vector3(25.0f,  200.0f, 3000.0f));
}


/* CREATION, DESTRUCTION */

Waypoint::Waypoint() :
entity(NULL),
node(NULL)
{
}

void Waypoint::attach(SceneManager* scene_manager, const char* name,
                      Vector3 position)
{
  // Create the entity
  entity = scene_manager->createEntity(name, "knot.mesh");

  // Create the scene node
  node = scene_manager->getRootSceneNode()->createChildSceneNode(name, position);
  node->attachObject(entity);
  node->setScale(0.1f, 0.1f, 0.1f);
}


/* QUERY */

Vector3 const& Waypoint::getPosition() const
{
  return node->getPosition();
}
