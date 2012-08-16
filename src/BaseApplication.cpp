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
// Model
root(NULL),
scene(NULL),
window(NULL),
// View
camera(NULL),
camera_man(NULL),
tray(NULL),
panel(NULL),
cursor_was_visible(false),
mShutDown(false),
// Resources
resources_cfg(Ogre::StringUtil::BLANK),
plugins_cfg(Ogre::StringUtil::BLANK),
//OIS Input devices
input(NULL),
mouse(NULL),
keyboard(NULL)
{
}

//------------------------------------------------------------------------------
BaseApplication::~BaseApplication(void)
{
  // Remove objects if they exist
  if (tray)
    delete tray;
  if (camera_man)
    delete camera_man;

  //Remove ourself as a Window listener
  Ogre::WindowEventUtilities::removeWindowEventListener(window, this);
  windowClosed(window);

  // Delete the root last of all
  delete root;
}

//------------------------------------------------------------------------------
bool BaseApplication::configure(void)
{
  // Show the configuration dialog and initialise the system
  // You can skip this and use root.restoreConfig() to load configuration
  // settings if you were sure there are valid ones saved in ogre.cfg
  if(root->showConfigDialog())
  {
    // If returned true, user clicked OK so initialise
    // Here 'true' makes the system create a default rendering window
    window = root->initialise(true, "Open War");

    return true;
  }
  else
    return false;
}
//------------------------------------------------------------------------------
void BaseApplication::chooseSceneManager(void)
{
  // Create the scene manager
  scene = root->createSceneManager(Ogre::ST_EXTERIOR_CLOSE);
  // scene = root->createSceneManager(Ogre::ST_EXTERIOR_REAL_FAR);
}
//------------------------------------------------------------------------------
void BaseApplication::createCamera(void)
{
  // Create the camera
  camera = scene->createCamera("PlayerCam");
  // Clipping
  camera->setNearClipDistance(5);
  bool infinite_distance =
    root->getRenderSystem()->getCapabilities()->hasCapability(Ogre::RSC_INFINITE_FAR_PLANE);
  camera->setFarClipDistance(infinite_distance ? 0 : 50000);

  // create camera controller
  camera_man = new OverheadCamera(camera);
}
//------------------------------------------------------------------------------
void BaseApplication::createFrameListener(void)
{
  Ogre::LogManager::getSingletonPtr()->logMessage("*** Initializing OIS ***");
  OIS::ParamList pl;
  size_t windowHnd = 0;
  std::ostringstream windowHndStr;

  window->getCustomAttribute("WINDOW", &windowHnd);
  windowHndStr << windowHnd;
  pl.insert(std::make_pair(std::string("WINDOW"), windowHndStr.str()));

  // Input managers and devices
  input = OIS::InputManager::createInputSystem( pl );
  keyboard = static_cast<OIS::Keyboard*>(
                input->createInputObject( OIS::OISKeyboard, true ));
  mouse = static_cast<OIS::Mouse*>(
                input->createInputObject( OIS::OISMouse, true ));
  mouse->setEventCallback(this);
  keyboard->setEventCallback(this);

  //Set initial mouse clipping size
  windowResized(window);

  // Register as a Window listener
  Ogre::WindowEventUtilities::addWindowEventListener(window, this);

  tray =
    new OgreBites::SdkTrayManager("InterfaceName", window, mouse, this);
  tray->showFrameStats(OgreBites::TL_BOTTOMLEFT);
  tray->showLogo(OgreBites::TL_BOTTOMRIGHT);
  tray->hideCursor();

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

  panel =
    tray->createParamsPanel(OgreBites::TL_NONE, "DetailsPanel", 200, items);
  panel->setParamValue(9, "Bilinear");
  panel->setParamValue(10, "Solid");
  panel->hide();

  root->addFrameListener(this);
}
//------------------------------------------------------------------------------
void BaseApplication::destroyScene(void)
{
}
//------------------------------------------------------------------------------
void BaseApplication::createViewports(void)
{
  // Create one viewport, entire window
  Ogre::Viewport* vp = window->addViewport(camera);
  vp->setBackgroundColour(Ogre::ColourValue(0,0,0));

  // Alter the camera aspect ratio to match the viewport
  camera->setAspectRatio(
    Ogre::Real(vp->getActualWidth()) / Ogre::Real(vp->getActualHeight()));
}
//------------------------------------------------------------------------------
void BaseApplication::setupResources(void)
{
  // Load resource paths from config file
  Ogre::ConfigFile cf;
  cf.load(resources_cfg);

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
  resources_cfg = "resources_d.cfg";
  plugins_cfg = "plugins_d.cfg";
#else
  resources_cfg = "resources.cfg";
  plugins_cfg = "plugins.cfg";
#endif

  // Check for errors
  if (!setup())
    return;

  // Launch
  root->startRendering();

  // Clean up
  destroyScene();
}
//------------------------------------------------------------------------------
bool BaseApplication::setup(void)
{
  root = new Ogre::Root(plugins_cfg);

  setupResources();

  bool carryOn = configure();
  if (!carryOn)
    return false;

  chooseSceneManager();
  createCamera();
  createViewports();

  // Set default mipmap level (NB some APIs ignore this)
  Ogre::TextureManager::getSingleton().setDefaultNumMipmaps(5);

  // Create any resource listeners (for loading screens) and load everything
  createResourceListener();
  loadResources();

  // Create the scene
  createScene();

  createFrameListener();

  return true;
};
//------------------------------------------------------------------------------
bool BaseApplication::frameRenderingQueued(const Ogre::FrameEvent& evt)
{
  if(window->isClosed())
    return false;

  if(mShutDown)
    return false;

  //Need to capture/update each device
  keyboard->capture();
  mouse->capture();

  tray->frameRenderingQueued(evt);

  if (!tray->isDialogVisible())
  {
    // if dialog isn't up, then update the camera
    camera_man->frameRenderingQueued(evt);
    // if details panel is visible, then update its contents
    if (panel->isVisible())
    {
      panel->setParamValue(0,
        Ogre::StringConverter::toString(camera->getDerivedPosition().x));
      panel->setParamValue(1,
        Ogre::StringConverter::toString(camera->getDerivedPosition().y));
      panel->setParamValue(2,
        Ogre::StringConverter::toString(camera->getDerivedPosition().z));
      panel->setParamValue(4,
        Ogre::StringConverter::toString(camera->getDerivedOrientation().w));
      panel->setParamValue(5,
        Ogre::StringConverter::toString(camera->getDerivedOrientation().x));
      panel->setParamValue(6,
        Ogre::StringConverter::toString(camera->getDerivedOrientation().y));
      panel->setParamValue(7,
      Ogre::StringConverter::toString(camera->getDerivedOrientation().z));
    }
  }

  return true;
}
//------------------------------------------------------------------------------
bool BaseApplication::keyPressed( const OIS::KeyEvent &evt )
{
  // don't process any more keys if dialog is up
  if (tray->isDialogVisible())
    return true;

  // toggle visibility of advanced frame stats
  if (evt.key == OIS::KC_F)
    tray->toggleAdvancedFrameStats();

  // toggle visibility of even rarer debugging details
  else if (evt.key == OIS::KC_G)
  {
    if (panel->getTrayLocation() == OgreBites::TL_NONE)
    {
      tray->moveWidgetToTray(panel, OgreBites::TL_TOPRIGHT, 0);
      panel->show();
    }
    else
    {
      tray->removeWidgetFromTray(panel);
      panel->hide();
    }
  }

  // cycle polygon rendering mode
  else if (evt.key == OIS::KC_T)
  {
    Ogre::String newVal;
    Ogre::TextureFilterOptions tfo;
    unsigned int aniso;

    switch (panel->getParamValue(9).asUTF8()[0])
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
    panel->setParamValue(9, newVal);
  }

  // cycle polygon rendering mode
  else if (evt.key == OIS::KC_R)
  {
    Ogre::String newVal;
    Ogre::PolygonMode pm;

    switch (camera->getPolygonMode())
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

    camera->setPolygonMode(pm);
    panel->setParamValue(10, newVal);
  }

  // refresh all textures
  else if(evt.key == OIS::KC_F5)
    Ogre::TextureManager::getSingleton().reloadAll();

  // take a screenshot
  //else if (evt.key == OIS::KC_SYSRQ)
    //window->writeContentsToTimestampedFile("screenshot", ".jpg");

  // exit
  else if (evt.key == OIS::KC_ESCAPE)
    mShutDown = true;

  // pass key input to camera
  camera_man->injectKeyDown(evt);
  return true;
}

