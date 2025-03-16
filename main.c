#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include "db.h"

#define MAX 52
// 0 for no, 1 for yes;
typedef enum
{
    HIT,
    STAND,
    DOUBLE,
    SPLIT,
    INVALID
} Action;

typedef struct
{
    char *rank;
    char suite;
    int value;
} Card;

typedef struct
{
    Card hand[12];
    int *handSize;
    int isBusted;
    int revealCard;
} Dealer;

typedef struct
{
    Card hand[12];
    int total_winnings;
    int *handSize;
    int balance;
    int currentBet;
    int hasSplit;
    int isBusted;

} Player;

int calc(Card hand[], int *handSize);
void showBalance(Player *player);
int placeBet(Player *player);
void shuffle(Card deck[]);
int top = (MAX) * 6;
Card deck[MAX * 6];
Player p1;
Dealer d1;
int p1h = 0;
int d1h = 0;

void push(Card hand[], int *handSize, Card card)
{
    if (!handSize)
    {
        fprintf(stderr, "Error: handSize pointer is NULL\n");
        return;
    }

    if (*handSize < 0 || *handSize >= 12)
    {
        fprintf(stderr, "Error: Invalid hand size %d\n", *handSize);
        return;
    }

    hand[*handSize] = card;
    (*handSize)++;
}

void displayOptions(int canSplit)
{
    printf("Choose your move:\n");
    printf("1. Hit\n");
    printf("2. Stand\n");
    printf("3. Double Down\n");
    if (canSplit)
    {
        printf("4. Split\n");
    }
    printf("Enter your choice: ");
}

Action getPlayerAction(int canSplit)
{
    int choice;
    displayOptions(canSplit);
    if (scanf("%d", &choice) != 1)
    {
        int c;
        while ((c = getchar()) != '\n' && c != EOF)
            ;
        return INVALID;
    }

    switch (choice)
    {
    case 1:
        return HIT;
    case 2:
        return STAND;
    case 3:
        return DOUBLE;
    case 4:
        if (canSplit)
        {
            return SPLIT;
        }
        else
        {
            return INVALID;
        }
    default:
        return INVALID;
    }
}

void deal(Card deck[])
{
    if (top >= 4)
    {
        for (int i = 0; i < 2; i++)
        {
            push(p1.hand, p1.handSize, deck[--top]);
            push(d1.hand, d1.handSize, deck[--top]);
        }
    }
    else
    {
        printf("Error: Not enough cards in deck\n");
    }
}

void buildDeck(Card deck[])
{
    char suites[] = {'H', 'S', 'C', 'D'};
    char *ranks[] = {"A", "2", "3", "4", "5", "6", "7", "8", "9", "10", "J", "Q", "K"};
    int idx = 0;
    for (int t = 0; t < 6; t++)
    {
        for (int i = 0; i < 4; i++)
        {
            for (int j = 0; j < 13; j++)
            {
                deck[idx].suite = suites[i];
                deck[idx].rank = ranks[j];
                if (strcmp(ranks[j], "A") == 0)
                {
                    deck[idx].value = 11;
                }
                else if (strcmp(ranks[j], "K") == 0 || strcmp(ranks[j], "Q") == 0 || strcmp(ranks[j], "J") == 0)
                {
                    deck[idx].value = 10;
                }
                else
                {
                    deck[idx].value = atoi(ranks[j]);
                }
                idx++;
            }
        }
    }
}

void show(Card hand[], int *handSize, int flag)
{
    if (!handSize)
    {
        fprintf(stderr, "Error: handSize pointer is NULL\n");
        return;
    }

    if (*handSize > 12)
    {
        *handSize = 12;
        printf("Warning: Hand size exceeded maximum limit\n");
    }

    for (int i = 0; i < *handSize; i++)
    {
        printf(" .------. ");
    }
    printf("\n");
    for (int i = 0; i < *handSize; i++)
    {
        if (i == 0 && flag == 0)
        {
            printf(" |XXXXXX| ");
        }
        else
        {
            if (strlen(hand[i].rank) == 1)
                printf(" |%s     | ", hand[i].rank);
            else
                printf(" |%s    | ", hand[i].rank);
        }
    }
    printf("\n");

    for (int i = 0; i < *handSize; i++)
    {
        if (i == 0 && flag == 0)
        {
            printf(" |XXXXXX| ");
        }
        else
        {
            printf(" |  %c   | ", hand[i].suite);
        }
    }
    printf("\n");

    for (int i = 0; i < *handSize; i++)
    {
        if (i == 0 && flag == 0)
        {
            printf(" |XXXXXX| ");
        }
        else
        {
            if (strlen(hand[i].rank) == 1)
                printf(" |     %s| ", hand[i].rank);
            else
                printf(" |    %s| ", hand[i].rank);
        }
    }
    printf("\n");

    for (int i = 0; i < *handSize; i++)
    {
        printf(" '------' ");
    }
    printf("\n");

    if (flag == 1)
    {
        printf("Total: %d\n", calc(hand, handSize));
    }
}

