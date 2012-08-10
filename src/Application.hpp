#ifndef APPLICATION_HPP_INCLUDED
#define APPLICATION_HPP_INCLUDED

#include <deque>
#include <list>

#include <CEGUI/CEGUI.h>
#include <CEGUI/RendererModules/Ogre/CEGUIOgreRenderer.h>

#include "BaseApplication.h"
#include "Soldier.hpp"

class Application : public BaseApplication
{
  /// ATTRIBUTES
private:
  std::list<Soldier*> soldiers;
  Ogre::RaySceneQuery *ray_query;       // The ray scene query pointer
  bool r_mouse, l_mouse;		            // True if the mouse buttons are down
  Ogre::Vector3 cursor_pos;             // Position of the cursor in the world
  CEGUI::Renderer *gui_renderer;		    // CEGUI renderer

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
  virtual bool mouseMoved(const OIS::MouseEvent &evt);
  virtual bool mousePressed(const OIS::MouseEvent &evt,OIS::MouseButtonID id);
  virtual bool mouseReleased(const OIS::MouseEvent &evt,OIS::MouseButtonID id);
};

#endif // APPLICATION_HPP_INCLUDED
