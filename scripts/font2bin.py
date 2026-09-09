#!/usr/bin/env python3
# SPDX-License-Identifier: GPL-3.0-or-later

from PIL import Image, ImageFont, ImageDraw
import sys
import os


def hex_to_bin(path):
    data = bytearray(256 * 16)

    with open(path, "r") as f:
        for line in f:
            line = line.strip()

            if not line:
                continue

            parts = line.split(":")
            if len(parts) != 2:
                continue

            code = int(parts[0], 16)

            if code >= 256:
                continue

            raw = parts[1]

            if len(raw) != 32:
                continue

            data[code * 16:code * 16 + 16] = bytes.fromhex(raw)

    return data


def ttf_to_bin(path):
    data = bytearray(256 * 16)

    font = ImageFont.truetype(
        path,
        16,
        layout_engine=ImageFont.Layout.BASIC
    )

    for c in range(256):
        if c < 32:
            continue

        ch = chr(c)

        try:
            if not font.getmask(ch):
                continue
        except Exception:
            continue

        img = Image.new("1", (8, 16), 0)
        draw = ImageDraw.Draw(img)

        try:
            bbox = draw.textbbox(
                (0, 0),
                ch,
                font=font
            )
        except Exception:
            continue

        x = -bbox[0]
        y = 0

        draw.text(
            (x, y),
            ch,
            font=font,
            fill=1
        )

        for row in range(16):
            value = 0

            for col in range(8):
                if img.getpixel((col, row)):
                    value |= 1 << (7 - col)

            data[c * 16 + row] = value

    return data


def bdf_to_bin(path):
    data = bytearray(256 * 16)
    current_encoding = None
    in_bitmap = False
    bitmap_rows = []

    with open(path, "r", encoding="utf-8", errors="ignore") as f:
        for line in f:
            line = line.strip()

            if line.startswith("ENCODING "):
                parts = line.split()
                if len(parts) > 1:
                    try:
                        current_encoding = int(parts[1])
                    except ValueError:
                        current_encoding = None
            elif line == "BITMAP":
                in_bitmap = True
                bitmap_rows = []
            elif line == "ENDCHAR":
                if in_bitmap and current_encoding is not None and 0 <= current_encoding < 256:
                    for row_idx, hex_row in enumerate(bitmap_rows[:16]):
                        try:
                            row_bytes = bytes.fromhex(hex_row)
                            if row_bytes:
                                data[current_encoding * 16 + row_idx] = row_bytes[0]
                        except ValueError:
                            continue
                in_bitmap = False
                current_encoding = None
            elif in_bitmap:
                bitmap_rows.append(line)

    return data


def convert(inp, out):
    ext = os.path.splitext(inp)[1].lower()

    if ext == ".hex":
        data = hex_to_bin(inp)
    elif ext == ".ttf":
        data = ttf_to_bin(inp)
    elif ext == ".bdf":
        data = bdf_to_bin(inp)
    else:
        raise SystemExit("Unsupported font format")

    with open(out, "wb") as f:
        f.write(data)


if __name__ == "__main__":
    if len(sys.argv) != 3:
        raise SystemExit(
            "Usage: font2bin.py <font> <output>"
        )

    convert(sys.argv[1], sys.argv[2])
