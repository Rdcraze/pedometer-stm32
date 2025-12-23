# STM32 Intelligent Pedometer System (基于STM32的智能计步器)

A high-precision embedded pedometer system powered by STM32F407 and MPU6050, featuring advanced DSP algorithms, orientation-independent step counting, and a rich OLED user interface.

## 🌟 Key Features

### Core Pedometer Technology
*   **High-Precision Sampling**: 50Hz fixed-rate sampling using MPU6050 and hardware timer (TIM3).
*   **Orientation Independent**: Vector Magnitude ($Mag = \sqrt{x^2+y^2+z^2}$) calculation allows counting in any position (wrist, pocket, bag).
*   **Smart Filtering**:
    *   **High-Pass Filter (HPF)**: Cutoff at ~1.2Hz to remove gravity and DC components.
    *   **Anti-Shake State Machine**: 4-step buffer mechanism combined with time-interval regularity check (<40% variance) to eliminate false steps (e.g., typing, car bumps).

### Hardware Specifications
*   **MCU**: STM32F407IGT6 (Cortex-M4F @ 168MHz)
*   **Sensor**: InvenSense MPU6050 (6-Axis IMU) via Hardware I2C.
*   **Display**: 0.96" SSD1306 OLED (128x64) via High-Speed SPI.
*   **Storage**: Internal Flash (Sector 11) for data persistence.
*   **Input**: 3 x Mechanical Buttons with Debounce Logic.
*   **Feedback**: Passive Buzzer (Melodic Alerts) & LED Indicators.

### Advanced Metrics
*   **Adaptive Stride Length**: Dynamically calculates stride based on Height and Cadence (Walking vs Running).
*   **Real-time Speed/Pace**: 
    *   Speed: km/h
    *   Pace: min/km (selectable)
*   **Calories Burned**: Calculated using METs (Metabolic Equivalent of Task) table.
*   **Training Load**: Innovation metric based on sRPE (Session Rate of Perceived Exertion).

### User Interface & Experience
*   **Dual Language**: Support for **Chinese (中文)** and **English**.
*   **Animated UI**: Dynamic "Walking Person" animation speed-synced with real-time cadence.
*   **Data Persistence**: 
    *   Auto-save to Flash (Sector 11) every 5 minutes or 500 steps.
    *   Wear-leveling strategy and Integrity Checks (Magic Number `0xCAFEBABE` + Checksum).
*   **Smart Alerts**:
    *   **Target Reached**: Melodic celebration.
    *   **Sedentary Reminder**: Alerts after 30 minutes of inactivity.

---

## 🎮 Controls (Button Map)

| Key | Action | Function |
| :--- | :--- | :--- |
| **KEY1** (PI8) | **Double Click** | Toggle Language (CN/EN) |
| | **Triple Click** | Test Mode (Set steps to Target - 50) |
| | **Long Press** | **Reset All Data** (Clear steps, calories, etc.) |
| **KEY2** (PC13) | **Single Click** | Increase Target (+500) |
| | **Double Click** | Stopwatch (Start / Pause / Resume) |
| | **Triple Click** | Reset Stopwatch |
| | **Long Press** | **Demo Mode** (Simulate Walking/Running) |
| **KEY3** (PI11) | **Single Click** | Decrease Target (-500) |
| | **Double Click** | Toggle Pace Mode (km/h <-> min/km) |
| | **Long Press** | **Manual Save** (Force write to Flash) |

---

## 📊 Display Layout

**Main Screen:**
```
 步数 (Steps)
 12345 / 10000      <-- Progress vs Target
 
 速度 (Speed)       [DEMO] <-- Status Indicators
 5.2 km/h
 
 卡路里 (Cal) / 负荷 (Load)
 245 / 125 
 
 1.25km             <-- Distance
 [====>      ]      <-- Animated Progress Bar
```

---

## 🛠 Project Structure

*   `Program/`: Main application logic (`main.c`, `stm32f4xx_it.c`).
*   `Driver/`: Hardware abstraction layer.
    *   `pedometer.c`: Core step counting algorithm.
    *   `mpu6050.c`: I2C sensor driver.
    *   `oled.c`: SPI display driver and graphics.
    *   `key_handler.c`: Multi-press state machine.
    *   `flash_data.c`: Internal Flash storage management.
*   `diagrams.md`: Mermaid diagrams referencing system architecture.
*   `presentation_plan.md`: Detailed presentation slide deck plan.
*   `report.md`: Comprehensive project report.

## 📜 References
1.  **Step Detection**: Peak detection with adaptive time-window thresholding.
2.  **Calorie Calculation**: *Compendium of Physical Activities* (Ainsworth et al.).
3.  **Training Load**: Foster's Session RPE Scale.