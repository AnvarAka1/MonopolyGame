#include <stdbool.h>
struct Player
{
  int id;
  char *name;
  int color;
  int money;
  int position;
  bool noMoney;
  bool needMoney;
  bool inJail;
  bool hasJailFreeCard;
  int jailCounter;
};
struct Dice
{
  int num1;
  int num2;
  int counter;
};