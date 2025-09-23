# Module Issues Overview

This document describes the types of **issues** that can be reported by firmware running inside each module.  
Unlike **system errors**, which are detected by API-side checks, **module issues** are raised directly by the firmware when it detects abnormal internal conditions.

---

## Module Issue Types
If something abnormal is detected, the module will automatically report an **issue**.  
Each issue message includes:

- **ID** – a unique numeric code identifying the issue  
- **Index** – additional numeric field, e.g. to indicate a channel/index or other integer parameter related to the issue  
- **Value** – additional context, e.g. the actual temperature value for an `OverTemp` issue  

Below are the possible issue types:

---

### **Common (0–29)**

Affects all modules (Core, Control, Sensor).

- **[0] HighLoad** – CPU load exceeds `90 %`.  
- **[1] CoreOverTemp** – CPU temperature exceeds `70 °C`.  
- **[2] BoardOverTemp** – Board temperature exceeds `70 °C`.  
- **[3] LowEEPROMMemory** – Free EEPROM memory below `10 %`.  

---

### **Core (30–59)**

Specific to the Core module.

- **[30] Invalid5VSupply** – 5V supply voltage out of safe range (`4.8–5.2 V`).  
- **[31] InvalidVinSupply** – VIN voltage out of safe range (`10–14 V`).  
- **[32] InvalidPoESupply** – PoE supply voltage out of safe range (`10–14 V`).  
- **[33] OverCurrentDraw** – Current draw exceeds `3 A`.  
- **[34] OverPowerDraw** – Power draw exceeds `36 W`.  

---

### **Control (60–99)**

Specific to the Control module.

- **[60] LEDPanelOverTemp** – LED panel temperature exceeds `70 °C`.  
- **[61] HeaterOverTemp** – Heater plate temperature exceeds `80 °C`.  
- **[62] PumpInvalidSpeed** – Pump speed out of range (`-1.0–1.0`).  
- **[63] PumpInvalidFlowrate** – Pump flowrate out of range (`-1000–1000`). 
- **[64] AeratorOverSpeed** – Aerator speed out of range (`0–1.0`).
- **[65] AeratorInvalidFlowrate** – Aerator flowrate out of range (`10–5000`).   
- **[66] MixerOverSpeed** – Mixer speed out of range (`0–1.0`). 
- **[67] MixerOverRPM** – Mixer RPM exceeds `6000 RPM`.  

---

### **Sensor (100–139)**

Specific to the Sensor module.

- **[100] BottleOverTemp** – Bottle average temperature exceeds `70 °C`.  
- **[101] BottleTopOverMeasuredTemp** – Top measured bottle temperature exceeds `70 °C`.  
- **[102] BottleBottomOverMeasuredTemp** – Bottom measured bottle temperature exceeds `70 °C`.  
- **[103] BottleTopOverSensorTemp** – Top bottle sensor internal temp exceeds `70 °C`.  
- **[104] BottleBottomOverSensorTemp** – Bottom bottle sensor internal temp exceeds `70 °C`.  
- **[105] FluorometerDetectorOverTemp** – Fluorometer detector temperature exceeds `70 °C`.  
- **[106] FluorometerEmitorOverTemp** – Fluorometer emitter temperature exceeds `70 °C`.  
- **[107] SpectrophotometerEmitorOverTemp** – Spectrophotometer emitter temperature exceeds `70 °C`.  

---

## Reference

The full list of issue IDs and their corresponding `enum` definitions is maintained in the **SMBR-CANBUS-Codes** repository:
`SMBR-CANBUS-Codes/messages/module_issues/issue_enum.hpp`
