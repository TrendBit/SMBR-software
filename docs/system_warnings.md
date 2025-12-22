# System Warning Overview

This document describes the types of warnings the system might report during a status check.  

---

## Normal System Status

If no warnings are detected, the system will respond with:

**"System is operating normally. No warnings detected."**

**Response Code:** `200 OK`

---

## System Warning Types

If a warning is detected, the system will return a warning message with code `200`, along with details about the warning.  
Below are the possible warning types:

---

### 1. **FirmwareVersionMismatch**

Firmware versions differ between modules of different types (excluding core modules).  

**Warning Code:** `FirmwareVersionMismatch`

**Example Message:**  
> "Firmware versions differ between modules of different types (excluding core modules)"

---

### 2. **DirtyBuildFirmware**

A module is running firmware that was built from a dirty state.  

**Warning Code:** `DirtyBuildFirmware`

**Example Message:**  
> "Dirty build firmware detected on a module"

---

### 3. **CANBusUnreachable**

The CAN bus appears unreachable, with no transmitted or received packets.  

**Warning Code:** `CANBusUnreachable`  

**Example Message:**  
> "No packets observed on CAN bus (possibly unreachable)"

---

### 4. **CANBusErrorRateHigh**

The CAN bus error rate is above the safe threshold.  

**Warning Code:** `CANBusErrorRateHigh`  

**Example Message:**  
> "CAN bus error rate above threshold"

---

### 5. **ModuleHighPing**

High ping time detected (>500 ms) for one of the modules (core, sensor, control).  

**Warning Code:** `ModuleHighPing`  

**Example Message:**  
> "High ping time detected for module"


