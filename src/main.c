#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <math.h>

#define EMPTY 0
#define PAWN 1
#define KNIGHT 2
#define BISHOP 3
#define ROOK 4
#define QUEEN 5
#define KING 6

#define WHITE 8
#define BLACK 16

#define WHITE_PAWN (WHITE | PAWN)
#define WHITE_KNIGHT (WHITE | KNIGHT)
#define WHITE_BISHOP (WHITE | BISHOP)
#define WHITE_ROOK (WHITE | ROOK)
#define WHITE_QUEEN (WHITE | QUEEN)
#define WHITE_KING (WHITE | KING)

#define BLACK_PAWN (BLACK | PAWN)
#define BLACK_KNIGHT (BLACK | KNIGHT)
#define BLACK_BISHOP (BLACK | BISHOP)
#define BLACK_ROOK (BLACK | ROOK)
#define BLACK_QUEEN (BLACK | QUEEN)
#define BLACK_KING (BLACK | KING)

#define BOARD_SIZE 8

int board[BOARD_SIZE][BOARD_SIZE] = {
    {BLACK_ROOK, BLACK_KNIGHT, BLACK_BISHOP, BLACK_QUEEN, BLACK_KING, BLACK_BISHOP, BLACK_KNIGHT, BLACK_ROOK},
    {BLACK_PAWN, BLACK_PAWN, BLACK_PAWN, BLACK_PAWN, BLACK_PAWN, BLACK_PAWN, BLACK_PAWN, BLACK_PAWN},
    {EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY},
    {EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY},
    {EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY},
    {EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY},
    {WHITE_PAWN, WHITE_PAWN, WHITE_PAWN, WHITE_PAWN, WHITE_PAWN, WHITE_PAWN, WHITE_PAWN, WHITE_PAWN},
    {WHITE_ROOK, WHITE_KNIGHT, WHITE_BISHOP, WHITE_QUEEN, WHITE_KING, WHITE_BISHOP, WHITE_KNIGHT, WHITE_ROOK}
};

void print_board() {
    printf("   0 1 2 3 4 5 6 7\n");
    printf("   ---------------\n");
    for (int r = 0; r < BOARD_SIZE; r++) {
        printf("%d |", r);
        for (int c = 0; c < BOARD_SIZE; c++) {
            int piece = board[r][c];
            switch (piece) {
                case EMPTY: printf(". "); break;
                case WHITE_PAWN: printf("♟ "); break;
                case WHITE_KNIGHT: printf("♞ "); break;
                case WHITE_BISHOP: printf("♝ "); break;
                case WHITE_ROOK: printf("♜ "); break;
                case WHITE_QUEEN: printf("♛ "); break;
                case WHITE_KING: printf("♚ "); break;
                case BLACK_PAWN: printf("♙ "); break;
                case BLACK_KNIGHT: printf("♘ "); break;
                case BLACK_BISHOP: printf("♗ "); break;
                case BLACK_ROOK: printf("♖ "); break;
                case BLACK_QUEEN: printf("♕ "); break;
                case BLACK_KING: printf("♔ "); break;
                default: printf("? "); break;
            }
        }
        printf("\n");
    }
}

bool is_valid_square(int r, int c) {
    return r >= 0 && r < BOARD_SIZE && c >= 0 && c < BOARD_SIZE;
}

bool is_path_clear(int sr, int sc, int dr, int dc) {
    int r_step = (dr > sr) ? 1 : (dr < sr) ? -1 : 0;
    int c_step = (dc > sc) ? 1 : (dc < sc) ? -1 : 0;
    int r = sr + r_step;
    int c = sc + c_step;

    while (r != dr || c != dc) {
        if (board[r][c] != EMPTY) return false;
        r += r_step;
        c += c_step;
    }
    return true;
}

bool is_valid_pawn_move(int sr, int sc, int dr, int dc, int color) {
    int direction = (color == WHITE) ? -1 : 1;
    if (sc == dc && board[dr][dc] == EMPTY) {
        if (dr == sr + direction) return true;
        if ((sr == 1 && color == BLACK || sr == 6 && color == WHITE) && dr == sr + 2 * direction) {
            return board[sr + direction][sc] == EMPTY;
        }
    }
    if (abs(sc - dc) == 1 && dr == sr + direction && (board[dr][dc] & (WHITE | BLACK)) && !(board[dr][dc] & color)) {
        return true;
    }
    return false;
}

bool is_valid_knight_move(int sr, int sc, int dr, int dc, int color) {
    int dr_abs = abs(dr - sr);
    int dc_abs = abs(dc - sc);
    return (dr_abs == 2 && dc_abs == 1) || (dr_abs == 1 && dc_abs == 2);
}

