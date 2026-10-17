void nflSetup()
{
  JsonDocument root = getData("/v2/sports/football/leagues/nfl", 0);

  JsonDocument currentSeason = getData(root["season"]["$ref"], SL);

  JsonDocument currentWeek = getData(currentSeason["type"]["week"]["$ref"], SL);

  JsonDocument currentEvents = getData(currentWeek["events"]["$ref"], SL);

  JsonDocument chosenEvent, competition, homeTeamComp, awayTeamComp, homeTeam, awayTeam;
  int loopIndex = 0;
  for(JsonVariant comp : currentEvents.as<JsonArray>())
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
  }
}