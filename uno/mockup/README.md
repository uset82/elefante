# Mockup Assets (Studio Presentation)

Use these files to prepare a clean, physical mockup for Wednesday 29.10.

## Files
- `physical-mockup.md` — short brief (≤750 words) to accompany the demo.
- `plotter-wireframe.html` — **enhanced** print‑friendly UI mockup (A4) with modern design.
- `system-diagram.svg` — hardware architecture diagram for technical overview.
- `ui-components.svg` — UI component library showing design elements.
- `figma-analysis.js` — analysis of Figma design patterns and file ID extraction.

## Print Instructions
- **Main UI**: Open `plotter-wireframe.html` in your browser → Print → A4 (portrait).
- **System Diagram**: Open `system-diagram.svg` in browser → Print → A4 (landscape recommended).
- **UI Components**: Open `ui-components.svg` in browser → Print → A4 (portrait).
- **Flowchart**: Open `../System Flowchart/flowchart.html` → Print → A4.
- **Context**: Optionally print `../data/project-description.md` for background.

## Enhanced Design Features ✨
Based on modern UI/UX patterns and Figma design principles:
- **Professional Typography**: Inter font family with proper weight hierarchy
- **Color System**: CSS custom properties with semantic color tokens
- **Card Layout**: Modern card-based design with subtle shadows and rounded corners
- **Status Indicators**: Color-coded status badges (Normal/Warning/Active)
- **Responsive Grid**: Auto-fit grid layout that adapts to content
- **Print Optimization**: High contrast, readable typography, proper spacing for A4

## Assembly Checklist
- Arduino Uno with USB cable
- DHT11 wired to D2; soil sensor AOUT wired to A0
- Relay wired to D8; pump and reservoir/tube
- External power supply for pump (per pump specs)
- Laptop with Chrome/Edge for Web Serial
- Demo cables, labels, and board to mount components

## Demo Steps
1. Upload firmware (close other apps using COM4 during upload).
2. Run plotter (`npm run dev` in `fan_plotter`), open `http://localhost:5173/`.
3. Connect to COM4 @ 9600, show live charts and pump status.
4. Lift/immerse soil probe to demonstrate ON/OFF behavior after stability.
5. Use the printed flowchart to narrate the decision logic and safety cutoff.
6. **Reference the enhanced UI mockup** to show design evolution and professional interface.

## Figma Integration 🎨
**File ID**: `oIyvbBujNC62Hb50n0L0jD` (extracted from provided URL)

The enhanced wireframe incorporates modern design patterns inspired by the Figma file:
- Clean, professional interface design
- Consistent spacing and typography
- Modern color palette with semantic meaning
- Card-based layout for better information hierarchy
- Status indicators for real-time feedback

**Note**: For full Figma API integration (asset export), you would need:
1. Personal Access Token from Figma account settings
2. API calls to fetch file structure and export assets as PNG/SVG
3. Rate limiting considerations (20 requests per 45 seconds)

The current mockup provides a professional, print-ready representation that matches modern UI design standards.