bool is_valid_bishop_move(int sr, int sc, int dr, int dc, int color) {
    return abs(dr - sr) == abs(dc - sc) && is_path_clear(sr, sc, dr, dc);
}

bool is_valid_rook_move(int sr, int sc, int dr, int dc, int color) {
    return (sr == dr || sc == dc) && is_path_clear(sr, sc, dr, dc);
}

bool is_valid_queen_move(int sr, int sc, int dr, int dc, int color) {
    return is_valid_bishop_move(sr, sc, dr, dc, color) || is_valid_rook_move(sr, sc, dr, dc, color);
}

bool is_valid_king_move(int sr, int sc, int dr, int dc, int color) {
    int dr_abs = abs(dr - sr);
    int dc_abs = abs(dc - sc);
    return dr_abs <= 1 && dc_abs <= 1;
}

bool is_valid_move(int sr, int sc, int dr, int dc, int color) {
    if (!is_valid_square(sr, sc) || !is_valid_square(dr, dc)) return false;

    int piece = board[sr][sc];
    if ((color == WHITE && !(piece & WHITE)) || (color == BLACK && !(piece & BLACK))) return false;

    int dest_piece = board[dr][dc];
    if ((dest_piece & WHITE) && (piece & WHITE)) return false;
    if ((dest_piece & BLACK) && (piece & BLACK)) return false;

    switch (piece & 7) {
        case PAWN: return is_valid_pawn_move(sr, sc, dr, dc, color);
        case KNIGHT: return is_valid_knight_move(sr, sc, dr, dc, color);
        case BISHOP: return is_valid_bishop_move(sr, sc, dr, dc, color);
        case ROOK: return is_valid_rook_move(sr, sc, dr, dc, color);
        case QUEEN: return is_valid_queen_move(sr, sc, dr, dc, color);
        case KING: return is_valid_king_move(sr, sc, dr, dc, color);
        default: return false;
    }
}

void find_king_position(int color, int *kr, int *kc) {
    int king = (color == WHITE) ? WHITE_KING : BLACK_KING;
    for (int r = 0; r < BOARD_SIZE; r++) {
        for (int c = 0; c < BOARD_SIZE; c++) {
            if (board[r][c] == king) {
                *kr = r;
                *kc = c;
                return;
            }
        }
    }
}

bool is_position_threatened(int r, int c, int color) {
    for (int sr = 0; sr < BOARD_SIZE; sr++) {
        for (int sc = 0; sc < BOARD_SIZE; sc++) {
            if ((color == WHITE && (board[sr][sc] & BLACK)) || (color == BLACK && (board[sr][sc] & WHITE))) {
                if (is_valid_move(sr, sc, r, c, board[sr][sc] & (WHITE | BLACK))) {
                    return true;
                }
            }
        }
    }
    return false;
}

bool is_checkmate(int color) {
    for (int sr = 0; sr < BOARD_SIZE; sr++) {
        for (int sc = 0; sc < BOARD_SIZE; sc++) {
            if (board[sr][sc] & color) {
                for (int dr = 0; dr < BOARD_SIZE; dr++) {
                    for (int dc = 0; dc < BOARD_SIZE; dc++) {
                        if (is_valid_move(sr, sc, dr, dc, color)) {
                            int captured_piece = board[dr][dc];
                            board[dr][dc] = board[sr][sc];
                            board[sr][sc] = EMPTY;

                            int kr, kc;
                            find_king_position(color, &kr, &kc);
                            if (!is_position_threatened(kr, kc, color)) {
                                board[sr][sc] = board[dr][dc];
                                board[dr][dc] = captured_piece;
                                return false;
                            }

                            board[sr][sc] = board[dr][dc];
                            board[dr][dc] = captured_piece;
                        }
                    }
                }
            }
        }
    }
    return true;
}

bool is_stalemate(int color) {
    for (int sr = 0; sr < BOARD_SIZE; sr++) {
        for (int sc = 0; sc < BOARD_SIZE; sc++) {
            if (board[sr][sc] & color) {
                for (int dr = 0; dr < BOARD_SIZE; dr++) {
                    for (int dc = 0; dc < BOARD_SIZE; dc++) {
                        if (is_valid_move(sr, sc, dr, dc, color)) {
                            int captured_piece = board[dr][dc];
                            board[dr][dc] = board[sr][sc];
                            board[sr][sc] = EMPTY;

                            int kr, kc;
                            find_king_position(color, &kr, &kc);
                            if (!is_position_threatened(kr, kc, color)) {
                                board[sr][sc] = board[dr][dc];
                                board[dr][dc] = captured_piece;
                                return false;
                            }

                            board[sr][sc] = board[dr][dc];
                            board[dr][dc] = captured_piece;
                        }
                    }
                }
            }
        }
    }
    return true;
}

