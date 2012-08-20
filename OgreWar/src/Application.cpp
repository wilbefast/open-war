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

#include <iostream>

#include "Application.hpp"

using namespace std;
using namespace Ogre;

/// CREATION, DESTRUCTION
//------------------------------------------------------------------------------
Application::Application() :
BaseApplication(),
soldiers(),
ray_query(NULL),
r_mouse(false), l_mouse(false),
focus(Vector3::ZERO),
gui_renderer()
{
}
//------------------------------------------------------------------------------
Application::~Application()
{
  // We created the query, and we are also responsible for deleting it.
  scene->destroyQuery(ray_query);

  // Delete all the Soldiers
  for(SoldierIter i = soldiers.begin(); i != soldiers.end(); i++)
    delete i->second;
}

//------------------------------------------------------------------------------
void Application::createScene()
{
    Ogre::MaterialManager::getSingleton().setDefaultTextureFiltering(Ogre::TFO_ANISOTROPIC);
    Ogre::MaterialManager::getSingleton().setDefaultAnisotropy(7);

    Ogre::Vector3 lightdir(0.55, -0.3, 0.75);
    lightdir.normalise();

    Ogre::Light* light = scene->createLight("tstLight");
    light->setType(Ogre::Light::LT_DIRECTIONAL);
    light->setDirection(lightdir);
    light->setDiffuseColour(Ogre::ColourValue::White);
    light->setSpecularColour(Ogre::ColourValue(0.4, 0.4, 0.4));

    scene->setAmbientLight(Ogre::ColourValue(0.2, 0.2, 0.2));

    mTerrainGlobals = OGRE_NEW Ogre::TerrainGlobalOptions();

    mTerrainGroup = OGRE_NEW Ogre::TerrainGroup(scene, Ogre::Terrain::ALIGN_X_Z, 513, 12000.0f);
    mTerrainGroup->setFilenameConvention(Ogre::String("BasicTutorial3Terrain"), Ogre::String("dat"));
    mTerrainGroup->setOrigin(Ogre::Vector3::ZERO);

    configureTerrainDefaults(light);

    for (long x = 0; x <= 0; ++x)
        for (long y = 0; y <= 0; ++y)
            defineTerrain(x, y);

    // sync load since we want everything in place when we start
    mTerrainGroup->loadAllTerrains(true);

    if (mTerrainsImported)
    {
        Ogre::TerrainGroup::TerrainIterator ti = mTerrainGroup->getTerrainIterator();
        while(ti.hasMoreElements())
        {
            Ogre::Terrain* t = ti.getNext()->instance;
            initBlendMaps(t);
        }
    }

    mTerrainGroup->freeTemporaryResources();

	// CEGUI setup
  gui_renderer = &CEGUI::OgreRenderer::bootstrapSystem();

  // Mouse
  CEGUI::SchemeManager::getSingleton().create((CEGUI::utf8*)"TaharezLook.scheme");
  CEGUI::MouseCursor::getSingleton().setImage("TaharezLook", "MouseArrow");
}
//------------------------------------------------------------------------------
void Application::destroyScene()
{
  OGRE_DELETE mTerrainGroup;
  OGRE_DELETE mTerrainGlobals;
}
//------------------------------------------------------------------------------
/// QUERY
//------------------------------------------------------------------------------
Ray Application::getMouseRay(OIS::MouseState mouse_state) const
{
  // Calculate the ray implied by the cursor's position on the screen
  CEGUI::Point mouse_pos = CEGUI::MouseCursor::getSingleton().getPosition();
  return camera->getCameraToViewportRay(mouse_pos.d_x/float(mouse_state.width),
                                      mouse_pos.d_y/float(mouse_state.height));
}
//------------------------------------------------------------------------------
bool Application::getTerrainCollision(Ray ray, Vector3* out)
{
  // Perform the scene query
  TerrainGroup::RayResult result = mTerrainGroup->rayIntersects(ray);
  if(result.hit)
  {
      if(out)
        (*out) = result.position;
  }
  else
    return false;
}
//------------------------------------------------------------------------------
bool Application::getSoldierCollision(Ray ray, Soldier** out)
{
  // Execute the query and return the result
  ray_query->setRay(ray);
  RaySceneQueryResult result = ray_query->execute();

  // Get the first Soldier collided with
  for(RaySceneQueryResult::iterator i = result.begin(); i != result.end(); i++)
  {
    // Ray intersects movable Entity
    if(i->movable)
    {
      // Search for Entity amongst Soldiers
      SoldierIter soldier_i = soldiers.find(i->movable);
      if(soldier_i != soldiers.end())
      {
        if(out)
          (*out) = soldier_i->second;
        return true;
      }
    }
  }
  return false;
}
//------------------------------------------------------------------------------
Real Application::getTerrainHeight(Vector3 position)
{
  return mTerrainGroup->getHeightAtWorldPosition(position);
}
//------------------------------------------------------------------------------
/// FRAME LISTENER
//------------------------------------------------------------------------------
void Application::createFrameListener(void)
{
  // Listen out from screen refreshes
	BaseApplication::createFrameListener();

  // Create RaySceneQuery
  ray_query = scene->createRayQuery(Ogre::Ray());

  // Create tray label for terrain information
  mInfoLabel = tray->createLabel(OgreBites::TL_TOP, "TInfo", "", 350);
}
//------------------------------------------------------------------------------
bool Application::frameRenderingQueued(const Ogre::FrameEvent &evt)
{
  // Process the base frame listener code.  Since we are going to be
  // manipulating the translate vector, we need this to happen first.
  if (!BaseApplication::frameRenderingQueued(evt))
   return false;


  // Save the terrain
  if (mTerrainGroup->isDerivedDataUpdateInProgress())
  {
      tray->moveWidgetToTray(mInfoLabel, OgreBites::TL_TOP, 0);
      mInfoLabel->show();
      if (mTerrainsImported)
        mInfoLabel->setCaption("Building terrain, please wait...");

      else
        mInfoLabel->setCaption("Updating textures, patience...");
  }
  else
  {
      tray->removeWidgetFromTray(mInfoLabel);
      mInfoLabel->hide();
      if (mTerrainsImported)
      {
          mTerrainGroup->saveAllTerrains(true);
          mTerrainsImported = false;
      }
  }

  // Don't fly camera below the terrain
  camera_man->stayAbove(getTerrainHeight(camera->getPosition()) + 20.0f,
                          evt.timeSinceLastFrame);

  // Update the game objects
  for(SoldierIter i = soldiers.begin(); i != soldiers.end(); i++)
    i->second->update(evt.timeSinceLastFrame, this);

	return true;
}
//------------------------------------------------------------------------------

