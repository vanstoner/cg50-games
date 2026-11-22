#!/bin/sh
# Turn a printable's HTML into an A4 PDF next to it, and report the page count.
#
#   seasonal/printable-kit/make-pdf.sh seasonal/valentines-day/beating-heart/printable/beating-heart.html
#
# Needs Google Chrome or Chromium installed.
set -e

HTML="$1"
if [ -z "$HTML" ] || [ ! -f "$HTML" ]; then
    echo "Usage: $0 <printable.html>" >&2
    exit 1
fi

for c in \
    "/Applications/Google Chrome.app/Contents/MacOS/Google Chrome" \
    "/Applications/Chromium.app/Contents/MacOS/Chromium" \
    google-chrome chromium chromium-browser; do
    if command -v "$c" >/dev/null 2>&1 || [ -x "$c" ]; then CHROME="$c"; break; fi
done
if [ -z "$CHROME" ]; then
    echo "Couldn't find Chrome or Chromium" >&2
    exit 1
fi

DIR=$(cd "$(dirname "$HTML")" && pwd)
BASE=$(basename "$HTML" .html)
PDF="$DIR/$BASE.pdf"

# --virtual-time-budget gives the web fonts time to load
"$CHROME" --headless=new --disable-gpu --no-pdf-header-footer \
    --virtual-time-budget=8000 \
    --print-to-pdf="$PDF" "file://$DIR/$BASE.html" 2>/dev/null

PAGES=$(grep -aoE '/Type */Page([^s]|$)' "$PDF" | wc -l | tr -d ' ')
echo "Wrote $PDF ($PAGES page(s))"
if [ "$PAGES" -gt 2 ]; then
    echo "Warning: printables should fit on 1-2 A4 sides" >&2
fi
