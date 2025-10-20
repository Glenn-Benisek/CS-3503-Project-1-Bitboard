// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//
//      Given checker pieces never exists on white squares, this assignment
//      sections the uint64_t int Bitboard nto bits (0-31, 32-63)
//
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

typedef uint64_t Bitboard;

// Basic bit operations
void SetBit(Bitboard *board, int pos) {
    *board |= (1ULL << pos);
}

void ClearBit(Bitboard *board, int pos) {
    *board &= ~(1ULL << pos);
}

int GetBit(Bitboard board, int pos) {
    return (board >> pos) & 1;
}

int CountBits(Bitboard board) {
    return __builtin_popcountll(board);
}


// Display functions
void PrintBinary(Bitboard board) {      // Show in binary
    printf("Binary: ");
    for (int i = 63; i >= 0; i--) {
        printf("%d", GetBit(board, i));
        if (i % 8 == 0) printf(" ");
    }
    printf("\n");
}

void PrintHex(Bitboard board) {         // Show in hex
    printf("Hex: 0x%016llX\n", board);
}


// Implementations
typedef struct {
    Bitboard pieces;
    Bitboard kings;
    int current_turn;
} GameState;

typedef struct {                // Used to calculate the shifts
    int from_row, from_col;
    int to_row, to_col;
    int capture_row, capture_col;
} Move;

int board_to_bitpos(int row, int col) {     // shifts map coordinates
    int bit_row = row;
    int bit_pos = bit_row * 4 + (col / 2);
    return bit_pos;
}

void InitializeGame(GameState *game) {
    game->pieces = 0;
    game->kings = 0;
    game->current_turn = 0;
    
    for (int row = 5; row < 8; row++) {         // In rows 6-8 (1-3) set every other, first 32 bits of 64 int
        for (int col = 0; col < 8; col++) {
            if ((row + col) % 2 == 1) {
                int bit_pos = board_to_bitpos(row, col);
                SetBit(&game->pieces, bit_pos); 
            }
        }
    }
    
    for (int row = 0; row < 3; row++) {         // In rows 0-2 (6-8) set every other, first 32 bits of 64 int
        for (int col = 0; col < 8; col++) {
            if ((row + col) % 2 == 1) {
                int bit_pos = board_to_bitpos(row, col);
                SetBit(&game->pieces, bit_pos + 32);
            }
        }
    }
}

void PrintBoard(GameState *game) {
    printf("\n    a   b   c   d   e   f   g   h\n");
    printf("  ┏━━━┳━━━┳━━━┳━━━┳━━━┳━━━┳━━━┳━━━┓\n");
    
    for (int row = 0; row < 8; row++) {
        printf("%d ┃", 8 - row);
        for (int col = 0; col < 8; col++) {
            int bit_pos = board_to_bitpos(row, col);            // Converts coords to bit pos
            int is_red = GetBit(game->pieces, bit_pos);
            int is_black = GetBit(game->pieces, bit_pos + 32);      // Offset for condensed 64 bit
            int is_king = GetBit(game->kings, bit_pos) || GetBit(game->kings, bit_pos + 32);
            
            char piece = ' ';
            if (is_red) {                               // Most effecient labeling
                piece = is_king ? 'R' : 'r';           
            } else if (is_black) {
                piece = is_king ? 'B' : 'b';
            }
            
            if ((row + col) % 2 == 0) {                 // Print the board grid
                printf("▓▓▓┃");
            } else {
                if (piece == ' ') {
                    printf("   ┃");
                } else {
                    printf(" %c ┃", piece);
                }
            }
        }
        printf(" %d\n", 8 - row);
        
        if (row < 7) {
            printf("  ┣━━━╋━━━╋━━━╋━━━╋━━━╋━━━╋━━━╋━━━┫\n");
        }
    }
    printf("  ┗━━━┻━━━┻━━━┻━━━┻━━━┻━━━┻━━━┻━━━┛\n");
    printf("    a   b   c   d   e   f   g   h\n");
}

int IsValidPosition(int row, int col) {         // Checks the row and column for playable black square
    return row >= 0 && row < 8 && col >= 0 && col < 8 && (row + col) % 2 == 1;
}

