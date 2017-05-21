#include "boost/program_options.hpp"
#include <dlfcn.h>
#include <iostream>

#include "parser.hh"

#include "adaptater.hh" // TO DELETE
#include "engine.hh"
#include "plugin/listener.hh"
namespace po = boost::program_options;

int main(int argc, char* argv[])
{
  argc = argc;
  po::options_description desc("Allowed options");
  desc.add_options()("help,h",
                     "show usage")("port,p", po::value<unsigned short>(),
                                   "select the listening port for the network")(
    "pgn", po::value<std::string>(), "path to the PGN game file")(
    "listeners,l",
    po::value<std::vector<std::string>>()->multitoken()->value_name("path"),
    "list of paths to listener plugins");

  po::variables_map vm;
  po::store(po::parse_command_line(argc, argv, desc), vm);
  po::notify(vm);

  if (vm.count("help"))
  {
    std::cout << desc << "\n";
    return 0;
  }
  std::vector<Listener*> listeners;
  std::vector<void*> handles;
  if (vm.count("listeners"))
  {
    std::vector<std::string> listeners_name =
      vm["listeners"].as<std::vector<std::string>>();
    for (auto s : listeners_name)
    {
      void* handle = dlopen(s.c_str(), RTLD_NOW);
      handles.push_back(handle);
      if (handle == nullptr)
        std::invalid_argument("Can't open librairy");
      dlerror();
      Listener* listener =
      reinterpret_cast<Listener* (*)()>(dlsym(handle, "listener_create"))();
      listeners.push_back(listener);
    }
  }

  if (vm.count("port"))
  {
    unsigned short port = vm["port"].as<unsigned short>();
    Engine engine(listeners, port);
    engine.start();
  }
  else if (vm.count("pgn"))
  {
    std::string pgn_path = vm["pgn"].as<std::string>();
    Engine engine(listeners, pgn_path);
    engine.start();
  }
  else
    std::cerr << "No option were given." << std::endl
            << "Please use --pgn or -port option" << std::endl;
  for (auto l : listeners)
    delete l;
  for (auto h : handles)
    dlclose(h);
  return 0;
}
