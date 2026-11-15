# AGENTS.md

Guidance for AI coding agents working in this repository.

## What this project is

Custom hardware and firmware that converts conventional (AC track-powered, Lionel-style O-gauge) model locomotives into WiFi-controlled locomotives. Every device is an **ESP8266** (Wemos D1 Mini, or NodeMCU v2) talking over **MQTT** to a broker at `mqtt.local:1883`. Locomotives run a physics simulation onboard, drive motors through a PID loop closed on a homemade Hall-effect tachometer, and stream state for a separate sound board and a physical control panel.

Currently configured locomotives: Union Pacific **UP1292** and **UP8805**, both **Alco RS-3**.

## Repository layout

| Path | What it is | Toolchain |
|---|---|---|
| `LocoEngine/` | Engine microcontroller firmware (motor, smoke, lights, tach, simulator, PID). Runs inside each loco. | PlatformIO, Arduino/ESP8266 |
| `LocoSounds/` | Sound microcontroller firmware. Plays WAVs from LittleFS over I2S, driven by engine MQTT attributes. Runs inside each loco. | PlatformIO, ESP8266Audio |
| `LocoPanel/` | Physical control panel firmware: OLED loco selector, motorized throttle slider and rotary knobs (force-feedback detents), buttons with LEDs, all over Qwiic/I2C. | PlatformIO, SparkFun/Adafruit libs |
| `TachCalibrator/` | Bench tool firmware for testing/calibrating a truck's Hall-effect tach. Publishes `measuredwheelrpms`. | PlatformIO |
| `LocoCLI/` | .NET Core 3.1 command-line tool (`scan`, `listen`, `send`, `reset`, `republish`, `lashup`). | `dotnet`, MQTTnet 3.0.15, CommandLineParser 2.8.0 |
| `Documentation/` | Per-board READMEs with Fritzing (`.fzz`) files, schematic/PCB/breadboard images, and a step-by-step loco retrofit guide. | Fritzing |
| `3d Printed Parts/` | STLs for Alco RS-3 undercarriage (plastic-frame and metal-frame variants). | — |

Each firmware folder is an independent PlatformIO project (`platformio.ini`, `include/`, `src/`, `lib/`). There is no shared library between them: code such as `tachDriver` and the `MqttHandler` WiFi/OTA/reconnect boilerplate is **copy-pasted** across projects. When fixing a bug in one copy, check the others (`grep -r` the function name) and decide deliberately whether to propagate it. Note the TachCalibrator's `tachDriver` intentionally differs from LocoEngine's (low-speed adjustment enabled, different timeout and anomaly thresholds).

## Building

Neither PlatformIO nor the .NET SDK is installed in a fresh cloud container. You can install PlatformIO with `pip install platformio` if you need to compile-check.

### Required secret header: `wifi.h`

Every firmware project `#include "wifi.h"` from `config.h`. It is **gitignored and never committed**. To build, create `include/wifi.h` in the project:

```c
#define WIFI_SSID     "your-ssid"
#define WIFI_PASSWORD "your-password"
```

Never commit this file or put real credentials anywhere in the repo.

### Firmware

```bash
cd LocoEngine && pio run                 # default env: UP1292
cd LocoEngine && pio run -e UP8805
cd LocoEngine && pio run -e d1_mini -t upload   # USB upload, generic board
cd LocoSounds && pio run -t buildfs -e UP1292   # build LittleFS image of data/*.wav
cd LocoSounds && pio run -t uploadfs -e UP1292  # upload sound files
cd LocoPanel  && pio run                 # single env: nodemcuv2
cd TachCalibrator && pio run             # env: d1_mini
```