int GetPieceAt(GameState *game, int row, int col) {     // Returns 0 (red), 1 (black), empty for row column
    if (!IsValidPosition(row, col)) return -1;
    
    int bit_pos = board_to_bitpos(row, col);
    if (GetBit(game->pieces, bit_pos)) return 0;
    if (GetBit(game->pieces, bit_pos + 32)) return 1;
    return -1;
}

int IsKing(GameState *game, int row, int col) {         // Checks the kings board for a king
    if (!IsValidPosition(row, col)) return 0;
    
    int bit_pos = board_to_bitpos(row, col);
    return GetBit(game->kings, bit_pos) || GetBit(game->kings, bit_pos + 32);
}

int CanPieceCapture(GameState *game, int row, int col) {    // Can the given piece jump over opposite piece
    int player = GetPieceAt(game, row, col);
    if (player == -1) return 0;     // End if there is no piece
    
    int is_king = IsKing(game, row, col);
    int directions[4][2] = {{-1, -1}, {-1, 1}, {1, -1}, {1, 1}};        // Kings move in all corners
    
    for (int i = 0; i < 4; i++) {
        int drow = directions[i][0];
        int dcol = directions[i][1];
        
        if (!is_king) {         // Move orientation of colors
            if (player == 0 && drow >= 0) continue;
            if (player == 1 && drow <= 0) continue;
        }
        
        int jump_row = row + drow;
        int jump_col = col + dcol;
        int land_row = row + 2 * drow;
        int land_col = col + 2 * dcol;
        
        if (IsValidPosition(jump_row, jump_col) && IsValidPosition(land_row, land_col)) {
            int jump_player = GetPieceAt(game, jump_row, jump_col);
            int land_player = GetPieceAt(game, land_row, land_col);
            
            if (jump_player == 1 - player && land_player == -1) {       // Jump must have opponent and empty land
                return 1;
            }
        }
    }
    return 0;
}

int HasForcedCapture(GameState *game, int player) {
    for (int row = 0; row < 8; row++) {
        for (int col = 0; col < 8; col++) {
            if (GetPieceAt(game, row, col) == player) {
                if (CanPieceCapture(game, row, col)) {
                    return 1;
                }
            }
        }
    }
    return 0;
}

void GetPossibleMoves(GameState *game, int row, int col, Move moves[], int *move_count, int must_capture) {
    *move_count = 0;
    int player = GetPieceAt(game, row, col);
    if (player == -1) return;
    
    int is_king = IsKing(game, row, col);
    int directions[4][2] = {{-1, -1}, {-1, 1}, {1, -1}, {1, 1}};
    int has_capture = 0;
    
    for (int i = 0; i < 4; i++) {
        int drow = directions[i][0];
        int dcol = directions[i][1];
        
        if (!is_king) {
            if (player == 0 && drow >= 0) continue;
            if (player == 1 && drow <= 0) continue;
        }
        
        int jump_row = row + drow;
        int jump_col = col + dcol;
        int land_row = row + 2 * drow;
        int land_col = col + 2 * dcol;
        
        if (IsValidPosition(jump_row, jump_col) && IsValidPosition(land_row, land_col)) {
            int jump_player = GetPieceAt(game, jump_row, jump_col);
            int land_player = GetPieceAt(game, land_row, land_col);
            
            if (jump_player == 1 - player && land_player == -1) {       // Adds this as a capture move
                has_capture = 1;
                moves[*move_count].from_row = row;
                moves[*move_count].from_col = col;
                moves[*move_count].to_row = land_row;
                moves[*move_count].to_col = land_col;
                moves[*move_count].capture_row = jump_row;
                moves[*move_count].capture_col = jump_col;
                (*move_count)++;
            }
        }
    }
    
    if (has_capture && must_capture) {
        return;
    }
    
    if (!has_capture || !must_capture) {
        for (int i = 0; i < 4; i++) {
            int drow = directions[i][0];
            int dcol = directions[i][1];
            
            if (!is_king) {
                if (player == 0 && drow >= 0) continue;
                if (player == 1 && drow <= 0) continue;
            }
            
            int new_row = row + drow;
            int new_col = col + dcol;
            
            if (IsValidPosition(new_row, new_col) && GetPieceAt(game, new_row, new_col) == -1) {        // Now checks for single diagonal moves
                moves[*move_count].from_row = row;
                moves[*move_count].from_col = col;
                moves[*move_count].to_row = new_row;
                moves[*move_count].to_col = new_col;
                moves[*move_count].capture_row = -1;
                moves[*move_count].capture_col = -1;
                (*move_count)++;
            }
        }
    }
}

