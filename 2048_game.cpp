#include<iostream>
#include<ctime>
#include<unistd.h>
#include<cstdlib>
#include<cstdio>
#include<cmath>

using namespace std;

int total_wins = 0;

int press_enter;
int random_index(int);

const char available_moves[4] = {'w','s','a','d'};

class Game;

class Game_AI
{

  int response;
  int apocalypse;
  char control;
  
public:
  
  int max;
  int win;
  int plus;
  int score;
  int grid[4][4];
  int bgrid[4][4];
  
  Game_AI(): score(0),plus(0),win(2048),max(0),response(0),apocalypse(1) {}

  Game_AI(const Game_AI &g) {
        int i,j;
        for(i=0;i<4;i++)
          for(j=0;j<4;j++)
            grid[i][j] = g.grid[i][j];
}
  
  void logic_flow(Game*);
  void game_end_check(Game*);
  void key_press(char);
  void start_grid();
  void update_grid();
  void fill_space();
  void spawn();
  void find_greatest_tile();
  void backup_grid();
  void undo();
  
  int full();
  int block_moves();
};

class Game : public Game_AI
{
  char option;
  string name;
  
public:
  void display_grid();
  void display_help_screen();
  void display_win_screen();
  void display_loser_screen();
  char display_try_again_screen(int);

};



int detect_change(Game g1, Game g2) {

  int change=0;
  int i,j;
  for(i=0;i<4;i++) 
    {
    for(j=0;j<4;j++)
      {
        if (g1.grid[i][j]!=g2.grid[i][j]) {
          change = 1;
          return change;
          }
      }
    } 
  return change;

}

void get_available_moves(Game game, char* moves)
{
  
  int i;
  
  for(i=0;i<4;i++)
    {
      Game game_copy = game;
      game_copy.key_press(moves[i]);
      game_copy.logic_flow(&game_copy);
      if (detect_change(game_copy,game)==0) {
        moves[i]='0';
      } 
    }
}

float evaluate_board(Game game)
{

  char m[4];
  m[0]=available_moves[0];m[1]=available_moves[1];m[2]=available_moves[2];m[3]=available_moves[3];
  get_available_moves(game,m);

  float max=0,empty_tiles=0,available_moves=4;
  //float h_eval=0;
  
  int temp;

  //float h_grid[4][4] = {{64.0,32.0,16.0,8.0},{0.5,1.0,2.0,4.0},{0.25,0.125,0.0625,0.03125},{0.001953125,0.00390625,0.0078125,0.015625}};

  int i,j;

  for(i=0;i<4;i++) 
    {
    for(j=0;j<4;j++)
      {
        temp = game.grid[i][j];
        if (temp==0)
           empty_tiles += 1;
        if (temp>=max)
	   max = temp; 
	//h_eval += h_grid[i][j]*temp;
      }
    }

  for(i=0;i<4;i++) 
    if(m[i]=='0')
      available_moves -= 1;

  float ret;

  //cout<<max<<" "<<empty_tiles<<" "<<available_moves<<" "<<h_eval<<endl;
  
  //ret = empty_tiles*205.0 + max*0.25 + available_moves*307.5; + h_eval*.35; 
  //ret = empty_tiles*50.0 + max*0.15 + available_moves*50; + h_eval*0.1575; 
  ret = empty_tiles*140.0 + max*0.9 + available_moves*40.0; 

  return ret;

}


char best_move(Game game)
{

  Game game_copy;
  char m[4];
  m[0]=available_moves[0];m[1]=available_moves[1];m[2]=available_moves[2];m[3]=available_moves[3];
  get_available_moves(game,m);

  int i;


  char temp='0';
/*  float scores=0.0,eval;

  for(i=0;i<4;i++)
    {
      game_copy = game;
      if (m[i]!='0')
        {
          game_copy.key_press(m[i]);
          game_copy.logic_flow(&game_copy);
          eval = evaluate_board(game_copy);
          if (eval>scores) 
	      {
	        temp = m[i];
                scores = eval;
              }
        }
    }
*/
  temp=m[rand()%4+0];
  if (m[0]=='0' and m[1]=='0' and m[2]=='0' and m[3]=='0') return temp;
  
  while (temp=='0') temp=m[rand()%4+0];

  return temp;
      

}

