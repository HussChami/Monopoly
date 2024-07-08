#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <sstream>

using namespace std;

// Struct for a property
struct Property {
    string name;
    int price;
    int rent;
    int owner;
};

// Struct for a player
struct Player {
    int id;
    int money;  // Initialize this field
    int position;
    bool inJail;
    vector<int> properties;
    int getOutOfJailFree;
    bool bankrupt;
};

// Function to print out a player's properties
void printPlayerProperties(const vector<Property>& board, const Player& player) {
    std::cout << "Player " << player.id << "'s properties:" << std::endl;
    for (int i = 0; i < player.properties.size(); i++) {
        std::cout << board[player.properties[i]].name << std::endl;
    }
    std::cout << std::endl;
}

// Function to print out a transaction
void printTransaction(std::ofstream& outfile, const Player& player1, const Player& player2, int amount) {
    outfile << "Player " << player1.id << " paid $" << amount << " to Player " << player2.id << std::endl;
}

// Function to simulate a dice roll
int rollDice() {
    return rand() % 6 + 1;
}

// Function to save the game state
void saveGame(const vector<Player>& players) {
    ofstream saveFile("monopoly_save.txt");
    if (saveFile.is_open()) {
        for (const Player& player : players) {
            // Save basic player info
            saveFile << player.id << " " << player.money << " " << player.position << " "
                << player.inJail << " " << player.getOutOfJailFree << " " << player.bankrupt << " ";

            // Save the number of properties and then the property IDs
            saveFile << player.properties.size() << " ";  // Saving the number of properties
            for (int property : player.properties) {
                saveFile << property << " ";
            }
            saveFile << endl;
        }
        saveFile.close();
        cout << "Game saved successfully." << endl;
    }
    else {
        cout << "Unable to save the game." << endl;
    }
}
//function to loadgame
vector<Player> loadGame(vector<Property>& board) {
    vector<Player> players;
    ifstream loadFile("monopoly_save.txt");
    if (loadFile.is_open()) {
        while (!loadFile.eof()) {
            Player player;
            loadFile >> player.id >> player.money >> player.position
                >> player.inJail >> player.getOutOfJailFree >> player.bankrupt;

            if (loadFile.fail()) {
                break; // Exit loop if reading fails
            }

            int numOfProperties;
            loadFile >> numOfProperties;
            player.properties.clear(); // Clear existing properties

            for (int i = 0; i < numOfProperties; i++) {
                int property;
                if (loadFile >> property) {
                    player.properties.push_back(property);
                }
                else {
                    break; // Exit loop if reading fails
                }
            }

            // Ensure loaded player position is within board bounds
            player.position %= board.size();

            players.push_back(player);
        }
        loadFile.close();

        if (!players.empty()) {
            cout << "Game loaded successfully. Continuing the game." << endl;
        }
        else {
            cout << "No saved game found. Starting a new game." << endl;
        }
    }
    else {
        cout << "Unable to open saved game file. Starting a new game." << endl;
    }
    return players;
}




