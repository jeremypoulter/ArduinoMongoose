#!/usr/bin/env bash
set -euo pipefail

RELEASE_URL="https://api.github.com/repos/cesanta/mongoose/releases/latest"
TAGS_URL="https://api.github.com/repos/cesanta/mongoose/tags"
FORK_URL="https://github.com/jeremypoulter/mongoose.git"
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

# The patch stack lives on the fork; its head is part of the vendored version.
stack_head=$(git ls-remote "$FORK_URL" refs/heads/stack/mdns | cut -f1)
if [ -z "$stack_head" ]; then
  echo "Failed to read stack/mdns head from $FORK_URL"
  exit 1
fi

current_tag="unknown"
current_stack="none"
if [ -f .mongoose-version ]; then
  current_tag=$(sed -n '1p' .mongoose-version)
  current_stack=$(sed -n 's/^stack=//p' .mongoose-version)
  current_stack=${current_stack:-none}
fi

echo "latest_tag=$latest_tag" >> "$GITHUB_OUTPUT"
echo "current_tag=$current_tag" >> "$GITHUB_OUTPUT"
echo "stack_head=$stack_head" >> "$GITHUB_OUTPUT"

if [ "$latest_tag" = "$current_tag" ] && [ "$stack_head" = "$current_stack" ]; then
  echo "changed=false" >> "$GITHUB_OUTPUT"
  echo "Mongoose already up to date: $current_tag (stack $current_stack)"
else
  echo "changed=true" >> "$GITHUB_OUTPUT"
  echo "Mongoose update available: $current_tag/$current_stack -> $latest_tag/$stack_head"
fi
