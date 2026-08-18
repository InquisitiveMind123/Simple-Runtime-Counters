unsigned long systemSeconds = 0;

void setup()
{
}

void loop()
{
  updateSystemSeconds();

  /*
    systemSeconds always contains the current system uptime in complete seconds.
    The value can be displayed using Serial or a display, or used elsewhere
    in the application.
  */

  // rest of program
}

void updateSystemSeconds()
{
  static unsigned long previousMillis = 0;

  unsigned long currentMillis = millis();
  unsigned long elapsedMillis = currentMillis - previousMillis;

  if (elapsedMillis >= 1000UL)
  {
    unsigned long elapsedSeconds = elapsedMillis / 1000UL;
    systemSeconds += elapsedSeconds;
    previousMillis += elapsedSeconds * 1000UL;
  }
}