void showBalance(Player *player)
{
    if (!player)
    {
        fprintf(stderr, "Error: player pointer is NULL\n");
        return;
    }
    printf("\n===================================\n");
    printf("Current Balance: $%d\n", player->balance);
    printf("===================================\n");
}

int placeBet(Player *player)
{
    if (!player)
    {
        fprintf(stderr, "Error: player pointer is NULL\n");
        return 0;
    }

    int bet;
    do
    {
        printf("Your balance: $%d\n", player->balance);
        printf("Enter your bet amount: $");
        if (scanf("%d", &bet) != 1)
        {
            int c;
            while ((c = getchar()) != '\n' && c != EOF)
                ;
            printf("Invalid input. Please enter a number.\n");
            bet = 0;
            continue;
        }

        if (bet <= 0)
        {
            printf("Invalid bet. Please enter a positive amount.\n");
        }
        else if (bet > player->balance)
        {
            printf("Insufficient funds. Your balance is $%d.\n", player->balance);
        }
    } while (bet <= 0 || bet > player->balance);

    player->currentBet = bet;
    player->balance -= bet;
    printf("Bet placed: $%d\n", bet);

    return bet;
}

int gameLoop(User user, PGconn *conn)
{
    *p1.handSize = 0;
    *d1.handSize = 0;
    p1.isBusted = 0;
    d1.isBusted = 0;
    d1.revealCard = 0;
    p1.hasSplit = 0;
    showBalance(&p1);
    if (p1.balance <= 0)
    {
        printf("You're out of money! Game over.\n");
        return -1;
    }

    placeBet(&p1);

    deal(deck);

    if (top < 52)
    {
        printf("Shuffling deck...\n");
        buildDeck(deck);
        shuffle(deck);
        top = MAX * 6;
    }

    printf("\nDealer's hand:\n");
    show(d1.hand, d1.handSize, 0);

    printf("\nYour hand:\n");
    show(p1.hand, p1.handSize, 1);

    if (calc(p1.hand, p1.handSize) == 21)
    {
        printf("Blackjack! You win!\n");
        p1.balance += p1.currentBet + (int)(p1.currentBet * 1.5);
        p1.total_winnings += (int)(p1.currentBet * 1.5);
        user.balance = p1.balance;
        user.total_winnings = p1.total_winnings;
        if (conn && user.username && strcmp(user.username, "Guest") != 0)
        {
            update_balance(conn, user.username, user.balance);
        }
        return 1;
    }

    while (!p1.isBusted)
    {
        int canSplit = (*(p1.handSize) == 2 && p1.hand[0].value == p1.hand[1].value && !p1.hasSplit && p1.balance >= p1.currentBet);
        int canDouble = (*(p1.handSize) == 2 && p1.balance >= p1.currentBet);

        Action action = getPlayerAction(canSplit);
        if (action == HIT)
        {
            if (top > 0)
            {
                push(p1.hand, p1.handSize, deck[--top]);
                printf("\nYour hand after hit:\n");
                show(p1.hand, p1.handSize, 1);

                if (calc(p1.hand, p1.handSize) > 21)
                {
                    p1.isBusted = 1;
                    printf("Busted! You lose $%d.\n", p1.currentBet);
                    return 0;
                }
            }
            else
            {
                printf("Error: Deck is empty\n");
                return -1;
            }
        }
        else if (action == STAND)
        {
            break;
        }
        else if (action == DOUBLE)
        {
            if (*(p1.handSize) != 2)
            {
                printf("You can only double down on your initial two cards.\n");
                continue;
            }

            if (p1.balance < p1.currentBet)
            {
                printf("Insufficient funds to double down.\n");
                continue;
            }

            p1.balance -= p1.currentBet;
            user.balance = p1.balance;
            if (conn && user.username && strcmp(user.username, "Guest") != 0)
            {
                update_balance(conn, user.username, user.balance);
            }

            p1.currentBet *= 2;
            printf("Bet increased to $%d\n", p1.currentBet);

            if (top > 0)
            {
                push(p1.hand, p1.handSize, deck[--top]);
                printf("\nYour hand after double down:\n");
                show(p1.hand, p1.handSize, 1);

                if (calc(p1.hand, p1.handSize) > 21)
                {
                    p1.isBusted = 1;
                    printf("Busted! You lose $%d.\n", p1.currentBet);
                    return 0;
                }
            }
            else
            {
                printf("Error: Deck is empty\n");
                return -1;
            }
            break;
        }
        else if (action == SPLIT && canSplit)
        {
            if (*(p1.handSize) != 2)
            {
                printf("You can only split on your initial two cards\n");
                continue;
            }

            if (p1.balance < p1.currentBet)
            {
                printf("Insufficient funds to split. You need $%d more.\n",
                       p1.currentBet - p1.balance);
                continue;
            }

            p1.balance -= p1.currentBet;
            user.balance = p1.balance;
            if (conn && user.username && strcmp(user.username, "Guest") != 0)
            {
                update_balance(conn, user.username, user.balance);
            }
            printf("Second bet placed: $%d\n", p1.currentBet);

            int sh1 = 0;
            int sh2 = 0;
            Card split1[12];
            Card split2[12];

            split1[sh1++] = p1.hand[0];

            split2[sh2++] = p1.hand[1];

            split1[sh1++] = deck[--top];
            split2[sh2++] = deck[--top];

            int hand1Busted = 0;
            int hand2Busted = 0;
            int hand1Value = 0;
            int hand2Value = 0;

            printf("\n--- Playing Hand 1 ---\n");
            show(split1, &sh1, 1);
            hand1Value = calc(split1, &sh1);

            while (1)
            {
                if (hand1Value > 21)
                {
                    printf("Hand 1 busted!\n");
                    hand1Busted = 1;
                    break;
                }

                printf("Options for Hand 1: (1) Hit (2) Stand\n");
                int choice;
                scanf("%d", &choice);

                if (choice == 1)
                { // Hit
                    push(split1, &sh1, deck[--top]);
                    show(split1, &sh1, 1);
                    hand1Value = calc(split1, &sh1);
                }
                else if (choice == 2)
                { // Stand
                    break;
                }
                else
                {
                    printf("Invalid choice. Try again.\n");
                }
            }

            printf("\n--- Playing Hand 2 ---\n");
            show(split2, &sh2, 1);
            hand2Value = calc(split2, &sh2);

            while (1)
            {
                if (hand2Value > 21)
                {
                    printf("Hand 2 busted!\n");
                    hand2Busted = 1;
                    break;
                }

                printf("Options for Hand 2: (1) Hit (2) Stand\n");
                int choice;
                scanf("%d", &choice);

                if (choice == 1)
                { // Hit
                    push(split2, &sh2, deck[--top]);
                    show(split2, &sh2, 1);
                    hand2Value = calc(split2, &sh2);
                }
                else if (choice == 2)
                { // Stand
                    break;
                }
                else
                {
                    printf("Invalid choice. Try again.\n");
                }
            }

            printf("\nDealer reveals cards:\n");
            show(d1.hand, d1.handSize, 1);

            while (calc(d1.hand, d1.handSize) < 17)
            {
                printf("\nDealer hits...\n");
                if (top > 0)
                {

                    push(d1.hand, d1.handSize, deck[--top]);
                    show(d1.hand, d1.handSize, 1);
                }
                else
                {
                    printf("Error: Deck is empty\n");
                    return -1;
                }
            }

            int dealerScore = calc(d1.hand, d1.handSize);
            int dealerBusted = (dealerScore > 21);

            printf("\nFinal scores - Dealer: %d\n", dealerScore);
            printf("Hand 1: %d, Hand 2: %d\n", hand1Value, hand2Value);

            if (!hand1Busted)
            {
                if (dealerBusted || hand1Value > dealerScore)
                {
                    printf("Hand 1 wins! You win $%d!\n", p1.currentBet);
                    p1.balance += p1.currentBet * 2;
                    p1.total_winnings += p1.currentBet;
                }
                else if (hand1Value == dealerScore)
                {
                    printf("Hand 1 push! Your $%d bet is returned.\n", p1.currentBet);
                    p1.balance += p1.currentBet;
                }
                else
                {
                    printf("Hand 1 loses. You lose $%d.\n", p1.currentBet);
                }
            }
            else
            {
                printf("Hand 1 busted. You lose $%d.\n", p1.currentBet);
            }

            if (!hand2Busted)
            {
                if (dealerBusted || hand2Value > dealerScore)
                {
                    printf("Hand 2 wins! You win $%d!\n", p1.currentBet);
                    p1.balance += p1.currentBet * 2;
                    p1.total_winnings += p1.currentBet;
                }
                else if (hand2Value == dealerScore)
                {
                    printf("Hand 2 push! Your $%d bet is returned.\n", p1.currentBet);
                    p1.balance += p1.currentBet;
                }
                else
                {
                    printf("Hand 2 loses. You lose $%d.\n", p1.currentBet);
                }
            }
            else
            {
                printf("Hand 2 busted. You lose $%d.\n", p1.currentBet);
            }

            user.balance = p1.balance;
            user.total_winnings = p1.total_winnings;
            if (conn && user.username && strcmp(user.username, "Guest") != 0)
            {
                update_balance(conn, user.username, user.balance);
            }

            p1.hasSplit = 1;

            return 0;
        }
        else
        {
            printf("Invalid choice. Try again.\n");
        }
    }

    if (!p1.isBusted)
    {
        printf("\nDealer reveals cards:\n");
        show(d1.hand, d1.handSize, 1);

        while (calc(d1.hand, d1.handSize) < 17)
        {
            printf("\nDealer hits...\n");
            if (top > 0)
            {
                push(d1.hand, d1.handSize, deck[--top]);
                show(d1.hand, d1.handSize, 1);
            }
            else
            {
                printf("Error: Deck is empty\n");
                return -1;
            }
        }

        int playerScore = calc(p1.hand, p1.handSize);
        int dealerScore = calc(d1.hand, d1.handSize);

        printf("\nFinal scores - You: %d, Dealer: %d\n", playerScore, dealerScore);

        if (dealerScore > 21)
        {
            printf("Dealer busts! You win $%d!\n", p1.currentBet);
            p1.balance += p1.currentBet * 2;
            p1.total_winnings += p1.currentBet;
            user.balance = p1.balance;
            user.total_winnings = p1.total_winnings;
            if (conn && user.username && strcmp(user.username, "Guest") != 0)
            {
                update_balance(conn, user.username, user.balance);
            }
        }
        else if (playerScore > dealerScore)
        {
            printf("You win $%d!\n", p1.currentBet);
            p1.balance += p1.currentBet * 2;
            p1.total_winnings += p1.currentBet;
            user.balance = p1.balance;
            user.total_winnings = p1.total_winnings;
            if (conn && user.username && strcmp(user.username, "Guest") != 0)
            {
                update_balance(conn, user.username, user.balance);
            }
        }
        else if (playerScore == dealerScore)
        {
            printf("Push! It's a draw. Your $%d bet is returned.\n", p1.currentBet);
            p1.balance += p1.currentBet;
            user.balance = p1.balance;
            if (conn && user.username && strcmp(user.username, "Guest") != 0)
            {
                update_balance(conn, user.username, user.balance);
            }
        }
        else
        {
            printf("Dealer wins. You lose $%d.\n", p1.currentBet);
        }
    }

    return 0;
}

