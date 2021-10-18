#include <bits/stdc++.h>
using namespace std;
/*
Requirements:
1. Input
- Number of players
- Number of overs
- Players batting order
- state/score for each ball ( Run/Wide/No ball/Wicket)
2. At end of each over print
    - Score of each player, balls faced, 4s, 6s
    - Total
    - Extras
    - Overs
3. Then after overs for one team is finised or its got all out, we need to start
with 2nd team and print similar info
4. In the end print which team won
5. Keep track of all the players, their scores, strike change
*/

class Player;
class Team;
class Game;
class ScoreCard;

enum ScoreCardE { DISPLAY_WHOLE_TEAM_SCORE, SCORE_CARD_LAST };

enum GameStatusE {
  GAME_OVER,
  GAME_NEXT_OVER,
  GAME_ON_GOING_OVER,
  GAME_STATUS_LAST
};

enum PlayerStatusE {
  PLAYER_OUT,
  PLAYER_PLAYING,
  PLAYER_TO_PLAY,
  PLAYER_STATUS_LAST
};

class Player {
public:
  Player(string n)
      : name(n), score(0), ball_faced(0), fours(0), sixes(0),
        status(PLAYER_TO_PLAY) {}
  void AddScore(int s);
  void IncreaseBallCount() { ++ball_faced; }
  void SetStatus(PlayerStatusE s) { status = s; }
  string GetName() { return name; }
  int GetScore() { return score; }
  int GetBallsFaced() { return ball_faced; }
  int GetFours() { return fours; }
  int GetSixes() { return sixes; }
  PlayerStatusE GetStatus() { return status; }

private:
  string name;
  int score;
  int ball_faced;
  int fours;
  int sixes;
  PlayerStatusE status;
};

// Strategy Pattern to select the way of printing the score card
class AbstractScoreCard {
public:
  virtual void PrintScoreCard(Team *t) = 0;
};

class ScoreCard : public AbstractScoreCard {
public:
  void PrintScoreCard(Team *t);
};

class Team {
public:
  Team()
      : total_score(0), extras(0), over(0), curr_over_ball(0), nextPlayer(0),
        wickets(0), onStrike(nullptr), nonStrike(nullptr) {}
  GameStatusE ThrowBall(string inp);
  void AddPlayers(const vector<string> &names);
  int GetTotalScore() { return total_score; }
  int GetOver() { return over; }
  int GetExtras() { return extras; }
  int GetWickets() { return wickets; }
  int GetCurrBallCount() { return curr_over_ball; }
  vector<Player *> GetPlayers() { return players; }
  Player *getNextPlayer();

private:
  void changeStrike();

private:
  int total_score;
  int extras;
  int over;
  int curr_over_ball;
  int nextPlayer = 0;
  int wickets;
  vector<Player *> players;
  Player *onStrike;
  Player *nonStrike;
};

class Game {
public:
  static Game *getInstance();
  bool ThrowBall(string inp);
  void setScoreCardDisplayStrategy(ScoreCardE sc = SCORE_CARD_LAST);
  void PrintScoreCard();
  void InningStart(int inning);
  void InitializeTeam1(const vector<string> &p);
  void InitializeTeam2(const vector<string> &p);
  void setTotalOvers(int t) { total_overs = t; }
  void setTotalPlayerCount(int p) { total_player_count = p; }
  bool isOverLeft() { return (battingTeam->GetOver() <= total_overs - 1); }
  void GameResult();

private:
  int total_overs;
  int total_player_count;
  Team *Team1;
  Team *Team2;
  Team *battingTeam;
  AbstractScoreCard *score_card;
  // singleton access
  static Game *g;
  Game()
      : Team1(nullptr), Team2(nullptr), battingTeam(nullptr),
        score_card(nullptr) {}
};
