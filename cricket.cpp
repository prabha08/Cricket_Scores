#include "cricket.h"

Game *Game::g = nullptr;

// Game
Game *Game::getInstance() {
  if (g == nullptr) {
    g = new Game();
  }
  return g;
}

void Game::setScoreCardDisplayStrategy(ScoreCardE sc) {
  switch (sc) {
  case DISPLAY_WHOLE_TEAM_SCORE: {
    score_card = new ScoreCard();
  }
  default: {
    score_card = new ScoreCard();
  }
  }
}

bool Game::ThrowBall(string inp) {
  if (!battingTeam) {
    cout << "Batting Team null. Check.";
    return false;
  }
  GameStatusE status = battingTeam->ThrowBall(inp);
  // cout << inp << " " << status << endl;
  switch (status) {
  case GAME_OVER: {
    return false;
  }
  case GAME_NEXT_OVER: {
    PrintScoreCard();
    break;
  }
  }
  return true;
}

void Game::PrintScoreCard() {
  if (score_card == nullptr) {
    setScoreCardDisplayStrategy();
  }
  score_card->PrintScoreCard(battingTeam);
}
void Game::InningStart(int inning) {
  cout << "Inning: " << inning << endl;
  cout << "Enter Player Names in batting order" << endl;
  vector<string> players1;
  for (int i = 0; i < total_player_count; ++i) {
    string name;
    cin >> name;
    players1.push_back(name);
  }
  if (inning == 1) {
    Game::getInstance()->InitializeTeam1(players1);
  } else {
    Game::getInstance()->InitializeTeam2(players1);
  }
  bool isOngoingGame = true;
  cout << "Batting Start" << endl;
  while (isOngoingGame && Game::getInstance()->isOverLeft()) {
    if (battingTeam->GetCurrBallCount() == 0) {
      cout << "Starting Over: " << battingTeam->GetOver() + 1 << endl;
    }
    string inp;
    cin >> inp;
    isOngoingGame = Game::getInstance()->ThrowBall(inp);
  }
}

void Game::InitializeTeam1(const vector<string> &p) {
  if (Team1)
    return;
  Team1 = new Team();
  battingTeam = Team1;
  Team1->AddPlayers(p);
}

void Game::InitializeTeam2(const vector<string> &p) {
  if (Team2)
    return;
  Team2 = new Team();
  battingTeam = Team2;
  Team2->AddPlayers(p);
}

void Game::GameResult() {
  int t1 = Team1->GetTotalScore();
  int t2 = Team2->GetTotalScore();
  cout << endl << "Team1 Final ScoreCard: " << endl;
  score_card->PrintScoreCard(Team1);
  cout << endl << "Team2 Final ScoreCard: " << endl;
  score_card->PrintScoreCard(Team2);
  int diff = abs(t1 - t2);
  if (t1 == t2) {
    cout << "Game Draw" << endl;
  } else if (t1 > t2) {
    cout << "Team1 won by " << diff << " runs." << endl;
  } else {
    cout << "Team2 won by " << diff << " runs." << endl;
  }
}

// Team
void Team::AddPlayers(const vector<string> &names) {
  for (auto &p_name : names) {
    Player *p = new Player(p_name);
    players.push_back(p);
  }
}

Player *Team::getNextPlayer() {
  if (nextPlayer >= players.size())
    return nullptr;
  Player *next = players[nextPlayer];
  next->SetStatus(PLAYER_PLAYING);
  nextPlayer++;
  return next;
}

void Team::changeStrike() { swap(onStrike, nonStrike); }

GameStatusE Team::ThrowBall(string inp) {
  if (over == 0 && curr_over_ball == 0) {
    // Game just started
    onStrike = getNextPlayer();
    nonStrike = getNextPlayer();
  }
  if (!onStrike || !nonStrike) {
    // Players not there for batting
    return GAME_OVER;
  }

  // cout << "OnStrike: " << onStrike->GetName() << endl;
  if (inp == "W") {
    ++curr_over_ball;
    ++wickets;
    onStrike->IncreaseBallCount();
    onStrike->SetStatus(PLAYER_OUT);
    onStrike = getNextPlayer();
    if (!onStrike)
      return GAME_OVER;
  } else if (inp == "Wd") {
    extras++;
    total_score += 1;
  } else if (inp == "N") {
    extras++;
    total_score += 1;
  } else if (inp == "0" || inp == "2" || inp == "4" || inp == "6") {
    ++curr_over_ball;
    total_score += stoi(inp);
    onStrike->AddScore(stoi(inp));
    onStrike->IncreaseBallCount();
  } else if (inp == "1" || inp == "3") {
    ++curr_over_ball;
    total_score += stoi(inp);
    onStrike->AddScore(stoi(inp));
    onStrike->IncreaseBallCount();
    changeStrike();
  }
  if (curr_over_ball == 6) {
    ++over;
    curr_over_ball = 0;
    changeStrike();
    return GAME_NEXT_OVER;
  }
  return GAME_ON_GOING_OVER;
}

// Player
void Player::AddScore(int s) {
  score += s;
  if (s == 6) {
    sixes++;
  } else if (s == 4) {
    fours++;
  }
}

// ScoreCard
void ScoreCard::PrintScoreCard(Team *t) {
  cout << setw(15) << "Player" << setw(15) << "Score" << setw(15) << "4s"
       << setw(15) << "6s" << setw(15) << "Balls" << setw(15) << "Strike Rate"
       << setw(15) << endl;
  for (Player *p : t->GetPlayers()) {
    double strike_rate = 0;
    if (p->GetStatus() != PLAYER_TO_PLAY) {
      strike_rate = ((double)p->GetScore() / (double)p->GetBallsFaced()) * 100;
      int ss = strike_rate * 100;
      strike_rate = (double)ss / 100;
    }
    string status = (p->GetStatus() == PLAYER_PLAYING) ? "*" : " ";
    cout << setw(15) << p->GetName() << status << setw(15) << p->GetScore()
         << setw(15) << p->GetFours() << setw(15) << p->GetSixes() << setw(15)
         << p->GetBallsFaced() << setw(15) << strike_rate << setw(15) << endl;
  }
  cout << endl;
  cout << "Total: " << t->GetTotalScore() << "/" << t->GetWickets() << endl;
  string curr_ball_count = "";
  if (t->GetCurrBallCount() != 0) {
    curr_ball_count = "." + to_string(t->GetCurrBallCount());
  }
  cout << "Overs: " << t->GetOver() << curr_ball_count << endl;
  cout << "Extras: " << t->GetExtras() << endl;
}

// Main

int main() {
  int o, p;
  cin >> p >> o;
  Game::getInstance()->setTotalOvers(o);
  Game::getInstance()->setTotalPlayerCount(p);
  Game::getInstance()->InningStart(1);
  Game::getInstance()->InningStart(2);
  Game::getInstance()->GameResult();
}