int play_simulated(Game game)
{

  char best_move_='0';
  int total_count = 0;

  while(1)
    {
      best_move_ = best_move(game);
      if (best_move_=='0') break;
      total_count += 1;
      game.key_press(best_move_);
      game.logic_flow(&game);
    }

  return total_count;
}



char AI_function(Game game)
{

  Game game_copy;
  char m[4];
  m[0]=available_moves[0];m[1]=available_moves[1];m[2]=available_moves[2];m[3]=available_moves[3];
  get_available_moves(game,m);

  char best_move = '0';
  int max_plays = 0;

  int plays;

  int i=0;
  int j=0;

  for(i=0;i<4;i++)
    {
      plays = 0;
      for(j=0;j<100;j++)
      {
	game_copy = game;
        if (m[i]!='0')
        {
           game_copy.key_press(m[i]);
           game_copy.logic_flow(&game_copy);
           plays += play_simulated(game_copy);
        }
      }
     //cout<<m[i]<<":"<<plays<<endl;
     if (plays>max_plays) {max_plays=plays;best_move=m[i];}
    }
  return best_move;
}



void check_something(Game game)
{

  cout<<"in check fn\n";
  Game game_copy = game;
  game_copy.start_grid();
  cout<"check copied\n";
  game_copy.display_grid();
  cout<<"check original\n";
  game.display_grid();

}



int main()
{
  Game exec;
  
  srand(time(NULL));

  char move;

  float temp;

  int total_ = 0;

  char m[4];

  int i,moves_;

  int all_moves[100];

  for(i=0;i<100;i++)
  {
  exec.start_grid();
  moves_=0;
  //exec.display_grid();
  
    while(1)
    {

      m[0]=available_moves[0];m[1]=available_moves[1];m[2]=available_moves[2];m[3]=available_moves[3];
      //temp = evaluate_board(exec);
      get_available_moves(exec,m);
      move = AI_function(exec);
      //move = best_move(exec);
      if (move!='w' and move!='s' and move!='a' and move!='d')
         {cout<<"end of one game: "<<moves_<<endl;all_moves[i]=moves_;total_ += moves_;break;}
      exec.key_press(move);
      exec.logic_flow(&exec);
      //exec.display_grid();
      exec.game_end_check(&exec);
      moves_ += 1;
    };
  }
cout<<endl<<"total: "<<total_<<" wins: "<<total_wins<<endl;
return 0;
}


void Game_AI::key_press(char c)
{
  control = c;
}


void Game_AI::logic_flow(Game *execute)
{
  
  switch(control)
  {
      case 'w':
      case 'a':
      case 's':
      case 'd':
		  execute->backup_grid();
		  execute->fill_space();
		  execute->update_grid();
		  execute->fill_space();
		  execute->find_greatest_tile();
		  //execute->display_grid();
		  //usleep(100*160);

		  if(execute->full()&&apocalypse)
		  {
		      response=-1;
		      break;
		  }
		  else if(execute->block_moves())
		  {
		      execute->spawn();
		      break;
		  }
		  else
		  {
		      response=0;	
		      break;
		  }

      case 'u':	
		  if(execute->block_moves())
		    score-=plus;
		  execute->undo();
		  break;
						  
						  
      case 'r': 	
		  execute->start_grid();
		  score=0;
		  plus=0;
		  break;

      case 'q':		
		  response=-1;
		  break;
			  
      case 'h': 	
		  execute->display_help_screen();
		  press_enter=getchar();
		  execute->display_grid();
		  break;
		  
  }
}