bool BaseApplication::keyReleased( const OIS::KeyEvent &evt )
{
  // pass to the camera controller
  camera_man->injectKeyUp(evt);

  // consume event
  return true;
}

bool BaseApplication::mouseMoved( const OIS::MouseEvent &evt )
{
  // the tray may consume the event before it gets to the camera
  if (tray->injectMouseMove(evt))
    return true;

  // pass to the camera controller if not consumed
  camera_man->injectMouseMove(evt);

  // consume event
  return true;
}

bool BaseApplication::mousePressed( const OIS::MouseEvent &evt, OIS::MouseButtonID id )
{
  // the tray may consume the event before it gets to the camera
  if (tray->injectMouseDown(evt, id))
    return true;

  // pass to the camera controller if not consumed
  camera_man->injectMouseDown(evt, id);

  return true;
}

bool BaseApplication::mouseReleased( const OIS::MouseEvent &evt, OIS::MouseButtonID id )
{
  // the tray may consume the event before it gets to the camera
  if (tray->injectMouseUp(evt, id))
    return true;

  // pass to the camera controller if not consumed
  camera_man->injectMouseUp(evt, id);

  return true;
}

//Adjust mouse clipping area
void BaseApplication::windowResized(Ogre::RenderWindow* rw)
{
  unsigned int width, height, depth;
  int left, top;
  rw->getMetrics(width, height, depth, left, top);

  const OIS::MouseState &ms = mouse->getMouseState();
  ms.width = width;
  ms.height = height;
}

//Unattach OIS before window shutdown (very important under Linux)
void BaseApplication::windowClosed(Ogre::RenderWindow* rw)
{
  //Only close for window that created OIS (the main window in these demos)
  if( rw == window )
  {
    if( input )
    {
      input->destroyInputObject( mouse );
      input->destroyInputObject( keyboard );

      OIS::InputManager::destroyInputSystem(input);
      input = NULL;
    }
  }
}
