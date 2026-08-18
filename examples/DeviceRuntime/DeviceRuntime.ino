unsigned long YourDeviceSeconds = 0;  // Replace "YourDevice" with "Pump", "Fan", "Relay", ...
bool YourDeviceRunning = false;       // Set true/false when the device is switched on/off

void setup()
{
}

void loop()
{
  updateYourDeviceSeconds();

  /*
    YourDeviceSeconds always contains the accumulated runtime of the device
    in complete seconds. The value can be displayed using Serial or a display,
    or used elsewhere in the application.

    Set YourDeviceRunning to true or false where your application switches
    the device on or off.
  */

  // rest of program
}

void updateYourDeviceSeconds()
{
  static unsigned long previousMillis = 0;
  static unsigned long accumulatedMillis = 0;

  unsigned long currentMillis = millis();
  unsigned long elapsedMillis = currentMillis - previousMillis;
  previousMillis = currentMillis;

  if (YourDeviceRunning)
  {
    accumulatedMillis += elapsedMillis;

    while (accumulatedMillis >= 1000UL)
    {
      accumulatedMillis -= 1000UL;
      YourDeviceSeconds++;
    }
  }
}