void Game_AI::game_end_check(Game *screen)
{
  if(max==win)
  {
    screen->display_win_screen();
    
    if(screen->display_try_again_screen(0)=='n')
	response=-1;
    else
	win*=2;
  }
  
  else if(response==-1)
  {
    screen->display_loser_screen();
  
    if(screen->display_try_again_screen(1)=='y')
    {
	screen->start_grid();
	response=0;
    }
  }
    
  if(response==-1)
  {
    cout<<"\n\n\t\t            > Thank you for playing. "
        <<"\n\n\n\t\t\t\t\t\t   DEVELOPED BY SCM [-_-] \n\n";
    press_enter=getchar();
    exit(0);
  }
}

void Game_AI::start_grid()
{
  int i,j;
  
  plus=0;
  score=0;
  max=0;
  
	for(i=0;i<4;i++)	
		for(j=0;j<4;j++)
			grid[i][j]=0;

	i=random_index(4);
	j=random_index(4);

		grid[i][j]=2;

	i=random_index(4);
	j=random_index(4);

		grid[i][j]=2;
}

void Game::display_grid()
{
	/* system("clear");

		
			
		cout<<"\n  ::[  THE 2048 PUZZLE  ]::\t\t\t\tDeveloped by SCM [-_-]\n\n\t";
		
			if(plus)
				cout<<"+"<<plus<<"!";
			else
				cout<<"   ";
			
		cout<<"\t\t\t\t\t\tSCORE::"<<score<<" \n\n\n\n";

		*/
				
				for(int i=0;i<4;i++)
				{
				cout<<"\t\t     |";

					for(int j=0;j<4;j++)
					{
						if(grid[i][j])
							printf("%4d    |",grid[i][j]);
						else
							printf("%4c    |",' ');
					} 

				cout<<endl<<endl;
				}
                 

		/*cout<<"\n\n Controls (+ :: o)\t\t\t\tu - undo\tr - restart\n\n\tW\t\t     ^\t\t\th - help\tq - quit\n\t\t\t\t\t\t\t\t"
		    <<" \n   A    S    D\t\t<    v    >\t\t\t     ."
		    <<" \n\t\t\t\t                             ";
		*/
}

int random_index(int x)
{
    int index;
    index=rand()%x+0;

return index;
}

void Game_AI::backup_grid()
{
		for(int i=0;i<4;i++)
			for(int j=0;j<4;j++)
				bgrid[i][j]=grid[i][j];
}

		
void Game_AI::fill_space()
{

	switch(control)
	{
		case 'w':
					for(int i=0;i<4;i++)
						for(int j=0;j<4;j++)
						{
							if(!grid[j][i])
							{
								for(int k=j+1;k<4;k++)
									if(grid[k][i])
									{
										grid[j][i]=grid[k][i];
										grid[k][i]=0;
										break;
									}
							}

						}break;

		case 's':
					for(int i=0;i<4;i++)
						for(int j=3;j>=0;j--)
						{
							if(!grid[j][i])
							{
								for(int k=j-1;k>=0;k--)
									if(grid[k][i])
									{
										grid[j][i]=grid[k][i];
										grid[k][i]=0;
										break;
									}
							}

						}break;
		case 'a':
					for(int i=0;i<4;i++)
						for(int j=0;j<4;j++)
						{
							if(!grid[i][j])
							{
								for(int k=j+1;k<4;k++)
									if(grid[i][k])
									{
										grid[i][j]=grid[i][k];
										grid[i][k]=0;
										break;
									}
							}

						}break;


		case 'd':
					for(int i=0;i<4;i++)
						for(int j=3;j>=0;j--)
						{
							if(!grid[i][j])
							{
								for(int k=j-1;k>=0;k--)
									if(grid[i][k])
									{
										grid[i][j]=grid[i][k];
										grid[i][k]=0;
										break;
									}
							}

						}break; 

	}
}


