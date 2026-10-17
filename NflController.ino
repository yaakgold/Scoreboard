const int SL = 31; // Substring location

void getWeekEvents()
{
  JsonDocument root = getData("/v2/sports/football/leagues/nfl", 0);
  JsonDocument currentSeason = getData(root["season"]["$ref"], SL);
  JsonDocument currentWeek = getData(currentSeason["type"]["week"]["$ref"], SL);
  if (nflEvents[0].week == currentWeek["number"]) return; // Don't do anything if it is still the same week

  for (SportEvent sEvt : nflEvents)
  {
    sEvt.eventName = "";
    sEvt.drivesRef = "";
    sEvt.week = -1;
  }

  JsonDocument currentEvents = getData(currentWeek["events"]["$ref"], SL);
  
  JsonArray events = currentEvents["items"];

  int index = 0;
  for (JsonVariant comp : events)
  {
    JsonDocument event = getData(comp["$ref"], SL);
    JsonDocument competition = getData(event["competitions"][0]["$ref"], SL);
    
    JsonDocument homeTeamComp;
    JsonDocument awayTeamComp;
    JsonArray competitors = competition["competitors"];
    for (JsonVariant competitor : competitors)
    {
      String homeAway = competitor["homeAway"];
      if (homeAway == "home")
      {
        homeTeamComp = competitor;
        continue;
      }
      else if (homeAway == "away")
      {
        awayTeamComp = competitor;
        continue;
      }
    }

    SportEvent evt;
    String shortName = event["shortName"];
    evt.eventName = shortName;

    String ref = competition["drives"]["$ref"];
    evt.drivesRef = ref;

    evt.week = currentWeek["number"];

    evt.homeTeam = getData(homeTeamComp["team"]["$ref"], SL);;
    evt.awayTeam = getData(awayTeamComp["team"]["$ref"], SL);;

    nflEvents[index] = evt;
    index++;
  }
}

void showGameOptionsScreen()
{
  displayGameOption(nflEvents[currentNflEventIndex].eventName);
  downBtnFunc = increaseEvent;
  upBtnFunc = decreaseEvent;
  enterBtnFunc = displayGame;
}

void increaseEvent()
{
  currentNflEventIndex++;
  if (nflEvents[currentNflEventIndex].eventName == "") currentNflEventIndex = 0;
  displayGameOption(nflEvents[currentNflEventIndex].eventName);
}

void decreaseEvent()
{
  currentNflEventIndex--;
  if (currentNflEventIndex < 0)
  {
    currentNflEventIndex = 20;
    while (nflEvents[currentNflEventIndex].eventName == "")
    {
      currentNflEventIndex--;
    }
  }
  displayGameOption(nflEvents[currentNflEventIndex].eventName);
}

void displayGame()
{
  enterBtnFunc = showGameOptionsScreen;
  updateScoreFunc = displayNFLScore;
  matrix.fillScreen(BLACK);

  matrix.show();

  // TODO: Make this look at the current game
  displayNFLScore();

  matrix.show();
}

void displayNFLScore()
{
  displaySituation();
  displayAwayScore();
  displayHomeScore();
}

void displayAwayScore()
{
  matrix.setTextColor(WHITE);
  matrix.setCursor(0, 0);
  String awayTeamName = nflEvents[currentNflEventIndex].awayTeam["abbreviation"];
  matrix.print(awayTeamName);
  matrix.print(":99");
  
  matrix.setCursor(0, 5);
  matrix.setTextColor(YELLOW);
  matrix.print("---");
  matrix.drawRect(0, 10, 2, 12, WHITE);
}

void displayHomeScore()
{
  matrix.setTextColor(MAGENTA);
  matrix.setCursor(0, 25);
  String homeTeamName = nflEvents[currentNflEventIndex].homeTeam["abbreviation"];
  matrix.print(homeTeamName);
  matrix.print(":99");

  matrix.setCursor(0, 20);
  matrix.setTextColor(YELLOW);
  matrix.print("---");
  matrix.drawRect(62, 10, 2, 12, MAGENTA);
}

void displaySituation()
{
  matrix.setTextColor(WHITE);
  matrix.setCursor(46, 0);
  matrix.print("2OT");
  matrix.drawRect(0, 10, 64, 12, GREEN);
  matrix.drawRect(31, 10, 2, 12, WHITE);

  matrix.drawLine(42, 11, 42, 20, MAGENTA);

  matrix.drawLine(41, 15, 36, 12, MAGENTA);
  matrix.drawLine(41, 16, 36, 19, MAGENTA);
  matrix.drawLine(36, 12, 36, 19, MAGENTA);

  matrix.setCursor(40, 25);
  matrix.setTextColor(MAGENTA);
  matrix.print("4&25");
}
