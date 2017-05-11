#pragma once

class Bishop : public Piece
{
public:
  bool isMoveAuthorized(Move)  override;
  bool isMoveLegal(Move) override;
};
