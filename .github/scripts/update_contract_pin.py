#!/usr/bin/env python3
import datetime as dt
import hashlib
import json
import os
from pathlib import Path


def parse_semver(tag: str):
    raw = tag.lstrip("v")
    parts = raw.split(".")
    if len(parts) != 3:
        return None
    try:
        return tuple(int(p) for p in parts)
    except ValueError:
        return None


def write_output(name: str, value: str) -> None:
    output = os.environ.get("GITHUB_OUTPUT")
    if not output:
        return
    with open(output, "a", encoding="utf-8") as out:
        out.write(f"{name}={value}\n")


pin_path = Path("contracts/contract-pin.json")
schema_path = Path("contracts/schemas/vitals/vitals.schema.json")

tag = os.environ["TAG"]
commit = os.environ["COMMIT"]

previous_tag = ""
if pin_path.exists():
    try:
        previous = json.loads(pin_path.read_text(encoding="utf-8"))
        previous_tag = str(previous.get("tag") or previous.get("pin", {}).get("tag") or "")
    except json.JSONDecodeError:
        previous_tag = ""

classification = "unknown"
breaking = False
prev = parse_semver(previous_tag) if previous_tag else None
curr = parse_semver(tag)
if curr is not None:
    if prev is None:
        classification = "breaking"
        breaking = True
    elif curr[0] > prev[0]:
        classification = "breaking"
        breaking = True
    elif curr[1] > prev[1]:
        classification = "minor"
    elif curr[2] >= prev[2]:
        classification = "patch"

schema_digest = hashlib.sha256(schema_path.read_bytes()).hexdigest()
expected_payload_version = tag.lstrip("v")

pin = {
    "contract_repo": "chaithubk/medtech-telemetry-contract",
    "tag": tag,
    "commit_sha": commit,
    "schema_path": "schemas/vitals/vitals.schema.json",
    "local_schema": "contracts/schemas/vitals/vitals.schema.json",
    "synced_at_utc": dt.datetime.now(dt.UTC).replace(microsecond=0).isoformat().replace("+00:00", "Z"),
    "contract": "vitals",
    "source": {
        "repository": "https://github.com/chaithubk/medtech-telemetry-contract",
        "schema_path": "schemas/vitals/vitals.schema.json",
    },
    "pin": {
        "tag": tag,
        "upstream_commit": commit,
        "schema_revision": f"sha256:{schema_digest}",
    },
    "compatibility": {
        "classification": classification,
        "breaking": breaking,
        "source": "semver",
        "change_type": "breaking" if breaking else "non-breaking",
        "notes": f"Automated vendored update to {tag}.",
    },
    "consumer": {
        "vendored_schema_path": "contracts/schemas/vitals/vitals.schema.json",
        "runtime_schema_path": "/usr/share/medtech/contracts/schemas/vitals/vitals.schema.json",
        "expected_payload_version": expected_payload_version,
    },
}

pin_path.write_text(json.dumps(pin, indent=2) + "\n", encoding="utf-8")
write_output("classification", classification)
print(f"compatibility_classification={classification}")
