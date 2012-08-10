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