void shuffle(Card deck[])
{
    for (int i = (MAX * 6) - 1; i >= 0; i--)
    {
        int j = rand() % (i + 1);
        Card temp = deck[i];
        deck[i] = deck[j];
        deck[j] = temp;
    }
}

int calc(Card hand[], int *handSize)
{
    if (!handSize)
    {
        fprintf(stderr, "Error: handSize pointer is NULL\n");
        return 0;
    }

    int val = 0;
    int aceCount = 0;

    if (*handSize > 12)
    {
        *handSize = 12;
    }

    for (int i = 0; i < *handSize; i++)
    {
        if (strcmp(hand[i].rank, "A") == 0)
        {
            aceCount++;
        }
        val += hand[i].value;
    }

    while (val > 21 && aceCount > 0)
    {
        val -= 10;
        aceCount--;
    }

    return val;
}

User user_options(PGconn *conn)
{
    char c;
    int b = 50000;
    int tw = 0;
    char u[50] = {0};
    char p[20] = {0};
    User user = {0};

    printf("Do you want to register or login ? - you will be able to see your progress and view leaderboards (y/n): ");
    scanf(" %c", &c);

    if (c == 'y' || c == 'Y')
    {
        printf("Enter username and password: ");
        scanf("%49s %19s", u, p);

        if (conn)
        {
            user = authenticate_player(conn, u, p);
            if (user.id == 0)
            {
                printf("User not found. Creating new account...\n");
                if (conn)
                {
                    register_player(conn, u, p, &b, &tw);
                    user = authenticate_player(conn, u, p);
                }
            }
        }
        else
        {
            printf("Database connection not available. Playing as guest.\n");
            strncpy(user.username, "Guest", sizeof(user.username) - 1);
            user.balance = 10000;
            user.total_winnings = 0;
        }

        p1.balance = user.balance;
        p1.total_winnings = user.total_winnings;
    }
    else
    {
        printf("Enjoy the game - Here's $10000 for starting up...\n");
        p1.balance = 10000;
        p1.total_winnings = 0;
        strncpy(user.username, "Guest", sizeof(user.username) - 1);
    }

    return user;
}

