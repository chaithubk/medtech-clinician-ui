#!/usr/bin/env python3
import json
from pathlib import Path

pin_path = Path("contracts/contract-pin.json")
if not pin_path.exists():
    raise SystemExit("Missing contracts/contract-pin.json")

pin = json.loads(pin_path.read_text(encoding="utf-8"))
print(str(pin.get("tag", "")).strip())
