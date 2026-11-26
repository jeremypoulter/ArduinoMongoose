#!/usr/bin/env bash
# Self-check for update-vendored-mongoose.sh's conflict handling, using a tiny
# synthetic upstream and fork (no network). Run: .github/scripts/test-update-vendored-mongoose.sh
set -euo pipefail

script=$(cd "$(dirname "$0")" && pwd)/update-vendored-mongoose.sh
tmp=$(mktemp -d)
trap 'rm -rf "$tmp"' EXIT
export GIT_AUTHOR_NAME=t GIT_AUTHOR_EMAIL=t@t GIT_COMMITTER_NAME=t GIT_COMMITTER_EMAIL=t@t

git init -q "$tmp/up" && cd "$tmp/up"
mkdir test
printf 'mongoose.c mongoose.h:\n\tcd .. && cat src.txt > mongoose.c && cat src.txt > mongoose.h\n' > test/Makefile
for f in src.txt mongoose.c mongoose.h f.txt; do echo base > $f; done
git add -A && git commit -qm base && base=$(git rev-parse HEAD)
echo up > f.txt && echo up > mongoose.c && git commit -qam "upstream edits f.txt and mongoose.c" && git tag t-source-conflict
git checkout -q "$base" && echo up-gen > mongoose.c && git commit -qam "upstream edits mongoose.c" && git tag t-generated-conflict

git init -q "$tmp/fork" && cd "$tmp/fork"
git fetch -q "$tmp/up" "$base" && git checkout -q -b stack/mdns "$base"
echo stack > f.txt && echo stack-gen > mongoose.c && echo stack-src > src.txt && git commit -qam "stack edits f.txt, mongoose.c, src.txt"
git checkout -q "$base" -- f.txt && git commit -qm "stack reverts f.txt"

new_app() { rm -rf "$tmp/app"; mkdir -p "$tmp/app/src"; cd "$tmp/app"; echo orig > src/mongoose.c; echo orig > src/mongoose.h; echo orig > .mongoose-version; }
run() { LATEST_TAG=$1 FORK_URL=$tmp/fork UPSTREAM_URL=$tmp/up CONFLICT_FILE=$tmp/conflict.txt GITHUB_OUTPUT=$tmp/out "$script" >/dev/null 2>&1; }

# A conflict in a real source file must fail, report it, and leave src/ alone.
new_app; : > "$tmp/out"; rm -f "$tmp/conflict.txt"
if run t-source-conflict; then echo "FAIL: source conflict did not fail"; exit 1; fi
grep -q '^Conflicting files:.*f\.txt' "$tmp/conflict.txt" || { echo "FAIL: conflict file does not list f.txt as conflicting"; exit 1; }
[ "$(cat src/mongoose.c)" = orig ] && [ "$(cat src/mongoose.h)" = orig ] && [ "$(cat .mongoose-version)" = orig ] \
  || { echo "FAIL: src/ or .mongoose-version changed after a source conflict"; exit 1; }

# A conflict only in the generated files must be absorbed and the sources regenerated.
new_app; : > "$tmp/out"
run t-generated-conflict || { echo "FAIL: generated-file conflict aborted the update"; exit 1; }
[ "$(cat src/mongoose.c)" = stack-src ] && [ "$(cat src/mongoose.h)" = stack-src ] \
  || { echo "FAIL: sources were not regenerated from the stack"; exit 1; }
head=$(git -C "$tmp/fork" rev-parse stack/mdns)
grep -qx "stack_head=$head" "$tmp/out" || { echo "FAIL: stack_head output missing or wrong"; exit 1; }
[ "$(sed -n 2p .mongoose-version)" = "stack=$head" ] || { echo "FAIL: .mongoose-version does not record the stack head"; exit 1; }

echo "PASS"
