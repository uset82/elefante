import os
from pathlib import Path
from typing import List

try:
    from PyPDF2 import PdfReader
except Exception as e:
    raise SystemExit(f"PyPDF2 not available: {e}")

BASE = Path(r"d:\HVL2025\ADA525\uno")

ordered_targets = [
    BASE / "annotated-Assignment 3-1.pdf",
    BASE / "annotated-Elephant_Mockup_Assignment.pdf",
    BASE / "annotated-Project proposal.pdf",
    BASE / "annotated-Sensor_Measurement_Watering_System_CCM.pdf",
    BASE / "Assignment3_Report_ccm.pdf",
]

# Provide alternative percent-encoded filenames for items with spaces
alt_map = {
    str(BASE / "annotated-Assignment 3-1.pdf"): str(BASE / "annotated-Assignment%203-1.pdf"),
    str(BASE / "annotated-Project proposal.pdf"): str(BASE / "annotated-Project%20proposal.pdf"),
}


def resolve_existing_path(p: Path) -> Path:
    """Return the existing path, trying percent-encoded alternative if needed."""
    if p.exists():
        return p
    alt = alt_map.get(str(p))
    if alt and Path(alt).exists():
        return Path(alt)
    return p  # Return original (may not exist) for error reporting


def extract_text(pdf_path: Path) -> str:
    reader = PdfReader(str(pdf_path))
    texts: List[str] = []
    for i, page in enumerate(reader.pages):
        try:
            t = page.extract_text() or ""
        except Exception:
            t = ""
        if t.strip():
            texts.append(t)
    return "\n\n".join(texts)


def main():
    success = 0
    failures = []
    for target in ordered_targets:
        resolved = resolve_existing_path(target)
        if not resolved.exists():
            failures.append(f"Missing: {resolved}")
            print(f"[MISS] {resolved}")
            continue
        print(f"[READ] {resolved}")
        try:
            text = extract_text(resolved)
            out_path = resolved.with_suffix(".txt")
            with open(out_path, "w", encoding="utf-8") as f:
                f.write(text)
            print(f"[DONE] Wrote {out_path} ({len(text)} chars)")
            success += 1
        except Exception as e:
            failures.append(f"Failed: {resolved} -> {e}")
            print(f"[FAIL] {resolved}: {e}")
    print(f"[SUMMARY] Extracted {success}/{len(ordered_targets)} PDFs. Failures: {len(failures)}")
    for msg in failures:
        print(f" - {msg}")


if __name__ == "__main__":
    main()