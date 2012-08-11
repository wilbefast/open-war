/*
-----------------------------------------------------------------------------
Filename:    BaseApplication.cpp
--------------------------------------------------------------------------------

This source file is part of the
   ___                 __    __ _ _    _
  /___\__ _ _ __ ___  / / /\ \ (_) | _(_)
 //  // _` | '__/ _ \ \ \/  \/ / | |/ / |
/ \_// (_| | | |  __/  \  /\  /| |   <| |
\___/ \__, |_|  \___|   \/  \/ |_|_|\_\_|
      |___/
      Tutorial Framework
      http://www.ogre3d.org/tikiwiki/
--------------------------------------------------------------------------------
*/
#include "BaseApplication.h"

//------------------------------------------------------------------------------
BaseApplication::BaseApplication(void):
  mRoot(NULL),
  mCamera(NULL),
  mSceneMgr(NULL),
  mWindow(NULL),
  mResourcesCfg(Ogre::StringUtil::BLANK),
  mPluginsCfg(Ogre::StringUtil::BLANK),
  mTrayMgr(NULL),
  mCameraMan(NULL),
  mDetailsPanel(NULL),
  mCursorWasVisible(false),
  mShutDown(false),
  mInputManager(NULL),
  mMouse(NULL),
  mKeyboard(NULL)
{
}

//------------------------------------------------------------------------------
BaseApplication::~BaseApplication(void)
{
  if (mTrayMgr)
    delete mTrayMgr;
  if (mCameraMan)
    delete mCameraMan;

  //Remove ourself as a Window listener
  Ogre::WindowEventUtilities::removeWindowEventListener(mWindow, this);
  windowClosed(mWindow);
  delete mRoot;
}

