# MiniGit

A lightweight, Git-like version control system built from scratch in C++.

MiniGit implements the core workflow behind version control — repository
initialization, file staging, content hashing, committing, and viewing
commit history — without relying on any existing VCS library.

## Why I built this

To understand how Git actually works under the hood (content-addressable
storage, staging areas, commit objects) by implementing a simplified
version of it myself, rather than just using Git as a black box.

## Supported Commands

| Command | Description |
|---|---|
| `minigit init` | Initializes a new MiniGit repository in the current directory |
| `minigit add <file>` | Stages a file for the next commit |
| `minigit status` | Shows new, modified, and unchanged files |
| `minigit commit -m "message"` | Creates a commit from staged files |
| `minigit log` | Displays commit history, newest first |

## Example Usage

```bash
./minigit init
./minigit add main.cpp
./minigit status
./minigit commit -m "Initial commit"
./minigit log
```

## Architecture

MiniGit separates CLI handling from core logic using three classes:

- **`Repository`** — handles `init`, `status`, `commit`, and `log`
- **`ObjectStore`** — hashes file content (FNV-1a) and stores it in
  `.minigit/objects/`, keyed by hash (content-addressable storage)
- **`Staging`** — manages `.minigit/staging/index`, recording which
  files are staged and their current hash

### Repository structure created by `init`

.minigit/
├── objects/ # file contents, stored by content hash
├── commits/ # one file per commit, containing metadata
├── staging/ # index file tracking staged files
└── HEAD # points to the most recent commit ID




### How staging + hashing works

1. `add <file>` reads the file's content and computes a hash (FNV-1a).
2. If that exact content hasn't been stored before, it's saved to
   `.minigit/objects/<hash>`.
3. The file's name and hash are recorded in `.minigit/staging/index`.
4. Identical content always produces the same hash, so unchanged files
   are never stored twice.

### How commit works

1. Reads the current staging index.
2. Generates a commit ID by hashing the staged content + timestamp + message.
3. Writes a commit record to `.minigit/commits/<commit-id>` containing
   the commit ID, message, timestamp, and staged file → hash mappings.
4. Updates `.minigit/HEAD` to point to the new commit.
5. Clears the staging area.

## Building

Requires CMake and a C++17 compiler.

```bash
mkdir build && cd build
cmake ..
make
```

## Limitations (v1.0)

This is an educational v1 and intentionally excludes:

- Branches and merging
- Checkout / restoring previous versions
- Remote repositories, push/pull, networking
- Authentication
- SHA-256 (uses a simpler FNV-1a hash instead — sufficient for
  demonstrating content-addressable storage)

**Known simplification:** `status` compares the working directory only
against the staging index, not against the last commit. This means
that immediately after a commit (which clears staging), previously
committed files will show as "new" until staged again. A future version
could compare against the last commit's file list instead.

## Roadmap

- [ ] Branching
- [ ] Checkout
- [ ] Diff between commits
- [ ] Merge support