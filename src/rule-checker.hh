#pragma once

#include "move.hh"
#include "quiet-move.hh"

class RuleChecker
{
public:
  RuleChecker() = default;
  bool check(uint64_t board[6], const Move& m);
private:
  bool isMoveAuthorized(uint64_t board[6], const Move& m);
  bool isMoveLegal(uint64_t board[6], Move m);
};