int IsValidMove(GameState *game, int from_row, int from_col, int to_row, int to_col) {
    int player = GetPieceAt(game, from_row, from_col);      // Finds starting position
    if (player == -1 || player != game->current_turn) return 0;
    
    if (!IsValidPosition(to_row, to_col)) return 0;
    if (GetPieceAt(game, to_row, to_col) != -1) return 0;
    
    int has_forced_capture = HasForcedCapture(game, player);
    
    if (has_forced_capture && !CanPieceCapture(game, from_row, from_col)) {     // Enforces capture rule
        return 0;
    }
    
    Move moves[12];
    int move_count;
    
    GetPossibleMoves(game, from_row, from_col, moves, &move_count, has_forced_capture);     // Looks through legal moves
    
    for (int i = 0; i < move_count; i++) {      // If a legal move is what the user submits then valid
        if (moves[i].to_row == to_row && moves[i].to_col == to_col) {
            return 1;
        }
    }
    
    return 0;
}

int CanContinueCapturing(GameState *game, int row, int col) {       // Checks for available captures after a capture
    return CanPieceCapture(game, row, col);
}

void MakeMove(GameState *game, int from_row, int from_col, int to_row, int to_col) {
    int bit_pos_from = board_to_bitpos(from_row, from_col);     // Converts coords to bit
    int bit_pos_to = board_to_bitpos(to_row, to_col);
    int player = game->current_turn;
    
    if (player == 0) {
        ClearBit(&game->pieces, bit_pos_from);      // "Deletes" previous bit
        SetBit(&game->pieces, bit_pos_to);          // Updates where it moves to
        if (GetBit(game->kings, bit_pos_from)) {        // Dont forget to update the king board too
            ClearBit(&game->kings, bit_pos_from);
            SetBit(&game->kings, bit_pos_to);
        }
    } else {
        ClearBit(&game->pieces, bit_pos_from + 32);
        SetBit(&game->pieces, bit_pos_to + 32);
        if (GetBit(game->kings, bit_pos_from + 32)) {
            ClearBit(&game->kings, bit_pos_from + 32);
            SetBit(&game->kings, bit_pos_to + 32);
        }
    }
    
    int row_diff = to_row - from_row;
    int col_diff = to_col - from_col;
    
    if (abs(row_diff) == 2 && abs(col_diff) == 2) {     // If the move is a capture, clear the additional squares
        int jump_row = from_row + row_diff / 2;
        int jump_col = from_col + col_diff / 2;
        int jump_bit_pos = board_to_bitpos(jump_row, jump_col);
        
        ClearBit(&game->pieces, jump_bit_pos);
        ClearBit(&game->pieces, jump_bit_pos + 32);
        ClearBit(&game->kings, jump_bit_pos);
        ClearBit(&game->kings, jump_bit_pos + 32);
        printf("Captured %c%d\n", 'a' + jump_col, 8 - jump_row);
    }
    
    if (!IsKing(game, to_row, to_col)) {        // When player's piece reaches the end switch it to king's board
        if ((player == 0 && to_row == 0) || (player == 1 && to_row == 7)) {
            if (player == 0) {
                SetBit(&game->kings, bit_pos_to);
            } else {
                SetBit(&game->kings, bit_pos_to + 32);
            }
            printf("Kinged!\n");
        }
    }
}

int HasValidMoves(GameState *game, int player) {        // Is there no legal move or must capture, etc
    for (int row = 0; row < 8; row++) {
        for (int col = 0; col < 8; col++) {
            if (GetPieceAt(game, row, col) == player) {
                Move moves[12];
                int move_count;
                int must_capture = HasForcedCapture(game, player);
                GetPossibleMoves(game, row, col, moves, &move_count, must_capture);
                if (move_count > 0) {
                    return 1;
                }
            }
        }
    }
    return 0;
}

