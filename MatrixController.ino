void displayGameOption(String competition)
{
  matrix.fillScreen(BLACK);

  matrix.drawTriangle(4, 0, 0, 4, 8, 4, WHITE);
  matrix.drawTriangle(4, 31, 0, 27, 8, 27, WHITE);

  matrix.setCursor(10, 12);
  matrix.setTextColor(WHITE);
  matrix.print(competition);

  matrix.show();
}