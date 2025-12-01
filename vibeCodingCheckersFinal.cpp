// made by Roman  Pena 11/28/25
// AI used: ChatGPT & Google Gemini



#include <iostream>
#include <vector>
#include <cmath>
#include <cstdlib>


using namespace std;

// We start by defining the fundamental constants of our game. These variables, like `BOARD_SIZE` and the characters for `PLAYER_1` ('X') and `KING_1` ('K'), are crucial symbols that represent the building blocks of our Checkers universe.
// --- Game Constants and Symbols ---
const int BOARD_SIZE = 8;

// Piece definitions
const char EMPTY = ' ';
const char PLAYER_1 = 'X'; // Moves UP (index decreases)
const char PLAYER_2 = 'O'; // Moves DOWN (index increases)
const char KING_1 = 'K';
const char KING_2 = 'Q';

// The CheckersGame class is the master blueprint for our entire application. Think of it as the game manager; it holds the board data, tracks whose turn it is, and centralizes all the rules needed to play the game correctly.
// --- Main Game Class ---
class CheckersGame {
private:
    char board[BOARD_SIZE][BOARD_SIZE];
    int turn; // 1 for Player 1, 2 for Player 2
    int p1Pieces;
    int p2Pieces;

public:
    // The constructor is the first method called when you create a new game object. Its job is simple: make sure the game is immediately ready to go by calling our setup function, `resetGame()`.
    // --- Constructor ---
    CheckersGame() {
        resetGame();
    }

    // This function builds the starting board setup. We use a nested loop to iterate through every square and carefully place Player 2 ('O') pieces in the top three rows and Player 1 ('X') pieces in the bottom three rows, always ensuring they land only on the dark, playable squares.
    // --- Board Initialization ---
    void resetGame() {
        turn = 1;
        p1Pieces = 12;
        p2Pieces = 12;

        // Initialize board
        for (int i = 0; i < BOARD_SIZE; i++) {
            for (int j = 0; j < BOARD_SIZE; j++) {
                // Place pieces on dark squares only
                if ((i + j) % 2 != 0) {
                    if (i < 3)
                        board[i][j] = PLAYER_2; // Top side
                    else if (i > 4)
                        board[i][j] = PLAYER_1; // Bottom side
                    else
                        board[i][j] = EMPTY;
                } else {
                    board[i][j] = EMPTY;
                }
            }
        }
    }

    // The `drawBoard()` function is how we communicate the game state to the player. It renders the 8x8 grid using `cout`, complete with row and column indices for easy coordinate input, so everyone knows exactly where the pieces are.
    // --- Display Board ---
    void drawBoard() {
        cout << "\n  0 1 2 3 4 5 6 7" << endl;
        cout << " +---------------+" << endl;
        for (int i = 0; i < BOARD_SIZE; i++) {
            cout << i << "|";
            for (int j = 0; j < BOARD_SIZE; j++) {
                cout << board[i][j] << "|";
            }
            cout << i << endl;
            cout << " +---------------+" << endl;
        }
        cout << "  0 1 2 3 4 5 6 7" << endl;
        cout << "\nCurrent Turn: Player " << (turn == 1 ? "1 (X)" : "2 (O)") << endl;
    }

    // This small, but essential, helper function acts as a safety guardrail. Before we access the board array, we use `isValidPos` to confirm the player's input coordinates are actually within the `0` to `7` range, preventing the program from crashing.
    // --- Helper: Check Bounds ---
    bool isValidPos(int r, int c) {
        return r >= 0 && r < BOARD_SIZE && c >= 0 && c < BOARD_SIZE;
    }

