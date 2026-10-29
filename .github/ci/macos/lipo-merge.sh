#!/bin/bash
# Merge two trees built for arm64 and x86_64 into one with universal binaries.
# The trees must have the same layout; files that are not Mach-O and symlinks are
# taken from the arm64 tree.
#
# Usage: lipo-merge.sh <arm64-tree> <x86_64-tree> <output-tree>
set -euo pipefail

ARM=${1:?usage: lipo-merge.sh <arm64-tree> <x86_64-tree> <output-tree>}
X86=${2:?}
OUT=${3:?}

rm -rf "$OUT"
mkdir -p "$OUT"
cp -a "$ARM/." "$OUT/"

status=0
while IFS= read -r -d '' file; do
	rel=${file#"$ARM"/}
	file -b "$file" | grep -q Mach-O || continue
	if [ ! -f "$X86/$rel" ]; then
		echo "error: $rel is missing from the x86_64 tree" >&2
		status=1
		continue
	fi
	lipo -create "$file" "$X86/$rel" -output "$OUT/$rel"
done < <(find "$ARM" -type f -print0)
exit $status
