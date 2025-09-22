# 5V to 4.2V Power Supply Schematic & Connections

This document details the complete schematic, component list, pinouts, and connections for the 5V to 4.2V buck converter power supply for the ESP32 and LTE module.

---

## 1. Final Component List

| Component | Part Number / Value | Package | Notes |
|---|---|---|---|
| **Regulator** | LM2596SX-ADJ | TO-263-5 | Main switching controller |
| **Diode** | SS34_C8678 | SMC (DO-214AB) | 3A, 40V Schottky Diode |
| **Inductor** | DMBJ CXP1265-101M-HG | SMD, D13.5xL12.5mm | 100µH, 130mΩ DCR, 4.5A Isat |
| **Input Cap (C1)** | 220µF, 25V | SMD Polymer | e.g., D6.3xL7.7mm |
| **Output Cap (C2)**| 470µF, 10V+ | SMD Polymer | (To be selected) |
| **Resistor (R1)** | 1.0kΩ ±1% | 1206 | Feedback to ground |
| **Resistor (R2)** | 2.4kΩ ±1% | 1206 | Feedback from output |

---

## 6. Bill of Materials (BOM)

| Component | Part Number / Value | Quantity | Status | Notes |
|---|---|---|---|---|
| **Regulator** | LM2596SX-ADJ | 1 | ✅ Confirmed | Main switching controller |
| **Diode** | SS34_C8678 | 1 | ✅ Confirmed | 3A, 40V Schottky Diode |
| **Inductor** | DMBJ CXP1265-101M-HG | 1 | ✅ Confirmed | 100µH, 130mΩ DCR, 4.5A Isat |
| **Input Cap (C1)** | 220µF, 25V SMD Polymer | 1 | ✅ Confirmed | e.g., D6.3xL7.7mm |
| **Output Cap (C2)** | 470µF, 10V+ SMD Polymer | 1 | ❌ Not Confirmed | To be selected |
| **Resistor (R1)** | 1.0kΩ ±1% 1206 | 1 | ✅ Confirmed | Feedback to ground |
| **Resistor (R2)** | 2.4kΩ ±1% 1206 | 1 | ✅ Confirmed | Feedback from output |

---

## 2. ASCII Schematic

```text
 5V DC IN ───┬───────────────────┬───────────────────┬───▶ To ESP32 (+5V)
             │                   │                   │
           C1 (+)                │                   │
         ┌───┴───┐             ┌─┴─┐ L1 (100µH)    ┌─┴─┐ C2 (+)
         │ 220µF │             │ 1 │               │   │
         └───────┘             │ V │   LM2596SX-ADJ│ 2 ├───●───●───▶ To LTE Module (+4.182V)
           C1 (-)              │ I │               │ O │   │   │
             │                 │ N │               │ U │   │ ┌─┴─┐
             │                 └─┬─┘               │ T │   │ │470│
             │                   │                 └─┬─┘   │ │µF │
             │                   │                   │     │ └─┬─┘
             │                   │                   │     │   │ C2 (-)
 GND ────────┼───────────────────┼───────────────────┼─────┼───┴───▶ GND
             │                   │                   │     │
             │                 ┌─┴─┐ 3               │     │ D1 (SS34)
             │                 │ G │                 │     ├───────┐
             │                 │ N │                 │     │       │
             │                 │ D │                 │     │      (+)▼(-)
             │                 └─┬─┘                 │     └───────┘
             │                   │                   │
             │                 ┌─┴─┐ 5               │ R2 (2.4kΩ)
             │                 │ O │                 ├────────●
             │                 │ N#│                 │        │
             │                 │ / │                 │      ┌─┴─┐ 4
             │                 │ O │                 │      │ F │
             │                 │ F │                 │      │ B │
             │                 │ F │                 │      └─┬─┘
             │                 └─┬─┘                 │        │
             │                   │                   │        │ R1 (1.0kΩ)
             └───────────────────┴───────────────────┴────────┴──────────▶ GND

```

---

## 3. Component Pinouts

### LM2596SX-ADJ (TO-263-5)

- **Pin 1:** VIN (Input voltage)
- **Pin 2:** OUTPUT (Switching output)
- **Pin 3:** GND (Ground)
- **Pin 4:** FB (Feedback)
- **Pin 5:** ON#/OFF (Enable - Active LOW)
- **Pin 6 (TAB):** GND (Thermal pad)

### SS34 Diode (SMC Package)

- **Anode (+):** Connects to Ground.
- **Cathode (-):** Marked with a band. Connects to the switching node.

---

## 4. Pin-to-Pin Connection List

### Power Input (5V DC Adapter)

- **5V Adapter (+)** → `LM2596SX Pin 1 (VIN)`
- **5V Adapter (+)** → `C1 Positive terminal`
- **5V Adapter (+)** → `ESP32 VCC`

- **5V Adapter (-)** → `LM2596SX Pin 3 (GND)`
- **5V Adapter (-)** → `LM2596SX Pin 6 (GND thermal pad)`
- **5V Adapter (-)** → `C1 Negative terminal`
- **5V Adapter (-)** → `C2 Negative terminal`
- **5V Adapter (-)** → `R1 Pin 2 (bottom)`
- **5V Adapter (-)** → `ESP32 GND`
- **5V Adapter (-)** → `LTE Module GND`
- **5V Adapter (-)** → `D1 Anode (+)`

### Enable Control (Always-On)

- **LM2596SX Pin 5 (ON#/OFF)** → `5V Adapter (-)` (Connect to Ground to enable)

### Switching Circuit

- **LM2596SX Pin 2 (OUTPUT)** → `L1 Pin 1 (input)`
- **LM2596SX Pin 2 (OUTPUT)** → `D1 Cathode (-)`

- **L1 Pin 2 (output)** → `+4.182V Rail`
- **L1 Pin 2 (output)** → `C2 Positive terminal`
- **L1 Pin 2 (output)** → `R2 Pin 1 (top)`
- **L1 Pin 2 (output)** → `LTE Module VCC`

### Feedback Network

- **R2 Pin 1 (top)** → `+4.182V Rail`
- **R2 Pin 2 (bottom)** → `LM2596SX Pin 4 (FB)`
- **R2 Pin 2 (bottom)** → `R1 Pin 1 (top)`
- **R1 Pin 2 (bottom)** → `Ground`

### Test Points

- **TP1:** Connect to `+5V Rail`
- **TP2:** Connect to `+4.182V Rail`
- **TP3:** Connect to `FB Junction (R1/R2)` - Should read 1.23V
- **TP4:** Connect to `GND`

---

## 5. PCB Layout Recommendations Summary

- **Keep Feedback Short:** The trace from `LM2596SX Pin 4 (FB)` to the `R1/R2 junction` must be **as short as possible (<3mm)**.
- **Place Components Tightly:** Cluster `D1`, `L1`, `C1`, `C2`, `R1`, and `R2` very close to the `LM2596SX` IC.
- **Wide Power Traces:** Use wide traces (≥1.5mm) for VIN, GND, OUTPUT, and the +4.182V rail.
- **Solid Ground Plane:** Use a large, solid ground plane on a dedicated layer for best performance and thermal dissipation.
- **Thermal Relief:** Connect the `LM2596SX Pin 6 (TAB)` to the ground plane with a large copper area and multiple thermal vias.
- **Component Placement:** Place `C1` close to the VIN pin. Place `D1` and `L1` close to the OUTPUT pin. Place `C2` close to the load.