    // This is the heart of the game! The `movePiece` function runs through a series of logical checks—from verifying the piece belongs to the current player, ensuring the destination is empty, to calculating if it's a valid single step or a capture jump. If all rules are passed, the piece is moved and the turn is switched.
    // --- Move Logic ---
    // Attempt to make a move. Returns true if successful.
    bool movePiece(int r1, int c1, int r2, int c2) {
        // 0. Boundary Check
        if (!isValidPos(r1, c1) || !isValidPos(r2, c2)) {
            cout << "Invalid coordinates!" << endl;
            return false;
        }

        char piece = board[r1][c1];
        char target = board[r2][c2];

        // 1. Check if source belongs to current player
        if (turn == 1 && piece != PLAYER_1 && piece != KING_1) {
            cout << "That's not your piece!" << endl;
            return false;
        }
        if (turn == 2 && piece != PLAYER_2 && piece != KING_2) {
            cout << "That's not your piece!" << endl;
            return false;
        }

        // 2. Check if target is empty
        if (target != EMPTY) {
            cout << "Target square is not empty!" << endl;
            return false;
        }

        // 3. Calculate distance
        int rowDiff = r2 - r1;
        int colDiff = c2 - c1;

        // Basic check: must move diagonally
        if (abs(colDiff) != abs(rowDiff)) {
            cout << "Must move diagonally!" << endl;
            return false;
        }

        // Determine direction allowed
        bool isKing = (piece == KING_1 || piece == KING_2);

        // Player 1 moves UP (-row), Player 2 moves DOWN (+row)
        if (!isKing) {
            if (turn == 1 && rowDiff > 0) {
                cout << "Regular pieces can only move forward!" << endl;
                return false;
            }
            if (turn == 2 && rowDiff < 0) {
                cout << "Regular pieces can only move forward!" << endl;
                return false;
            }
        }

        // 4. Handle Normal Move (1 step)
        if (abs(rowDiff) == 1) {
            // Normal move logic
            board[r2][c2] = piece;
            board[r1][c1] = EMPTY;
            checkKingPromotion(r2, c2);
            toggleTurn();
            return true;
        }

        // 5. Handle Jump Move (2 steps)
        else if (abs(rowDiff) == 2) {
            int midRow = r1 + (rowDiff / 2);
            int midCol = c1 + (colDiff / 2);
            char midPiece = board[midRow][midCol];

            if (midPiece == EMPTY) {
                cout << "Cannot jump over empty space!" << endl;
                return false;
            }

            // Check if jumping over own piece
            if ((turn == 1 && (midPiece == PLAYER_1 || midPiece == KING_1)) ||
                (turn == 2 && (midPiece == PLAYER_2 || midPiece == KING_2))) {
                cout << "Cannot jump over your own piece!" << endl;
                return false;
            }

            // Execute Jump
            board[r2][c2] = piece;
            board[r1][c1] = EMPTY;
            board[midRow][midCol] = EMPTY; // Remove captured piece

            if (turn == 1) p2Pieces--;
            else p1Pieces--;

            checkKingPromotion(r2, c2);
            toggleTurn();
            return true;
        }

        cout << "Invalid move distance!" << endl;
        return false;
    }

    // The `checkKingPromotion` function handles one of Checkers' most exciting rules: 'kinging.' It checks if a piece has successfully reached the far back row (row 0 for 'X' or row 7 for 'O') and instantly upgrades it to a King ('K' or 'Q'), granting it backward movement capabilities.
    // --- King Promotion Logic ---
    void checkKingPromotion(int r, int c) {
        if (turn == 1 && r == 0 && board[r][c] == PLAYER_1) {
            board[r][c] = KING_1;
            cout << "Player 1 promoted to King!" << endl;
        }
        else if (turn == 2 && r == BOARD_SIZE - 1 && board[r][c] == PLAYER_2) {
            board[r][c] = KING_2;
            cout << "Player 2 promoted to King!" << endl;
        }
    }

    // This is our simple turn switch mechanism, ensuring fair play. After a successful move, `toggleTurn()` flips the `turn` variable between 1 and 2, passing control to the next player.
    // --- Turn Management ---
    void toggleTurn() {
        turn = (turn == 1) ? 2 : 1;
    }

    // This function provides our victory condition check. It constantly monitors `p1Pieces` and `p2Pieces`; if one player's count drops to zero, the game ends immediately and declares the opponent the winner.
    // --- Win Condition Check ---
    bool isGameOver() {
        if (p1Pieces == 0) {
            cout << "Player 2 Wins!" << endl;
            return true;
        }
        if (p2Pieces == 0) {
            cout << "Player 1 Wins!" << endl;
            return true;
        }
        return false;
    }

    // The `play()` function is the engine that keeps the game running indefinitely. It loops continuously, first drawing the board, then prompting the player for input, and finally attempting to execute the move until a winner is found.
    // --- Main Game Loop ---
    void play() {
        int r1, c1, r2, c2;
        while (!isGameOver()) {
            drawBoard();
            cout << "Enter move (Row1 Col1 Row2 Col2): ";
            // Simple input validation loop
            if (!(cin >> r1 >> c1 >> r2 >> c2)) {
                cout << "Invalid input. Please enter numbers." << endl;
                cin.clear();
                cin.ignore(10000, '\n');
                continue;
            }

            if (!movePiece(r1, c1, r2, c2)) {
                cout << "Try again." << endl;
            }
        }
    }
};

// Our `main` function is where the C++ program starts execution. It greets the users, explains the basic input format, and then creates the `CheckersGame` object to start the main `play()` loop.
// --- Application Entry Point ---
int main() {
    cout << "Welcome to C++ Checkers!" << endl;
    cout << "Player 1 (X) starts at the bottom and moves UP." << endl;
    cout << "Player 2 (O) starts at the top and moves DOWN." << endl;
    cout << "Enter coordinates as: FromRow FromCol ToRow ToCol" << endl;
    cout << "Example: 5 0 4 1" << endl;

    CheckersGame game;
    game.play();

    return 0;
}