- The per-loco environments (`UP1292`, `UP8805`) upload **over the air** (`upload_protocol = espota`) to hard-coded LAN IPs. Uploading only works on the owner's home network; in a sandbox, only `pio run` (compile) is meaningful. Never run upload targets unless asked.
- The `USER_DEVICE_NETWORK_ID`, `LOCO_TYPE`, and `ROAD_NUMBER` macros are injected by `build_flags` in `platformio.ini`; they are not defined in source. Compiling a project without the right env gives confusing errors.
- `LocoSounds` uses a 4 MB flash layout with 3 MB LittleFS (`eagle.flash.4m3m.ld`). Raw source audio lives in the gitignored `LocoSounds/data/Raw files`.
- `LocoSounds/src/main.cpp` includes `SimpleTimer.h` but `SimpleTimer` is not in that project's `lib_deps`; it relies on `lib_ldf_mode = deep+` / a globally installed copy. Add it to `lib_deps` if a clean build fails.

### LocoCLI

```bash
cd LocoCLI && dotnet build
cd LocoCLI && dotnet run -- scan
cd LocoCLI && dotnet run -- send -n UP1292 -c throttle -v 50
dotnet publish -c Debug --runtime osx-x64   # how the owner builds it (macOS)
```

The broker host `mqtt.local` is hard-coded in `LocoClient.cs`. `LocoClientFactory.cs` is an empty placeholder.

### Tests

There are no automated tests and no CI. The `test/` folders are PlatformIO placeholders. Verification is compiling with `pio run` / `dotnet build` and then testing on real hardware, which only the owner can do. Say so explicitly when you hand back firmware changes, and call out anything that affects motor control.

## Architecture: LocoEngine

`main.cpp` builds all objects as globals and runs `processStep()` every `PHYSICS_DELTAT` (15 ms) via `SimpleTimer`; `loop()` services MQTT/OTA and switches control models.

Per tick: `BatteryDriver` (reads track voltage on A0) → active `IControlModel::ProcessStep()` → motor speed (PID or direct) → `SmokeDriver` → `TachDriver` → `LightingDriver` → `SoundController` → `MqttHandler::ProcessStep()` (publishes attributes).

**Control models** (`IControlModel` implementations, chosen at runtime by ID):

| ID | Class | Behavior |
|---|---|---|
| 1 | `SimulatorControlModel` | Physics sim: engine spin-up lag, rolling/air/brake resistance, can't reverse while moving, smoke from over-throttle and engine work. Default (`SIMULATOR_CONTROL_MODEL` in `config.h`). |
| 2 | `ToyControlModel` | Throttle maps straight to speed; engine always on; no brake. |
| 3 | `MuSlaveControlModel` | Multiple-unit slave: mirrors engine RPM/percent, commanded wheel RPM, reverser, engine-on, and master switch from a master loco. Entered only through the `lashup` command (`setcontrolmodel` accepts only 1 or 2). |

The model only changes when the loco is nearly stopped (`GetSpeedMph() < 1`).

**Speed control** (`config.h`): `PID_SPEED_CONTROL` (default) feeds the model's `GetEstimatedWheelRpms()` as the setpoint and `TachDriver::GetWheelRpm()` as the process variable into `PidController`. The output is then smoothed (`prev + (new-prev)/3`) and sent to `MotorDriver`. `DIRECT_SPEED_CONTROL` bypasses the tach.

**Configuration layering**:
- `include/config.h`: global feature switches, `PHYSICS_DELTAT`, MQTT server, and the `LOCO_TYPE` / `ROAD_NUMBER` include selection.
- `include/locoTypes/<type>.h`: per-model physics, smoke, engine RPM range, wheel-RPM↔speed% linear fit, PID gains.
- `include/roadNumbers/<number>.h`: per-unit motor PWM range (`MIN_SPEED_PWM` / `MAX_SPEED_PWM`), because individual motors differ.

To add a new locomotive, add a `roadNumbers/xxxx.h`, a numeric `#define` and `#if ROAD_NUMBER == ...` block in `config.h`, and a `[env:XXXX]` in `platformio.ini`. Do the same in `LocoSounds/platformio.ini`. For a new loco type, add `locoTypes/*.h` in **both** LocoEngine and LocoSounds, plus the WAV files.

