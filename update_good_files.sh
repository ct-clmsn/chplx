#!/usr/bin/env bash
shopt -s nullglob

BUILD_ROOT="build/backend/test"
SRC_ROOT="backend/test"
changed=0

# Loop only over actual test dirs in the source tree
for srcdir in "$SRC_ROOT"/*/; do
  testname=${srcdir%/}; testname=${testname##*/}
  gen_dir="$BUILD_ROOT/$testname"
  [[ -d "$gen_dir" ]] || continue

  echo -e "\n=== Test: $testname ==="

  # Now pick up .cpp, .hpp, and .txt (for CMakeLists.txt)
  for genfile in "$gen_dir"/*.{cpp,hpp,txt}; do
    [[ -f "$genfile" ]] || continue
    fname=${genfile##*/}                   # e.g. "CMakeLists.txt"
    dst="$srcdir/$fname.good"              # e.g. "backend/test/arr/CMakeLists.txt.good"

    # New file? copy & stage
    if [[ ! -f "$dst" ]]; then
      echo "New file: $fname → $dst"
      cp "$genfile" "$dst"
      git add "$dst"
      ((changed++))
      continue
    fi

    # Identical? skip
    if cmp -s "$dst" "$genfile"; then
      echo "  • $fname (unchanged)"
      continue
    fi

    # Build the patch, fix any '//' in paths, and show it
    patch=$(diff -u --label a/"$dst" --label b/"$dst" "$dst" "$genfile" \
      | sed 's://:/:g')  # normalize any double-slashes

    echo
    echo "Diff for $testname/$fname.good:"
    printf '%s\n' "$patch"

    # Prompt
    read -r -p "Apply & stage update to '$testname/$fname.good'? [y/N] " ans
    ans=${ans,,}   # lowercase
    if [[ "$ans" == "y" || "$ans" == "yes" ]]; then
      printf '%s\n' "$patch" | git apply --index
      echo "  → Patched and staged $dst"
      ((changed++))
    else
      echo "  → Skipped $fname"
    fi
  done
done

echo
if (( changed > 0 )); then
  echo "Total files updated/staged: $changed"
else
  echo "No changes detected."
fi

exit 0
