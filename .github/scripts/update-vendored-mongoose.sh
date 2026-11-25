#!/usr/bin/env bash
# Rebases the fork's patch stack (jeremypoulter/mongoose, branch stack/mdns) onto an
# upstream release tag, regenerates the amalgamated sources and vendors them.
# On a rebase conflict nothing is vendored: the failing commit and files are written
# to $CONFLICT_FILE (default: conflict.txt) and the script exits non-zero.
set -euo pipefail

tag="${LATEST_TAG:?LATEST_TAG is required}"
root=$(pwd)
conflict_file=${CONFLICT_FILE:-$root/conflict.txt}
fork_url=${FORK_URL:-https://github.com/jeremypoulter/mongoose.git}
upstream_url=${UPSTREAM_URL:-https://github.com/cesanta/mongoose.git}

work=$(mktemp -d)
trap 'rm -rf "$work"' EXIT
cd "$work"
git init -q .
git config user.name "mongoose-updater"
git config user.email "mongoose-updater@users.noreply.github.com"
git remote add fork "$fork_url"
git remote add upstream "$upstream_url"
git fetch -q --filter=blob:none fork refs/heads/stack/mdns:refs/heads/stack
git fetch -q --filter=blob:none upstream "refs/tags/$tag:refs/tags/$tag"
stack_head=$(git rev-parse stack)
base=$(git merge-base stack "$tag")

git checkout -q stack
if ! git rebase --onto "$tag" "$base" stack >/dev/null 2>&1; then
  while [ -d .git/rebase-merge ] || [ -d .git/rebase-apply ]; do
    files=$(git diff --name-only --diff-filter=U)
    # mongoose.c/.h are generated; they are rebuilt below, so take the upstream side.
    if [ -n "$files" ] && ! grep -qvxE 'mongoose\.[ch]' <<<"$files"; then
      git checkout --ours -- mongoose.c mongoose.h
      git add mongoose.c mongoose.h
      GIT_EDITOR=true git rebase --continue >/dev/null 2>&1 || continue
    else
      commit=$(git rev-parse --short REBASE_HEAD 2>/dev/null || echo unknown)
      subject=$(git log -1 --format=%s REBASE_HEAD 2>/dev/null || echo unknown)
      {
        echo "The fork patch stack ($fork_url stack/mdns @ $stack_head) does not apply to $tag."
        echo "Failing commit: $commit $subject"
        echo "Conflicting files: $(tr '\n' ' ' <<<"$files")"
      } | tee "$conflict_file" >&2
      git rebase --abort
      exit 1
    fi
  done
fi

(cd test && make -B mongoose.c mongoose.h >/dev/null)
cp mongoose.c "$root/src/mongoose.c"
cp mongoose.h "$root/src/mongoose.h"
cd "$root"
printf '%s\nstack=%s\n' "$tag" "$stack_head" > .mongoose-version
