#!/usr/bin/env bash
set -euo pipefail

RELEASE_URL="https://api.github.com/repos/cesanta/mongoose/releases/latest"
TAGS_URL="https://api.github.com/repos/cesanta/mongoose/tags"
curl_common=(
  -fsSL
  -H "Accept: application/vnd.github+json"
  -H "User-Agent: github-actions"
)

release_json=$(curl "${curl_common[@]}" "$RELEASE_URL" || true)
latest_tag=$(jq -r '.tag_name // empty' <<<"$release_json")

if [ -z "$latest_tag" ]; then
  tags_json=$(curl "${curl_common[@]}" "$TAGS_URL")
  latest_tag=$(jq -r '.[0].name // empty' <<<"$tags_json")
fi

if [ -z "$latest_tag" ]; then
  echo "Failed to determine latest upstream mongoose tag"
  exit 1
fi

current_tag="unknown"
if [ -f .mongoose-version ]; then
  current_tag=$(cat .mongoose-version)
fi

echo "latest_tag=$latest_tag" >> "$GITHUB_OUTPUT"
echo "current_tag=$current_tag" >> "$GITHUB_OUTPUT"

if [ "$latest_tag" = "$current_tag" ]; then
  echo "changed=false" >> "$GITHUB_OUTPUT"
  echo "Mongoose already up to date: $current_tag"
else
  echo "changed=true" >> "$GITHUB_OUTPUT"
  echo "Mongoose update available: $current_tag -> $latest_tag"
fi
