import argparse
import re
import sys
from pathlib import Path

REPO_ROOT = Path(__file__).resolve().parent.parent

# Matches case/default label lines. Groups: (indent, label_text, has_open_brace)
CASE_LABEL_RE = re.compile(
    r'^(\s*)((?:case\s+.+?|default)\s*:\s*)(\{)?\s*(\S.*)?$'
)
BREAK_RE = re.compile(r'^\s*break\s*;\s*$')
EMPTY_RE = re.compile(r'^\s*$')
# Matches `switch (...) {`
SWITCH_OPEN_RE = re.compile(r'\bswitch\s*\(.*\)\s*\{\s*$')
# Matches a closing brace at the switch body level
CLOSE_BRACE_RE = re.compile(r'^\s*\}\s*$')


def get_source_files() -> list[Path]:
    result: list[Path] = []
    for d in [REPO_ROOT / "mantle" / "src", REPO_ROOT / "mantle" / "tests"]:
        if not d.exists():
            continue
        for ext in ("*.h", "*.cpp"):
            for p in d.rglob(ext):
                if "third_party" in p.parts:
                    continue
                result.append(p)
    result.sort()
    return result


def find_matching_brace(lines: list[str], start: int) -> int | None:
    depth = 1
    i = start + 1
    while i < len(lines):
        for ch in lines[i]:
            if ch == '{':
                depth += 1
            elif ch == '}':
                depth -= 1
        if depth == 0:
            return i
        i += 1
    return None


class CaseSegment:
    """Represents one case/default segment inside a switch body."""

    def __init__(self) -> None:
        self.indent = ""
        self.labels: list[str] = []   # e.g. ["case SDL_EVENT_QUIT:"]
        self.body: list[str] = []      # lines between label(s) and next label / switch end
        self.has_braces = False        # body wrapped in {} ?
        self.sep_index: int | None = None  # index of the label line in body_lines

    def is_empty_body(self) -> bool:
        stripped = [l for l in self.body if not EMPTY_RE.match(l)]
        return len(stripped) == 0

    def has_break(self) -> bool:
        stripped = [l for l in self.body if not EMPTY_RE.match(l)]
        return bool(stripped) and BREAK_RE.match(stripped[-1])


def find_case_segments(
    body_lines: list[str], offset: int
) -> list[CaseSegment]:
    """Split switch body_lines (between { and }) into case segments.

    offset: the depth of the switch body (1). Used to detect nested
            switches — case labels at depth > offset belong to nested switches.
    """
    segments: list[CaseSegment] = []
    cur: CaseSegment | None = None
    depth = offset

    for idx, line in enumerate(body_lines):
        # Track brace depth changes
        open_count = line.count('{')
        close_count = line.count('}')

        # Check for case label at the switch body depth
        m = CASE_LABEL_RE.match(line)
        is_label = False
        if m and depth == offset:
            is_label = True

            # Flush previous segment
            if cur is not None:
                segments.append(cur)

            cur = CaseSegment()
            cur.indent = m.group(1)
            cur.labels = [line]
            cur.sep_index = idx
            if m.group(3) == '{':
                cur.has_braces = True

        depth += open_count
        depth -= close_count

        if cur is not None and not is_label:
            cur.body.append(line)

    if cur is not None:
        segments.append(cur)

    merged: list[CaseSegment] = []
    for s in segments:
        if merged and merged[-1].is_empty_body():
            merged[-1].labels.extend(s.labels)
            if s.body:
                merged[-1].body = s.body
            merged[-1].has_braces = s.has_braces
            merged[-1].indent = s.indent
            merged[-1].sep_index = s.sep_index
        else:
            merged.append(s)
    segments = merged

    return segments


def format_braced_segment(seg: CaseSegment) -> list[str]:
    """Format a segment that already has {} braces around its body.
    Handles: move break; after }, collapse empty bodies.
    """
    body = seg.body[:]
    if not body:
        return format_nobraced_segment(seg)

    while body and EMPTY_RE.match(body[-1]):
        body.pop()

    if body and body[-1].strip() == '}':
        body.pop()

    while body and EMPTY_RE.match(body[-1]):
        body.pop()

    has_break = body and BREAK_RE.match(body[-1])
    if has_break:
        body = body[:-1]

    while body and EMPTY_RE.match(body[-1]):
        body.pop()

    body_empty = all(EMPTY_RE.match(l) for l in body) if body else True

    if body_empty:
        return format_nobraced_segment(seg)

    out: list[str] = []
    # Reconstruct label lines: all but last keep original form, last gets `{`
    indent = seg.indent
    for lbl in seg.labels[:-1]:
        s = lbl.strip()
        if s.endswith('{'):
            s = s[:-1].rstrip()
        out.append(f"{indent}{s}")

    last = seg.labels[-1].strip()
    if last.endswith('{'):
        last = last[:-1].rstrip()
    out.append(f"{indent}{last} {{")

    out.extend(body)
    out.append(f"{indent}}} break;" if has_break else f"{indent}}}")
    return out


