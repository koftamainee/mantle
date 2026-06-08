import argparse
import re
import subprocess
import sys
from datetime import datetime
from pathlib import Path

REPO_ROOT = Path(__file__).resolve().parent.parent

LICENSE_PATH = REPO_ROOT / "LICENSE"

YEAR_RE = re.compile(r"\b\d{4}\b")
COPYRIGHT_COMMENT_RE = re.compile(r"// [^\n]*Copyright.*All rights reserved\.\s*")

def load_copyright_template() -> str:
    text = LICENSE_PATH.read_text(encoding="utf-8")
    for line in text.splitlines():
        if "Copyright" in line and "All rights reserved" in line:
            line = line.strip()
            if line.startswith("# "):
                line = "// " + line[2:]
            elif not line.startswith("//"):
                line = "// " + line
            return line
    raise RuntimeError("Could not find copyright line in LICENSE")


def get_source_files() -> list[Path]:
    result: list[Path] = []
    src_dirs = [REPO_ROOT / "mantle" / "src", REPO_ROOT / "mantle" / "tests"]
    for d in src_dirs:
        if not d.exists():
            continue
        for ext in ("*.h", "*.cpp"):
            for p in d.rglob(ext):
                if "third_party" in p.parts:
                    continue
                result.append(p)
    result.sort()
    return result


def get_year_from_git(file_path: Path) -> int:
    try:
        rel = file_path.relative_to(REPO_ROOT)
        out = subprocess.check_output(
            ["git", "log", "--reverse", "--diff-filter=A", "--follow",
             "--format=%ad", "--date=format:%Y", "--", str(rel)],
            cwd=REPO_ROOT, stderr=subprocess.DEVNULL, text=True
        )
        if out.strip():
            return int(out.strip().split("\n")[0])
    except Exception:
        pass
    return datetime.now().year


def has_copyright(text: str) -> bool:
    return bool(COPYRIGHT_COMMENT_RE.match(text.lstrip()))


def make_header(year: int) -> str:
    template = load_copyright_template()
    template = YEAR_RE.sub(str(year), template, count=1)
    return template + "\n"


def process_file(file_path: Path, fix: bool) -> bool:
    text = file_path.read_text(encoding="utf-8")

    if has_copyright(text):
        return False

    year = get_year_from_git(file_path)
    header = make_header(year)

    lines = text.split("\n")

    first_content = 0
    while first_content < len(lines) and lines[first_content].strip() == "":
        first_content += 1

    rest = lines[first_content:]
    new_lines = [header.rstrip("\n"), ""] + rest

    new_text = "\n".join(new_lines)

    if new_text == text:
        return False

    if fix:
        file_path.write_text(new_text, encoding="utf-8")
        print(f"FIXED  {file_path.relative_to(REPO_ROOT)}")
    else:
        print(f"ISSUE  {file_path.relative_to(REPO_ROOT)}")

    return True


def main() -> None:
    parser = argparse.ArgumentParser(description="Add copyright header to source files")
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
