<div align="center">

# 🎓 Smart Exam Hall Monitoring & Management System

**An embedded system for automated, secure, and real-time exam hall management**

[![Platform](https://img.shields.io/badge/Platform-LPC2148%20ARM7-blue?style=for-the-badge&logo=arm)](https://www.nxp.com)
[![Language](https://img.shields.io/badge/Language-Embedded%20C-brightgreen?style=for-the-badge&logo=c)](https://en.wikipedia.org/wiki/Embedded_C)
[![IDE](https://img.shields.io/badge/IDE-Keil%20%C2%B5Vision-orange?style=for-the-badge)](https://www.keil.com/)
[![Status](https://img.shields.io/badge/Status-Complete-success?style=for-the-badge)]()

</div>

---

## 📌 Overview

The **Smart Exam Hall Monitoring and Management System** is a real-time embedded application designed to automate and secure exam hall operations. Built on the **NXP LPC2148 ARM7TDMI-S microcontroller**, it integrates an RTC, keypad, LCD, 7-segment display, temperature sensor, LEDs, and a buzzer to manage everything from exam scheduling to environment monitoring — all behind a password-protected interface.

> 💡 This system eliminates manual timekeeping in exam halls by automating countdowns, alerts, and access control.

---

## ✨ Features

| Feature | Description |
|---|---|
| ⏱️ **Exam Countdown** | Automatically starts when RTC matches the set exam time; displays remaining time on 7-segment |
| ⏸️ **Pause & Resume** | EINT2 interrupt pauses the countdown; paused duration is excluded from total elapsed time |
| 🔐 **Password Protection** | 4-digit PIN (default: `1234`) guards all settings; PIN can be changed after authentication |
| 🌡️ **Temperature Monitoring** | LM35 sensor reads room temperature via ADC and displays in °C on LCD |
| 📅 **RTC Management** | Internal RTC for live time/date display; fully configurable via keypad |
| 💡 **LED Alerts** | Three-stage visual alert as exam time nears its end |
| 🔊 **Buzzer** | Sounds for 3 seconds when exam duration is over |
| 🖥️ **20×4 LCD Display** | Shows live time, date, temperature, and remaining duration simultaneously |
| ⌨️ **4×4 Keypad** | Full navigation and data entry for all settings |
| 🔢 **7-Segment Display** | Dedicated 2-digit display for exam countdown |

---

## 🚦 LED Alert Logic

```
Remaining Time        LED State
─────────────────     ──────────────────────────
> 15 minutes      →    All LEDs OFF
≤ 15 minutes      →    🟡 LED3 ON
≤ 10 minutes      →    🟠 LED2 ON  (LED3 OFF)
≤ 5 minute       →    🔴 LED1 ON  (LED2 OFF)
  0 minutes       →    🔊 BUZZER sounds for 3 sec
```

---

## 🏗️ System Architecture

<img width="1360" height="1240" alt="9acbb75f-2f01-4596-80e7-e62d117dbb41" src="https://github.com/user-attachments/assets/976310c1-121a-45a4-9d41-25616041e4e1" />


**Block overview:**
- **Interrupt sources** — `EINT0` (settings menu), `EINT2` (pause/resume), `Timer0 ISR` (7-seg multiplexer, 5ms tick)
- **Main loop (`exam.c`)** — RTC time/date display, exam start detection, elapsed-time calculation with pause offset, LED/buzzer control
- **Drivers & peripherals** — `lcd` · `kpm` · `seg` · `adc` · `lm35` · `rtc`
- **Security** — `password` module, invoked from the main loop when EINT0 fires

---

## 🔌 Hardware Components

| # | Component | Part / Spec | Role |
|---|---|---|---|
| 1 | Microcontroller | NXP LPC2148 (ARM7TDMI-S, 60 MHz) | Core processing |
| 2 | Display | 20×4 Character LCD (HD44780) | Time, date, temp, duration |
| 3 | Keypad | 4×4 Matrix Keypad | User input & navigation |
| 4 | 7-Segment | 2-Digit Common Anode (Multiplexed) | Countdown display |
| 5 | Temperature Sensor | LM35 (Analog, 10mV/°C) | Room temp monitoring |
| 6 | RTC | LPC2148 ON-Chip RTC | Real-time clock |
| 7 | LEDs | 3× General Purpose LEDs | Time-warning indicators |
| 8 | Buzzer | Active Buzzer | Exam-end alert |
| 9 | Push Buttons | 2× (EINT0, EINT2) | Settings / Pause-Resume |

---

## 📍 Pin Configuration

### GPIO — Port 0

| Pin | Label | Function |
|---|---|---|
| P0.5 | BUZZER | Buzzer output |
| P0.8 – P0.15 | LCD_DATA | 8-bit LCD data bus |
| P0.16 | LCD_RS | LCD register select |
| P0.17 | LCD_EN | LCD enable |
| P0.19 | DSEL1 | 7-seg digit select 1 |
| P0.20 | DSEL2 | 7-seg digit select 2 |
| P0.27 | LED1 | Red alert (≤1 min) |
| P0.28 | LED2 | Amber alert (≤3 min) |
| P0.29 | LED3 | Yellow alert (≤5 min) |

### GPIO — Port 1

| Pin | Label | Function |
|---|---|---|
| P1.16 – P1.19 | ROW0–ROW3 | Keypad row outputs |
| P1.20 – P1.23 | COL0–COL3 | Keypad column inputs |
| P1.24 – P1.31 | ca7seg_2_mux | 7-segment segment data |

### Special Function Pins

| Pin | Function |
|---|---|
| EINT0 | Settings menu trigger (falling edge) |
| EINT2 | Pause / Resume exam (falling edge) |
| ADC CH3 (P0.28) | LM35 analog input |

---

|VIRTUAL SIMULATION ARCHITECTURE IN PROTEUS|

---
<img width="800" height="500" alt="Screenshot 2026-07-19 213544" src="https://github.com/user-attachments/assets/05d6f01a-d978-49da-8c47-2fc3edf8d79b" />


---

## 📁 Project Structure

The codebase was refactored from a single monolithic file into a modular, multi-file structure — each peripheral driver and each app-level concern now lives in its own `.c`/`.h` pair for clean separation of compilation and easier maintenance.

```
Smart-Exam-Hall-System/
│
├── inc/                          ← Header files (declarations only)
│   ├── types.h                   ← Core typedefs (u8, s8, u32, s32, f32)
│   ├── defines.h                 ← Generic bit-manipulation macros (SETBIT, READBIT, etc.)
│   ├── rtc_defines.h             ← RTC prescaler constants (PCLK1, PREINT/PREFRAC)
│   ├── lcd_defines.h             ← HD44780 command codes + LCD pin mapping
│   ├── kpm_defines.h             ← Keypad row/column pin mapping
│   ├── seg_defines.h             ← 7-segment mux pin mapping
│   ├── adc_defines.h             ← ADC clock/channel/register-bit constants
│   ├── io_defines.h              ← LED + buzzer pin mapping
│   ├── all_macros.h              ← Umbrella header (LPC21xx.h + all defines above)
│   ├── lcd.h                     ← LCD driver API
│   ├── kpm.h                     ← Keypad driver API
│   ├── delay.h                   ← Busy-wait delay API
│   ├── seg.h                     ← 7-segment driver API
│   ├── adc.h                     ← ADC driver API
│   ├── lm35.h                    ← LM35 temperature sensor API
│   ├── rtc.h                     ← RTC driver API
│   ├── exam.h                    ← Exam state machine, ISRs, timer init
│   └── password.h                ← Password entry/verification API
│
├── src/                          ← Source files (implementations)
│   ├── lcd.c                     ← LCD driver (HD44780 control)
│   ├── kpm.c                     ← Keypad scan/decode logic
│   ├── delay_def.c               ← delay_us / delay_ms / delay_s
│   ├── seg.c                     ← 7-segment multiplex driver
│   ├── adc.c                     ← ADC init + read
│   ├── lm35.c                    ← LM35 → °C / °F conversion
│   ├── rtc.c                     ← RTC init, time/date display, EINT0 edit handler
│   ├── password.c                ← Password entry (masked PIN), verification, PIN reset
│   └── exam.c                    ← main(), EINT0/EINT2 ISRs, Timer0 ISR, exam countdown loop
│
└── Makefile / .uvproj            ← Build configuration
```
---

## ⚙️ How It Works

### 1️⃣ Boot & Initialization
```
Power ON → Initialize GPIO, LCD, Keypad, 7-Seg, ADC, Timer0, EINT0, EINT2
         → Set default RTC time/date
         → Display "SYSTEM LOADING..." → Enter Main Loop
```

### 2️⃣ Main Loop (`exam.c`)
- **Line 1:** Live RTC time `HH:MM:SS`
- **Line 2:** Live RTC date `DD/MM/YYYY`
- **Line 3:** Temperature `XX.XX °C` + PAUSE indicator (if paused)
- **Line 4:** `Duration: XX` (remaining minutes)
- **7-Seg:** Remaining minutes (multiplexed via Timer0 ISR)

### 3️⃣ Settings Access (EINT0 Button)
```
Press EINT0 → password() in password.c prompts for 4-digit PIN
            → On success: edit RTC time, exam start time, duration
            → edit_password() available to reset the PIN (1000–9999)
```
> ⚠️ EINT0 is **disabled** once an exam starts and **re-enabled** after it ends.

### 4️⃣ Exam Lifecycle
```
Set exam start time + duration via settings menu
         ↓
RTC matches exam start time
         ↓
Countdown begins → 7-seg counts down → LEDs trigger at thresholds
         ↓
        (optional) EINT2 to PAUSE → EINT2 again to RESUME
        (paused minutes are excluded from elapsed time)
         ↓
Duration reaches 0 → BUZZER sounds 3 sec → System resets for next exam
```

---

## ⏱️ Elapsed Time Calculation

The system uses a **total-minutes reference model** to accurately handle midnight rollovers and pauses:

```c
elapsed = (nowTotalMin - examStartTotalMin) - pause;
dur = (elapsed >= tempTime) ? 0 : tempTime - elapsed;
```

- `examStartTotalMin` — captured once at exam start (in total minutes from midnight)
- `pause` — accumulates paused minutes across multiple pause/resume cycles
- `flag2 % 2 == 0` → Resuming; `flag2 % 2 == 1` → Pausing

---

## 🔐 Password System (`password.c` / `password.h`)

- Default PIN: **`1234`**
- `password()` — masks each digit as `*` on the LCD while entering; `+` acts as backspace, `c` cancels; confirms entry with `=`
- `edit_password()` — resets the PIN to a new 4-digit value (`1000–9999`) via `ReadNum()`
- Compares entered PIN against the global `save_pass` (declared in `exam.c`, referenced via `extern` in `password.c`)

---

## 🌡️ Temperature Monitoring

```
LM35 Output → ADC Channel 3 → 10-bit conversion (0–1023)
→ eAR = (3.3V / 1024) × ADC_value
→ Temperature (°C) = eAR × 100
→ Displayed on LCD Line 3 with 2 decimal places
```

---

## 🛠️ Development Environment

| Tool | Details |
|---|---|
| **IDE** | Keil µVision 4/5 (ARM-MDK) |
| **Compiler** | Keil ARM C Compiler |
| **Target MCU** | NXP LPC2148 — ARM7TDMI-S @ 60 MHz |
| **PCLK** | 15 MHz |
| **Flash Tool** | Flash Magic (UART ISP) |
| **Debugger** | JTAG / Keil simulator |
| **Language** | Embedded C (non-standard extensions: `__irq`) |

---

## 🚀 Getting Started

### Prerequisites
- Keil µVision IDE installed
- LPC2148 development board
- Flash Magic (for flashing via UART) or JTAG programmer

### Steps

```bash
# 1. Clone the repository
git clone https://github.com/YOUR_USERNAME/Smart-Exam-Hall-System.git
cd Smart-Exam-Hall-System

# 2. Open in Keil µVision
#    File → Open Project → select the .uvproj file

# 3. Add ALL source files under src/ to the project
#    lcd.c, kpm.c, delay_def.c, seg.c, adc.c, lm35.c,
#    rtc.c, password.c, exam.c
#    (Project → Manage → Project Items, or right-click group
#     → Add Existing Files to Group)

# 4. Add inc/ to the include search path
#    Project → Options for Target → C/C++ → Include Paths → add "inc"

# 5. Set target to LPC2148
#    Project → Options for Target → Device: LPC2148

# 6. Build
#    Project → Build Target  (Shortcut: F7)

# 7. Flash to board
#    Use Flash Magic with UART or JTAG debugger
```

> ⚠️ **Common pitfall:** if you see linker errors like `Undefined symbol Kpm_init`, it means the corresponding `.c` file (e.g. `kpm.c`) was never added to the Keil project — declaring a function in a header is not enough, its `.c` implementation must be compiled and linked too.

---

<div align="center">

**Built with ❤️ on ARM7 | LPC2148 | Embedded C**

</div>