int IsGameOver(GameState *game) {
    return !HasValidMoves(game, 0) || !HasValidMoves(game, 1);
}

// Bot stuff
void MakeBotMove(GameState *game) {
    int must_capture = HasForcedCapture(game, 1);       // The bot will always take when able to
    Move all_moves[12 * 12];
    int total_moves = 0;
    
    for (int row = 0; row < 8; row++) {     // Generates all possible moves 
        for (int col = 0; col < 8; col++) {
            if (GetPieceAt(game, row, col) == 1) {
                Move piece_moves[12];
                int piece_move_count;
                GetPossibleMoves(game, row, col, piece_moves, &piece_move_count, must_capture);
                
                for (int i = 0; i < piece_move_count; i++) {
                    all_moves[total_moves++] = piece_moves[i];
                }
            }
        }
    }
    
    if (total_moves == 0) {
        printf("Bot has no moves\n");
        return;
    }
    
    if (must_capture) {     // Filter just capture moves and picks one at random if multiple exist
        Move capture_moves[12 * 12];
        int capture_count = 0;
        
        for (int i = 0; i < total_moves; i++) {
            if (all_moves[i].capture_row != -1) {
                capture_moves[capture_count++] = all_moves[i];
            }
        }
        
        if (capture_count > 0) {
            int random_index = rand() % capture_count;
            Move selected_move = capture_moves[random_index];
            
            printf("Bot: %c%d %c%d\n", 
                   'a' + selected_move.from_col, 8 - selected_move.from_row,
                   'a' + selected_move.to_col, 8 - selected_move.to_row);
            
            MakeMove(game, selected_move.from_row, selected_move.from_col,
                     selected_move.to_row, selected_move.to_col);
            
            if (CanContinueCapturing(game, selected_move.to_row, selected_move.to_col)) {       // Resrusion if another capture
                printf("Bot continues capturing...\n");
                game->current_turn = 1;
                MakeBotMove(game);
            }
            return;
        }
    }
    
    int random_index = rand() % total_moves;        // Random bot move
    Move selected_move = all_moves[random_index];
    
    printf("Bot: %c%d %c%d\n", 
           'a' + selected_move.from_col, 8 - selected_move.from_row,
           'a' + selected_move.to_col, 8 - selected_move.to_row);
    
    MakeMove(game, selected_move.from_row, selected_move.from_col, 
             selected_move.to_row, selected_move.to_col);
}


// File I/O
int SaveGame(GameState *game, const char *filename);
int LoadGame(GameState *game, const char *filename);

int SaveGame(GameState *game, const char *filename) {
    FILE *file = fopen(filename, "w");
    if (!file) {
        printf("Error: Could not open file '%s' for writing\n", filename);
        return 0;
    }
    
    fprintf(file, "%llu\n", game->pieces); // Logs all three of the game state values
    fprintf(file, "%llu\n", game->kings);
    fprintf(file, "%d\n", game->current_turn);
    
    fclose(file);
    printf("Game saved to '%s'\n", filename);
    return 1;
}

int LoadGame(GameState *game, const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("Error: Could not open file '%s' for reading\n", filename);
        return 0;
    }
    
    if (fscanf(file, "%llu", &game->pieces) != 1) {
        printf("Error: Invalid file format in '%s'\n", filename);
        fclose(file);
        return 0;
    }
    
    if (fscanf(file, "%llu", &game->kings) != 1) {
        printf("Error: Invalid file format in '%s'\n", filename);
        fclose(file);
        return 0;
    }
    
    if (fscanf(file, "%d", &game->current_turn) != 1) {
        printf("Error: Invalid file format in '%s'\n", filename);
        fclose(file);
        return 0;
    }
    
    fclose(file);
    printf("Game loaded from '%s'\n", filename);
    return 1;
}

// Game stuff
int input_to_coords(char *input, int *from_row, int *from_col, int *to_row, int *to_col) {
    if (strlen(input) < 5) return 0;        //converts algebraic to the board coords
    
    *from_col = tolower(input[0]) - 'a';
    *from_row = 8 - (input[1] - '0');
    *to_col = tolower(input[3]) - 'a';
    *to_row = 8 - (input[4] - '0');
    
    return 1;
}

