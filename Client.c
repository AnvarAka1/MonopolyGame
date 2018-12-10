#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include "board.h"
#define NULL ((void *)0)

void GUI();
void diceUI();
void makeOwner(struct Player p, struct Board b);
void calculateRent(struct Board b, int position);
int Randomize(struct Dice, struct Board, struct Player);
void goToJail(struct Board b, struct Player p);
void movePlayer(struct Player p, int targetPosition);
void moveAnimation(struct Player p, int targetPosition);
void playerOnCell(struct Board b, struct Player p);
void railroad(struct Board b, struct Player p);
void subtractMoney(struct Player p, struct BoardCell cell);
void electric(struct Board b, struct Player p);
void waterWorks(struct Board b, struct Player p);
void giveMoney(struct Player p, int money);
int cardRandom(struct Card c);
void makeCardAction(int comOrChance, int randNumber, struct Card c);
void communityChest(struct Player p, struct Card c);
void chance(struct Player p, struct Card c);
void sendDice(struct Dice d);
void buttonDisable();
void cardsInitialize(struct Card c);
void ownersInitialize(struct Board b);
void improvementsInitialize(struct Board b);
void boardFill(struct Board b);
void initializeCommunityChest(struct Board, int, int);

int main()
{
  time_t t;
  srand((unsigned)time(&t));
  struct Board b;
  struct Player p;
  struct Dice d;
  d.counter = 0;
  GUI(p, b);
  while (Randomize(d, b, p) != 0)
  {
    diceUI();
  }
}

void GUI(struct Player p, struct Board b)
{

  // whole GUI
}
void diceUI()
{
  // Button throwDice(OYBEK)
}

void makeOwner(struct Player p, struct Board b)
{
  //assigns owner to the cell
  b.cells[p.position].owner = p;
  //put the rent price
  calculateRent(b, p.position);
}

// here the rent is calculated. This function can be invoked every step players make
// or after each purchase
void calculateRent(struct Board b, int position)
{
  struct Improvement imp;
  //just to make it simple, we store impr structure into imp
  imp = b.cells[position].impr;
  // calculate initial rent
  int rent = b.cells[position].initialPrice / 3;
  // calculate the rent
  imp.rent = rent + imp.pricesOfHouses * imp.numOfHouses + imp.priceOfHotel * imp.hotelExists;
}

// Dice Generator function
int Randomize(struct Dice d, struct Board b, struct Player p)
{
  buttonDisable();
  /* Intializes random number generator */
  d.num1 = 1 + rand() % 6; //5
  d.num2 = 1 + rand() % 6; //6
  if (d.num1 == d.num2)
  {
    d.counter++;
    if (d.counter == 3)
    {
      //go to jail
      goToJail(b, p);
      d.counter = 0;
    }
  }
  else
  {
    d.counter = 0;
  }
  sendDice(d);
  return d.counter;
}

void goToJail(struct Board b, struct Player p)
{
  int n = 40;
  int jailPosition;
  for (int i = 0; i < n; i++)
  {
    if (b.cells[i].type == 6)
    {
      jailPosition = i;
    }
  }
  movePlayer(p, jailPosition);
  //or for animation this can be done like
  moveAnimation(p, jailPosition);
}

void movePlayer(struct Player p, int targetPosition)
{
  p.position = targetPosition;
}
void moveAnimation(struct Player p, int targetPosition)
{
  while (p.position != targetPosition)
  {
    p.position++;
    //GUI handler to show the movement
    //pause for .3 sec
  }
}

// function makes some behaviour depending on the cell where
// the player stays
void playerOnCell(struct Board b, struct Player p)
{
  struct Player pl = b.cells[p.position].owner;
  struct BoardCell cell = b.cells[p.position];
  // if the player is not the owner of the cell
  if (pl.id != p.id)
  {
    switch (cell.type)
    {
    case 0:
      break;
    case 1:
      subtractMoney(p, cell);
      break;
    case 2:
      communityChest(p, b.card);
      break;
    case 3:
      chance(p, b.card);
      break;
    case 4:
      subtractMoney(p, cell);
      break;
    case 5:
      railroad(b, p);
      break;
    case 6:
      goToJail(b, p);
      break;
    case 7:
      electric(b, p);
      break;
    case 8:
      //nothing
      break;
    case 9:
      waterWorks(b, p);
      break;
    default:
      printf("Out of Bounds");
      break;
    }
  }
}

//returns an array with cell numbers with railroads which are owned by the player
void railroad(struct Board b, struct Player p)
{
  int railroadNumber[4];
  for (int i = 0; i < b.sizeOfBoard; i++)
  {
    if (b.cells[i].type == 5 && b.cells[i].owner.id == p.id)
    {
      // number of railroad cell which is available for the player
      // i.e. the player is an owner
      // or there will be function with GUI to enable all the cells with railroads and disable all without them
      railroadNumber[i] = i;
    }
    else
    {
      //disable cells
    }
  }
  //return 1;
  // return railroadNumber;
}