/// MOUSE LISTENER
//------------------------------------------------------------------------------
bool Application::mouseMoved(const OIS::MouseEvent &evt)
{
  // Base application logic
  if(!BaseApplication::mouseMoved(evt))
    return false;

  // Find point that cursor is pointing to
  getTerrainCollision(getMouseRay(evt.state), &focus);


  // Update CEGUI with the mouse motion
  CEGUI::System::getSingleton().injectMouseMove(evt.state.X.rel, evt.state.Y.rel);

  return true;
}
//------------------------------------------------------------------------------
bool Application::mousePressed(const OIS::MouseEvent &evt, OIS::MouseButtonID id)
{
  // Base application logic
  if(!BaseApplication::mousePressed(evt, id))
    return false;


  // Left mouse button down
  if (id == OIS::MB_Left)
  {
    // Set mouse state
    l_mouse = true;

    // Select Soldiers under cursor
    Soldier* selection;
    if(getSoldierCollision(getMouseRay(evt.state), &selection))
      selection->setSelected(!selection->isSelected());
    else
      // Move Soldiers to empty area if nothing to select
      for(SoldierIter i = soldiers.begin(); i != soldiers.end(); i++)
        if(i->second->isSelected())
          i->second->addWaypoint(focus);
  }

  // Right mouse button down
  else if (id == OIS::MB_Right)
  {
    // Set mouse state
    r_mouse = true;

    // Create a new Soldier
    Soldier* new_soldier = new Soldier();
    new_soldier->attach(soldiers, scene, focus);
  }

  // consume event
  return true;
}
//------------------------------------------------------------------------------
bool Application::mouseReleased(const OIS::MouseEvent &evt, OIS::MouseButtonID id)
{
  // Base application logic
  if(!BaseApplication::mouseReleased(evt, id))
    return false;

  // Left mouse button down
  if (id == OIS::MB_Left)
    l_mouse = false;

  // Right mouse button down
  else if (id == OIS::MB_Right)
    r_mouse = false;

  // consume event
  return true;
}
//------------------------------------------------------------------------------

/// TERRAIN

