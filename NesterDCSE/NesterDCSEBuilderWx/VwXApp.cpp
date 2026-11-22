#include "app.h"
#include  <wx/image.h>


bool MyApp::VwXinitApp()
{
  wxInitAllImageHandlers();
  return true;
}
