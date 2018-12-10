#pragma region headers
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include "board.h"

#include <sys/socket.h>
#include <arpa/inet.h> //inet_addr
#include <unistd.h>		 //write
#include <pthread.h>	 //for threading , link with lpthread
#include <unistd.h>		 // foe sleep

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <netinet/in.h>

#include <stddef.h>

#pragma endregion

#define NULL ((void *)0)
#define PORT 8888

#pragma region Encription Function declarations
// handler
char *putPlayerIntoBuffer(char *buffer, struct Player);
void getPlayerFromBuffer(char *buffer, struct Player);
char *putBoardCellIntoBuffer(char *buffer, struct BoardCell);
void getBoardCellFromBuffer(char *buffer, struct BoardCell c);
char *putDiceIntoBuffer(char *buffer, struct Dice d);
void getDiceFromBuffer(char *buffer, struct Dice d);
#pragma endregion

#pragma region Sockets + Multithreading
void *read_handler(void *);
void *write_handler(void *);
int clientSocket();
void *sequence(void *);
void getSequence(char *buffer, char **sequence);
//multithreading
void *connection_handler(void *);
#pragma endregion

#pragma region Game Function declarations
//game
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
void subtractMoneyInCell(struct Player p, struct BoardCell cell);
void subtractMoney(struct Player p, int money);
void electric(struct Board b, struct Player p);
void waterWorks(struct Board b, struct Player p);
void giveMoney(struct Player p, int money);
int cardRandom(struct Card c);
void makeCardAction(int comOrChance, int randNumber, struct Player p, struct Board b);
void communityChest(struct Player p, struct Card c, struct Board b);
void chance(struct Player p, struct Card c, struct Board b);
void sendDice(struct Dice d);
void buttonDisable();
void cardsInitialize(struct Card c);
void ownersInitialize(struct Board b);
void improvementsInitialize(struct Board b);
void boardFill(struct Board b);
void initializeCommunityChest(struct Board, int, int);

#pragma endregion

int curDesc;
int server_fd, client_fds[4];
struct Player p;
struct Board b;
struct Dice c;

int main()
{
	char buffer[255] = "Anvar/is/here/people";
	buffer[254] = '\0';
	char *buf[4];
	getSequence(buffer, buf);
	return 0;
}

#pragma region Game

// ------------------------ GAME --------------------
int game()
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
	subtractMoney(p, b.cells[p.position].initialPrice);
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

void goToJail(struct Board b, struct Player p)
{
	p.inJail = true;
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
	//moveAnimation(p, jailPosition);
}

