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

/// CREATION, DESTRUCTION

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


/// QUERY

Vector3 const& Waypoint::getPosition() const
{
  return node->getPosition();
}