void subtractMoney(struct Player p, struct BoardCell cell)
{
  p.money -= cell.impr.rent;
}

void communityChest(struct Player p, struct Card c)
{
  makeCardAction(0, cardRandom(c), c);
}
void chance(struct Player p, struct Card c)
{
  makeCardAction(1, cardRandom(c), c);
}
// void railroad(struct Board b, struct Player p)
// {
//   findAllRailroads(b);
// }
void electric(struct Board b, struct Player p)
{
  // ???
}
void waterWorks(struct Board b, struct Player p)
{
  // ???
}
void giveMoney(struct Player p, int money)
{
  p.money += money;
}
// generates random number for cards
int cardRandom(struct Card c)
{
  int randNumber;
  randNumber = rand() % sizeof(c.comChest) / sizeof(int);
  return randNumber;
}

// invokes functions depending on which card is chosen

//needed to write 40 actions written in cards.
//some actions require the functions which already exist
//but some of them require new functions to be created
//Mokhlar must write as much as she can
void makeCardAction(int comOrChance, int randNumber, struct Card c)
{
  if (!comOrChance)
  {
    //Community Chest
    switch (randNumber)
    {
    case 0:
      break;
    case 1:
      break;
    case 2:
      break;
    case 3:
      break;
    case 4:
      break;
    case 5:
      break;
    case 6:
      break;
    case 7:
      break;
    case 8:
      break;
    case 9:
      break;
    case 10:
      break;
    case 11:
      break;
    case 12:
      break;
    case 13:
      break;
    case 14:
      break;
    case 15:
      break;
    case 16:
      break;
    case 17:
      break;
    case 18:
      break;
    case 19:
      break;
    default:
      printf("Out of Boundaries!");
      break;
    }
  }
  else
  {
    // Chance
    switch (randNumber)
    {
    case 0:
      break;
    case 1:
      break;
    case 2:
      break;
    case 3:
      break;
    case 4:
      break;
    case 5:
      break;
    case 6:
      break;
    case 7:
      break;
    case 8:
      break;
    case 9:
      break;
    case 10:
      break;
    case 11:
      break;
    case 12:
      break;
    case 13:
      break;
    case 14:
      break;
    case 15:
      break;
    case 16:
      break;
    case 17:
      break;
    case 18:
      break;
    case 19:
      break;
    default:
      printf("Out of Boundaries!");
      break;
    }
  }
}

void sendDice(struct Dice d)
{
  //send struct d through socket
}
void buttonDisable()
{
  //disable the Button
}

//Initialize Cards
void cardsInitialize(struct Card c)
{
   char *comChest[] = {"Advance to 'Go'. Collect $200", 
                      "Doctor's fees. Pay $50. ", 
                      "From sale of stock you get $50. ",
                      "Get Out of Jail Free.", 
                      "Go to Jail. Go directly to jail. Do not pass Go, Do not collect $200.", 
                      "Grand Opera Night. Collect $50 from every player for opening night seats. ", 
                      "Holiday Fund matures. Collect $100.", 
                      "Income tax refund. Collect $20.",
                      "It is your birthday. Collect $10 from every player. ", 
                      "Life insurance matures – Collect $100 ", 
                      "Hospital Fees. Pay $50.", 
                      "School fees. Pay $50. ", 
                      "Receive $25 consultancy fee.",
                      "You are assessed for street repairs: Pay $40 per house and $115 per hotel you own. ", 
                      "You have won second prize in a beauty contest. Collect $10. ", 
                      "You inherit $100.",
                      };
  char *chance[] = {"Advance to 'Go'. Collect $200", 
                    "Advance to St. Charles Place. If you pass Go, collect $200.", 
                    "Advance to Illinois Ave. If you pass Go, collect $200."
                    "Advance token to nearest Utility. If unowned, you may buy it from the Bank. If owned, throw dice and pay owner a total 10 times the amount thrown.", 
                    "Advance token to the nearest Railroad and pay owner twice the rental to which he/she {he} is otherwise entitled. If Railroad is unowned, you may buy it from the Bank. ", 
                    "Bank pays you dividend of $50.",
                    "Get out of Jail Free. This card may be kept until needed.", 
                    "Go Back Three {3} Spaces.", 
                    "Go to Jail. Go directly to Jail. Do not pass GO, do not collect $200.", 
                    "Make general repairs on all your property: For each house pay $25, For each hotel {pay} $100.", 
                    "Pay poor tax of $15 ", 
                    "Take a trip to Reading Railroad. {Take a ride on the Reading. Advance token and} If you pass Go, collect $200.", 
                    "Take a walk on the Boardwalk. Advance token to Boardwalk. {Board Walk in both sentences} ",
                    "You have been elected Chairman of the Board. Pay each player $50.", 
                    "Your building {and} loan matures. Receive {Collect} $150. ", 
                    "You have won a crossword competition. Collect $100.",
                    };  for (int i = 0; i < 20; i++)
  {
    strcpy(c.comChest[i], comChest[i]);
    strcpy(c.chance[i], chance[i]);
  }
}

