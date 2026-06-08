#!/usr/bin/env python3
"""Scan MANTLE_FATAL calls and generate an error code lookup table.

Usage:
    python scripts/gen_error_codes.py path/to/error_codes.json
"""

import json
import re
import subprocess
import sys
from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parent.parent

def project_filename(path: str) -> str:
    idx = path.find("mantle")
    return path[idx:] if idx >= 0 else path


def fnv1a(s: str, line: int) -> int:
    h = 0x811C9DC5 ^ line
    for c in s.encode("utf-8"):
        h ^= c
        h = (h * 0x01000193) & 0xFFFFFFFF
    return h


def get_source_files() -> list[str]:
    result = subprocess.run(
        ["git", "-C", str(REPO_ROOT), "ls-files", "-c", "-o", "--exclude-standard"],
        capture_output=True, text=True, check=True,
    )
    files = [f.strip() for f in result.stdout.split("\n") if f.strip()]
    return [f for f in files if f.endswith((".h", ".hpp", ".cpp", ".c"))]


def parse_fatal_args(text: str) -> tuple[str | None, str | None]:
    """Extract condition and format string from MANTLE_FATAL(cond, fmt, ...)."""
    args: list[str] = []
    current: list[str] = []
    depth = 0
    in_string = False
    string_char = ""
    i = 0

    while i < len(text):
        ch = text[i]
        if in_string:
            if ch == "\\":
                current.append(ch)
                i += 1
                if i < len(text):
                    current.append(text[i])
                i += 1
                continue
            elif ch == string_char:
                in_string = False
            current.append(ch)
            i += 1
            continue

        if ch in ('"', "'"):
            in_string = True
            string_char = ch
            current.append(ch)
            i += 1
            continue

        if ch == "(":
            depth += 1
            current.append(ch)
            i += 1
            continue

        if ch == ")":
            if depth == 0:
                args.append("".join(current).strip())
                break
            depth -= 1
            current.append(ch)
            i += 1
            continue

        if ch == "," and depth == 0:
            args.append("".join(current).strip())
            current = []
            i += 1
            continue

        current.append(ch)
        i += 1

    if len(args) >= 1:
        cond = re.sub(r'\s+', ' ', args[0]).strip()
        msg = parse_string_literal(args[1]) if len(args) >= 2 else None
        return cond, msg
    return None, None


def parse_string_literal(s: str) -> str | None:
    s = s.strip()
    if len(s) >= 2 and s[0] == s[-1] and s[0] in ('"', "'"):
        inner = s[1:-1]
        result = []
        i = 0
        while i < len(inner):
            if inner[i] == "\\" and i + 1 < len(inner):
                nxt = inner[i + 1]
                if nxt == "n":
                    result.append("\n")
                elif nxt == "t":
                    result.append("\t")
                elif nxt == '"':
                    result.append('"')
                elif nxt == "'":
                    result.append("'")
                elif nxt == "\\":
                    result.append("\\")
                else:
                    result.append(inner[i])
                    result.append(nxt)
                i += 2
            else:
                result.append(inner[i])
                i += 1
        return "".join(result)
    return s


def main() -> None:
    if len(sys.argv) < 2:
        print(f"Usage: {sys.argv[0]} <output.json>", file=sys.stderr)
        sys.exit(1)

    output_path = Path(sys.argv[1])
    source_files = get_source_files()

    pattern = re.compile(r"MANTLE_FATAL\(")
    entries: dict[str, dict] = {}

    for file_rel in source_files:
        full_path = REPO_ROOT / file_rel
        if not full_path.is_file():
            continue

        pf = project_filename(file_rel)

        try:
            content = full_path.read_text(encoding="utf-8")
        except Exception:
            continue

        for match in pattern.finditer(content):
            line_start = content[: match.start()].count("\n") + 1
            remaining = content[match.start() + len("MANTLE_FATAL(") :]

            cond, msg = parse_fatal_args(remaining)

            code = fnv1a(pf, line_start)
            code_str = f"E{code:08X}"

            if code_str in entries:
                prev = entries[code_str]
                print(
                    f"WARNING: hash collision for {code_str}:\n"
                    f"  {prev['file']}:{prev['line']}\n"
                    f"  {pf}:{line_start}",
                    file=sys.stderr,
                )

            entries[code_str] = {
                "file": pf,
                "line": line_start,
                "condition": cond or "",
                "message": msg or "",
            }

    output_path.parent.mkdir(parents=True, exist_ok=True)
    output_path.write_text(
        json.dumps(entries, indent=2, ensure_ascii=False) + "\n",
        encoding="utf-8",
    )

    print(f"Generated {len(entries)} error codes -> {output_path}")


if __name__ == "__main__":
    main()