//------------------------------------------------------------------------------
bool BaseApplication::configure(void)
{
  // Show the configuration dialog and initialise the system
  // You can skip this and use root.restoreConfig() to load configuration
  // settings if you were sure there are valid ones saved in ogre.cfg
  if(mRoot->showConfigDialog())
  {
    // If returned true, user clicked OK so initialise
    // Here we choose to let the system create a default rendering window by passing 'true'
    mWindow = mRoot->initialise(true, "TutorialApplication Render Window");

    return true;
  }
  else
    return false;
}
//------------------------------------------------------------------------------
void BaseApplication::chooseSceneManager(void)
{
  // Create the scene manager
  mSceneMgr = mRoot->createSceneManager(Ogre::ST_EXTERIOR_CLOSE);
}
//------------------------------------------------------------------------------
void BaseApplication::createCamera(void)
{
  // Create the camera
  mCamera = mSceneMgr->createCamera("PlayerCam");
  // Position
  mCamera->setPosition(Ogre::Vector3(0, 500, 0));
  // Angle
  mCamera->pitch(Ogre::Degree(-40));
  mCamera->yaw(Ogre::Degree(-45));
  mCamera->roll(Ogre::Degree(0));
  // Clipping
  mCamera->setNearClipDistance(5);
  bool infinite_distance =
    mRoot->getRenderSystem()->getCapabilities()->hasCapability(Ogre::RSC_INFINITE_FAR_PLANE);
  mCamera->setFarClipDistance(infinite_distance ? 0 : 50000);

  // create camera controller
  mCameraMan = new OverheadCamera(mCamera);
}
//------------------------------------------------------------------------------
void BaseApplication::createFrameListener(void)
{
  Ogre::LogManager::getSingletonPtr()->logMessage("*** Initializing OIS ***");
  OIS::ParamList pl;
  size_t windowHnd = 0;
  std::ostringstream windowHndStr;

  mWindow->getCustomAttribute("WINDOW", &windowHnd);
  windowHndStr << windowHnd;
  pl.insert(std::make_pair(std::string("WINDOW"), windowHndStr.str()));

  // Input managers and devices
  mInputManager = OIS::InputManager::createInputSystem( pl );
  mKeyboard = static_cast<OIS::Keyboard*>(
                mInputManager->createInputObject( OIS::OISKeyboard, true ));
  mMouse = static_cast<OIS::Mouse*>(
                mInputManager->createInputObject( OIS::OISMouse, true ));
  mMouse->setEventCallback(this);
  mKeyboard->setEventCallback(this);

  //Set initial mouse clipping size
  windowResized(mWindow);

  // Register as a Window listener
  Ogre::WindowEventUtilities::addWindowEventListener(mWindow, this);

  mTrayMgr =
    new OgreBites::SdkTrayManager("InterfaceName", mWindow, mMouse, this);
  mTrayMgr->showFrameStats(OgreBites::TL_BOTTOMLEFT);
  mTrayMgr->showLogo(OgreBites::TL_BOTTOMRIGHT);
  mTrayMgr->hideCursor();

  // Create a params panel for displaying sample details
  Ogre::StringVector items;
  items.push_back("cam.pX");
  items.push_back("cam.pY");
  items.push_back("cam.pZ");
  items.push_back("");
  items.push_back("cam.oW");
  items.push_back("cam.oX");
  items.push_back("cam.oY");
  items.push_back("cam.oZ");
  items.push_back("");
  items.push_back("Filtering");
  items.push_back("Poly Mode");

  mDetailsPanel =
    mTrayMgr->createParamsPanel(OgreBites::TL_NONE, "DetailsPanel", 200, items);
  mDetailsPanel->setParamValue(9, "Bilinear");
  mDetailsPanel->setParamValue(10, "Solid");
  mDetailsPanel->hide();

  mRoot->addFrameListener(this);
}
//------------------------------------------------------------------------------
void BaseApplication::destroyScene(void)
{
}
//------------------------------------------------------------------------------
void BaseApplication::createViewports(void)
{
  // Create one viewport, entire window
  Ogre::Viewport* vp = mWindow->addViewport(mCamera);
  vp->setBackgroundColour(Ogre::ColourValue(0,0,0));

  // Alter the camera aspect ratio to match the viewport
  mCamera->setAspectRatio(
    Ogre::Real(vp->getActualWidth()) / Ogre::Real(vp->getActualHeight()));
}
//------------------------------------------------------------------------------
void BaseApplication::setupResources(void)
{
  // Load resource paths from config file
  Ogre::ConfigFile cf;
  cf.load(mResourcesCfg);

  // Go through all sections & settings in the file
  Ogre::ConfigFile::SectionIterator seci = cf.getSectionIterator();

  Ogre::String secName, typeName, archName;
  while (seci.hasMoreElements())
  {
    secName = seci.peekNextKey();
    Ogre::ConfigFile::SettingsMultiMap *settings = seci.getNext();
    Ogre::ConfigFile::SettingsMultiMap::iterator i;
    for (i = settings->begin(); i != settings->end(); ++i)
    {
      typeName = i->first;
      archName = i->second;
      Ogre::ResourceGroupManager::getSingleton().addResourceLocation(
          archName, typeName, secName);
    }
  }
}
//------------------------------------------------------------------------------
void BaseApplication::createResourceListener(void)
{

}
//------------------------------------------------------------------------------
void BaseApplication::loadResources(void)
{
  Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
}
//------------------------------------------------------------------------------
void BaseApplication::go(void)
{
#ifdef _DEBUG
  mResourcesCfg = "resources_d.cfg";
  mPluginsCfg = "plugins_d.cfg";
#else
  mResourcesCfg = "resources.cfg";
  mPluginsCfg = "plugins.cfg";
#endif

  // Check for errors
  if (!setup())
    return;

  // Launch
  mRoot->startRendering();

  // Clean up
  destroyScene();
}
//------------------------------------------------------------------------------
bool BaseApplication::setup(void)
{
  mRoot = new Ogre::Root(mPluginsCfg);

  setupResources();

  bool carryOn = configure();
  if (!carryOn)
    return false;

  chooseSceneManager();
  createCamera();
  createViewports();

  // Set default mipmap level (NB some APIs ignore this)
  Ogre::TextureManager::getSingleton().setDefaultNumMipmaps(5);

  // Create any resource listeners (for loading screens)
  createResourceListener();
  // Load resources
  loadResources();

  // Create the scene
  createScene();

  createFrameListener();

  return true;
};
//------------------------------------------------------------------------------
bool BaseApplication::frameRenderingQueued(const Ogre::FrameEvent& evt)
{
  if(mWindow->isClosed())
    return false;

  if(mShutDown)
    return false;

  //Need to capture/update each device
  mKeyboard->capture();
  mMouse->capture();

  mTrayMgr->frameRenderingQueued(evt);

  if (!mTrayMgr->isDialogVisible())
  {
    // if dialog isn't up, then update the camera
    mCameraMan->frameRenderingQueued(evt);
    // if details panel is visible, then update its contents
    if (mDetailsPanel->isVisible())
    {
      mDetailsPanel->setParamValue(0,
        Ogre::StringConverter::toString(mCamera->getDerivedPosition().x));
      mDetailsPanel->setParamValue(1,
        Ogre::StringConverter::toString(mCamera->getDerivedPosition().y));
      mDetailsPanel->setParamValue(2,
        Ogre::StringConverter::toString(mCamera->getDerivedPosition().z));
      mDetailsPanel->setParamValue(4,
        Ogre::StringConverter::toString(mCamera->getDerivedOrientation().w));
      mDetailsPanel->setParamValue(5,
        Ogre::StringConverter::toString(mCamera->getDerivedOrientation().x));
      mDetailsPanel->setParamValue(6,
        Ogre::StringConverter::toString(mCamera->getDerivedOrientation().y));
      mDetailsPanel->setParamValue(7,
      Ogre::StringConverter::toString(mCamera->getDerivedOrientation().z));
    }
  }

  return true;
}
//------------------------------------------------------------------------------
bool BaseApplication::keyPressed( const OIS::KeyEvent &evt )
{
  // don't process any more keys if dialog is up
  if (mTrayMgr->isDialogVisible())
    return true;

  // toggle visibility of advanced frame stats
  if (evt.key == OIS::KC_F)
    mTrayMgr->toggleAdvancedFrameStats();

  // toggle visibility of even rarer debugging details
  else if (evt.key == OIS::KC_G)
  {
    if (mDetailsPanel->getTrayLocation() == OgreBites::TL_NONE)
    {
      mTrayMgr->moveWidgetToTray(mDetailsPanel, OgreBites::TL_TOPRIGHT, 0);
      mDetailsPanel->show();
    }
    else
    {
      mTrayMgr->removeWidgetFromTray(mDetailsPanel);
      mDetailsPanel->hide();
    }
  }

  // cycle polygon rendering mode
  else if (evt.key == OIS::KC_T)
  {
    Ogre::String newVal;
    Ogre::TextureFilterOptions tfo;
    unsigned int aniso;

    switch (mDetailsPanel->getParamValue(9).asUTF8()[0])
    {
      case 'B':
        newVal = "Trilinear";
        tfo = Ogre::TFO_TRILINEAR;
        aniso = 1;
      break;

      case 'T':
        newVal = "Anisotropic";
        tfo = Ogre::TFO_ANISOTROPIC;
        aniso = 8;
      break;

      case 'A':
        newVal = "None";
        tfo = Ogre::TFO_NONE;
        aniso = 1;
      break;

      default:
        newVal = "Bilinear";
        tfo = Ogre::TFO_BILINEAR;
        aniso = 1;
    }

    Ogre::MaterialManager::getSingleton().setDefaultTextureFiltering(tfo);
    Ogre::MaterialManager::getSingleton().setDefaultAnisotropy(aniso);
    mDetailsPanel->setParamValue(9, newVal);
  }

  // cycle polygon rendering mode
  else if (evt.key == OIS::KC_R)
  {
    Ogre::String newVal;
    Ogre::PolygonMode pm;

    switch (mCamera->getPolygonMode())
    {
      case Ogre::PM_SOLID:
        newVal = "Wireframe";
        pm = Ogre::PM_WIREFRAME;
      break;

      case Ogre::PM_WIREFRAME:
        newVal = "Points";
        pm = Ogre::PM_POINTS;
      break;

      default:
        newVal = "Solid";
        pm = Ogre::PM_SOLID;
    }

    mCamera->setPolygonMode(pm);
    mDetailsPanel->setParamValue(10, newVal);
  }

  // refresh all textures
  else if(evt.key == OIS::KC_F5)
    Ogre::TextureManager::getSingleton().reloadAll();

  // take a screenshot
  else if (evt.key == OIS::KC_SYSRQ)
    mWindow->writeContentsToTimestampedFile("screenshot", ".jpg");

  // exit
  else if (evt.key == OIS::KC_ESCAPE)
    mShutDown = true;

  // pass key input to camera
  mCameraMan->injectKeyDown(evt);
  return true;
}

