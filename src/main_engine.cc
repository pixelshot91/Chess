#include <iostream>

#include "boost/program_options.hpp"

#include "parser.hh"

#include "adaptater.hh" // TO DELETE

namespace po = boost::program_options;

int main(int argc, char* argv[])
{
  po::options_description desc("Allowed options");
  desc.add_options()
    ("help,h", "show usage")
    ("port,p", po::value<int>(),"select the listening port for the network")
    ("pgn", po::value<std::string>(), "path to the PGN game file")
    ("listeners,l", po::value<std::vector<std::string>>()->value_name("path"), "list of paths to listener plugins");

  po::variables_map vm;
  po::store(po::parse_command_line(argc, argv, desc), vm);
  po::notify(vm);

  if (vm.count("help")) {
    std::cout << desc << "\n";
    return 0;
  }
  if (vm.count("port")) {
    std::cout << "port is: " << vm["port"].as<int>() << std::endl;
    return 0;
  }
  else if (vm.count("pgn")) {
    std::string pgn_path = vm["pgn"].as<std::string>();
    std::cout << "pgn path is: " << pgn_path << std::endl;
    std::cout << "Parsing..." << std::endl;
    Parser parser(pgn_path);
    return parser.parse();
  }
  std::cerr << "No option were given." << std::endl << "Please use --pgn or -port option" << std::endl;
  return 0;
  /*if (vm.count("listeners"))
    std::cout << "listeners are: " << vm["listeners"].as<std::vector<std::string>>() << std::endl;*/
}
