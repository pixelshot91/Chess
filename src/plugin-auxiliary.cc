#include "plugin-auxiliary.hh"

char operator~(plugin::File f)
{
  return static_cast<char>(f);
}

char operator~(plugin::Rank r)
{
  return static_cast<char>(r);
}

plugin::Color operator!(plugin::Color color)
{
  return static_cast<plugin::Color>(not static_cast<bool>(color));
}

std::ostream& operator<<(std::ostream& o, const plugin::Color c)
{
  if (c == plugin::Color::WHITE)
    o << "White";
  else
    o << "Black";
  return o;
}

std::ostream& operator<<(std::ostream& o, const plugin::PieceType type)
{
  switch (type)
  {
    case plugin::PieceType::KING:
      o << "King";
      break;
    case plugin::PieceType::QUEEN:
      o << "Queen";
      break;
    case plugin::PieceType::ROOK:
      o << "Rook";
      break;
    case plugin::PieceType::BISHOP:
      o << "Bishop";
      break;
    case plugin::PieceType::KNIGHT:
      o << "Knight";
      break;
    case plugin::PieceType::PAWN:
      o << "Pawn";
      break;
  }
  return o;
}

std::ostream& operator<<(std::ostream& o, const plugin::Position p)
{
  o << (char)(static_cast<char>(p.file_get()) + 'a')
    << (char)(static_cast<char>(p.rank_get()) + '1');
  return o;
}


namespace auxiliary {
  /*char PieceTypeToInt(plugin::PieceType type)
  {
    char i;
    for (i = 0; plugin::piecetype_array()[i] != type; ++i)
      continue;
    if (i >= 6)
      throw std::invalid_argument("Not a piece Symbol");
    return i;
  }*/

  std::string to_lan(plugin::Position pos)
  {
    std::string result;
    result += static_cast<char>(pos.file_get()) + 'a';
    result += static_cast<char>(pos.rank_get()) + '1';
    return result;
  }

  int distance(plugin::Position& pos1, plugin::Position& pos2)
  {
    return abs(~pos1.file_get() - ~pos2.file_get()) + abs(~pos1.rank_get() - ~pos2.rank_get());
  }
  void GetUnicodeChar(unsigned int code, char chars[5])
  {
    if (code <= 0x7F)
    {
      chars[0] = (code & 0x7F);
      chars[1] = '\0';
    }
    else if (code <= 0x7FF)
    {
      // one continuation byte
      chars[1] = 0x80 | (code & 0x3F);
      code = (code >> 6);
      chars[0] = 0xC0 | (code & 0x1F);
      chars[2] = '\0';
    }
    else if (code <= 0xFFFF)
    {
      // two continuation bytes
      chars[2] = 0x80 | (code & 0x3F);
      code = (code >> 6);
      chars[1] = 0x80 | (code & 0x3F);
      code = (code >> 6);
      chars[0] = 0xE0 | (code & 0xF);
      chars[3] = '\0';
    }
    else if (code <= 0x10FFFF)
    {
      // three continuation bytes
      chars[3] = 0x80 | (code & 0x3F);
      code = (code >> 6);
      chars[2] = 0x80 | (code & 0x3F);
      code = (code >> 6);
      chars[1] = 0x80 | (code & 0x3F);
      code = (code >> 6);
      chars[0] = 0xF0 | (code & 0x7);
      chars[4] = '\0';
    }
    /*else
    {
      // unicode replacement character
      chars[2] = 0xEF;
      chars[1] = 0xBF;
      chars[0] = 0xBD;
      chars[3] = '\0';
    }*/
  }

}
