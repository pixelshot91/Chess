#include <iostream>
#include <fstream>
//#include <regex>
#include <boost/regex.hpp>

#include "parser.hh"

Parser::Parser(std::string pgn_path) {
  std::ifstream pgn(pgn_path);
  if (!pgn.is_open())
    throw std::invalid_argument("Try to read base from empty file");
  //std::string s = "[Event \"name\"]";
  std::string s;
  while (pgn.good()) {
    getline(pgn, s);
    //boost::regex exp("(?<var>\\S+)");
    boost::regex exp("\\[(?<var>[^\"]+) \"(?<value>[^\"]+)\"]");
    boost::smatch what;
    if (boost::regex_search(s, what, exp))
      std::cout << "var = " << what["var"] << std::endl << "value = " << what["value"] << std::endl;
    else {
      std::cout << "no match" << std::endl;
      break;
    }
  }
  while (pgn.good()) {
    getline(pgn, s, '.');
    std::cout << "line = " << s << std::endl;
    //boost::regex exp("(?<var>\\S+)");
    boost::regex exp1("(?<piece>\\S?)(?<start_file>\\S)(?<start_rank>\\d)(?<take>[-x])(?<end_file>\\S)(?<end_rank>\\d)(?<check>[+#]?)");
    boost::regex exp2("(?<piece>\\S?)(?<start_file>\\S)(?<start_rank>\\d)(?<take>[-x])(?<end_file>\\S)(?<end_rank>\\d)(?<check>[+#]?)");
    boost::regex game_termination("(?<resultat>(1-0|0-1|1/2-1/2))");

    boost::smatch what;
    if (boost::regex_search(s, what, exp1)) {
      std::cout << "WHAT " << what[0] << std::endl;
      std::cout << (what["piece"] == std::string("") ? "Pawn" : what["piece"].str() ) << " " << what["start_file"] << " " << what["start_rank"] << " " << what["take"] << " "
        << what["end_file"] << " " << what["end_rank"] << std::endl;
      //std::cout << "piece = " << what["piece"] << std::endl << "start = " << what["start_file"] << std::endl;
    }
    else {
      std::cout << "no match for EXP1" << std::endl;
    }
    std::string remaining_string = s.substr(what[0].str().length());
    if (boost::regex_search(remaining_string, what, exp2)) {
      std::cout << "WHAT " << what[0] << std::endl;
      std::cout << what["piece"] << " " << what["start_file"] << " " << what["start_rank"] << " " << what["take"] << " "
        << what["end_file"] << " " << what["end_rank"] << std::endl;
      //std::cout << "piece = " << what["piece"] << std::endl << "start = " << what["start_file"] << std::endl;
    }
    else {
      std::cout << "no match for EXP2" << std::endl;
      if (boost::regex_search(s, what, game_termination)) {
        std::cout << what["resultat"] << std::endl; 
      }
    }

  }


  /*std::regex word_regex("\\[(\\S+)");
  auto words_begin = std::sregex_iterator(s.begin(), s.end(), word_regex);
  auto words_end = std::sregex_iterator();

  //std::cout << "Found " << std::distance(words_begin words_end) << " words\n";
  for (std::sregex_iterator i = words_begin; i != words_end; ++i) {
    std::smatch match = *i;
    std::string match_str = match.str();
    std::cout << "  " << match_str << '\n';
  }*/

}

