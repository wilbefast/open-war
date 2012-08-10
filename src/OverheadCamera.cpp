#include "OverheadCamera.hpp"

#include <CEGUI/CEGUI.h> // for CEGUI::MouseCursor

using namespace Ogre;

/// CREATION, DESTRUCTION

OverheadCamera::OverheadCamera(Camera* _camera, RaySceneQuery* _ray_query) :
camera(_camera),
ray_query(_ray_query),
top_speed(250.0f),
speed(Vector3::ZERO),
cursor_pos(Vector3::ZERO),
input(Vector2::ZERO)
{
}

OverheadCamera::~OverheadCamera()
{
}

/// UPDATE

bool OverheadCamera::frameRenderingQueued(const FrameEvent& evt)
{
  // acceleration vector to be built based on keyboard input composite
  Vector3 delta = Vector3::ZERO,
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
  Ogre::Real tiny = std::numeric_limits<Real>::epsilon();

  // keep camera velocity below top speed
  if (speed.squaredLength() > top_speed*top_speed)
  {
    speed.normalise();
    speed *= top_speed;
  }
  // set camera velocity to 0 if below the tiny value
  else if (speed.squaredLength() < tiny*tiny)
    speed = Vector3::ZERO;

  // move the camera
  if (speed != Vector3::ZERO)
    camera->move(speed * evt.timeSinceLastFrame);

  return true;
}

/// CONTROL

void OverheadCamera::injectStop()
{
  speed = Vector3::ZERO;
  input = Vector2::ZERO;
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
  // Setup the ray scene query, use CEGUI's mouse position
  CEGUI::Point mouse_pos = CEGUI::MouseCursor::getSingleton().getPosition();
  Ray mouse_ray =
    camera->getCameraToViewportRay(mouse_pos.d_x/float(evt.state.width),
                                   mouse_pos.d_y/float(evt.state.height));
  ray_query->setRay(mouse_ray);

  // Execute query and overwrite current cursor position with result
  RaySceneQueryResult &result = ray_query->execute();
  RaySceneQueryResult::iterator itr = result.begin();
  if (itr != result.end() && itr->worldFragment)
    cursor_pos = itr->worldFragment->singleIntersection;

  // Zoom towards the object of interest
  Vector3 direction = cursor_pos - camera->getPosition();
  direction.normalise();
  direction *= evt.state.Z.rel * 0.1f;
  camera->move(direction);

  // Update CEGUI with the mouse motion
  CEGUI::System::getSingleton().injectMouseMove(evt.state.X.rel, evt.state.Y.rel);
}
