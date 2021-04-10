#include<stdio.h>   //for input and outputs
#include<malloc.h>  //for Dynamic Mem. tree data structure
#include<stdlib.h>  //for system call
#include<math.h>    //for calculating the UCB1 formulla
#include<conio.h>   //for getch()

#define computer_turn 1     //computer's turn
#define user_turn 2         //human's turn
#define computer_won 1      //if computer worn
#define computer_lost 2     //if human won
#define draw 0              //if the game is a draw
#define game_left -1        //if the game is still left

struct node
{
  struct node *par, *child[7];              //nodes parent (used in backtracking), and its 7 children
  unsigned long score, visit;               //to keep track of the node traffic for decision
  short expanded, turn, board[6][7], mov;   //the board and whose turn is it to play
};

void initialize_board(short board[6][7])    //clean the board
{
  int i,j;
  for(i=0;i<6;i++)
  {
      for(j=0;j<7;j++)
      {
        board[i][j]=0;
      }
  }
}

void print_board(short board[6][7]) //print the board on screen
{
    int i,j;
    system("cls");
    printf("Connect 4 Game [using Monte Carlo Search Tree Algorithm] :\n\n");

    for(i=0;i<6;i++)
    {
      for(j=0;j<7;j++)
      {
        if(board[i][j]==0) printf(" -");
        else if(board[i][j]==computer_turn) printf(" 1");
        else printf(" 2");
      }
      printf("\n");
    }
    printf("\n");
    for(j=0;j<7;j++){printf(" %d",j);}
    printf("\n");
}

short read_int(short turn)  //read the column number from the console screen
{
    int d;
    printf("Turn: %d\nEnter column number: ",turn);
    scanf("%d",&d);
    return d;
}

//check if the input column is available, if so return the line number which available
int is_col_aval(short board[6][7], int c)
{
    int i;
    if (c<0 || c>6) return -1;

    for(i=0;i<6;i++)
        if(board[i][c]!=0) break;

    return i-1;
}

int change_turn(int turn)   //swap turn
{
  if(turn==computer_turn) return user_turn;
  else return computer_turn;
}

//on the basis of current move, check if the game is over or not
int is_game_over(short board[6][7], int l, int c)
{
    int i,j,a, x;
    x=board[l][c];
    //vertical check
    for(i=l; i<=5 && board[i][c]==x; i++);
    if(i-l>=4)return x;
    //horozontal check
    for(i=c; i<=6 && board[l][i]==x; i++);
    i--;
    for(j=0; board[l][i]==x && i>=0; i--,j++);
    if(j>=4) return x;
    //diagonal check
    for(i=c,j=l; i<=6 && j<=5 && board[i][j]==x; i++,j++);
    i--;j--;
    for(a=0; i>=0 && j>=0 && board[i][j]==x; i--,j--,a++);
    if(a>=4) return x;
    //diagonal check
    for(i=c,j=l; i<=5 && j>=0 && board[i][j]==x; i++,j--);
    i--;j++;
    for(a=0;i>=0 && j<=6 && board[i][j]==x; i--,j++,a++);
    if(a>=4) return x;
    for(i=0;i<6;i++)
    {
        for(j=0;j<7;j++)
        {
            if(board[i][j]==0) return game_left;
        }
    }
    return draw;
}

//check for a particular pixel if its a sequence of 4
int check_board_pixel(short board[6][7], int i, int j)
{
    if((j+3)<=6)
        if(board[i][j]==board[i][j+1] && board[i][j+1]==board[i][j+2] && board[i][j+2]==board[i][j+3]) return board[i][j];
    if((i-3)>=0)
        if(board[i][j]==board[i-1][j] && board[i-1][j]==board[i-2][j] && board[i-2][j]==board[i-3][j]) return board[i][j];
    if((i-3)>=0 && (j+3)<=6)
        if(board[i][j]==board[i-1][j+1] && board[i-1][j+1]==board[i-2][j+2] && board[i-2][j+2]==board[i-3][j+3]) return board[i][j];
    if((i-3)>=0 && (j-3)>=0)
        if(board[i][j]==board[i-1][j-1] && board[i-1][j-1]==board[i-2][j-2] && board[i-2][j-2]==board[i-3][j-3]) return board[i][j];
    return 0;
}

