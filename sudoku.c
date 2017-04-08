/**
 * Programmer: Steven Watson
 * Date: 5/7/2016
 *
 * Purpose: HW3 Sudoku, check whether input is valid or not.
 *
 * gcc sudoku.c -o sudoku.x -lpthread
 **/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define TRUE 1
#define FALSE 0
#define NUM_THREADS 27
#define bool int

typedef struct {
    // The Sudoku board
    int game[9][9];
    
    // Numbers for threads
    int t_id[27];
    
    // Whether a thread found an issue or not
    bool isError[27];
    
    // Pointer to threads
    pthread_t* thread_handles;
    
    // If any issues found, set game to invalid
    bool gameValid;
} Sudoku;

// Function Prototypes
void* Check_Subgrid(void*);
void* Check_Row(void*);
void* Check_Col(void*);

int main(int argc, char* argv[])
{
    int thread_id;
    pthread_t* thread_handles;
    pthread_t main_thread = pthread_self();

    Sudoku sudoku;
    sudoku.thread_handles = malloc(NUM_THREADS * sizeof(pthread_t));
    
    // Assume game is valid to start
    sudoku.gameValid = TRUE;
    
    // Set initial values
    int j = 0;
    for( j = 0; j < 27; j++) {
        sudoku.isError[j] = FALSE;
    }
    
    // Get Sudoku board
    int i;
    int row = 0;
    int col = 0;
    for(row = 0; row < 9; row++) {
        for(col = 0; col < 9; col++) {
            scanf("%d", &sudoku.game[row][col]);
        }
    }

    int pt_create_stat = -1;
    for (thread_id = 0; thread_id < NUM_THREADS; thread_id++) {
        // Numbers for threads to use
        sudoku.t_id[thread_id] = thread_id;
        
        // Threads for checking ROWS
        if( thread_id >= 0 && thread_id < 9 ) {
            pt_create_stat = pthread_create(&sudoku.thread_handles[thread_id], NULL, Check_Row, &sudoku);
        }
        
        // Threads for checking COLS
        if( thread_id >= 9 && thread_id < 18 ) {
            pt_create_stat = pthread_create(&sudoku.thread_handles[thread_id], NULL, Check_Col, &sudoku);
        }
        
        // Threads for checking SUBGRIDS
        if( thread_id >= 18 && thread_id < 27 ) {
            pt_create_stat = pthread_create(&sudoku.thread_handles[thread_id], NULL, Check_Subgrid, &sudoku);
        }
    }
    
    // Join threads back together
    int pt_stat = -1;
    for (thread_id = 0; thread_id < NUM_THREADS; thread_id++) {
        pt_stat = pthread_join(sudoku.thread_handles[thread_id], NULL);
    }

    // Output
    int k = 0;
    for(k = 0; k < 27; k++) {
        if( k >= 0 && k < 9 ) {
            // Rows
            if( sudoku.isError[k] == 1 ) {
                printf("Row %d doesn't have the required values.\n", k + 1);
            }
        }
        else if( k >= 9 && k < 18 ) {
            // Columns
            if( sudoku.isError[k] == 1 ) {
                printf("Column %d doesn't have the required values.\n", (k % 9) + 1);
            }
        }
        else if( k >= 18 && k < 27 ) {
            // Subgrid
            if( sudoku.isError[k] == 1 ) {
                switch( k % 9 ) {
                    case 0:
                        printf("The left upper subgrid doesn't have the required values.\n");
                        break;
                    case 1:
                        printf("The middle upper subgrid doesn't have the required values.\n");
                        break;
                    case 2:
                        printf("The right upper subgrid doesn't have the required values.\n");
                        break;
                    case 3:
                        printf("The left middle subgrid doesn't have the required values.\n");
                        break;
                    case 4:
                        printf("The middle middle subgrid doesn't have the required values.\n");
                        break;
                    case 5:
                        printf("The right middle subgrid doesn't have the required values.\n");
                        break;
                    case 6:
                        printf("The left lower subgrid doesn't have the required values.\n");
                        break;
                    case 7:
                        printf("The middle lower subgrid doesn't have the required values.\n");
                        break;
                    case 8:
                        printf("The right lower subgrid doesn't have the required values.\n");
                        break;
                    default: break;
                }
            }
        }
    }

    // Print whether the Sudoku board is valid or not
    if( sudoku.gameValid ) {
        printf("The input is valid Sudoku.\n");
    }
    else {
        printf("The input is not valid Sudoku.\n");
    }

    // Release memory used for threads
    free(sudoku.thread_handles);
    
    return 0;
}

