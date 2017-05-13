#include <iostream>
#include <dlfcn.h>
#include "boost/program_options.hpp"

#include "parser.hh"

#include "adaptater.hh" // TO DELETE
#include "engine.hh"
#include "plugin/listener.hh"
//#include "chessboard.hh"
namespace po = boost::program_options;

int main(int argc, char* argv[])
{
  po::options_description desc("Allowed options");
  desc.add_options()
    ("help,h", "show usage")
    ("port,p", po::value<int>(),"select the listening port for the network")
    ("pgn", po::value<std::string>(), "path to the PGN game file")
    ("listeners,l", po::value<std::vector<std::string>>()->multitoken()->value_name("path"), "list of paths to listener plugins");

  po::variables_map vm;
  po::store(po::parse_command_line(argc, argv, desc), vm);
  po::notify(vm);

  if (vm.count("help")) {
    std::cerr << desc << "\n";
    return 0;
  }
  std::vector<Listener*> listeners;
  if (vm.count("listeners")) {
    std::vector<std::string> listeners_name = vm["listeners"].as<std::vector<std::string>>();
    for (auto s : listeners_name) {
      std::cerr << s << std::endl;
      void* handle = dlopen(s.c_str(), RTLD_NOW);
      if (handle == nullptr)
        std::invalid_argument("Can't open librairy");
      dlerror();
      Listener* listener = reinterpret_cast<Listener*(*)()>(dlsym(handle, "listener_create"))();
      listeners.push_back(listener);
    }
    std::cerr << listeners.size() << std::endl;
  }

  if (vm.count("port")) {
    std::cerr << "port is: " << vm["port"].as<int>() << std::endl;
    return 0;
  }
  if (vm.count("pgn")) {
    std::string pgn_path = vm["pgn"].as<std::string>();
    std::cerr << "pgn path is: " << pgn_path << std::endl;
    Engine engine(listeners, 0, pgn_path);
    engine.start();
    return 0;
  }
  std::cerr << "No option were given." << std::endl << "Please use --pgn or -port option" << std::endl;
  return 0;
  /*
   if (vm.count("listeners"))
    std::cerr << "listeners are: " << vm["listeners"].as<std::vector<std::string>>() << std::endl;
    */
}
