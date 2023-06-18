#include <cs50.h>
#include <stdio.h>
#include <strings.h>

// Max number of candidates
#define MAX 9

// preferences[i][j] is number of voters who prefer i over j
int preferences[MAX][MAX] = {};

// locked[i][j] means i is locked in over j
bool locked[MAX][MAX];

// Each pair has a winner, loser
typedef struct
{
    int winner;
    int loser;
}
pair;

// Array of candidates
string candidates[MAX];
pair pairs[MAX * (MAX - 1) / 2];

int pair_count;
int candidate_count;

// Function prototypes
bool vote(int rank, string name, int ranks[]);
void record_preferences(int ranks[]);
void add_pairs(void);
void sort_pairs(void);
void lock_pairs(void);
void print_winner(void);
void get_preferences_array_pos(void);
int max(int array[], int n);
bool check_cycle(int graph_position, int loser);

int main(int argc, string argv[])
{
    // Check for invalid usage
    if (argc < 2)
    {
        printf("Usage: tideman [candidate ...]\n");
        return 1;
    }

    // Populate array of candidates
    candidate_count = argc - 1;
    if (candidate_count > MAX)
    {
        printf("Maximum number of candidates is %i\n", MAX);
        return 2;
    }
    for (int i = 0; i < candidate_count; i++)
    {
        candidates[i] = argv[i + 1];
    }

    // Clear graph of locked in pairs
    for (int i = 0; i < candidate_count; i++)
    {
        for (int j = 0; j < candidate_count; j++)
        {
            locked[i][j] = false;
        }
    }
    //set pair count to 0
    pair_count = 0;

    int number_of_voters = get_int("Number of voters: ");

    // Query for votes
    for (int i = 0; i < number_of_voters; i++)
    {
        // ranks[i] is voter's ith preference
        int ranks[candidate_count];

        // Query for each rank
        for (int j = 0; j < candidate_count; j++)
        {
            string name = get_string("Rank %i: ", j + 1);

            if (!vote(j, name, ranks))
            {
                printf("Invalid vote.\n");
                return 3;
            }
        }

        record_preferences(ranks);

        printf("\n");
    }

    add_pairs();
    sort_pairs();
    lock_pairs();
    print_winner();
    return 0;
}

// Update ranks given a new vote
bool vote(int rank, string name, int ranks[])
{
    //search for index of candidate and set do vote position in voters rank array
    for(int i = 0; i < candidate_count; i++)
    {
        if(strcasecmp(name,candidates[i]) == 0)
        {
                //set index of candidate to rank
                ranks[rank] = i;
                return true;
        }
    }

    //when candidate does not exist
    return false;
}

// Update preferences given one voter's ranks
void record_preferences(int ranks[]) //1;3;0
{
    //set rank preferences from voter to global preferences array
    for(int i = 0; i < candidate_count; i++)
    {
        for(int j = i + 1; j < candidate_count; j++)
        {
            preferences[ranks[i]][ranks[j]]++;
        }
    }
    return;
}

// Record pairs of candidates where one is preferred over the other
void add_pairs(void)
{
    //minus 1, because it is not needed to check building pair for the last ith' position in array, because building pairs from the previous positions are covering the check for last position
    for (int i = 0; i < candidate_count-1; i++)
    {
        //check for ith+1 position to build a pair, because it is not needed to check for both 0;1 and 1;0
        for (int j = i + 1; j < candidate_count; j++)
        {
            //check for winner on pair
            if (preferences[i][j]>preferences[j][i])
            {
                pairs[pair_count].winner = i;
                pairs[pair_count].loser  = j;
                pair_count++;
            }
            else if (preferences[i][j]<preferences[j][i])
            {
                pairs[pair_count].winner = j;
                pairs[pair_count].loser  = i;
                pair_count++;
            }
            else
            {}
        }
    }
    return;
}

// Sort pairs in decreasing order by strength of victory
void sort_pairs(void)
{
    //Selection Sort to sort list decreasing
    pair tempPair;
    for (int i = 0; i < pair_count; i++)
    {
        int indexOfBiggest = i;

        for(int j = i + 1; j < pair_count; j++)
        {
            //check for every pair/candidate value in preferences array the strengh of the win and sort according to this
            //if (preferences[pairs[indexOfBiggest].winner][pairs[indexOfBiggest].loser] < preferences[pairs[j].winner][pairs[j].loser]) === CHANGE CODE ON 08.04.2022 to following, because indexOfBiggest is I
            if (preferences[pairs[i].winner][pairs[i].loser] < preferences[pairs[j].winner][pairs[j].loser])
            {
                indexOfBiggest = j;
            }
        }
        tempPair = pairs[i];
        pairs[i] = pairs[indexOfBiggest];
        pairs[indexOfBiggest] = tempPair;

    }
    return;
}

/*bool check_cycle(int graph_position, int loser)
{
    if (graph_position == 0)
    {
        return false;
    }
    else
    {
        if (check_cycle(graph_position -1, loser) == true)
        {
            return true;
        }
        else if (loser == pairs[graph_position].winner && locked[pairs[graph_position].winner][pairs[graph_position].loser] && )
        {


            for(int i = graph_position - 1; i > -1; i--)
            {
                if(pairs[graph_position].winner == pairs[i].loser)
                {

                }
            }
            */
           /*
            return true;
        }
        return false;
    }
}
*/

//check for creating cycle when locking the pair to the locked array
                    //1                   2
bool check_cycle(int current_winner, int loser)
{
    //true when current winner is previous loser in a circle/another pairing related to loser
            //1             2
    if (current_winner == loser)
    {
        return true;
    }
    //iterate through locked array to check for a win for the current checking loser
    for (int i = 0; i < candidate_count; i++)
    {
        //check if loser has another pairing as winner and this pair is locked in locked array to check if there is a possible circle
        if(locked[loser][i])
        {
            //if current loser has won in previous pairings, check loser of current loser, to check if it is possible to create a circle to current winner which gets checked
            if(check_cycle(current_winner, i))
            {
                //when checking pairings and found one loser in possible circle that is the winner of the candidate/pairing that gets checked - so there is a circle
                return true;
            }
        }
    }
    //no circle
    return false;
}

// Lock pairs into the candidate graph in order, without creating cycles
void lock_pairs(void)
{
    for(int i = 0; i < pair_count; i++)
    {
        //lock every pair in locked array
        if(!check_cycle(pairs[i].winner, pairs[i].loser))
        {
            locked[pairs[i].winner][pairs[i].loser] = true;
        }
    }

    return;
}


// Print the winner of the election
void print_winner(void)
{
    //check which user has in the second dimension of the array no TRUE to ensure, that user has no arrow/graph pointing on them
    for (int i = 0; i < candidate_count; i++)
    {
        int locked_true_sum = 0;

        for (int j = 0; j < candidate_count; j++)
        {
            if (locked[j][i] == true)
            {
                locked_true_sum++;
            }
        }

        if (locked_true_sum == 0)
        {
            printf("%s\n", candidates[i]);
        }
    }
}