**Pins** are `#define`d at the top of each driver header, with separate branches for `ARDUINO_ESP8266_NODEMCU` and `ARDUINO_ESP8266_WEMOS_D1MINI`. Wemos: motor fwd/back D1/D2, smoke D5, tach D6 (interrupt), cab D3, front D7, rear D8, track voltage A0, status LED GPIO2 (active-low: 0 = connected). The pins must match the Engine Hat PCB in `Documentation/EngineMicrocontroller/`. Don't change them without a hardware reason.

## MQTT protocol (the contract between all components)

All topics are under `locomotives/<ROADNUMBER>/`. Payloads are plain-text numbers (`"1"`, `"0"`, `"42.50"`). Nothing is published retained.

**Commands** (subscribed by LocoEngine; sent by LocoPanel, LocoCLI, or anything else):

| Topic | Payload |
|---|---|
| `commands/masterswitch` | 0/1. Gates everything: engine, lights, bell, horn are forced off when 0. |
| `commands/engineon` | 0/1 (ignored unless master switch is on) |
| `commands/throttle` | 0–100 (panel sends 8 notches: multiples of 12.5) |
| `commands/brake` | 0–100 |
| `commands/reverser` | -1, 0, 1 |
| `commands/headlights` | 0 off, 1 front, 2 rear, 3 both, 4 auto (follows reverser) |
| `commands/cablights` | 0/1 |
| `commands/bell` | 0/1 (auto-off after 3000 ticks) |
| `commands/horn` | 0/1 (auto-off after 1000 ticks) |
| `commands/disablesmoke` | 0/1 (smoke is **disabled by default** at boot) |
| `commands/disablesounds` | 0/1 (consumed by LocoSounds) |
| `commands/setcontrolmodel` | 1 or 2 |
| `commands/lashup` | master road number, e.g. `UP1292`. Switches to model 3 and subscribes to the master's attributes. |
| `commands/reset` | anything. Stops motors, then `ESP.restart()`. LocoSounds and TachCalibrator also restart on it. |
| `commands/republish` | anything. Republishes all attributes and commands. |

**Attributes** (published by LocoEngine): `masterswitch`, `engineon`, `throttle`, `brake`, `reverser`, `headlights`, `cablights`, `bell`, `horn`, `disablesmoke`, `enginerpms`, `enginepercent`, `speedpercent`, `speedmph`, `smokepercent`, `commandedwheelrpms`, `measuredwheelrpms`, `battery`, `controlmodel`, and PID debugging (`pidError`, `pidBTerm`, `pidPTerm`, `pidITerm`, `pidDTerm`, `pidControlValue`). These are published on change and/or on prime-number modulo counters, so they're staggered rather than bursty. `PUBLISH_*` flags in `config.h` gate groups of them.

**Status/other**: `engineStatus` / `audioStatus` / `panelStatus` / `tachCalibratorStatus` (`Rebooted`, `Reconnected`, `OK`), `engineIp` / `audioIp` / `panelIp`, `audioFreeHeap`. The discovery topic is the global `locomotives/discovery`, with the payload set to the road number every ~500 ticks.

**Who listens to what**:
- LocoSounds (client ID `<ROADNUMBER>AUDIO`) subscribes to `attributes/bell`, `attributes/horn`, `attributes/enginerpms`, `commands/disablesounds`, and `commands/reset`. It reacts to *attributes* (engine state after the master-switch gating), not commands.
- LocoPanel subscribes to `locomotives/discovery`, builds its loco list from it, then subscribes to each loco's state attributes. It publishes *commands* for the selected loco.
- On boot and on `republish`, LocoEngine publishes its current state back onto its own **command** topics (`republishCommands()`). This resets stale values and makes sure the engine state and command topics agree. Keep that in mind when adding commands: add the subscribe in `reconnect()`, the handler in the `Setup()` callback, and the republish line.

