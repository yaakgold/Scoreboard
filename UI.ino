void setupUI()
{
  setupButtons();
}


// Buttons
void setupButtons()
{
  pinMode(DOWN_BTN_PIN, INPUT_PULLUP);
  pinMode(UP_BTN_PIN, INPUT_PULLUP);
  pinMode(ENTER_BTN_PIN, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(DOWN_BTN_PIN), downISR, CHANGE);
  attachInterrupt(digitalPinToInterrupt(UP_BTN_PIN), upISR, CHANGE);
  attachInterrupt(digitalPinToInterrupt(ENTER_BTN_PIN), enterISR, CHANGE);
}

void downISR()
{
  downBtnState = digitalRead(DOWN_BTN_PIN);
  if (downBtnState == LOW && downCanUpdate)
  {
    downCanUpdate = false;
    lastDownDebounced = millis();

    downBtnFunc();
  }
}

void upISR()
{
  upBtnState = digitalRead(UP_BTN_PIN);
  if (upBtnState == LOW && upCanUpdate)
  {
    upCanUpdate = false;
    lastUpDebounced = millis();

    upBtnFunc();
  }
}

void enterISR()
{
  enterBtnState = digitalRead(ENTER_BTN_PIN);
  if (enterBtnState == LOW && enterCanUpdate)
  {
    enterCanUpdate = false;
    lastEnterDebounced = millis();

    enterBtnFunc();
  }
}