#include <iostream>
#include <fstream>
//#include <regex>

#include "parser.hh"

#include "chessboard.hh"

Parser::Parser(std::string pgn_path)
  : pgn_path_(pgn_path)
{}

Move& Parser::parse_move(std::string s)
{
    boost::regex exp("(?<piece>\\S?)(?<start_file>\\S)(?<start_rank>\\d)(?<take>[-x])(?<end_file>\\S)(?<end_rank>\\d)(?<check>[+#]?)");
    boost::regex kingside_rook("O-O");
    boost::smatch what;
    if (boost::regex_search(s, what, exp)) {
      return *generateMove(plugin::Color::WHITE, what);
    }
    else if (boost::regex_search(s, what, kingside_rook)) {
      return *(new Move(Move::Type::KING_CASTLING, plugin::Color::WHITE));
    }
    else
      throw std::invalid_argument("invalid move");
}

std::vector<Move*> Parser::parse()
{
  std::ifstream pgn(pgn_path_);
  if (!pgn.is_open())
    throw std::invalid_argument("Try to read base from empty file");
  //std::string s = "[Event \"name\"]";
  std::string s;
  while (pgn.good()) { //
    getline(pgn, s);
    //boost::regex exp("(?<var>\\S+)");
    boost::regex exp("\\[(?<var>[^\"]+) \"(?<value>[^\"]+)\"]");
    boost::smatch what;
    if (boost::regex_search(s, what, exp)) {
      //std::cerr << "var = " << what["var"] << std::endl << "value = " << what["value"] << std::endl;
    }
    else {
      //std::cerr << "no match" << std::endl;
      break;
    }
  }

  std::vector<Move*> moves;
  while (pgn.good()) { // Move
    getline(pgn, s, '.');
    std::cerr << "line = " << s << std::endl;
    //boost::regex exp("(?<var>\\S+)");
    boost::regex exp1("^[\n ]*(?<piece>\\S?)(?<start_file>[a-h])(?<start_rank>\\d)(?<take>[-x])(?<end_file>\\S)(?<end_rank>\\d)(?<check>[+#]?)");
    boost::regex exp2("(?<piece>\\S?)(?<start_file>\\S)(?<start_rank>\\d)(?<take>[-x])(?<end_file>\\S)(?<end_rank>\\d)(?<check>[+#]?)");
    boost::regex kingside_rook("O-O");
    boost::regex queenside_rook("O-O-O");
    boost::regex game_termination("(?<resultat>(1-0|0-1|1/2-1/2))");
    boost::smatch what;
    if (boost::regex_search(s, what, exp1)) {
      std::cerr << "WHAT " << what[0] << std::endl;
      std::cerr << (what["piece"] == std::string("") ? "Pawn" : what["piece"].str() ) << " " << what["start_file"] << " " << what["start_rank"] << " " << what["take"] << " "
        << what["end_file"] << " " << what["end_rank"] << std::endl;
      //std::cerr << "piece = " << what["piece"] << std::endl << "start = " << what["start_file"] << std::endl;
      moves.push_back(generateMove(plugin::Color::WHITE, what));
    }
    else if (boost::regex_search(s, what, queenside_rook)) {
      std::cerr << "QUEENSIDE MOVE" << std::endl;
      moves.push_back(new Move(Move::Type::QUEEN_CASTLING, plugin::Color::WHITE));
    }
   else if (boost::regex_search(s, what, kingside_rook)) {
      std::cerr << "KINGSIDE MOVE" << std::endl;
      moves.push_back(new Move(Move::Type::KING_CASTLING, plugin::Color::WHITE));
    }
    else
      std::cerr << "no match for EXP1" << std::endl;
    std::string remaining_string = s.substr(what[0].str().length() + 1);
    std::cerr << "remaining_string = " << remaining_string << std::endl;
    if (boost::regex_search(remaining_string, what, exp2)) {
      /*std::cerr << "WHAT " << what[0] << std::endl;
      std::cerr << what["piece"] << " " << what["start_file"] << " " << what["start_rank"] << " " << what["take"] << " "
        << what["end_file"] << " " << what["end_rank"] << std::endl;*/
      //std::cerr << "piece = " << what["piece"] << std::endl << "start = " << what["start_file"] << std::endl;
      std::cerr << "remaining_string = " << remaining_string << std::endl;
      moves.push_back(generateMove(plugin::Color::BLACK, what));
    }
    else if (boost::regex_search(remaining_string, what, queenside_rook)) {
      std::cerr << "QUEENSIDE MOVE" << std::endl;
      moves.push_back(new Move(Move::Type::QUEEN_CASTLING, plugin::Color::BLACK));
    }
    else if (boost::regex_search(remaining_string, what, kingside_rook)) {
      std::cerr << "KINGSIDE MOVE" << std::endl;
      moves.push_back(new Move(Move::Type::KING_CASTLING, plugin::Color::BLACK));
    }
    else {
      std::cerr << "no match for EXP2, remaining_string = " << remaining_string << "$" << std::endl;
      std::cerr << remaining_string[0] << std::endl;
      if (boost::regex_search(remaining_string, what, game_termination)) {
        std::cerr << "result is " << what["resultat"] << std::endl; 
      }
    }
  }
  /*ChessBoard board;
  for (auto m : moves) {
    std::cerr << "move description" << std::endl;
    std::cerr << *m << std::endl;
    std::cerr << "end move description" << std::endl;
    board.update(*m);
    board.print();
  }*/
  return moves;

  /*std::regex word_regex("\\[(\\S+)");
  auto words_begin = std::sregex_iterator(s.begin(), s.end(), word_regex);
  auto words_end = std::sregex_iterator();

  //std::cerr << "Found " << std::distance(words_begin words_end) << " words\n";
  for (std::sregex_iterator i = words_begin; i != words_end; ++i) {
    std::smatch match = *i;
    std::string match_str = match.str();
    std::cerr << "  " << match_str << '\n';
  }*/
}

QuietMove* Parser::generateMove(plugin::Color color, boost::smatch what)
{
  plugin::Position pos_start(static_cast<plugin::File>(what["start_file"].str()[0] - 'a'), static_cast<plugin::Rank>(what["start_rank"].str()[0] - '1'));
  plugin::Position pos_end(static_cast<plugin::File>(what["end_file"].str()[0] - 'a'), static_cast<plugin::Rank>(what["end_rank"].str()[0] - '1'));
  char type = (what["piece"] == std::string("") ? 'P' : what["piece"].str()[0]);
  return new QuietMove(color, pos_start, pos_end, static_cast<plugin::PieceType>(type), what["take"].str() == std::string("x"), false); // FIX PROMOTION
}
