#include <sstream>
#include "history.hh"

Move& History::last_get() {
  if (not data_.empty())
    return *data_.back();
  throw std::invalid_argument("Couldn't access last move of empty history !");
}
