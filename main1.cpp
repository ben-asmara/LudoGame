/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/cppFiles/main.cc to edit this template
 */

/* 
 * File:   main.cpp
 * Author: Ben Amara
 *
 * Created on November 10, 2024, 9:43 AM
 */

#include <iostream>
#include <vector>
#include <string>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <algorithm>

using namespace std;

//Constant Declaration
const int BOARD_ROWS = 15;
const int BOARD_COLS = 15;
const int BOARD_SIZE = 56;

char board[BOARD_ROWS][BOARD_COLS];

//Structure having the piece for each player
struct Piece {
    int position;
    bool in_play;
    bool at_home;
};

//structure that has the players information
struct Player {
    string name;
    bool isComputer;
    Piece pieces[4];
    int homePosition;
    int coins;
    bool savedGame;
};

//Initializing the first state of the board
void initializeBoard() {
    for (int i = 0; i < BOARD_ROWS; ++i) {
        for (int j = 0; j < BOARD_COLS; ++j) {
            board[i][j] = '.';
        }
    }
}

//Displaying the board informations
void displayBoard(const vector<Player>& players) {
    initializeBoard();
    
    for (const auto& player : players) {
        for (int i = 0; i < 4; ++i) {
            const Piece& piece = player.pieces[i];
            if (piece.in_play && !piece.at_home) {
                int row = piece.position / BOARD_COLS;
                int col = piece.position % BOARD_COLS;
                board[row][col] = player.name[0];
            }
        }
    }

    cout << "\n========= Current Board =========\n";
    for (int i = 0; i < BOARD_ROWS; ++i) {
        for (int j = 0; j < BOARD_COLS; ++j) {
            cout << board[i][j] << " ";
        }
        cout << endl;
    }
    cout << "=================================\n";
}

//The Random function that rolls the die
int rollDice() {
    return (rand() % 6) + 1;
}

int choosePiece(const Player& player) {
    cout << player.name << ", here are your pieces:\n";
    for (int i = 0; i < 4; ++i) {
        const Piece& piece = player.pieces[i];
        cout << "Piece " << i + 1 << ": ";
        if (piece.at_home) {
            cout << "Reached home\n";
        } else if (piece.in_play) {
            cout << "Position " << piece.position << "\n";
        } else {
            cout << "At start\n";
        }
    }

    int choice;
    do {
        cout << "Enter the number of the piece you want to move (1-4): ";
        cin >> choice;
    } while (choice < 1 || choice > 4);

    return choice - 1;
}

void movePiece(Player& player, int pieceIndex, int roll, vector<Player>& players) {
    Piece& piece = player.pieces[pieceIndex];

    if (!piece.in_play && roll == 6) {
        piece.in_play = true;
        piece.position = player.homePosition;
        cout << "🎲 " << player.name << " moved piece " << pieceIndex + 1 << " onto the board!\n";
    }
    else if (piece.in_play && !piece.at_home) {
        piece.position = (piece.position + roll) % BOARD_SIZE;
        
        for (auto& opponent : players) {
            if (opponent.name != player.name) {
                for (auto& opponentPiece : opponent.pieces) {
                    if (opponentPiece.in_play && opponentPiece.position == piece.position) {
                        opponentPiece.in_play = false;
                        opponentPiece.position = opponent.homePosition;
                        cout << "💥 " << player.name << " captured a piece from " << opponent.name << "!\n";
                    }
                }
            }
        }

        if (piece.position == (player.homePosition + BOARD_SIZE - 1) % BOARD_SIZE) {
            piece.at_home = true;
            cout << "🏠 " << player.name << "'s piece " << pieceIndex + 1 << " has reached home!\n";
        }
    }
}

bool allPiecesAtStart(const Player& player) {
    for (const auto& piece : player.pieces) {
        if (piece.in_play) {
            return false;
        }
    }
    return true;
}

bool checkWin(const Player& player) {
    for (const auto& piece : player.pieces) {
        if (!piece.at_home) {
            return false;
        }
    }
    cout << "🎉🎉 Congratulations, " << player.name << "! You've won the game! 🎉🎉\n";
    return true;
}

void waitForRoll(const string& playerName) {
    cout << playerName << ", press Enter to roll the dice...";
    cin.ignore();
    cin.get();
}

int computerChoosePiece(const Player& player) {
    for (int i = 0; i < 4; ++i) {
        if (!player.pieces[i].at_home && player.pieces[i].in_play) {
            return i;
        }
    }
    return rand() % 4;
}