int main() {
    srand(time(NULL));
    GameState game;
    InitializeGame(&game);
    
    printf("-------------------------------- Single Player Checkers --------------------------------\n");
    printf("Input options: 'xy xy' (c3 d4), 'binary', 'hex', 'save {name}', 'load {name}', 'quit'\n");
    printf("-------------------------------- ---------------------- --------------------------------\n");

    int chain_jump = 0;
    int chain_row = -1, chain_col = -1;
    
    while (!IsGameOver(&game)) {
        PrintBoard(&game);
        
        if (game.current_turn == 0) {   // Alternate 0,1 (player,bot)
            if (HasForcedCapture(&game, game.current_turn)) {   // "Mandatory" capture clause
                printf("You must capture\n");
            }
            
            char input[20];         // User input
            if (chain_jump) {       // Prompts chaining messages
                printf("\nChain jump on %c%d ", 
                       'a' + chain_col, 8 - chain_row);
            } else {
                printf("\nYour move (r): ");
            }
            
            if (fgets(input, sizeof(input), stdin) == NULL) break;
            
            input[strcspn(input, "\n")] = 0;    

            if (strcmp(input, "binary") == 0) {     // Display commands
                PrintBinary(game.pieces);
                continue;
            } else if (strcmp(input, "hex") == 0) {
                PrintHex(game.pieces);
                continue;
            } else if (strcmp(input, "quit") == 0) {
                break;
            } else if (strncmp(input, "save ", 5) == 0) {
                SaveGame(&game, input + 5);
                continue;
            } else if (strncmp(input, "load ", 5) == 0) {       // Resets variables upon load
                if (LoadGame(&game, input + 5)) {
                    chain_jump = 0;
                    chain_row = -1;
                    chain_col = -1;
                }
                continue;
            }
            
            int from_row, from_col, to_row, to_col;
            if (chain_jump) {
                from_row = chain_row;
                from_col = chain_col;
                if (!input_to_coords(input, &from_row, &from_col, &to_row, &to_col)) {
                    printf("Invalid format! Use algebraic (a3 b4)\n");
                    continue;
                }
                from_row = chain_row;
                from_col = chain_col;
            } else {
                if (!input_to_coords(input, &from_row, &from_col, &to_row, &to_col)) {
                    printf("Invalid format! Use algebraic (a3 b4)\n");
                    continue;
                }
            }
            
            if (IsValidMove(&game, from_row, from_col, to_row, to_col)) {       // Performs the new move when validated
                MakeMove(&game, from_row, from_col, to_row, to_col);
                
                int row_diff = to_row - from_row;
                if (abs(row_diff) == 2 && CanContinueCapturing(&game, to_row, to_col)) {
                    chain_jump = 1;
                    chain_row = to_row;
                    chain_col = to_col;
                    printf("Chain jump! Continue capturing.\n");
                } else {
                    chain_jump = 0;
                    chain_row = -1;
                    chain_col = -1;
                    game.current_turn = 1;
                }
            } else {
                printf("Invalid move ");
                if (HasForcedCapture(&game, game.current_turn)) {
                    printf("You must capture \n");
                } 
            }
        } else {        // Calls bot to move then switch
            MakeBotMove(&game);
            game.current_turn = 0;
            chain_jump = 0;
        }
    }
    
    PrintBoard(&game);
    printf("\nGame Over ");
    
    Bitboard red_pieces = game.pieces & 0xFFFFFFFF;
    Bitboard black_pieces = (game.pieces >> 32) & 0xFFFFFFFF;
    
    if (red_pieces == 0) {      // Conditions: All pieces for player is gone, no valid moves exist, or draw
        printf("Bot wins!\n");
    } else if (black_pieces == 0) {
        printf("You win! \n");
    } else if (!HasValidMoves(&game, game.current_turn)) {
        printf("%s has no moves ", game.current_turn == 0 ? "You" : "Bot");
        printf("%s wins\n", game.current_turn == 0 ? "Bot" : "You");
    } else {
        printf("Draw\n");
    }
    
    return 0;
}