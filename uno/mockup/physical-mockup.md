# Elephant Watering System — Physical Mockup Brief

This iteration delivers a working, demonstrable prototype of an Arduino‑based plant watering system and a paper mockup to present its digital interface and logic clearly in studio.

## Key Design Decisions
- Hardware: Arduino Uno + DHT11 (D2), capacitive/resistive soil sensor (A0), relay‑driven mini pump (D8). These choices keep cost low and parts widely available.
- Control strategy: Hysteresis with stability timers and a low‑pass filter (`soilAvg = 0.9*soilAvg + 0.1*soilValue`). Pump turns ON only after sustained dryness; turns OFF only after sustained wetness. Safety `MAX_ON_TIME` avoids long continuous runs.
- Data visibility: A simple serial protocol (`Temp|Pot|Servo|Motor|Humidity|ON/OFF`) feeds a browser plotter so users see real‑time conditions and pump status.
- Documentation separation: The flowchart is kept outside the plotter UI (in `System Flowchart/`) to keep the plotting interface clean while enabling printable process docs.

## Current Implementation Progress
- Firmware:
  - Reads DHT11 (temperature + humidity) and soil moisture.
  - Applies smoothing and stable‑time hysteresis; enforces minimum ON/OFF durations and safety cutoff.
  - Prints compact status lines for the plotter.
- Web plotter:
  - Charts temperature, humidity, soil ADC, derived soil moisture %, servo angle, and pump PWM.
  - Parses pump status (ON/OFF) and shows live values.
  - Runs at `http://localhost:5173/` via `npm run dev`.
- Documentation assets:
  - Flowchart (`System Flowchart/flowchart.html`, `flowchart.mmd`).
  - Project description (`data/project-description.md`).

## Final Outcome of This Iteration
- Stable pump behavior under noisy soil readings; relay chattering mitigated.
- Humidity charting and soil moisture % mapping added for clearer context.
- Clean separation of plotting UI from documentation.
- Print‑ready assets prepared for the studio demo (see Mockup Print below).

## New Elements and Their Benefits
- Stability timers (dry/wet): Prevent spurious toggles from transient spikes.
- Low‑pass soil filter: Smooths ADC fluctuations for more trustworthy decisions.
- Safety cutoff: Ensures hardware protection and predictable demos.
- Moisture % visualization: Converts raw ADC into a human‑readable scale.
- External flowchart: Faster comprehension of system logic in a single sheet.

## Mockup Print (for studio)
- Print `System Flowchart/flowchart.html` (browser → Print → A4) to show logic.
- Print `mockup/plotter-wireframe.html` to represent the UI layout (title, status, charts). Place these sheets next to the hardware on the board.

## Demo Plan (Wed 29.10)
1. Power Uno and pump supply; sensors connected (D2, A0, D8).
2. Open plotter (`npm run dev` → `http://localhost:5173/`), connect to COM4 at 9600.
3. Show live charts. Lift probe (dry) → pump ON after stability; immerse (wet) → pump OFF.
4. Use printed flowchart to narrate the decision points and safety behavior.

## Figma (optional)
If desired, we can import the wireframe into Figma for layout refinements, export PNG/SVG, and re‑print. Share your Figma file key to sync assets.