#!/usr/bin/env bash
set -euo pipefail

tag="${LATEST_TAG:?LATEST_TAG is required}"
base_url="https://raw.githubusercontent.com/cesanta/mongoose/$tag"

curl -fsSL "$base_url/mongoose.c" -o src/mongoose.c
curl -fsSL "$base_url/mongoose.h" -o src/mongoose.h
printf '%s\n' "$tag" > .mongoose-version