bool BaseApplication::keyReleased( const OIS::KeyEvent &evt )
{
  // pass to the camera controller
  mCameraMan->injectKeyUp(evt);

  // consume event
  return true;
}

bool BaseApplication::mouseMoved( const OIS::MouseEvent &evt )
{
  // the tray may consume the event before it gets to the camera
  if (mTrayMgr->injectMouseMove(evt))
    return true;

  // pass to the camera controller if not consumed
  mCameraMan->injectMouseMove(evt);

  // consume event
  return true;
}

bool BaseApplication::mousePressed( const OIS::MouseEvent &evt, OIS::MouseButtonID id )
{
  // the tray may consume the event before it gets to the camera
  if (mTrayMgr->injectMouseDown(evt, id))
    return true;

  // pass to the camera controller if not consumed
  mCameraMan->injectMouseDown(evt, id);

  return true;
}

bool BaseApplication::mouseReleased( const OIS::MouseEvent &evt, OIS::MouseButtonID id )
{
  // the tray may consume the event before it gets to the camera
  if (mTrayMgr->injectMouseUp(evt, id))
    return true;

  // pass to the camera controller if not consumed
  mCameraMan->injectMouseUp(evt, id);

  return true;
}

//Adjust mouse clipping area
void BaseApplication::windowResized(Ogre::RenderWindow* rw)
{
  unsigned int width, height, depth;
  int left, top;
  rw->getMetrics(width, height, depth, left, top);

  const OIS::MouseState &ms = mMouse->getMouseState();
  ms.width = width;
  ms.height = height;
}

//Unattach OIS before window shutdown (very important under Linux)
void BaseApplication::windowClosed(Ogre::RenderWindow* rw)
{
  //Only close for window that created OIS (the main window in these demos)
  if( rw == mWindow )
  {
    if( mInputManager )
    {
      mInputManager->destroyInputObject( mMouse );
      mInputManager->destroyInputObject( mKeyboard );

      OIS::InputManager::destroyInputSystem(mInputManager);
      mInputManager = NULL;
    }
  }
}
