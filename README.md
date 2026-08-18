# Simple Runtime Counters

Small, independent runtime counters for long-running Arduino projects and Arduino-compatible platforms such as ESP8266 and ESP32.

Many microcontroller projects run for much longer than a typical test on the workbench.

A controller may operate continuously for months or years, and sooner or later a simple question becomes interesting:

**How long has something actually been running?**

That "something" could be the complete system, a pump, a fan, a heater, a relay, a motor, or almost any other part of the application.

There is nothing fundamentally new about counting runtime. The usual Arduino techniques based on `millis()` have been used for many years.

The interesting part is how little code is actually required to turn this into a practical and reusable tool.

With a small independent counter for each device, several runtimes can be tracked in parallel without a library, RTC, custom timer interrupt, or complicated common infrastructure.

For example:

```text
System:  1843921 seconds
Pump:     782280 seconds
Fan:     1264430 seconds
```

Each counter is independent, easy to understand, and can simply be copied into an existing sketch.

---

## Why Count Seconds?

A traditional operating-hours counter suggests storing hours directly.

For a microcontroller, seconds are much more useful.

An `unsigned long` can represent:

```text
4,294,967,295 seconds
```

which corresponds to approximately:

```text
136 years
```

That is more than enough for most long-running embedded applications while still providing one-second resolution.

The application can later decide how the value should be used or displayed.

Seconds can become minutes, hours, days, maintenance intervals, statistics, or simply remain seconds.

---

## A Simple System Uptime Counter

The simplest case is system uptime because the system is always running while the sketch is executing.

```cpp
unsigned long systemSeconds = 0;

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
```

Call the function continuously from `loop()`:

```cpp
void loop()
{
  updateSystemSeconds();

  // rest of program
}
```

The global variable

```cpp
systemSeconds
```

now contains the number of complete seconds since the sketch started.

The code does not reset `previousMillis` to the current value after every complete second. Instead, it advances it only by the milliseconds already converted into seconds. Any remaining fraction of a second is therefore preserved automatically.

There is no special rollover code.

The expression

```cpp
currentMillis - previousMillis
```

uses unsigned arithmetic and therefore also produces the correct elapsed time when `millis()` rolls over under the conditions explained below.

Instead of trying to prevent or detect the rollover, the code simply works across it.

---

## Counting the Runtime of a Device

Usually, however, we do not only want to know how long the complete system has been running.

Suppose a pump operates only occasionally.

For this case we add a Boolean variable that tells the counter whether the device is currently running:

```cpp
unsigned long YourDeviceSeconds = 0;  // Replace "YourDevice" with "Pump", "Fan", "Relay", ...
bool YourDeviceRunning = false;       // Set true/false when the device is switched on/off

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
```

Again, the function is simply called continuously:

```cpp
void loop()
{
  updateYourDeviceSeconds();

  // rest of program
}
```

Whenever the application switches the device on:

```cpp
YourDeviceRunning = true;
```

and when it switches the device off:

```cpp
YourDeviceRunning = false;
```

Only the time during which `YourDeviceRunning` is true is accumulated.

---

## `YourDevice` Is Intentionally a Placeholder

The unusual name `YourDevice` has a practical purpose.

Copy the complete counter into your sketch and use **Search and Replace**:

```text
YourDevice -> Pump
```

The code immediately becomes:

```cpp
unsigned long PumpSeconds = 0;
bool PumpRunning = false;

void updatePumpSeconds()
{
  ...
}
```

Copy the original block again and replace:

```text
YourDevice -> Fan
```

and you get:

```cpp
unsigned long FanSeconds = 0;
bool FanRunning = false;

void updateFanSeconds()
{
  ...
}
```

The same works for:

```text
YourDevice -> Heater
YourDevice -> Motor
YourDevice -> Relay
YourDevice -> Compressor
```

or whatever makes sense in the application.

The capitalization of `YourDevice` is therefore intentional. It provides one unique search term that can be replaced everywhere in the copied block at once.

---

## Several Counters Can Run in Parallel

This is where the simplicity becomes particularly useful.

A long-running controller might contain:

```cpp
void loop()
{
  updateSystemSeconds();
  updatePumpSeconds();
  updateFanSeconds();
  updateRelaySeconds();

  // rest of program
}
```

Each counter has its own variables and its own internal state.

There is no common counter manager and no dependency between them.

If another runtime becomes interesting later, copy the block once more, replace `YourDevice`, and add one function call to `loop()`.

This deliberately trades a small amount of repeated code for transparency.

For small embedded projects, being able to look at `updatePumpSeconds()` and immediately see everything involved in counting the pump runtime can be more useful than creating a more abstract solution simply to avoid a few repeated lines.

---

## Why the Function Must Be Called Continuously

It may seem attractive to write:

```cpp
if (PumpRunning)
  updatePumpSeconds();
```

but this creates a problem.

