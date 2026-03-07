#!/usr/bin/env python3
"""Generate PDF-ready architecture diagram for VisionCast-AI.

Usage:
    python docs/generate_diagram.py          # SVG + PDF output
    python docs/generate_diagram.py --svg    # SVG only
    python docs/generate_diagram.py --pdf    # PDF only

Requirements:
    - graphviz (system): sudo apt-get install graphviz
    - graphviz (Python): pip install graphviz
"""

from __future__ import annotations

import argparse
import subprocess
import sys
from pathlib import Path

DOCS_DIR = Path(__file__).resolve().parent
DOT_FILE = DOCS_DIR / "architecture_diagram.dot"

FORMATS = {
    "svg": DOCS_DIR / "architecture_diagram.svg",
    "pdf": DOCS_DIR / "architecture_diagram.pdf",
}


def _check_graphviz() -> None:
    """Ensure the ``dot`` command is available."""
    try:
        subprocess.run(
            ["dot", "-V"],
            capture_output=True,
            check=True,
        )
    except FileNotFoundError:
        sys.exit(
            "Error: Graphviz is not installed.\n"
            "Install it with: sudo apt-get install graphviz"
        )


def render(fmt: str) -> Path:
    """Render the DOT source to *fmt* and return the output path."""
    out = FORMATS[fmt]
    subprocess.run(
        ["dot", f"-T{fmt}", "-o", str(out), str(DOT_FILE)],
        check=True,
    )
    return out


def main() -> None:
    parser = argparse.ArgumentParser(
        description="Generate VisionCast-AI architecture diagram",
    )
    parser.add_argument("--svg", action="store_true", help="SVG only")
    parser.add_argument("--pdf", action="store_true", help="PDF only")
    args = parser.parse_args()

    _check_graphviz()

    if not DOT_FILE.exists():
        sys.exit(f"Error: DOT source not found at {DOT_FILE}")

    formats = []
    if args.svg:
        formats.append("svg")
    if args.pdf:
        formats.append("pdf")
    if not formats:
        formats = list(FORMATS)

    for fmt in formats:
        path = render(fmt)
        print(f"  ✓ {fmt.upper()} → {path}")

    print("Done.")


if __name__ == "__main__":
    main()
