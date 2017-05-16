#include <fstream>
#include <iostream>
//#include <regex>
#include <boost/algorithm/string.hpp>

#include "parser.hh"

#include "chessboard.hh"
#include "plugin-auxiliary.hh"

Parser::Parser(std::string pgn_path)
  : pgn_path_(pgn_path)
{
}

std::shared_ptr<Move> Parser::parse_move(std::string s, plugin::Color color, bool pgn_check)
{
  /*boost::regex exp("(?<piece>\\S?)(?<start_file>\\S)(?<start_rank>\\d)(?<take>["
    "-x])(?<end_file>\\S)(?<end_rank>\\d)(?<check>[+#]?)");
    boost::regex kingside_rook("O-O");
    boost::smatch what;
    if (boost::regex_search(s, what, exp))
    {
    return generateMove(color, what);
    }
    else if (boost::regex_search(s, what, kingside_rook))
    {
    return std::make_shared(Move(Move::Type::KING_CASTLING, color));
    }
    else
    throw std::invalid_argument("invalid move");*/
  //std::cerr << "s = " << s << std::endl;
  std::string quiet_move_s("^[\n ]*(?<piece>[BRNQK]?)(?<start_file>[a-h])(?<start_rank>["
      "1-8])(?<take>[-x])(?<end_file>[a-h])(?<end_rank>[1-8])(?<promotion>(=[BRNQ])?)");

  std::string end_exp = pgn_check ? "(?<check>[+#]?)" : "$";
  std::string king_side_rook_s("O-O");
  std::string queen_side_rook_s("O-O-O");

  boost::regex quiet_move(quiet_move_s + end_exp);
  boost::regex kingside_rook(king_side_rook_s + end_exp);
  boost::regex queenside_rook(queen_side_rook_s + end_exp);
  boost::regex game_termination("(?<resultat>(1-0|0-1|1/2-1/2))");
  boost::smatch what;

  if (boost::regex_search(s, what, quiet_move))
  {
    /*std::cerr << "WHAT " << what[0] << std::endl;
      std::cerr << (what["piece"] == std::string("") ? "Pawn"
      : what["piece"].str())
      << " " << what["start_file"] << " " << what["start_rank"]
      << " " << what["take"] << " " << what["end_file"] << " "
      << what["end_rank"] << std::endl;*/
    return generateMove(color, what);
  }
  else if (boost::regex_search(s, what, queenside_rook))
  {
    //std::cerr << "QUEENSIDE MOVE : " << s << std::endl;
    return std::make_shared<Move>(Move::Type::QUEEN_CASTLING, color);
  }
  else if (boost::regex_search(s, what, kingside_rook))
  {
    //std::cerr << "KINGSIDE MOVE" << std::endl;
    return std::make_shared<Move>(Move::Type::KING_CASTLING, color);
  }
  else if (boost::regex_search(s, what, game_termination)) {
    //std::cerr << "Game termination" << s << std::endl;
    throw std::invalid_argument("game termination");
  }
  else
    throw std::invalid_argument("invalid move");

}

Parser::moves_t Parser::parse()
{
  std::ifstream pgn(pgn_path_);
  if (!pgn.is_open())
    throw std::invalid_argument("Try to read base from empty file");
  // std::string s = "[Event \"name\"]";
  while (pgn.good())
  {
    std::string s;
    getline(pgn, s);
    // boost::regex exp("(?<var>\\S+)");
    boost::regex exp("\\[(?<var>[^\"]+) \"(?<value>[^\"]+)\"]");
    boost::smatch what;
    if (boost::regex_search(s, what, exp))
    {
      // std::cerr << "var = " << what["var"] << std::endl << "value = " <<
      // what["value"] << std::endl;
    }
    else
    {
      // std::cerr << "no match" << std::endl;
      break;
    }
  }

  std::string whole_file;
  while (pgn) {
    std::string line;
    std::getline(pgn, line);
    whole_file += line + '\n';
  }
  std::vector<std::string> tokens;
  boost::split(tokens, whole_file, boost::is_any_of("\n "));

  std::vector<std::shared_ptr<Move>> moves;
  try {
    for (size_t i = 0; i < tokens.size(); ++i)
    { // Move
      // boost::regex exp("(?<var>\\S+)");

      //std::cerr << "s = ^" << tokens[i] << "$" << std::endl;
      if (i % 3 == 0)
        continue;
      moves.push_back(parse_move(tokens[i], static_cast<plugin::Color>((i % 3)- 1), true));
    }
  }
  catch (std::invalid_argument e)
  {}
 return moves;
}

std::shared_ptr<QuietMove> Parser::generateMove(plugin::Color color, boost::smatch what)
{
  plugin::Position pos_start(
      static_cast<plugin::File>(what["start_file"].str()[0] - 'a'),
      static_cast<plugin::Rank>(what["start_rank"].str()[0] - '1'));
  plugin::Position pos_end(
      static_cast<plugin::File>(what["end_file"].str()[0] - 'a'),
      static_cast<plugin::Rank>(what["end_rank"].str()[0] - '1'));
  char type = (what["piece"] == std::string("") ? 'P' : what["piece"].str()[0]);
  char promotion_piecetype = -1;
  if (what["promotion"].str() != "")
    promotion_piecetype = auxiliary::PieceTypeToInt(static_cast<plugin::PieceType>(what["promotion"].str()[1]));
  return std::make_shared<QuietMove>(
      color, pos_start, pos_end, static_cast<plugin::PieceType>(type),
      what["take"].str() == std::string("x"), false, promotion_piecetype); // FIX PROMOTION
}