void* Check_Row(void* game)
{
    // Typecast void* to Sudoku*
    Sudoku* tmp = (Sudoku*) game;
    bool check_row[9] = {FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE};
    
    pthread_t id = pthread_self();
    
    //int row = 0;
    int thread_num = -1;
    
    // Threads 0-8 : Check Rows
    int i = 0;
    for(i = 0; i < 9; i++) {
        if( tmp->thread_handles[i] == id ) {
            int col = 0;
            int spot = 0;
            thread_num = tmp->t_id[i];
            
            for(col = 0; col < 9; col++) {
                spot = tmp->game[thread_num][col];
                spot -= 1;
                
                if( check_row[spot] == TRUE ) {
                    tmp->isError[thread_num] = TRUE;
                    tmp->gameValid = FALSE;
                }
                else {
                    check_row[spot] = TRUE;
                }
            }
        }
    }
    
    //printf("CHECK ROW THREAD, with #: %d\n", thread_num);
    pthread_exit(NULL);
}

void* Check_Col(void* game)
{
    // Typecast void* to Sudoku*
    Sudoku* tmp = (Sudoku*) game;
    
    bool check_col[9] = {FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE};
    
    pthread_t id = pthread_self();
    
    //int col = 0;
    int thread_num = -1;
    
    // Threads 9-17 : Check Columns
    int i = 0;
    for(i = 9; i < 18; i++) {
        if( tmp->thread_handles[i] == id ) {
            int row = 0;
            int spot = 0;
            thread_num = tmp->t_id[i];
            
            for(row = 0; row < 9; row++) {
                spot = tmp->game[row][i % 9];
                spot -= 1;
                
                if( check_col[spot] == TRUE ) {
                    tmp->isError[thread_num] = TRUE;
                    tmp->gameValid = FALSE;
                }
                else {
                    check_col[spot] = TRUE;
                }
            }
        }
    }
    
    //printf("CHECK COL THREAD, with #: %d\n", thread_num);
    pthread_exit(NULL);
}

void* Check_Subgrid(void* game)
{
    // Typecast void* to Sudoku*
    Sudoku* tmp = (Sudoku*) game;
    
    bool check_grid[9] = {FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE};
    
    pthread_t id = pthread_self();
    
    //int subgrid = 0;
    int thread_num = -1;
    
    // Threads 18-26 : Check Subgrids
    int i = 0;
    for(i = 18; i < 27; i++) {
        if( tmp->thread_handles[i] == id ) {
            thread_num = i;
            int row = 0;
            int col = 0;
            switch( i % 9 ) {
                case 0: // Upper Left
                    row = 0;
                    col = 0;
                    break;
                case 1: // Upper Middle
                    row = 0;
                    col = 3;
                    break;
                case 2: // Upper Right
                    row = 0;
                    col = 6;
                    break;
                case 3: // Middle Left
                    row = 3;
                    col = 0;
                    break;
                case 4: // Middle Middle
                    row = 3;
                    col = 3;
                    break;
                case 5: // Middle Right
                    row = 3;
                    col = 6;
                    break;
                case 6: // Lower Left
                    row = 6;
                    col = 0;
                    break;
                case 7: // Lower Middle
                    row = 6;
                    col = 3;
                    break;
                case 8: // Lower Right
                    row = 6;
                    col = 6;
                    break;
                default: break;
            }
            
            int k = row;
            int j = col;
            int spot = 0;
            int row_bound = row + 3;
            int col_bound = col + 3;
            for(k = row; k < row_bound; k++) {
                for(j = col; j < col_bound; j++) {
                    spot = tmp->game[k][j];
                    spot -= 1;

                    if( check_grid[spot] == TRUE ) {
                        tmp->isError[i] = TRUE;
                        tmp->gameValid = FALSE;
                    }
                    
                    check_grid[spot] = TRUE;
                }
            }
        }
    }

    //printf("CHECK SUBGRID THREAD, with #: %d\n", thread_num);
    pthread_exit(NULL);
}