If you add or rename a topic, update every component that uses it: LocoEngine, LocoSounds, LocoPanel (`mqttHandler`, `locoState`, `locoStateCache`), LocoCLI, and this file.

## Architecture: LocoPanel

- A 15 ms `processStep` reads the selected loco's `LocoState` from `LocoStateCache` and calls each `*Controller::ProcessStep(state)`.
- Hardware is all I2C: two `TCA9534` GPIO expanders (0x27 = bell/horn/cablights, 0x26 = master switch/engine-on), an `ADS1015` ADC (ch0 headlights knob, ch1 reverser knob, ch2 throttle slider), and two SparkFun `SCMD` motor drivers (0x5D = headlights/reverser motors, 0x5E = throttle slider motor). An SSD1306 OLED is at 0x3C. The next-loco button is on D7. `setup()` blocks forever if a device is missing.
- Motorized controls use a `ControllerMode` state machine. In `Resetting`, a newly selected loco drives the physical control to that loco's current value. In `AcceptingInput`, the motor snaps the control to the nearest detent and publishes a command only when the detent changes. The thresholds and detent positions are tuned constants in the headers.
- Buttons are edge-triggered on the press (1→0). LEDs mirror the cached attribute state, not the local press.
- Built with `-fpermissive`.

## Coding conventions

- C++ for Arduino: classes with `Setup()` + `ProcessStep()` (and `Loop()` for MQTT). Collaborators are injected by reference through the constructor. Globals are wired in `main.cpp`.
- Private members are named `_camelCase` (some older code uses bare `camelCase`). Methods are `PascalCase`, file names `camelCase.cpp/.h`, and constants are `UPPER_SNAKE` `#define`s.
- Include guards are `#ifndef NAME_H`, often placed *after* the `#include` lines. Follow the surrounding file.
- Topic strings are built with literal concatenation: `"locomotives/"USER_DEVICE_NETWORK_ID"/commands/x"`. This compiles with a GCC `-Wliteral-suffix` warning. Match the existing style when editing those lines, or use spaces (`"locomotives/" USER_DEVICE_NETWORK_ID "/..."`), which is warning-free.
- Code that runs on the timer tick must not block. Avoid `delay()` outside setup/reset paths, avoid heap churn in hot paths (`String` is already used heavily; don't make it worse), and mark ISR code `ICACHE_RAM_ATTR`.
- Timing constants are counted in ticks, so changing `PHYSICS_DELTAT` changes real-world durations. For example, the bell's "30 seconds" is 3000 ticks, which is ~45 s at 15 ms, and the publish intervals scale the same way.
- C#: `LocoCLI` uses `CommandLineParser` verbs. Add a verb by creating an `*Options` class in `CommandLineOptions/`, adding it to the `ParseArguments<...>`/`MapResult` lists in `Program.cs`, and adding a `Run*AndReturnExitCode` method.

## Safety notes

This firmware drives real motors on a live track and a heated smoke unit.

- Fail toward stopped: zero motor output, smoke disabled. Preserve the reset path that drives D1/D2 low before restarting, and the "must be stopped to change direction or model" rules.
- Be conservative with PID gains, PWM ranges, and smoke PWM. The changes are untestable here, so describe them clearly to the owner rather than tuning blind.
- Pin, PWM-frequency (`analogWriteFreq(16384)`), and I2C address changes must match the physical boards documented in `Documentation/`.

## Documentation

- The root `README.md` is an index of `Documentation/*/README.md`. When adding a new board or doc page, link it there.
- Images live beside each README (often in `Images/`). The `.fzz` files are binary Fritzing projects; don't edit them as text.
- `Documentation/WhistleController/` describes a prototype (an ESP8266 that drives a conventional sound unit's horn/bell through DC offset) that has no firmware in the repo yet. It's meant to listen to the same MQTT topics as LocoSounds.