void ownersInitialize(struct Board b)
{
  // Initialize owners as null
  b.cells[0].owner.color = 0;
  b.cells[0].owner.id = 0;
  b.cells[0].owner.inJail = 0;
  b.cells[0].owner.money = 0;
  b.cells[0].owner.name = 0;
  b.cells[0].owner.needMoney = 0;
  b.cells[0].owner.noMoney = 0;
  b.cells[0].owner.position = 0;
  // Null to all owner cells
  for (int i = 1; i < b.sizeOfBoard; i++)
  {
    b.cells[i].owner = b.cells[0].owner;
  }
}

void improvementsInitialize(struct Board b)
{
  for (int i = 0; i < b.sizeOfBoard; i++)
  {
    b.cells[i].impr.hotelExists = false;
    b.cells[i].impr.numOfHouses = 0;
    b.cells[i].impr.playerPrice = 0;
  }
  for (int i = 0; i < b.sizeOfBoard; i++)
  {
    if (b.cells[i].type == 1)
    {
      if (b.cells[i].color == 1 || b.cells[i].color == 2)
      {
        b.cells[i].impr.priceOfHotel = 100;
        b.cells[i].impr.pricesOfHouses = 100;
      }
      else if (b.cells[i].color == 3 || b.cells[i].color == 4)
      {
        b.cells[i].impr.priceOfHotel = 150;
        b.cells[i].impr.pricesOfHouses = 150;
      }
      else if (b.cells[i].color == 5 || b.cells[i].color == 6)
      {
        b.cells[i].impr.priceOfHotel = 200;
        b.cells[i].impr.pricesOfHouses = 200;
      }
      else if (b.cells[i].color == 7 || b.cells[i].color == 8)
      {
        b.cells[i].impr.priceOfHotel = 250;
        b.cells[i].impr.pricesOfHouses = 250;
      }
      b.cells[i].impr.rent = b.cells[i].initialPrice / 3;
    }
  }
}

void boardFill(struct Board b)
{
  char *titles[] = {
      "Start", "Mediter-Renean Avenue", "Community Chest", "Baltic Avenue",
      "Income Tax", "Reading Railroad", "Oriental Avenue", "Chance", "Vermont Avenue",
      "Connecticut Avenue", "Jail", "St. Charles Place", "Electric Company", "States Avenue",
      "Virginia Avenue", "Pennsylvania Railroad", "ST. James Place", "Community Chest",
      "Tennessee Avenue", "New York Avenue", "New York Avenue", "Free Parking", "Kentucky Avenue",
      "Chance", "Indiana Avenue", "Illinois Avenue", "B. & O. Railroad", "Atlantic Avenue",
      "Ventnor Avenue", "Water Works", "Marvin Gardens", "Go to Jail", "Pacific Avenue",
      "North Carolina Avenue", "Community Chest", "Pennsylvania Avenue", "Short Line",
      "Chance", "Park Place", "Luxury Tax", "Boardwalk"};
  int types[] = {
      0, 1, 2, 1, 4, 5, 1, 3, 1, 1, 6,
      1, 7, 1, 1, 5, 1, 2, 1, 1, 8,
      1, 3, 1, 1, 5, 1, 1, 9, 1, 8,
      1, 1, 2, 1, 5, 3, 1, 4, 1};
  int initialPrices[] = {
      -1, 60, -1, 60, 60, 200, 100, -1, 100, 120, -1,
      140, 150, 140, 160, 200, 180, -1, 180, 200, -1,
      220, -1, 220, 240, 200, 260, 260, 150, 280, -1,
      300, 300, -1, 320, 200, -1, 350, 100, 400};
  int colors[] = {
      0, 1, 0, 1, 0, 0, 2, 0, 2, 2, 0,
      3, 0, 3, 3, 0, 4, 0, 4, 4, 0,
      5, 0, 5, 5, 0, 6, 6, 0, 6, 0,
      7, 7, 0, 7, 0, 0, 8, 0, 8};

  for (int i = 0; i < b.sizeOfBoard; i++)
  {
    strcpy(b.cells[i].title, titles[i]);
    b.cells[i].type = types[i];
    b.cells[i].initialPrice = initialPrices[i];
    b.cells[i].color = colors[i];
  }
}

// Initialize the BOARD
void boardInitialize(struct Board b)
{
  int n = 40;
  b.sizeOfBoard = n;
  //struct Card c;
  // Fill the cards with values
  cardsInitialize(b.card);
  // Fill owners with Null
  ownersInitialize(b);
  // Fill the entire board
  boardFill(b);
  // Fill improvement levels with 0
  // and fill the rent and houses prices
  improvementsInitialize(b);

  //if type = 1 or 2 then the function invoked and the arrays are passed
}