void loading_terminal()
{
    printf("********************************************\n");
    printf("*                                          *\n");
    printf("*            WELCOME TO BLACKJACK          *\n");
    printf("*                                          *\n");
    printf("********************************************\n");
    printf("\n");
    printf("Blackjack is a classic card game of strategy and chance, where\n");
    printf("players compete against the dealer to reach a hand value closest\n");
    printf("to 21 without exceeding it. This command-line version offers an\n");
    printf("immersive experience with realistic gameplay mechanics.\n");
    printf("\n");
    printf("Features include user authentication, balance tracking, and a\n");
    printf("leaderboard to showcase top players. All data is securely managed\n");
    printf("using a PostgreSQL database.\n");
    printf("\n");
    printf("Sharpen your strategy, manage your bets, and aim for the top!\n");
    printf("\n");
    printf("********************************************\n");
}

int main()
{
    PGconn *conn = NULL;

    const char *db_uri = getenv("DB_URI");
    if (!db_uri)
    {
        fprintf(stderr, "DB_URI environment variable not set. Playing in offline mode.\n");
    }
    else
    {
        conn = connect_db(db_uri);
    }

    srand(time(NULL));

    loading_terminal();

    User user = user_options(conn);

    if (p1.balance <= 0)
    {
        printf("You have used all your money :( Wait for a while ...\n");
        if (conn)
            close_db(conn);
        return 1;
    }

    p1.handSize = &p1h;
    d1.handSize = &d1h;

    buildDeck(deck);
    shuffle(deck);

    char choice;
    do
    {
        if (conn && user.username && strcmp(user.username, "Guest") != 0)
        {
            check_leaderboards(conn);
        }
        int result = gameLoop(user, conn);
        if (result == -1 || p1.balance <= 0)
        {
            if (p1.balance <= 0)
            {
                printf("You're out of money! Game over.\n");
            }
            break;
        }

        printf("\nDo you want to bet again? (y/n): ");
        scanf(" %c", &choice);

    } while (choice == 'y' || choice == 'Y');

    if (conn && user.username && strcmp(user.username, "Guest") != 0)
    {
        printf("Thank you for playing - your final balance: $%d\n", p1.balance);
    }
    else
    {
        printf("Thank you for playing.\n");
    }

    if (conn && user.username && strlen(user.username) > 0 && strcmp(user.username, "Guest") != 0)
    {
        user.balance = p1.balance;
        user.total_winnings = p1.total_winnings;
        save_user_data(conn, user);
    }

    if (conn)
        close_db(conn);

    return 0;
}
