void setupWifi()
{
    while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);

    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass),

    // wait 10 seconds for connection:
    delay(10000);
  }
}

JsonDocument getData(String path, int substring)
{
  path = path.substring(substring);
  client.stop();

  client.beginRequest();
  client.get(path);
  client.sendHeader("Host", "sports.core.api.espn.com");
  client.endRequest();

  int statusCode = client.responseStatusCode();
  String response = client.responseBody();

  JsonDocument doc;
  DeserializationError error = deserializeJson(doc, response, DeserializationOption::NestingLimit(15));

  if (error) {
    Serial.print("deserializeJson() failed: ");
    Serial.println(error.c_str());
    if (response)
    {
      Serial.println(response);
    }
    Serial.println(path);
    return doc;
  }

  client.stop();

  return doc;
}