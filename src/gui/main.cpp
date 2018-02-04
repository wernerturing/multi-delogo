#include "MultiDelogoApp.hpp"


int main(int argc, char *argv[])
{
  auto app = mdl::MultiDelogoApp::create();
  return app->run(argc, argv);
}
