import argparse
import re
import subprocess
import sys
from datetime import datetime
from pathlib import Path

REPO_ROOT = Path(__file__).resolve().parent.parent

APACHE_HEADER_TEMPLATE = """\
// Copyright {year} Mantle
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License."""


COPYRIGHT_RE = re.compile(r"// Copyright")


def make_header(year: int) -> str:
    return APACHE_HEADER_TEMPLATE.format(year=year) + "\n"


def has_copyright(text: str) -> bool:
    for line in text.split("\n"):
        stripped = line.strip()
        if stripped and not stripped.startswith("//"):
            return False
        if COPYRIGHT_RE.match(stripped):
            return True
    return False


def strip_copyright_block(text: str) -> str:
    lines = text.split("\n")
    start = 0
    while start < len(lines) and lines[start].strip() == "":
        start += 1
    while start < len(lines) and lines[start].strip().startswith("//"):
        start += 1
    while start < len(lines) and lines[start].strip() == "":
        start += 1
    return "\n".join(lines[start:])


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


def process_file(file_path: Path, fix: bool) -> bool:
    text = file_path.read_text(encoding="utf-8")

    if has_copyright(text):
        body = strip_copyright_block(text)
    else:
        body = text.lstrip("\n")

    year = get_year_from_git(file_path)
    header = make_header(year)

    new_text = header.rstrip("\n") + "\n\n" + body.lstrip("\n")
    new_text = new_text.rstrip("\n") + "\n"

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
