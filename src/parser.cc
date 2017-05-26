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
  if (pgn_path_.substr(pgn_path_.size() - 4) != ".pgn")
    throw std::invalid_argument(pgn_path_ + " is not a valid PGN file");
}

std::shared_ptr<Move> Parser::parse_uci(std::string s, plugin::Color color, const ChessBoard& board)
{
  boost::regex uci_move("([a-h][1-8]){2}([BRNQ])?");
  if (not boost::regex_match(s, uci_move))
    throw std::invalid_argument("Invlid move: " + s + "\nThe syntax is a1b2");
  plugin::Position pos_start(static_cast<plugin::File>(s[0] - 'a'), static_cast<plugin::Rank>(s[1] - '1'));
  plugin::Position pos_end(static_cast<plugin::File>(s[2] - 'a'), static_cast<plugin::Rank>(s[3] - '1'));
  //std::cerr << "first pos is " << pos_start << " end";
  if (board.piecetype_get(pos_start) == std::experimental::nullopt)
    throw std::invalid_argument("Invalid move: the start cell is empty");
    
  auto type = board.piecetype_get(pos_start).value();
  if (type == plugin::PieceType::KING) //castling
  {
    auto diff = ~pos_end.file_get() - ~pos_start.file_get();
    if (diff > 1) // Kingside caslting
      return std::make_shared<Move>(Move::Type::KING_CASTLING, color);
    if (diff < -1)
      return std::make_shared<Move>(Move::Type::QUEEN_CASTLING, color);
  }
  char promotion = -1;
  if (s[4] != '\0')
    promotion = auxiliary::PieceTypeToInt(static_cast<plugin::PieceType>(s[4]));
  bool attack = (board.piecetype_get(pos_end) != std::experimental::nullopt);
  if (type == plugin::PieceType::PAWN and pos_start.file_get() != pos_end.file_get())
    attack = 1;
  return std::make_shared<QuietMove>(color, pos_start, pos_end, type,
      attack, false, promotion); // FIX PROMOTION
}

std::shared_ptr<Move> Parser::parse_move(std::string s, plugin::Color color, bool pgn_check)
{
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
    return generateMove(color, what);
  else if (boost::regex_search(s, what, queenside_rook))
    return std::make_shared<Move>(Move::Type::QUEEN_CASTLING, color);
  else if (boost::regex_search(s, what, kingside_rook))
    return std::make_shared<Move>(Move::Type::KING_CASTLING, color);
  else if (boost::regex_search(s, what, game_termination)) {
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
  while (pgn.good())
  {
    std::string s;
    getline(pgn, s);
    boost::regex exp("\\[(?<var>[^\"]+) \"(?<value>[^\"]+)\"]");
    boost::smatch what;
    if (boost::regex_search(s, what, exp))
    {
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
