#pragma region headers
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include "board.h"
#include "main.c"

#include <sys/socket.h>
#include <arpa/inet.h> //inet_addr
#include <unistd.h>    //write
#include <pthread.h>   //for threading , link with lpthread
#include <unistd.h>    // foe sleep

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <netinet/in.h>

#include <stddef.h>

#pragma endregion

#define NULL ((void *)0)
#define PORT 8888

int main()
{

  return 0;
}

void actionToMake(int action)
{
  switch (action)
  {
  case 0:
    //send the sequence
    //array
    break;
  case 1:
    //generate dice
    //distribute dice
    break;
  case 2:
    //distribute player
    //and board
    break;
  case 3:
    //distribute chat
    break;
  case 4:
    //distribute history
    break;
  default:
    break;
  }
}

int Randomize(struct Dice d, struct Board b, struct Player p)
{
  int move;
  buttonDisable();
  /* Intializes random number generator */
  d.num1 = 1 + rand() % 6; //5
  d.num2 = 1 + rand() % 6; //6
  if (d.num1 == d.num2)
  {
    if (p.inJail)
    {
      p.inJail = false;
      p.jailCounter = 0;
    }
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

  // if the player is in jail, then he just skips his turn
  // and jailCounter++
  if (p.inJail)
  {
    p.jailCounter++;
  }
  else
  {
    move = p.position + d.num1 + d.num2;
    movePlayer(p, move);
  }
  sendDice(d);
  return d.counter;
}

#pragma region STRINGS
//receives the sequence in which the players must go
//invoked only once at the beginning
void *sequence(void *sock_desc)
{
  char *sequence[4];

  char *bye = "Bye";
  int sock = *(int *)sock_desc;
  int valread;
  char buffer[1024] = {0};
  char buffer_num[1024] = {0};
  while (1)
  {
    memset(buffer, 0, sizeof(buffer));
    valread = read(sock, buffer, 1024);
    printf("FRIEND: %s\n", buffer);
    if (strncmp(buffer, bye, strlen(bye)) == 0)
      break;
  }
  sprintf(buffer_num, "%d", valread);
  getSequence(buffer_num, sequence);
}

//string of dice
char *putDiceIntoBuffer(char *buffer, struct Dice d)
{
  sprintf(buffer, "%d/%d/%d", d.num1, d.num2, d.counter);
  printf("\n\n%s\n\n", buffer);
  return buffer;
}

//string of player
char *putPlayerIntoBuffer(char *buffer, struct Player p)
{

  sprintf(buffer, "%d/%s/%d/%d/%d/%d/%d/%d",
          p.id, p.name, p.color, p.money, p.position, p.noMoney,
          p.needMoney, p.inJail);
  printf("\n\n%s\n", buffer);
  return buffer;
}

//resolve Player
void getPlayerFromBuffer(char *buffer, struct Player p)
{
  int i = 0;
  int n = 8;
  char *string = buffer;
  char *running, *found[8];
  running = strdup(string);
  printf("running = %s", running);
  while ((found[i] = strsep(&running, "/")) != NULL)
  {
    i++;
  }
  printf("\n");
  for (int i = 0; i < n; i++)
  {
    printf("found[%d] = %s\n", i, found[i]);
  }

  p.id = atoi(found[0]);
  p.name = found[1];
  p.color = atoi(found[2]);
  p.money = atoi(found[3]);
  p.position = atoi(found[4]);
  p.noMoney = atoi(found[5]);
  p.needMoney = atoi(found[6]);
  p.inJail = atoi(found[7]);
}

//string of boardcell
//CELL ONLY!
char *putBoardCellIntoBuffer(char *buffer, struct BoardCell c)
{
  // Maybe just to make changes in owner, we need to pass a string with number of cell
  //and the changes in struct Player
  //same with improvements
  sprintf(buffer, "%s/%d/%d/%d/%d/%s/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d/%d",
          c.title, c.initialPrice, c.color, c.type, c.owner.id, c.owner.name, c.owner.color,
          c.owner.money, c.owner.position, c.owner.noMoney, c.owner.needMoney, c.owner.inJail,
          c.impr.rent, c.impr.numOfHouses, c.impr.pricesOfHouses, c.impr.hotelExists,
          c.impr.priceOfHotel, c.impr.playerPrice);
  return buffer;
}

//resolve boardCELL
void getBoardCellFromBuffer(char *buffer, struct BoardCell c)
{
  int i = 0;
  int n = 18;
  char *string = buffer;
  char *running, *found[18];
  running = strdup(string);
  printf("running = %s", running);
  while ((found[i] = strsep(&running, "/")) != NULL)
  {
    i++;
  }
  printf("\n");
  for (int i = 0; i < n; i++)
  {
    printf("found[%d] = %s\n", i, found[i]);
  }

  c.title = found[0];
  c.initialPrice = atoi(found[1]);
  c.color = atoi(found[2]);
  c.type = atoi(found[3]);
  c.owner.id = atoi(found[4]);
  c.owner.name = found[5];
  c.owner.color = atoi(found[6]);
  c.owner.money = atoi(found[7]);
  c.owner.position = atoi(found[8]);
  c.owner.noMoney = atoi(found[9]);
  c.owner.needMoney = atoi(found[10]);
  c.owner.inJail = atoi(found[11]);
  c.impr.rent = atoi(found[12]);
  c.impr.numOfHouses = atoi(found[13]);
  c.impr.pricesOfHouses = atoi(found[14]);
  c.impr.hotelExists = atoi(found[15]);
  c.impr.priceOfHotel = atoi(found[16]);
  c.impr.playerPrice = atoi(found[17]);
}

#pragma endregion