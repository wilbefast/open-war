#ifndef APPLICATION_HPP_INCLUDED
#define APPLICATION_HPP_INCLUDED

#include <deque>
#include <map>

#include <CEGUI/CEGUI.h>
#include <CEGUI/RendererModules/Ogre/CEGUIOgreRenderer.h>

#include "BaseApplication.h"
#include "Soldier.hpp"

class Application : public BaseApplication
{
  /// ATTRIBUTES
private:
  std::map<unsigned int, Soldier*> soldiers;
  Ogre::RaySceneQuery *mRaySceneQuery;  // The ray scene query pointer
  bool r_mouse, l_mouse;		            // True if the mouse buttons are down
  unsigned int count;							      // The number of robots on the screen
  Ogre::SceneNode *current_node;	      // The newly created object
  CEGUI::Renderer *mGUIRenderer;		    // CEGUI renderer
  float mRotateSpeed;

  /// METHODS
public:
  Application();
  virtual ~Application();

protected:
  virtual void createScene();
  // frame listener
  virtual void createFrameListener();
  virtual bool frameRenderingQueued(const Ogre::FrameEvent &evt);
  // mouse listener
  virtual bool mouseMoved(const OIS::MouseEvent &arg);
  virtual bool mousePressed(const OIS::MouseEvent &arg,OIS::MouseButtonID id);
  virtual bool mouseReleased(const OIS::MouseEvent &arg,OIS::MouseButtonID id);
};

#endif // APPLICATION_HPP_INCLUDED
