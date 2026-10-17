const int SL = 31; // Substring location

void nflSetup()
{
  JsonDocument root = getData("/v2/sports/football/leagues/nfl", 0);
  JsonDocument currentSeason = getData(root["season"]["$ref"], SL);
  JsonDocument currentWeek = getData(currentSeason["type"]["week"]["$ref"], SL);
  JsonDocument currentEvents = getData(currentWeek["events"]["$ref"], SL);
  
  JsonDocument chosenEvent, competition, homeTeamComp, awayTeamComp, homeTeam, awayTeam;
  int loopIndex = 0;
  for(JsonVariant comp : currentEvents["items"].as<JsonArray>())
  {
    chosenEvent = getData(comp["$ref"], SL);
    competition = getData(chosenEvent["competitions"][0]["$ref"], SL);

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

    homeTeam = getData(homeTeamComp["team"]["$ref"], SL);
    awayTeam = getData(awayTeamComp["team"]["$ref"], SL);

    SportEvent nflEvent;
    nflEvent.eventName = awayTeam["abbreviation"].as<String>() + " @ " + homeTeam["abbreviation"].as<String>();
    nflEvent.drivesRef = competition["drives"]["$ref"].as<String>();
    nflEvent.awayTeam = awayTeam;
    nflEvent.homeTeam = homeTeam;
    nflEvent.week = currentWeek["number"];

    nflEvents[loopIndex] = nflEvent;
    loopIndex++;
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

  displayNFLScore();

  matrix.show();
}

void displayNFLScore()
{
  JsonArray currentDrives = getData(nflEvents[currentNflEventIndex].drivesRef, SL).as<JsonArray>();
  size_t currentDrivesSize = currentDrives.size();
  JsonDocument currentDrive = currentDrives[currentDrivesSize - 1];
  
  size_t driveListSize = driveList.size();
  if (driveListSize == 0 
    || driveList[driveListSize - 1]["id"].as<String>() != currentDrives[currentDrivesSize - 1]["id"].as<String>())
  {
    //This means a new drive is starting!

    //Update the drive list to have the current drive
    //TODO: Figure out why I am did it this way instead of just adding the drive to the end of the list in the original code
    driveList.clear();
    for(JsonDocument d : currentDrives)
    {
      driveList.add(d);
    }

    //Reset plays for the new drive
    plays.clear();
  }

  JsonArray currentPlays = getData(currentDrive["$ref"], SL)["plays"]["items"].as<JsonArray>();

  if (currentPlays.size() == plays.size()) return; //No new plays have happened yet;

  bool playIdExists = false;
  for (JsonVariant play : currentPlays)
  {
    for (JsonVariant existingPlay : plays)
    {
      if (existingPlay["id"].as<String>() == play["id"].as<String>())
      {
        playIdExists = true;
        break;
      }
    }

    if (!playIdExists)
    {
      plays.add(play["id"].as<String>());

      if (!(play["type"]["text"].as<String>() == "Kickoff"
          || play["type"]["text"].as<String>() == "Official Timeout"
          || play["type"]["text"].as<String>() == "Pass Incompletion"
          || play["type"]["text"].as<String>() == "Timeout"
          || play["type"]["text"].as<String>() == "End Period"
          || play["type"]["text"].as<String>() == "Two-minute warning"
          || play["type"]["text"].as<String>() == "End of Half"
          || play["type"]["text"].as<String>() == "End of Game"))
      {
        displayPlay(play);
      }
    }

    playIdExists = false;
  }

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

void displayPlay(JsonDocument play)
{
  Serial.print(play["type"]["text"].as<String>());
  Serial.print(" from ");
  Serial.print(play["start"]["yardline"].as<String>());
  Serial.print(" to ");
  Serial.print(" on ");
  Serial.print(play["start"]["down"].as<String>());
  Serial.print("&");
  Serial.print(play["start"]["distance"].as<String>());
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
