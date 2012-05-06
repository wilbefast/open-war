#ifndef APPLICATION_HPP_INCLUDED
#define APPLICATION_HPP_INCLUDED

#include <deque>
#include <map>

#include "BaseApplication.h"
#include "Soldier.hpp"

class Application : public BaseApplication
{
  /* ATTRIBUTES */
private:
  std::map<unsigned int, Soldier*> soldiers;

/* METHODS */
public:
  Application();
  virtual ~Application();

protected:
  virtual void createScene();
  virtual void createFrameListener();
  virtual bool frameRenderingQueued(const Ogre::FrameEvent &evt);
};

#endif // APPLICATION_HPP_INCLUDED