void promote_pawn(int dr, int dc, int color) {
    int new_piece;
    if (color == WHITE) {
        printf("Promote pawn to (2=Knight, 3=Bishop, 4=Rook, 5=Queen): ");
        int choice;
        scanf("%d", &choice);
        new_piece = choice | color;
    } else {
        new_piece = BLACK_QUEEN; // Automatically promote to black queen
    }
    board[dr][dc] = new_piece;
}

void user_move(int *sr, int *sc, int *dr, int *dc) {
    printf("Enter your move (source row, source col, dest row, dest col): ");
    scanf("%d %d %d %d", sr, sc, dr, dc);
}

double evaluate_board();

void select_best_move(int *sr, int *sc, int *dr, int *dc, int color) {
    int best_sr = -1, best_sc = -1, best_dr = -1, best_dc = -1;
    double best_score = -INFINITY;
    double current_score;

    for (int sr = 0; sr < BOARD_SIZE; sr++) {
        for (int sc = 0; sc < BOARD_SIZE; sc++) {
            if (board[sr][sc] & color) {
                for (int dr = 0; dr < BOARD_SIZE; dr++) {
                    for (int dc = 0; dc < BOARD_SIZE; dc++) {
                        if (is_valid_move(sr, sc, dr, dc, color)) {
                            int captured_piece = board[dr][dc];
                            board[dr][dc] = board[sr][sc];
                            board[sr][sc] = EMPTY;

                            if (color == WHITE) {
                                if (is_position_threatened(dr, dc, BLACK)) {
                                    current_score = -INFINITY;
                                } else {
                                    current_score = evaluate_board(WHITE);
                                }
                            } else {
                                if (is_position_threatened(dr, dc, WHITE)) {
                                    current_score = -INFINITY;
                                } else {
                                    current_score = evaluate_board(BLACK);
                                }
                            }

                            if (current_score > best_score) {
                                best_score = current_score;
                                best_sr = sr;
                                best_sc = sc;
                                best_dr = dr;
                                best_dc = dc;
                            }

                            board[sr][sc] = board[dr][dc];
                            board[dr][dc] = captured_piece;
                        }
                    }
                }
            }
        }
    }

    if (best_sr != -1 && best_sc != -1 && best_dr != -1 && best_dc != -1) {
        *sr = best_sr;
        *sc = best_sc;
        *dr = best_dr;
        *dc = best_dc;
    } else {
        *sr = *sc = *dr = *dc = -1;
    }
}

double evaluate_board(int color) {
    double score = 0;
    for (int r = 0; r < BOARD_SIZE; r++) {
        for (int c = 0; c < BOARD_SIZE; c++) {
            int piece = board[r][c];
            if (piece != EMPTY) {
                double piece_value = 0;
                switch (piece &7) {
                    case PAWN: piece_value = 1; break;
                    case KNIGHT: piece_value = 3; break;
                    case BISHOP: piece_value = 3; break;
                    case ROOK: piece_value = 5; break;
                    case QUEEN: piece_value = 9; break;
                    case KING: piece_value = 100; break;
                }
                if (piece & color) {
                    score += piece_value;
                } else {
                    score -= piece_value;
                }
            }
        }
    }
    return score;
}

int main() {
    srand(time(NULL));
    int turn = WHITE;
    int sr, sc, dr, dc;

    while (true) {
        print_board();
        if (is_checkmate(turn)) {
            printf("Checkmate! %s wins!\n", (turn == WHITE) ? "Black" : "White");
            break;
        }
        if (is_stalemate(turn)) {
            printf("Stalemate!\n");
            break;
        }
        if (turn == WHITE) {
            user_move(&sr, &sc, &dr, &dc);
        } else {
            select_best_move(&sr, &sc, &dr, &dc, BLACK);
            if (sr == -1 && sc == -1 && dr == -1 && dc == -1) {
                printf("Black has no valid moves. Game over.\n");
                break;
            }
        }

        if (is_valid_move(sr, sc, dr, dc, turn)) {
            int captured_piece = board[dr][dc];
            board[dr][dc] = board[sr][sc];
            board[sr][sc] = EMPTY;

            if ((turn == WHITE && dr == 0) || (turn == BLACK && dr == BOARD_SIZE - 1)) {
                promote_pawn(dr, dc, turn);
            }

            int opponent_color = (turn == WHITE) ? BLACK : WHITE;
            int kr, kc;
            find_king_position(opponent_color, &kr, &kc);
            if (is_position_threatened(kr, kc, turn)) {
                printf("%s's king is in check.\n", (opponent_color == WHITE) ? "White" : "Black");
            }

            turn = opponent_color;
        } else {
            printf("Invalid move. Try again.\n");
        }
    }

    return 0;
}