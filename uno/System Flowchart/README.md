# Elephant Watering System — Flowchart Docs

This folder centralizes the flowchart documentation for the firmware logic.

## Files
- `flowchart.html` — Standalone viewer using Mermaid (open directly in your browser).
- `flowchart.mmd` — Mermaid source you can edit or reuse.

## View the Flowchart
- Double-click `flowchart.html` (or right-click → Open with → your browser).
- The diagram renders locally using the Mermaid CDN (UMD build).
- To tweak, edit the text inside the `<div class="mermaid">` block in the HTML, or edit `flowchart.mmd` in your editor.

## Use in the Web Plotter
The plotter at `fan_plotter/` already embeds a copy of this flowchart. If you prefer to load this external file directly in the plotter UI, add a fetch step and render the content into a `<div class="mermaid">` (requires serving the folder via Vite). Easiest options:

1. Keep the current embedded copy (no additional config).
2. Copy `flowchart.mmd` into `fan_plotter/src/assets/` and import it.
3. Add a Vite config to allow serving this folder outside the project root (advanced):
   ```js
   // fan_plotter/vite.config.js
   import { defineConfig } from 'vite'
   export default defineConfig({
     server: {
       fs: {
         strict: false,
         allow: ['..'] // allow parent folders (Windows paths supported)
       }
     }
   })
   ```
   Then you can fetch `../System%20Flowchart/flowchart.mmd` from the plotter.

## Exporting to Image/PDF
- Online: paste `flowchart.mmd` into https://mermaid.live and export PNG/SVG/PDF.
- Node CLI: `npm i -g @mermaid-js/mermaid-cli` then:
  ```bash
  mmdc -i flowchart.mmd -o flowchart.svg
  ```

## Notes
- Mermaid diagrams are sensitive to special characters. Prefer simple labels and move decisions to edges using `-->|yes|` and `-->|no|`.
- The logic mirrors the firmware: DHT sampling (~2s), soil smoothing, dry/wet stability, pump ON/OFF minimums, and safety cutoff.