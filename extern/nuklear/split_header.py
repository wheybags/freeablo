#!/usr/bin/env python3

with open('nuklear.h.orig', 'r') as f:
    all_lines = f.readlines()

header_lines = []
impl_lines = []

in_header = True

for line in all_lines:
    if line.strip() == '#ifdef NK_IMPLEMENTATION':
        in_header = False

    if in_header:
        header_lines.append(line)
    else:
        impl_lines.append(line)


with open('nuklear_header.h', 'w') as f:
    f.writelines(header_lines)

with open('nuklear_impl.h', 'w') as f:
    f.writelines(impl_lines)

