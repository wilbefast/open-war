#include "OverheadCamera.hpp"

#include <iostream>
using namespace std;

/// CREATION, DESTRUCTION

OverheadCamera::OverheadCamera(Ogre::Camera* _camera) :
camera(_camera),
top_speed(250.0f),
speed(Ogre::Vector3::ZERO),
input(Ogre::Vector3::ZERO)
{
}

OverheadCamera::~OverheadCamera()
{
}

/// UPDATE

bool OverheadCamera::frameRenderingQueued(const Ogre::FrameEvent& evt)
{
  // acceleration vector to be built based on keyboard input composite
  Ogre::Vector3 delta = Ogre::Vector3::ZERO,
  // prepare the axes along which the camera will move
                right = camera->getRight(),
                forwards = camera->getDirection();
  right.y = forwards.y = 0;

  // left and right
  if (input.x > 0)
    delta += right;
  else if (input.x < 0)
    delta -= right;

  // forwards and backwards
  if (input.y > 0)
    delta -= forwards;
  else if (input.y < 0)
    delta += forwards;

  // if accelerating, try to reach top speed in a certain time
  if (delta.squaredLength() != 0)
  {
    delta.normalise();
    speed += delta * top_speed * evt.timeSinceLastFrame * 10;
  }
  // if not accelerating, try to stop in a certain time
  else
    speed -= speed * evt.timeSinceLastFrame * 10;

  // tiny value represents 0
  Ogre::Real tiny = std::numeric_limits<Ogre::Real>::epsilon();

  // keep camera velocity below top speed
  if (speed.squaredLength() > top_speed*top_speed)
  {
    speed.normalise();
    speed *= top_speed;
  }
  // set camera velocity to 0 if below the tiny value
  else if (speed.squaredLength() < tiny*tiny)
    speed = Ogre::Vector3::ZERO;

  // move the camera
  if (speed != Ogre::Vector3::ZERO)
    camera->move(speed * evt.timeSinceLastFrame);

  return true;
}

/// CONTROL

void OverheadCamera::injectStop()
{
  speed = Ogre::Vector3::ZERO;
  input = Ogre::Vector3::ZERO;
}

void OverheadCamera::injectKeyDown(const OIS::KeyEvent& evt)
{
  // left and right
  if (evt.key == OIS::KC_LEFT)
    input.x = (input.x > 0) ? 0 : -1;
  else if (evt.key == OIS::KC_RIGHT)
    input.x = (input.x < 0) ? 0 : 1;
  // up and down
  else if (evt.key == OIS::KC_UP)
    input.y = (input.y > 0) ? 0 : -1;
  else if (evt.key == OIS::KC_DOWN)
    input.y = (input.y < 0) ? 0 : 1;
}

void OverheadCamera::injectKeyUp(const OIS::KeyEvent& evt)
{
  // left and right
  if (evt.key == OIS::KC_LEFT && input.x < 0)
    input.x = 0;
  else if (evt.key == OIS::KC_RIGHT && input.x > 0)
    input.x = 0;
  // up and down
  else if (evt.key == OIS::KC_UP && input.y < 0)
    input.y = 0;
  else if (evt.key == OIS::KC_DOWN && input.y > 0)
    input.y = 0;
}

void OverheadCamera::injectMouseMove(const OIS::MouseEvent& evt)
{
  // up and down
  camera->move(Ogre::Vector3(0, -evt.state.Z.rel, 0));
}
