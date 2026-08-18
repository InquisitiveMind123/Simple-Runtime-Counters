unsigned long systemSeconds = 0;

unsigned long PumpSeconds = 0;
bool PumpRunning = false;

unsigned long FanSeconds = 0;
bool FanRunning = false;

void setup()
{
}

void loop()
{
  updateSystemSeconds();
  updatePumpSeconds();
  updateFanSeconds();

  /*
    systemSeconds always contains the current system uptime in complete seconds.

    PumpSeconds and FanSeconds contain the accumulated runtime of each device
    in complete seconds.

    The values can be displayed using Serial or a display, or used elsewhere
    in the application.

    Set PumpRunning and FanRunning to true or false where your application
    switches the corresponding device on or off.
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

void updatePumpSeconds()
{
  static unsigned long previousMillis = 0;
  static unsigned long accumulatedMillis = 0;

  unsigned long currentMillis = millis();
  unsigned long elapsedMillis = currentMillis - previousMillis;
  previousMillis = currentMillis;

  if (PumpRunning)
  {
    accumulatedMillis += elapsedMillis;

    while (accumulatedMillis >= 1000UL)
    {
      accumulatedMillis -= 1000UL;
      PumpSeconds++;
    }
  }
}

void updateFanSeconds()
{
  static unsigned long previousMillis = 0;
  static unsigned long accumulatedMillis = 0;

  unsigned long currentMillis = millis();
  unsigned long elapsedMillis = currentMillis - previousMillis;
  previousMillis = currentMillis;

  if (FanRunning)
  {
    accumulatedMillis += elapsedMillis;

    while (accumulatedMillis >= 1000UL)
    {
      accumulatedMillis -= 1000UL;
      FanSeconds++;
    }
  }
}