//check the board configuration if its game over, without using the move information
int is_game_over_check_full_board(short board[6][7])
{
    int i,j,x;
    for(i=5;i>=0;i--)
    {
        for(j=0;j<7;j++)
        {
            if(board[i][j]!=-1)
            {
                x=check_board_pixel(board,i,j);
                if(x==computer_won || x==computer_lost) return x;
            }
        }
    }
    for(i=0;i<6;i++)
    {
        for(j=0;j<7;j++)
        {
            if(board[i][j]==0) return game_left;
        }
    }
    return draw;
}

//check if the game has ended or not, and print result accordingly
int game_ended(short board[6][7], int d, int c)
{
    int x;
    x=is_game_over_check_full_board(board);
    if(x!=game_left)
    {
       print_board(board);
       switch(x)
       {
         case computer_won: printf("Computer won");
                       break;
         case computer_lost: printf("User won");
                       break;
         case draw:    printf("its a draw");
       }
      return 1;
    }
  return 0;
}

//create the root node of the tree
struct node * create_head(short board[6][7])
{
  struct node *temp;
  temp=(struct node *)malloc(sizeof(struct node));
  int i,j;
  for(i=0;i<7;i++)
  {
    temp->child[i]=NULL;
  }

  for(i=0;i<6;i++)
  {
      for(j=0;j<7;j++)
      {
          temp->board[i][j]=board[i][j];
      }
  }

  temp->visit=1;
  temp->score=0;
  temp->turn=computer_turn;
  temp->expanded=0;
  temp->par=NULL;
  return temp;
}

//delete the whole tree
void delete_tree(struct node * head)
{
    if(head==NULL) return;
    int i;

    if(head->expanded==0)
   {
     free(head);
     head=NULL;
     return;
   }

    for(i=0; head->child[i]!=NULL && i<7; i++)
    {
        delete_tree(head->child[i]);
        head->child[i]=NULL;
    }
    free(head);
    head=NULL;
}

//calculate UCB1 for a particular node
double UCB_for(unsigned long v, unsigned long s, unsigned long t_v)
{
    double result = (double)s/(double)v;
    result = result + (2*(sqrt(log(t_v)/(double)v)));
    return result;
}

//calculate the UCB1 for all nodes and return the max
struct node * UCB1(struct node * head, unsigned long t_v)
{
    double val=0.0, temp_val=0.0;
    struct node *temp_head;
    int i;
    for(i=0; head->child[i]!=NULL && i<7; i++)
    {
        if(head->child[i]->visit==0) return head->child[i];
        temp_val=UCB_for(head->child[i]->visit, head->child[i]->score, t_v);
        if(val<temp_val)
        {
            val=temp_val;
            temp_head=head->child[i];
        }
    }
    return temp_head;
}

//select the node which is to be expanded or simulated
struct node * selection(struct node *head, unsigned long t_v)
{
    struct node * temp = head;
    while(temp->expanded)
    {
             temp=UCB1(temp, t_v);
             if(head==temp) break;
             head=temp;
    }
    return temp;
}

//create child nodes of the node - expansion
struct node * create_child(struct node * head, int d, int c)
{
    struct node *temp=(struct node *)malloc(sizeof(struct node));
    int turn=change_turn(head->turn),i,j;
    temp->turn=turn;

    for(i=0;i<6;i++)
    {
        for(j=0;j<7;j++)
        {
            temp->board[i][j]=head->board[i][j];
        }
    }
    for(i=0;i<7;i++)        temp->child[i]=NULL;
    temp->mov=c;
    temp->board[d][c]=head->turn;
    temp->visit=0;
    temp->score=0;
    temp->expanded=0;
    temp->par=head;
    return temp;
}

