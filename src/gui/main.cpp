#include <libintl.h>

#include "MultiDelogoApp.hpp"


int main(int argc, char *argv[])
{
  bindtextdomain(GETTEXT_PACKAGE, LOCALEDIR);
  bind_textdomain_codeset(GETTEXT_PACKAGE, "UTF-8");
  textdomain(GETTEXT_PACKAGE);

  auto app = mdl::MultiDelogoApp::create();
  return app->run(argc, argv);
}