void Game_AI::update_grid()
{
	plus=0;
	apocalypse=1;

	switch(control)
	{
		case 'w':
					for(int i=0;i<4;i++)
						for(int j=0;j<3;j++)
						{
							if(grid[j][i]&&grid[j][i]==grid[j+1][i])
							{
								apocalypse=0;
								grid[j][i]+=grid[j+1][i];
								grid[j+1][i]=0;
								plus+=(((log2(grid[j][i]))-1)*grid[j][i]);
								score+=(((log2(grid[j][i]))-1)*grid[j][i]);
								
							}
						}break;

		case 's':
					for(int i=0;i<4;i++)
						for(int j=3;j>0;j--)
						{
							if(grid[j][i]&&grid[j][i]==grid[j-1][i])
							{
								apocalypse=0;
								grid[j][i]+=grid[j-1][i];
								grid[j-1][i]=0;
								plus+=(((log2(grid[j][i]))-1)*grid[j][i]);
								score+=(((log2(grid[j][i]))-1)*grid[j][i]);
							}
						}break;

		case 'a':
					for(int i=0;i<4;i++)
						for(int j=0;j<3;j++)
						{
							if(grid[i][j]&&grid[i][j]==grid[i][j+1])
							{
								apocalypse=0;
								grid[i][j]+=grid[i][j+1];
								grid[i][j+1]=0;
								plus+=((log2(grid[i][j]))-1)*grid[i][j];
								score+=((log2(grid[i][j]))-1)*grid[i][j];
							}
						}break;

		case 'd':
					for(int i=0;i<4;i++)
						for(int j=3;j>0;j--)
						{
							if(grid[i][j]&&grid[i][j]==grid[i][j-1])
							{
								apocalypse=0;
								grid[i][j]+=grid[i][j-1];
								grid[i][j-1]=0;
								plus+=((log2(grid[i][j]))-1)*grid[i][j];
								score+=(((log2(grid[i][j]))-1)*grid[i][j]);
							}
						}break;


	}
	
	

}


void Game_AI::spawn()
{
		int i,j,k;
		
		do
		{
		    i=random_index(4);
		    j=random_index(4);
		    k=random_index(10);

		}while(grid[i][j]);
				
		
			
		//if(k<2)
		if(k<1)
		    grid[i][j]=4;

		else
		    grid[i][j]=2;
	

}
		
		
void Game_AI::find_greatest_tile()
{
	for(int i=0;i<4;i++)
		for(int j=0;j<4;j++)
			if(grid[i][j]>max)
				max=grid[i][j];
}

int Game_AI::full()
{
	int k=1;

	for(int i=0;i<4;i++)
		for(int j=0;j<4;j++)
		{
			if(!grid[i][j])
				k=0;
				
		}
return k;
}

void Game::display_help_screen()
{
	system("clear");
	
	cout<<endl<<"  ::[  THE 2048 PUZZLE  ]::\t\t\t\tDeveloped by SCM [-_-]";
	
	char text[]="> The game starts with 1 or 2 randomly placed numbers in a 4x4 grid (16 cells).> Use the controls to move the cells. ( Press W/A/S/D )> 2 cells of the same number will merge and add up.> New 2s and 4s will appear randomly on an empty cell with each move you make.> Your objective is to make 2048 in a cell without getting stuck!> Press Enter to continue...";

	for(int i=0; text[i] ; i++)
	{
	    if(text[i]=='>')
	    {
	    usleep(1000*500);
	    cout<<"\n\n\n";
	    }
    
	    cout<<text[i];
	}
}
	

void Game::display_win_screen()
{
total_wins += 1;
cout<<endl<<"won once: "<<total_wins<<endl;
}

void Game::display_loser_screen()
{
}

char Game::display_try_again_screen(int lose)
{	
}
  
void Game_AI::undo()
{
		for(int i=0;i<4;i++)
			for(int j=0;j<4;j++)
				grid[i][j]=bgrid[i][j];
}

int Game_AI::block_moves()
{
	int k=0;

	for(int i=0;i<4;i++)
		for(int j=0;j<4;j++)
			if(bgrid[i][j]!=grid[i][j])
			{	
				k=1;
				break;
			}
return k;
}				
					