While the pump is stopped, the function would no longer update `previousMillis`. When the pump starts again, the next call would see the complete time since the previous call — including the time during which the pump was stopped.

Instead, call the function continuously:

```cpp
updatePumpSeconds();
```

and let:

```cpp
if (PumpRunning)
```

inside the function decide whether the elapsed time should be accumulated.

This keeps the time reference current even while the device is stopped.

*For accurate device runtimes, the update function should be called frequently. If the sketch contains long blocking operations, update the counter immediately before changing `YourDeviceRunning`.*

---

## What About `millis()` Rollover?

On typical Arduino platforms using a 32-bit `millis()` counter, `millis()` rolls over after approximately 49.7 days.

That sounds like a serious limitation for a long-running project, but it does not have to be.

The counter never tries to use the absolute value of `millis()` as its long-term runtime.

Instead, it repeatedly calculates:

```cpp
unsigned long elapsedMillis = currentMillis - previousMillis;
```

With unsigned arithmetic, this difference remains correct across a `millis()` rollover, as long as the function is called at least once within one complete `millis()` cycle. Calling it continuously from `loop()` normally satisfies this requirement automatically.

For the conditional device counter, the elapsed milliseconds are then accumulated only until they form complete seconds:

```cpp
while (accumulatedMillis >= 1000UL)
{
  accumulatedMillis -= 1000UL;
  YourDeviceSeconds++;
}
```

The long-term value is therefore `YourDeviceSeconds`, not `millis()`.

A `millis()` rollover becomes just another transition between two successive measurements.

---

## From Seconds to Operating Hours

The counter deliberately stores seconds rather than deciding how the runtime should be presented.

A classic operating-hours display is just one possible use.

For example, operating hours with one decimal place can be displayed without floating-point arithmetic:

```cpp
// This example shows operating hours with one decimal place

Serial.print("Operating hours: ");
Serial.print(YourDeviceSeconds / 3600UL);
Serial.print(".");
Serial.println((YourDeviceSeconds % 3600UL) / 360UL);
```

A result could look like:

```text
Operating hours: 217.3
```

The decimal digit is truncated rather than rounded, which is appropriate for an operating-hours counter: it does not display operating time that has not yet been reached.

This is only an example.

The same `YourDeviceSeconds` value could be used for maintenance intervals, statistics, display output, logging, or any other application-specific purpose.

---

## Counting Within a Time Interval

Sometimes both the total runtime and the runtime within a specific interval are useful.

A simple way to achieve this is to run a second independent counter in parallel. For example, one counter could continuously track the total runtime of a pump, while another tracks its runtime during the current 24-hour period.

Both counters can use the same `PumpRunning` state while maintaining their own runtime values independently.

The total counter continues without interruption, while the interval counter can be reset when a new 24-hour period begins.

This is only one possible application of the basic counter. The same idea could be used for a day, a week, a work shift, a production cycle, or any other application-defined period.

The important point is that the original counter does not need any additional interval logic. If another runtime value is useful, another small independent counter can simply run alongside it.

---

## Reading Runtime from Otherwise Inaccessible Devices

Runtime information becomes particularly useful in devices that operate unattended and do not have a permanently connected display or serial interface.

For example, the value can simply be transmitted using SerialLED:

```cpp
SerialLED.println(PumpSeconds);
```

The runtime counter itself does not know or care how its value is used.

Serial, SerialLED, a display, network communication, or persistent storage are all application decisions.

This separation keeps the counter small and reusable.

### SerialLED

https://github.com/InquisitiveMind123/SerialLED

---

## Persistent Operating Hours

The examples above count runtime since the current program start.

A traditional machine operating-hours counter often needs to retain its value across power cycles.

That can be added by periodically storing the seconds or another derived value in EEPROM, flash, FRAM, or another form of non-volatile storage.

This is deliberately **not** part of the runtime counter.

Persistent storage introduces additional questions such as write endurance, update intervals, power-loss behavior, and storage format. Those requirements depend strongly on the application.

The runtime counter has only one job:

**Accumulate runtime while the program is running.**

What happens to that information afterward remains up to the application.

---

## Nothing New — and That Is the Point

None of the individual techniques used here are new.

Using `millis()` for non-blocking timing is standard Arduino practice. Unsigned subtraction across a `millis()` rollover is well established. Accumulating the operating time of motors, pumps, heaters, and other devices has been done countless times.

The useful part is the combination and reduction.

A complete runtime counter requires only:

- one global seconds variable,
- one Boolean state variable for conditionally running devices,
- one small update function,
- and one function call in `loop()`.

Need another counter?

Copy the block, replace `YourDevice`, and continue.

A project can therefore track system uptime, pump runtime, fan runtime, relay runtime, heater runtime, or several other operating states simultaneously without introducing a framework or making the sketch difficult to follow.

Sometimes a useful embedded tool does not require a new algorithm.

Sometimes the improvement is simply making an old idea **small enough, clear enough, and convenient enough to use everywhere.**