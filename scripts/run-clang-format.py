import subprocess
import sys
from pathlib import Path


def main() -> None:
    dirs = sys.argv[1:]
    if not dirs:
        return

    files: list[str] = []
    for d in dirs:
        root = Path(d)
        for ext in ("*.h", "*.cpp"):
            for p in root.rglob(ext):
                if "third_party" in p.parts:
                    continue
                files.append(str(p))
    if files:
        subprocess.run(["clang-format", "-i", *files], check=True)


if __name__ == "__main__":
    main()
