# System Error Overview

This document describes the types of errors the system might report during a status check. 

---

## Normal System Status

If everything is working correctly, the system will respond with:

**"System is operating normally. No errors detected."**

**Response Code:** `200 OK`

---

## System Error Types

If something goes wrong, the system will return an error message with code `422`, along with details about the error. 
Below are the possible error types:

---

### 1. **ModuleUnavailable**

One of the system’s key modules (sensor, control, core) is not responding.

**Error Code:** `ModuleUnavailable`

**Example Message:**  
> "One or more modules are unavailable"

---

### 2. **UnknownInstance**

The system detected a module instance labeled as All, Undefined, or Reserved.

**Error Code:** `UnknownInstance`

**Example Message:**  
> "Unknown module instance detected: Reserved"

---

### 3. **DuplicateInstance**

The system detected one or more modules sharing the same instance identifier.

**Error Code:** `DuplicateInstance`

**Example Message:**  
> "Detected more than one Instance_1 in module type sensor"

---

## Example Error Output

```text
[
  {
    "type": "ModuleUnavailable",
    "id": 1,
    "message": "One or more modules are unavailable",
    "detail": "Module sensor not responding"
  },
  {
    "type": "UnknownInstance",
    "id": 2,
    "message": "Unknown module instance detected: Reserved",
    "detail": "Module control reported instance value Reserved"
  },
  {
    "type": "DuplicateInstance",
    "id": 3,
    "message": "Multiple instances of the same module instance detected: Instance_1",
    "detail": "Detected more than one Instance_1 in module type sensor"
  }
]
```
