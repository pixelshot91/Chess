#pragma once

#include <string>

class Parser
{
public:
  Parser(std::string pgn_path);
private:
  std::string pgn_path;
};