void playTurn(Player& player, vector<Player>& players) {
    displayBoard(players);
    cout << "\n🔹🔹🔹 " << player.name << "'s Turn 🔹🔹🔹\n";
    
    if (!player.isComputer) {
        waitForRoll(player.name);
    }

    int roll = rollDice();
    cout << player.name << " rolled a " << roll << " 🎲\n"; // Display the dice roll

    int pieceIndex;

    // Only prompt to choose a piece if they rolled a 6 or have pieces in play
    if (allPiecesAtStart(player) && roll != 6) {
        cout << player.name << " needs a 6 to move a piece out of the start.\n";
    } else {
        if (player.isComputer) {
            pieceIndex = computerChoosePiece(player);
            cout << "🤖 " << player.name << " (computer) chose piece " << pieceIndex + 1 << " to move.\n";
        } else {
            pieceIndex = choosePiece(player);
        }
        movePiece(player, pieceIndex, roll, players);
    }

    displayBoard(players);
    cout << "=================================\n";
}

void displayGameSummary(const vector<Player>& players) {
    cout << "\n🎉🎉 Game Over! Here are the rankings: 🎉🎉\n";
    
    vector<Player> sortedPlayers = players;
    sort(sortedPlayers.begin(), sortedPlayers.end(), [](const Player& a, const Player& b) {
        return a.coins > b.coins;
    });
    
    cout << "\nPlayer Rankings:\n";
    cout << setw(15) << left << "Player Name" << setw(10) << left << "Coins" << setw(10) << left << "Position\n";
    cout << "---------------------------------------------\n";

    for (int i = 0; i < sortedPlayers.size(); ++i) {
        cout << setw(15) << left << sortedPlayers[i].name 
             << setw(10) << left << sortedPlayers[i].coins 
             << setw(10) << left << i + 1 << endl;
    }
}

void displayRules() {
    cout << "\n********** Game Rules **********\n";
    cout << "1. The game is played by 2 to 4 players.\n";
    cout << "2. Each player starts with 4 pieces at the 'Start' position.\n";
    cout << "3. Players roll a dice to move their pieces around the board.\n";
    cout << "4. A roll of 6 allows a player to move a piece out of the start position.\n";
    cout << "5. The objective is to move all 4 pieces to the 'Home' position.\n";
    cout << "6. Players can capture opponent pieces by landing on their position.\n";
    cout << "7. The first player to move all their pieces to 'Home' wins.\n";
    cout << "********************************\n\n";
}


int main() {
    srand(time(0));

    displayRules();
    int numPlayers;
    cout << "Enter the number of players (2-4): ";
    cin >> numPlayers;

    vector<Player> players;
    for (int i = 0; i < numPlayers; ++i) {
        string name;
        cout << "Enter name for Player " << i + 1 << ": ";
        cin >> name;

        bool isComputer = false;
        if (numPlayers == 2 && i == 1) {
            char playAgainstComputer;
            cout << "Do you want to play against the computer? (y/n): ";
            cin >> playAgainstComputer;
            isComputer = (playAgainstComputer == 'y' || playAgainstComputer == 'Y');
        }

        Player player{name, isComputer, {{0, false, false}, {0, false, false}, {0, false, false}, {0, false, false}}, i * 14, 1000, false};
        players.push_back(player);
    }

    int betAmount;
    cout << "Enter the bet amount for all players: ";
    cin >> betAmount;

    for (auto& player : players) {
        player.coins -= betAmount;
    }

    cin.ignore();

    bool gameInProgress = true;
    while (gameInProgress) {
        for (auto& player : players) {
            playTurn(player, players);

            if (checkWin(player)) {
                gameInProgress = false;
                break;
            }
        }

        char saveChoice;
        cout << "Do you want to save the game? (y/n): ";
        cin >> saveChoice;
        if (saveChoice == 'y' || saveChoice == 'Y') {
            ofstream outFile("game_save.txt");
            for (const auto& player : players) {
                outFile << player.name << " " << player.isComputer << " " << player.coins << endl;
                for (const auto& piece : player.pieces) {
                    outFile << piece.position << " " << piece.in_play << " " << piece.at_home << endl;
                }
            }
            cout << "Game saved successfully!\n";
            outFile.close();
        }
    }

    // Update coins based on the winners' rankings
    int totalBet = betAmount * numPlayers;
    if (numPlayers == 2) {
        players[0].coins += totalBet;  // Winner takes all
    } else {
        players[0].coins += totalBet * 0.7;
        players[1].coins += totalBet * 0.5;
        players[2].coins += totalBet * 0.3;
    }

    displayGameSummary(players);

    return 0;
}