//expand the passed node
struct node* expand(struct node *head)
{
  int d,i,j;
  d=is_game_over_check_full_board(head->board);

  if(d!=game_left) return NULL;

  for( i=0, j=0; i<7;i++)
  {
      d=is_col_aval(head->board,i);
      if(d>=0)
      {
        head->child[j++]=create_child(head,d,i);
      }
  }
  head->expanded=1;
  return head->child[0];
}

//random simulation of the sent state
int simulate(struct node *head)
{
    short b[6][7];
    int i,j,x,c,d,t_turn=head->turn;

    for(i=0;i<6;i++)
    {
        for(j=0;j<7;j++)
        {
            b[i][j]=head->board[i][j];
        }
    }
    x=is_game_over_check_full_board(b);
    while(x==game_left)
    {
        do
        {
          c=rand()%7;
          d=is_col_aval(b,c);
        }
        while(d<0);

        b[d][c]=t_turn;
        t_turn=change_turn(t_turn);

        x=is_game_over(b, d, c);
    }
    return x;
}

//update the values of score and visit from the leaf node to the root
void backtrack(struct node * head, unsigned long val)
{
    if(head==NULL) return;
    do
    {
        head->visit++;
        if((val==computer_won && head->turn==user_turn) || (val==computer_lost && head->turn==computer_turn))
            head->score++;
        head=head->par;
    }
    while(head!=NULL);
}

//select the move with the maximum visit count
int max_mov(struct node *head)
{
  unsigned long v=0, m=0,i;
  for(i=0; head->child[i]!=NULL &&i<7; i++)
  {
//    printf("visit : %d\n",head->child[i]->visit);
    if(head->child[i]->visit>v)
    {
      v=head->child[i]->visit;
      m=head->child[i]->mov;
    }
  }
  return m;
}

//monte carlo search tree algorithm
int c4solve(short board[6][7], int level)
{
    unsigned long val, mov;
    unsigned long iterations=level*1000,i;
    struct node *head=NULL, *temp, *temp2;
    head=create_head(board);

    for(i=0;i<iterations;i++)
    {
        temp=selection(head, i);
        if(temp->visit!=0)
        {
            temp2=expand(temp);
            if(temp2==NULL)
            {
                val=is_game_over_check_full_board(temp->board);
                temp->visit++;
                temp->score+=val;
            }
            else
            {
                temp=temp2;
                val=simulate(temp);
            }
        }
        else
        {
            val=simulate(temp);
        }
        backtrack(temp,val);
    }
    mov=max_mov(head);
    delete_tree(head);
    return mov;
}

int main()
{
  short board[6][7], d, c, turn=computer_turn;
  char ch;
  int level=1;
  initialize_board(board);

  //input the first mover
  printf("Who gets the first chance? Computer or Human [c/h]?");
  scanf("%c",&ch);
  if(ch=='c') turn=computer_turn;
  else if(ch=='h') turn=user_turn;

  //input the game level
  level_select:
//  printf("Select the game level [1-10] : ");
//  scanf("%d", &level);
    level=5;

  if(level>0 && level<11) level*=50;
  else goto level_select;

  while(1)
  {
    print_board(board);
    //human's move
    if(turn==user_turn)
    {
        input_again:
      c=read_int(turn);
      d=is_col_aval(board, c);
      if(d>=0) board[d][c]=turn;
      else goto input_again;
    }
    //computer's move
    else
    {
      printf("Evaluating User's Move...\n\nComputer choosing it's next move...");
      c=c4solve(board, level);
      d=is_col_aval(board, c);
      if(d>=0) board[d][c]=computer_turn;
    }
    //swap the turn
    turn=change_turn(turn);
    //check is the game is over or not
    if(game_ended(board,d,c)) break;
  }
  getch();
  return 0;
}
