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

#include "Soldier.hpp"

using namespace Ogre;
using namespace std;

/// CLASS VARIABLES

unsigned int Soldier::count = 0;

/// CONSTANTS

const Real Soldier::WALK_SPEED = 150.0f;

/// CREATION, DESTRUCTION

Soldier::Soldier() :
state(IDLING),
selected(false),
distance_left(0.0f),
direction(Vector3::ZERO),
destination(Vector3::ZERO),
waypoints(),
animation(NULL),
entity(NULL),
node(NULL)
{
}

Soldier::~Soldier()
{
}

void Soldier::attach(SoldierMap& map, SceneManager* scene_manager,
                      Vector3 position)
{
  // Create the Entity
  char name[16];
  sprintf( name, "Soldier%d", count++ );
  entity = scene_manager->createEntity(name, "robot.mesh");

  // Store in the map
  map[entity] = this;

  // Create the scene Node
  string node_name = string(name) + "Node";
  node = scene_manager->getRootSceneNode()->createChildSceneNode(node_name,
                                                                  position);

  // Attach Entity to Node
  node->attachObject(entity);
  //node->setScale(0.1f, 0.1f, 0.1f);

  // Set to the idle animation and loop
  animation = entity->getAnimationState("Idle");
  animation->setLoop(true);
  animation->setEnabled(true);
}

/// MOVEMENT

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
    destination = waypoints.front().getPosition();
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

/// UPDATE

void Soldier::update(Real d_time)
{
  // Try to get a new destination if currently idle
  if(state == IDLING)
    nextWaypoint();

  // Currently has a given
  else
  {
    // Move an amount dependent on the time elapsed since last frame
    Ogre::Real move = WALK_SPEED * d_time;
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
  animation->addTime(d_time);
}

/// CONTROL

void Soldier::setSelected(bool _selected)
{
  node->showBoundingBox(_selected);
  selected = _selected;
}

void Soldier::addWaypoint(Waypoint new_waypoint)
{
  waypoints.push_back(new_waypoint);
}


/// QUERY

bool Soldier::isSelected() const
{
  return selected;
}
