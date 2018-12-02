#include "player.h"

struct Card
{
  char *comChest[20];
  char *chance[20];
};
struct Improvement
{
  int rent;
  int numOfHouses;
  int pricesOfHouses;
  bool hotelExists;
  int priceOfHotel;
  int playerPrice;
};
struct BoardCell
{
  char *title;
  int initialPrice;
  // 0 - none, 1 - brown, 2 - skyblue, 3 - pink, 4 - orange,
  // 5 - red, 6 - yellow, 7 - green, 8 - blue
  int color;
  // 0 - start, 1 - usual, 2 - community chest, 3 - chance,
  // 4 - tax, 5 - railroad, 6 - jail, 7 - electric,
  // 8 - nothing(parking+gotojail), 9 - water works
  int type;
  struct Player owner;
  struct Improvement impr;
};
struct Board
{
  int sizeOfBoard;
  struct Card card;
  struct BoardCell cells[40];
};