/// FIXME
void getTerrainImage(bool flipX, bool flipY, Ogre::Image& img)
{
  img.load("height_map.png", Ogre::ResourceGroupManager::AUTODETECT_RESOURCE_GROUP_NAME);
  if (flipX)
      img.flipAroundY();
  if (flipY)
      img.flipAroundX();
}

//------------------------------------------------------------------------------
void Application::defineTerrain(long x, long y)
{
  Ogre::String filename = mTerrainGroup->generateFilename(x, y);
  if (Ogre::ResourceGroupManager::getSingleton().resourceExists(mTerrainGroup->getResourceGroup(), filename))
  {
      mTerrainGroup->defineTerrain(x, y);
  }
  else
  {
      Ogre::Image img;
      getTerrainImage(x % 2 != 0, y % 2 != 0, img);
      mTerrainGroup->defineTerrain(x, y, &img);
      mTerrainsImported = true;
  }
}
//------------------------------------------------------------------------------
void Application::initBlendMaps(Ogre::Terrain* terrain)
{
  Ogre::TerrainLayerBlendMap* blendMap0 = terrain->getLayerBlendMap(1);
  Ogre::TerrainLayerBlendMap* blendMap1 = terrain->getLayerBlendMap(2);
  Ogre::Real minHeight0 = 70;
  Ogre::Real fadeDist0 = 40;
  Ogre::Real minHeight1 = 70;
  Ogre::Real fadeDist1 = 15;
  float* pBlend0 = blendMap0->getBlendPointer();
  float* pBlend1 = blendMap1->getBlendPointer();
  for (Ogre::uint16 y = 0; y < terrain->getLayerBlendMapSize(); ++y)
  {
    for (Ogre::uint16 x = 0; x < terrain->getLayerBlendMapSize(); ++x)
    {
      Ogre::Real tx, ty;

      blendMap0->convertImageToTerrainSpace(x, y, &tx, &ty);
      Ogre::Real height = terrain->getHeightAtTerrainPosition(tx, ty);
      Ogre::Real val = (height - minHeight0) / fadeDist0;
      val = Ogre::Math::Clamp(val, (Ogre::Real)0, (Ogre::Real)1);
      *pBlend0++ = val;

      val = (height - minHeight1) / fadeDist1;
      val = Ogre::Math::Clamp(val, (Ogre::Real)0, (Ogre::Real)1);
      *pBlend1++ = val;
    }
  }
  blendMap0->dirty();
  blendMap1->dirty();
  blendMap0->update();
  blendMap1->update();
}
//------------------------------------------------------------------------------
void Application::configureTerrainDefaults(Ogre::Light* light)
{
  // Configure global
  mTerrainGlobals->setMaxPixelError(8);
  // testing composite map
  mTerrainGlobals->setCompositeMapDistance(3000);
  // Important to set these so that the terrain knows what to use for derived (non-realtime) data
  mTerrainGlobals->setLightMapDirection(light->getDerivedDirection());
  mTerrainGlobals->setCompositeMapAmbient(scene->getAmbientLight());
  mTerrainGlobals->setCompositeMapDiffuse(light->getDiffuseColour());
  // Configure default import settings for if we use imported image
  Ogre::Terrain::ImportData& defaultimp = mTerrainGroup->getDefaultImportSettings();
  defaultimp.terrainSize = 513;
  defaultimp.worldSize = 12000.0f;
  defaultimp.inputScale = 600; // due terrain.png is 8 bpp
  defaultimp.minBatchSize = 33;
  defaultimp.maxBatchSize = 65;
  // textures
    defaultimp.layerList.resize(3);
    defaultimp.layerList[0].worldSize = 100;
    defaultimp.layerList[0].textureNames.push_back("dirt_grayrocky_diffusespecular.dds");
    defaultimp.layerList[0].textureNames.push_back("dirt_grayrocky_normalheight.dds");
    defaultimp.layerList[1].worldSize = 30;
    defaultimp.layerList[1].textureNames.push_back("grass_green-01_diffusespecular.dds");
    defaultimp.layerList[1].textureNames.push_back("grass_green-01_normalheight.dds");
    defaultimp.layerList[2].worldSize = 200;
    defaultimp.layerList[2].textureNames.push_back("growth_weirdfungus-03_diffusespecular.dds");
    defaultimp.layerList[2].textureNames.push_back("growth_weirdfungus-03_normalheight.dds");
}