int main() {
    // Seed the random number generator
    srand(static_cast<unsigned int>(time(nullptr)));

    // Read in the number of players
    int numPlayers;
    cout << "Enter the number of players (2-8): ";
    cin >> numPlayers;

    if (numPlayers < 2 || numPlayers > 8) {
        cout << "Invalid number of players. Exiting." << endl;
        return 1;
    }

    // Ask if the player wants to start a new game or load a saved game
    char startOption;
    cout << "Do you want to start a new game (enter 'n') or load a saved game (enter 'l'): ";
    cin >> startOption;

    vector<Player> players;

    // Declare the board vector
    vector<Property> board;

    if (startOption == 'n' || startOption == 'l') {
        // Initialize the board
        board = {
            {"Go", 0, 0, -1},
            {"Mediterranean Avenue", 60, 204, -1},
            {"Blank", 0, 0, -1},
            {"Baltic Avenue", 60, 208, -1},
            {"Income Tax", 0, 200, -1},
            {"Reading Railroad", 200, 250, -1},
            {"Oriental Avenue", 100, 212, -1},
            {"Blank", 0, 0, -1},
            {"Vermont Avenue", 100, 212, -1},
            {"Connecticut Avenue", 120, 216, -1},
            {"Jail", 0, 0, -1},
            {"St. Charles Place", 140, 220, -1},
            {"Electric Company", 150, 50, -1},
            {"States Avenue", 140, 220, -1},
            {"Virginia Avenue", 160, 224, -1},
            {"Pennsylvania Railroad", 200, 250, -1},
            {"St. James Place", 180, 228, -1},
            {"Blank", 0, 0, -1},
            {"Tennessee Avenue", 180, 228, -1},
            {"New York Avenue", 200, 232, -1},
            {"Free Parking", 0, 0, -1},
            {"Kentucky Avenue", 220, 236, -1},
            {"Blank", 0, 0, -1},
            {"Indiana Avenue", 220, 236, -1},
            {"Illinois Avenue", 240, 240, -1},
            {"B. & O. Railroad", 200, 250, -1},
            {"Atlantic Avenue", 260, 244, -1},
            {"Water Works", 150, 0, -1},
            {"Marvin Gardens", 280, 248, -1},
            {"Go To Jail", 0, 0, -1},
            {"Pacific Avenue", 300, 252, -1},
            {"North Carolina Avenue", 300, 252, -1},
            {"Blank", 0, 0, -1},
            {"Pennsylvania Avenue", 320, 256, -1},
            {"Short Line", 200, 250, -1},
            {"Blank", 0, 0, -1},
            {"Park Place", 350, 270, -1},
            {"Luxury Tax", 0, 200, -1},
            {"Boardwalk", 400, 300, -1},
        };
    }



    // Initialize the players for a new game
    if (startOption == 'n') {
        for (int i = 0; i < numPlayers; i++) {
            std::ostringstream oss;
            oss << "Player " << (i + 1);
            players.push_back({ i + 1, 1500, 0, false, {}, 0, false });  // Initialize bankrupt to false
        }

    }
    else if (startOption == 'l') {
        // Load the game
        players = loadGame(board);

    }

    else {
        std::cout << "Invalid option. Exiting." << endl;
        return 1;
    }

    // Main game loop
    int currentPlayer = 0;
    int numPlayersLeft = numPlayers;
    int numBankruptPlayers = 0;  // Define the variable
    while (numPlayersLeft > 1) {
        // Print out the current player's turn and money
        std::cout << "Player " << players[currentPlayer].id << "'s turn. Money: $" << players[currentPlayer].money << std::endl;

        if (players[currentPlayer].inJail) {
            std::cout << "You are in jail. Pay $50 to get out? (y/n): ";
            char answer;
            cin >> answer;
            if (answer == 'y' && players[currentPlayer].money >= 50) {
                players[currentPlayer].money -= 50; // Deduct the money
                players[currentPlayer].inJail = false; // Release from jail
                std::cout << "You have paid $50 and are now out of jail." << std::endl;
            }
            else {
                std::cout << "You remain in jail." << std::endl;
                currentPlayer = (currentPlayer + 1) % numPlayers; // Move to the next player
                continue; // Skip the rest of the turn
            }
        }


        // Roll the dice
        int roll1 = rollDice();
        int roll2 = rollDice();
        int roll = roll1 + roll2;
        cout << "Rolled a " << roll << "." << endl;

        // Move the player
        players[currentPlayer].position += roll;
        if (players[currentPlayer].position >= board.size()) {
            players[currentPlayer].position -= board.size();
            cout << "Passed Go. Collect $200." << endl;
            players[currentPlayer].money += 200;
        }

        // Debug output for checking player position
        cout << "New position: " << players[currentPlayer].position << endl;
        if (players[currentPlayer].position >= board.size()) {
            cout << "Error: Player position is out of board bounds!" << endl;
            return 1; // Exit the program with an error
        }

        cout << "Landed on " << board[players[currentPlayer].position].name << "." << endl;
        // Check if the player lands on Income Tax or Luxury Tax
        string landedSpace = board[players[currentPlayer].position].name;
        if (landedSpace == "Income Tax" || landedSpace == "Luxury Tax") {
            cout << landedSpace << ": Pay $200." << endl;
            players[currentPlayer].money -= 200;

            // Check for player bankruptcy after paying tax
            if (players[currentPlayer].money < 0) {
                cout << "Player " << players[currentPlayer].id << " is bankrupt." << endl;
                players[currentPlayer].bankrupt = true;
                numBankruptPlayers++;
                numPlayersLeft--;
            }
        }
        // Handle landing on a property
        if (board[players[currentPlayer].position].owner == -1) {
            // Exclude "Blank", "Go to Jail", "Go", "Free Parking", "Income Tax", "Luxury Tax", and "Jail"
            if (landedSpace != "Blank" && landedSpace != "Go To Jail" && landedSpace != "Go" &&
                landedSpace != "Free Parking" && landedSpace != "Income Tax" &&
                landedSpace != "Luxury Tax" && landedSpace != "Jail") {
                // Property is unowned, so the player can buy it
                if (players[currentPlayer].money >= board[players[currentPlayer].position].price) {
                    std::cout << "Buy " << landedSpace << " for $" << board[players[currentPlayer].position].price << "?" << std::endl;
                    char answer;
                    do {
                        std::cout << "(y/n): ";
                        std::cin >> answer;
                    } while (answer != 'y' && answer != 'n');
                    if (answer == 'y') {
                        players[currentPlayer].money -= board[players[currentPlayer].position].price;
                        board[players[currentPlayer].position].owner = currentPlayer;
                        players[currentPlayer].properties.push_back(players[currentPlayer].position);
                        std::cout << "Bought " << landedSpace << "." << std::endl;
                    }
                }
                else {
                    std::cout << "Cannot buy " << board[players[currentPlayer].position].name
                        << " because you do not have enough money." << std::endl;
                }
            }
        }
        else if (board[players[currentPlayer].position].owner != currentPlayer) {
            // Property is owned by another player, so the player pays rent
            int rent = board[players[currentPlayer].position].rent;
            std::cout << "Rent: $" << rent << " to Player " << board[players[currentPlayer].position].owner << std::endl;
            players[currentPlayer].money -= rent;
            players[board[players[currentPlayer].position].owner].money += rent;
        }



        // Handle landing on Go To Jail
        if (board[players[currentPlayer].position].name == "Go To Jail") {
            cout << "Go to jail. Do not pass Go, do not collect $200." << endl;
            players[currentPlayer].position = 10;
            players[currentPlayer].inJail = true;
        }

        // Handle player bankruptcy
        if (players[currentPlayer].money <= 0) {
            cout << "Player " << players[currentPlayer].id << " has gone bankrupt and is out of the game." << endl;
            players[currentPlayer].bankrupt = true;
            numBankruptPlayers++;  // Increment the number of bankrupt players
            numPlayersLeft--;      // Decrement the number of active players
        }

        // Move to the next player
        currentPlayer = (currentPlayer + 1) % numPlayers;

        // Check for game end
        if (numPlayersLeft - numBankruptPlayers == 1) {
            break;
        }
        char saveOption;
        cout << "Do you want to save the game (enter 's') or continue (enter 'c'): ";
        cin >> saveOption;

        if (saveOption == 's') {
            saveGame(players);
            cout << "Game saved successfully. Exiting the game." << endl;
            return 0; // End the game after saving
        }
        else if (saveOption != 'c') {
            cout << "Invalid option. Continuing the game." << endl;
        }


    }

    // Print out the final results
    std::cout << std::endl << "Game over!" << std::endl;
    int winner = -1;
    int highestMoney = -1;
    for (int i = 0; i < numPlayers; i++) {
        if (!players[i].bankrupt) {
            if (winner == -1 || players[i].money > players[winner].money) {
                winner = i;
            }
        }
    }


    if (winner == -1) {
        std::cout << "No one wins. Everyone went bankrupt." << std::endl;
    }
    else {
        std::cout << "Player " << players[winner].id << " wins with $" << players[winner].money << "!" << std::endl;
    }

    return 0;
}