def format_nobraced_segment(seg: CaseSegment) -> list[str]:
    """Format a segment that has NO braces.
    Wraps body in {} and adds } break; at end.
    """
    body = seg.body[:]

    # Strip leading blank lines
    while body and EMPTY_RE.match(body[0]):
        body.pop(0)
    # Strip trailing blank lines
    while body and EMPTY_RE.match(body[-1]):
        body.pop()

    has_break = body and BREAK_RE.match(body[-1])
    if has_break:
        body = body[:-1]

    while body and EMPTY_RE.match(body[-1]):
        body.pop()

    body_empty = all(EMPTY_RE.match(l) for l in body) if body else True

    indent = seg.indent
    # Build label lines: all but the last keep original form
    label_texts: list[str] = []
    for lbl in seg.labels:
        s = lbl.strip()
        if s.endswith('{'):
            s = s[:-1].rstrip()
        label_texts.append(s)

    if body_empty:
        # Only the last label gets `{} break;`
        out_lines: list[str] = []
        for lbl in label_texts[:-1]:
            out_lines.append(f"{indent}{lbl}")
        out_lines.append(f"{indent}{label_texts[-1]} {{}} break;")
        return out_lines

    out: list[str] = []
    for lbl in label_texts[:-1]:
        out.append(f"{indent}{lbl}")
    out.append(f"{indent}{label_texts[-1]} {{")
    for l in body:
        out.append(l)
    out.append(f"{indent}}} break;")
    return out


def process_switch_block(lines: list[str], switch_idx: int) -> list[str] | None:
    """Process a switch block starting at lines[switch_idx].
    Returns the new block lines or None if no change needed.
    """
    # Find the opening brace (might be on same line as `switch (...) {`)
    brace_line = switch_idx
    if '{' not in lines[brace_line]:
        # Brace is on the next line
        brace_line += 1

    # Find matching closing brace
    close_idx = find_matching_brace(lines, brace_line)
    if close_idx is None:
        return None

    # Body lines (between { and })
    body_start = brace_line + 1
    body_lines = lines[body_start:close_idx]

    # Parse segments
    segments = find_case_segments(body_lines, 0)

    if not segments:
        return None  # No case labels found (empty switch or no cases)

    new_body: list[str] = []
    changed = False

    for seg in segments:
        old_block = (
            seg.labels + seg.body
        )

        if seg.has_braces:
            new_part = format_braced_segment(seg)
        else:
            new_part = format_nobraced_segment(seg)

        # The body block in segments doesn't include the braces content
        # Let me just compare the raw segment vs new format

        if new_part != old_block:
            changed = True
        new_body.extend(new_part)

    if not changed:
        return None

    # Reconstruct the switch block
    result = lines[:body_start]
    result.extend(new_body)
    result.append(lines[close_idx])
    result.extend(lines[close_idx + 1:])

    return result


def process_file(file_path: Path, fix: bool) -> bool:
    text = file_path.read_text(encoding="utf-8")
    lines = text.split("\n")
    changed = False

    i = 0
    while i < len(lines):
        if SWITCH_OPEN_RE.match(lines[i]):
            new_lines = process_switch_block(lines, i)
            if new_lines is not None:
                lines = new_lines
                changed = True
                # Continue from the same position (switch block was replaced)
                continue
        i += 1

    if not changed:
        return False

    new_text = "\n".join(lines)
    if new_text == text:
        return False

    if fix:
        file_path.write_text(new_text, encoding="utf-8")
        print(f"FIXED  {file_path.relative_to(REPO_ROOT)}")
    else:
        print(f"ISSUE  {file_path.relative_to(REPO_ROOT)}")

    return True


def main() -> None:
    parser = argparse.ArgumentParser(description="Normalize switch-case style")
    parser.add_argument("--fix", action="store_true", help="Rewrite files in-place")
    parser.add_argument("--check", action="store_true", help="Check-only (default)")
    args = parser.parse_args()

    files = get_source_files()

    changed = 0
    for f in files:
        if process_file(f, fix=args.fix):
            changed += 1

    if changed:
        print(f"\n{changed} file(s) {'fixed' if args.fix else 'need fixing'}")
    else:
        print("All clean!")

    if not args.fix and changed > 0:
        sys.exit(1)


if __name__ == "__main__":
    main()
