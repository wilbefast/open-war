#include <Ogre.h>

#include "platform.h"           // needed or MAIN and ERROR
#include "Application.hpp"

using namespace std;
using namespace Ogre;

MAIN
{
  Application app;
  try
  {
    app.go();
	}
	catch(Ogre::Exception& e)
	{
    ERROR(e.getFullDescription().c_str());
  }

  return EXIT_SUCCESS;
}

