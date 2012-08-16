/*
-----------------------------------------------------------------------------
Filename:    BaseApplication.h
-----------------------------------------------------------------------------

This source file is part of the
   ___                 __    __ _ _    _
  /___\__ _ _ __ ___  / / /\ \ (_) | _(_)
 //  // _` | '__/ _ \ \ \/  \/ / | |/ / |
/ \_// (_| | | |  __/  \  /\  /| |   <| |
\___/ \__, |_|  \___|   \/  \/ |_|_|\_\_|
      |___/
      Tutorial Framework
      http://www.ogre3d.org/tikiwiki/
-----------------------------------------------------------------------------
*/
#ifndef __BaseApplication_h_
#define __BaseApplication_h_

#include <OgreCamera.h>
#include <OgreEntity.h>
#include <OgreLogManager.h>
#include <OgreRoot.h>
#include <OgreViewport.h>
#include <OgreSceneManager.h>
#include <OgreRenderWindow.h>
#include <OgreConfigFile.h>

#include <OISEvents.h>
#include <OISInputManager.h>
#include <OISKeyboard.h>
#include <OISMouse.h>

#include <SdkTrays.h>
#include <SdkCameraMan.h>
#include "OverheadCamera.hpp"

class BaseApplication :
public Ogre::FrameListener,
public Ogre::WindowEventListener,
public OIS::KeyListener,
public OIS::MouseListener,
OgreBites::SdkTrayListener
{
  /// ATTRIBUTES
protected:
  // Model
  Ogre::Root *root;
  Ogre::SceneManager* scene;
  Ogre::RenderWindow* window;
  // View
  Ogre::Camera* camera;
  OverheadCamera* camera_man;
  //OgreBites::SdkCameraMan* camera_man;
  OgreBites::SdkTrayManager* tray;
  OgreBites::ParamsPanel* panel;  // sample details panel
  bool cursor_was_visible;  // was cursor visible before dialog appeared ?
  bool mShutDown;
  // Resources
  Ogre::String resources_cfg;
  Ogre::String plugins_cfg;
  //OIS Input devices
  OIS::InputManager* input;
  OIS::Mouse* mouse;
  OIS::Keyboard* keyboard;


  /// METHODS
public:
  BaseApplication(void);
  virtual ~BaseApplication(void);
  virtual void go(void);

  /// SUBROUTINES
protected:
  virtual bool setup();
  virtual bool configure(void);
  virtual void chooseSceneManager(void);
  virtual void createCamera(void);
  virtual void createFrameListener(void);
  virtual void createScene(void) = 0; // Override me!
  virtual void destroyScene(void);
  virtual void createViewports(void);
  virtual void setupResources(void);
  virtual void createResourceListener(void);
  virtual void loadResources(void);

  // Ogre::FrameListener
  virtual bool frameRenderingQueued(const Ogre::FrameEvent& evt);

  // OIS::KeyListener
  virtual bool keyPressed(const OIS::KeyEvent &evt);
  virtual bool keyReleased(const OIS::KeyEvent &evt);
  // OIS::MouseListener
  virtual bool mouseMoved(const OIS::MouseEvent &evt);
  virtual bool mousePressed(const OIS::MouseEvent &evt, OIS::MouseButtonID id);
  virtual bool mouseReleased(const OIS::MouseEvent &evt, OIS::MouseButtonID id);

  // Ogre::WindowEventListener
  //Adjust mouse clipping area
  virtual void windowResized(Ogre::RenderWindow* rw);
  //Unattach OIS before window shutdown (very important under Linux)
  virtual void windowClosed(Ogre::RenderWindow* rw);
};

#endif // #ifndef __BaseApplication_h_
