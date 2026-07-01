  # Industrial Motor Condition Monitoring & Fault Logging System
  
  ## Overview
  Bare-metal firmware for real-time industrial motor health monitoring using PIC16F877A.
  Developed in Embedded C using MPLAB X IDE and XC8 Compiler. Simulated and debugged in Proteus 8.
  
  ## Features
  - ⚡ Interrupt-driven RPM measurement using IR sensor + timer/counter
  - 🌡 LM35 temperature sensing via ADC
  - 📳 SW-420 vibration sensor for fault detection
  - 🕐 DS3232 RTC via I2C for real-time timestamping
  - 💾 24LC64 EEPROM via I2C for persistent fault event logging
  - 📡 UART serial terminal for real-time diagnostics
  - 📺 16x2 LCD for live status display
  
  ## Tech Stack
  | Component | Detail |
  |---|---|
  | MCU | PIC16F877A |
  | Language | Embedded C (bare-metal) |
  | IDE | MPLAB X IDE + XC8 Compiler |
  | Simulation | Proteus 8 |
  | Protocols | I2C, UART |
  | Sensors | LM35, SW-420, IR Speed Sensor |
  
  ## Hardware Architecture
  (Add your Proteus schematic screenshot here)
  
  ## How to Run
  1. Open project in MPLAB X IDE
  2. Build with XC8 Compiler
  3. Load .hex file into Proteus simulation
  4. Run simulation and observe UART terminal + LCD output
  
  ## Author
  Gokulan S | [LinkedIn](https://linkedin.com/in/s-gokulan-525900235)
