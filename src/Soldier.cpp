#include "Soldier.hpp"

using namespace Ogre;

/* CONSTANTS */

const Real Soldier::WALK_SPEED = 150.0f;

/* CREATION, DESTRUCTION */

Soldier::Soldier() :
state(IDLING),
distance_left(0.0f),
direction(Vector3::ZERO),
destination(Vector3::ZERO),
waypoints(),
animation(NULL),
entity(NULL),
node(NULL)
{
}

void Soldier::attach(SceneManager* scene_manager, const char* name,
                      Vector3 position)
{
  // Create the entity
  entity = scene_manager->createEntity(name, "robot.mesh");

  // Create the scene node
  node = scene_manager->getRootSceneNode()->createChildSceneNode(name, position);
  node->attachObject(entity);

  // Set to the idle animation and loop
  animation = entity->getAnimationState("Idle");
  animation->setLoop(true);
  animation->setEnabled(true);

  // Create the walking list
  waypoints.push_back(&Waypoint::one);
  waypoints.push_back(&Waypoint::two);
  waypoints.push_back(&Waypoint::three);
}

/* MOVEMENT */

void Soldier::nextWaypoint()
{
  if(waypoints.empty())
  {
    // Set Idle animation
    animation = entity->getAnimationState("Idle");
    animation->setLoop(true);
    animation->setEnabled(true);

    // We are now idling again
    state = IDLING;
  }
  else
  {
    // get the next destination from the queue
    destination = waypoints.front()->getPosition();
    waypoints.pop_front();

    // turn towards the new destination
    direction = destination - node->getPosition();
    distance_left = direction.normalise();

    Vector3 src = node->getOrientation() * Vector3::UNIT_X;
    if ((1.0f + src.dotProduct(direction)) < 0.0001f)
        node->yaw(Degree(180));
    else
    {
        Ogre::Quaternion quat = src.getRotationTo(direction);
        node->rotate(quat);
    }

    // Set walking animation
    animation = entity->getAnimationState("Walk");
    animation->setLoop(true);
    animation->setEnabled(true);

    // We are now moving again
    state = WALKING;
  }
}

/* UPDATE */
void Soldier::update(Real amount)
{
  // Try to get a new destination if currently idle
  if(state == IDLING)
    nextWaypoint();

  // Currently has a given
  else
  {
    // Move an amount dependent on the time elapsed since last frame
    Ogre::Real move = WALK_SPEED * amount;
    // Decrement the amount of distance left to move
    distance_left -= move;
    // Check whether we have reached our destination
    if (distance_left <= 0.0f)
    {
      // Jump to target position if an overlap occurs
      node->setPosition(destination);
      // Start towards new location if there is one
      nextWaypoint();
    }
    else
      // Move the soldier
      node->translate(direction * move);
  }

  // Animate an amount dependent on the elapsed time since the last frame
  animation->addTime(amount);
}
