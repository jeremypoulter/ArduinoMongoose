#!/usr/bin/env bash
# Every directory under examples/ must be packaged in library.json and built
# by CI. This drifts silently otherwise: mdns_discovery shipped in #112
# without a library.json entry, and simple_websocket_client(_native) were
# never built by CI at all, which is how a call to a function removed from
# vendored mongoose sat broken for a whole mongoose upgrade.
set -euo pipefail
cd "$(dirname "$0")/../.."

fail=0

has_native_env() {
  [ -f "examples/$1/platformio.ini" ] && grep -q '^\[env:native\]' "examples/$1/platformio.ini"
}

mapfile -t examples < <(find examples -mindepth 1 -maxdepth 1 -type d -printf '%f\n' | sort)

for example in "${examples[@]}"; do
  if ! jq -e --arg base "examples/$example" '.examples[] | select(.base == $base)' library.json >/dev/null; then
    echo "::error::library.json has no entry for examples/$example"
    fail=1
  fi

  if ! grep -qE "^[[:space:]]*- example: ${example}\$" .github/workflows/platformio.yml; then
    echo "::error::.github/workflows/platformio.yml never builds examples/$example"
    fail=1
  fi

  if has_native_env "$example" && ! grep -qE "^[[:space:]]*- example: ${example}\$" .github/workflows/codeql-analysis.yml; then
    echo "::error::.github/workflows/codeql-analysis.yml never analyses examples/$example (it has a native env)"
    fail=1
  fi
done

# And the reverse: library.json shouldn't reference an example that no longer exists.
while IFS= read -r base; do
  name=${base#examples/}
  if [ ! -d "$base" ]; then
    echo "::error::library.json references $base, which does not exist"
    fail=1
  fi
done < <(jq -r '.examples[].base' library.json)

if [ "$fail" -ne 0 ]; then
  echo "Examples are out of sync between examples/, library.json and CI. See errors above."
  exit 1
fi

echo "All ${#examples[@]} examples are in sync across library.json, platformio.yml and codeql-analysis.yml."