void movePlayer(struct Player p, int targetPosition)
{

	p.position = targetPosition % 40;
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
			subtractMoneyInCell(p, cell);
			break;
		case 2:
			communityChest(p, b.card, b);
			break;
		case 3:
			chance(p, b.card, b);
			break;
		case 4:
			subtractMoneyInCell(p, cell);
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

void subtractMoneyInCell(struct Player p, struct BoardCell cell)
{
	p.money -= cell.impr.rent;
}

void subtractMoney(struct Player p, int money)
{
	p.money -= money;
}

void communityChest(struct Player p, struct Card c, struct Board b)
{
	makeCardAction(0, cardRandom(c), p, b);
}
void chance(struct Player p, struct Card c, struct Board b)
{
	makeCardAction(1, cardRandom(c), p, b);
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
void makeCardAction(int comOrChance, int randNumber, struct Player p, struct Board b)
{
	if (!comOrChance)
	{
		//Community Chest
		switch (randNumber)
		{
		case 0:
			movePlayer(p, 0);	//Advance to 'Go'.
			giveMoney(p, 200); //Collect $200
			break;
		case 1:
			subtractMoney(p, 50); //Pay $50.
			break;
		case 2:
			giveMoney(p, 50);
			break;
		case 3:
			p.hasJailFreeCard = true;
			break;
		case 4:
			goToJail(b, p);
			break;
		case 5:
			giveMoney(p, 50); // collect from each player
			break;
		case 6:
			giveMoney(p, 100);
			break;
		case 7:
			giveMoney(p, 20);
			break;
		case 8:
			giveMoney(p, 10);
			break;
		case 9:
			giveMoney(p, 100);
			break;
		case 10:
			subtractMoney(p, 50);
			break;
		case 11:
			subtractMoney(p, 50);
			break;
		case 12:
			giveMoney(p, 100);
			break;
		case 13:
			int sum = 0;
			for (int i = 0; i < b.sizeOfBoard; i++)
			{
				if ((b.cells[i].owner.id == p.id) && (b.cells[i].type == 1))
				{
					sum += b.cells[i].impr.numOfHouses * 40 + 1 * 115; //Pay $40 per house and $115 per hotel you own
				}
			}
			subtractMoney(p, sum);
			break;
		case 14:
			giveMoney(p, 10);
			break;
		case 15:
			giveMoney(p, 100);
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
			movePlayer(p, 0);
			giveMoney(p, 200);
			break;
		case 1:
			int go_to;
			for (int i = 0; i < b.sizeOfBoard; i++)
			{
				if (b.cells[i].title == "St. Charles Place")
				{
					go_to = i;
					movePlayer(p, i);
					if (go_to - p.position < 0 || go_to - p.position > 40)
						giveMoney(p, 200);
					break;
				}
			}
			break;
		case 2:
			int go_to;
			for (int i = 0; i < b.sizeOfBoard; i++)
			{
				if (b.cells[i].title == "Illinois Avenue")
				{
					go_to = i;
					movePlayer(p, i);
					if (go_to - p.position < 0 || go_to - p.position > 40)
						giveMoney(p, 200);
					break;
				}
			}
			break;
		case 3: //Bank pays you dividend of $50.
			giveMoney(p, 50);
			break;
		case 4: //Get out of Jail Free. This card may be kept until needed.

			break;

		case 5: //Go Back Three {3} Spaces.
			movePlayer(p, -3);
			break;
		case 6: //Go to Jail. Go directly to Jail. Do not pass GO, do not collect $200.
			movePlayer(p, 11);
			break;
		case 7: //"Make general repairs on all your property: For each house pay $25, For each hotel {pay} $100.
			int sum = 0;
			for (int i = 0; i < b.sizeOfBoard; i++)
			{
				if (b.cells[i].owner.id == p.id)
				{
					if (b.cells[i].impr.numOfHouses != 0)
						sum += ((b.cells[i].impr.numOfHouses) * 25);
					else if (b.cells[i].impr.hotelExists == true)
						sum += 100;
				}
			}
			subtractMoney(p, sum);
			break;
		case 8: //Pay poor tax of $15
			subtractMoney(p, 15);
			break;
		case 9: //Take a trip to Reading Railroad.If you pass Go, collect $200.
			bool out_of_bound = true;
			bool pass_start = false;
			for (int i = p.position; i < b.sizeOfBoard; i++)
			{
				if (b.cells[i].title == "Start")
				{
					pass_start = true;
				}
				if (b.cells[i].title == "Reading Railroads")
				{
					movePlayer(p, i);
					out_of_bound = false;
					if (pass_start == true)
						giveMoney(p, 200);
				}
			}
			if (out_of_bound == true)
			{
				for (int i = 0; i < b.sizeOfBoard; i++)
				{
					if (b.cells[i].title == "Reading Railroads")
					{
						movePlayer(p, i);
						giveMoney(p, 200);
						out_of_bound = false;
					}
				}
			}
			break;
		case 10: //Take a walk on the Boardwalk. Advance token to Boardwalk. {Board Walk in both sentences}
			bool out_of_bound = true;
			for (int i = p.position; i < b.sizeOfBoard; i++)
			{
				if (b.cells[i].title == "Boardwalk")
				{
					movePlayer(p, i);
					out_of_bound = false;
				}
			}
			if (out_of_bound == true)
			{
				for (int i = 0; i < b.sizeOfBoard; i++)
				{
					if (b.cells[i].title == "Boardwalk")
						movePlayer(p, i);
				}
			}
			break;

		case 11: //Your building {and} loan matures. Receive {Collect} $150.
			giveMoney(p, 150);
			break;
		case 12: //You have won a crossword competition. Collect $100
			giveMoney(p, 100);
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
	char buffer[255];
	putDiceIntoBuffer(buffer, d);
	send(server_fd, buffer, sizeof(buffer));
}
void buttonDisable()
{
	//disable the Button
}

//Initialize Cards
void cardsInitialize(struct Card c)
{
	char *comChest[] = {
			"Advance to 'Go'. Collect $200",
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
	char *chance[] = {
			"Advance to 'Go'. Collect $200",
			"Advance to St. Charles Place. If you pass Go, collect $200.",
			"Advance to Illinois Ave. If you pass Go, collect $200."
			"Bank pays you dividend of $50.",
			"Get out of Jail Free. This card may be kept until needed.",
			"Go Back Three {3} Spaces.",
			"Go to Jail. Go directly to Jail. Do not pass GO, do not collect $200.",
			"Make general repairs on all your property: For each house pay $25, For each hotel {pay} $100.",
			"Pay poor tax of $15 ",
			"Take a trip to Reading Railroad. {Take a ride on the Reading. Advance token and} If you pass Go, collect $200.",
			"Take a walk on the Boardwalk. Advance token to Boardwalk. {Board Walk in both sentences} ",
			"Your building {and} loan matures. Receive {Collect} $150. ",
			"You have won a crossword competition. Collect $100.",
	};

	//needed to be edited
	for (int i = 0; i < 20; i++)
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
	b.cells[0].owner.jailCounter = 0;
	b.cells[0].owner.hasJailFreeCard = 0;
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

#pragma endregion

#pragma region Socket + Client
// ----------- Socket + Client -------------
int clientSocket()
{
	struct sockaddr_in address;
	int sock = 0, valread;
	struct sockaddr_in serv_addr;
	char *hello = "Hello from client";
	char buffer[1024] = {0}, cchat[1024];
	char *bye = "bye";

	printf("CREATING CLIENT SOCKET .....\n");
	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		printf("\n Socket creation error \n");
		return -1;
	}

	printf("DEFINING CLIENT SOCKET FAMILY, ADDRESS & PORT .....\n");
	memset(&serv_addr, '0', sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(PORT);
	// Convert IPv4 and IPv6 addresses from text to binary form
	if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0)
	{
		printf("\nInvalid address/ Address not supported \n");
		return -1;
	}

	printf("CLIENT CONNECTING ON PORT 8888 TO COMMUNICATE WITH SERVER.....\n");
	if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
	{
		printf("\nConnection Failed \n");
		return -1;
	}

	pthread_t readThread;
	int *sock_arg;
	sock_arg = (int *)malloc(sizeof(int));
	*sock_arg = sock;
	if (pthread_create(&readThread, NULL, read_handler, sock_arg) < 0)
	{
		perror("could not create thread");
		return 1;
	}

	pthread_t writeThread;
	if (pthread_create(&writeThread, NULL, write_handler, sock_arg) < 0)
	{
		perror("could not create thread");
		return 1;
	}

	pthread_join(readThread, NULL);
	pthread_join(writeThread, NULL);
	return 0;
}

void *read_handler(void *sock_desc)
{
	int sock = *(int *)sock_desc;
	int valread;
	char buffer[1024] = {0};
	char *bye = "Bye";

	puts("In read Handler: \n");

	while (1)
	{
		memset(buffer, 0, sizeof(buffer));
		valread = read(sock, buffer, 1024);
		printf("FRIEND: %s\n", buffer);
		if (strncmp(buffer, bye, strlen(bye)) == 0)
			break;
	}
}

void *write_handler(void *sock_desc)
{
	int sock = *(int *)sock_desc;
	char buffer[1024] = {0};
	char *bye = "Bye";

	puts("In write Handler: \n");

	while (1)
	{
		memset(buffer, 0, sizeof(buffer));
		printf("CLIENT : ");
		fgets(buffer, sizeof(buffer), stdin);
		send(sock, buffer, strlen(buffer), 0);
		buffer[strlen(buffer)] = '\0';
		if (strncmp(buffer, bye, strlen(bye)) == 0)
			break;
	}
}
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
#pragma endregion

#pragma region Encription

void getSequence(char *buffer, char **sequence)
{
	int i = 0;
	int n = 4;
	char *string = buffer;
	char *running, *found[4];
	running = strdup(string);
	printf("running = %s", running);
	while ((sequence[i] = strsep(&running, "/")) != NULL)
	{
		i++;
	}
	printf("\n");

	for (int i = 0; i < 4; i++)
		printf("Sequence[%d] = %s\n", i, sequence[i]);
}

int structHandler()
{
	struct Player p;

	char buffer[256] = {0};
	buffer[255] = '\0';
	p.id = 1;
	p.name = "Anvar Abdulsatarov";
	p.color = 3;
	p.money = 200000;
	p.position = 8;
	p.noMoney = false;
	p.needMoney = true;
	p.inJail = false;

	printf("\n\nputPlayerIntoBuffer = %s\n\n", putPlayerIntoBuffer(buffer, p));
	getPlayerFromBuffer(buffer, p);

	char boardCellBuffer[256] = {0};
	struct Board b;
	struct BoardCell c;
	c = b.cells[0];
	struct Player pc;
	pc = c.owner;
	struct Improvement impc;
	impc = c.impr;
	char *string = "string";
	//strcpy(b.cells[0].title, string);
	c.title = strdup(string);
	c.initialPrice = 200;
	c.color = 2;
	c.type = 1;
	c.owner.id = 1;
	c.owner.name = "Anvar Abdulsatarov";
	c.owner.color = 3;
	c.owner.money = 100;
	c.owner.position = 12;
	c.owner.noMoney = 0;
	c.owner.needMoney = 1;
	c.owner.inJail = 1;
	c.impr.rent = 200;
	c.impr.numOfHouses = 3;
	c.impr.pricesOfHouses = 100;
	c.impr.hotelExists = 1;
	c.impr.priceOfHotel = 150;
	c.impr.playerPrice = 400;

	printf("\nPutBoardCellIntoBuffer = %s\n", putBoardCellIntoBuffer(boardCellBuffer, c));
	getBoardCellFromBuffer(boardCellBuffer, c);

	struct Dice d;
	char diceBuffer[256] = {0};
	d.num1 = 3;
	d.num2 = 4;
	d.counter = 1;

	printf("\nPutDiceIntoBuffer = %s\n", putDiceIntoBuffer(diceBuffer, d));
	getDiceFromBuffer(diceBuffer, d);
	return 0;
}

char *putDiceIntoBuffer(char *buffer, struct Dice d)
{
	sprintf(buffer, "%d/%d/%d", d.num1, d.num2, d.counter);
	printf("\n\n%s\n\n", buffer);
	return buffer;
}
void getDiceFromBuffer(char *buffer, struct Dice d)
{
	int i = 0;
	int n = 3;
	char *string = buffer;
	char *running, *found[4];
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
	d.num1 = atoi(found[0]);
	d.num2 = atoi(found[1]);
	d.counter = atoi(found[2]);
}
char *putPlayerIntoBuffer(char *buffer, struct Player p)
{

	sprintf(buffer, "%d/%s/%d/%d/%d/%d/%d/%d",
					p.id, p.name, p.color, p.money, p.position, p.noMoney,
					p.needMoney, p.inJail);
	printf("\n\n%s\n", buffer);
	return buffer;
}

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