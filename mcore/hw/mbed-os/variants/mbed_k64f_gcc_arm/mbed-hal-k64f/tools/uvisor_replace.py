# Copyright 2015-2015 ARM Limited
#
# Licensed under the Apache License, Version 2.0
import re
import argparse

# parse input arguments
parser = argparse.ArgumentParser(description="""Search and replace Freescale pre-processor
                                             macros for memory access with uvisor ones""",
                                 usage='%(prog)s filename(s)')
parser.add_argument('filenames', nargs='+', help='input file(s) to process', metavar='FILE')
parser.parse_args()

# regex array
regex_dict = []

# regular read, 1/2 args, 8/16/32 bits
regex_dict.append({
    'src': r'(#define\s+HW_[A-Za-z_0-9]+_RD\((.+)\)\s+)\((HW_[A-Za-z_0-9]+)\(.+\).U\)',
    'dst': lambda p: p.group(1) + '(' + 'ADDRESS_READ('+p.group(3).lower()+'_t, '+p.group(3)+'_ADDR('+p.group(2)+'))' + ')'
})

# regular write, 2/3 args, 8/16/32 bits
regex_dict.append({
    'src': r'(#define\s+HW_[A-Za-z_0-9]+_WR\((.+), v\)\s+)\((HW_[A-Za-z_0-9]+)\(.+\).U = \(v\)\)',
    'dst': lambda p: p.group(1) + '(' + 'ADDRESS_WRITE('+p.group(3).lower()+'_t, '+p.group(3)+'_ADDR('+p.group(2)+'), v)' + ')'
})

# bitband read, 1/2 args, 8/16/32 bits
regex_dict.append({
    'src': r'(#define\s+BR_[A-Za-z_0-9]+\((.+)\)\s+)\(BITBAND_ACCESS(8|16|32)\((.+)\)\)',
    'dst': lambda p: p.group(1) + '(' + 'ADDRESS_READ(uint'+p.group(3)+'_t, '+'BITBAND_ADDRESS'+p.group(3)+'('+p.group(4)+'))' + ')'
})

# bitband write, 2/3 args, 8/16/32 bits
regex_dict.append({
    'src': r'(#define\s+BW_[A-Za-z_0-9]+\((.+) v\)\s+)\(BITBAND_ACCESS(8|16|32)\((.+)\) = \(v\)\)',
    'dst': lambda p: p.group(1) + '(' + 'ADDRESS_WRITE(uint'+p.group(3)+'_t, '+'BITBAND_ADDRESS'+p.group(3)+'('+p.group(4)+'), v)' + ')'
})

# regular read, 2/3 args, 8/16/32 bits
regex_dict.append({
    'src': r'(#define\s+BR_[A-Za-z_0-9]+\((.+)\)\s+)\((HW_[A-Za-z_0-9]+)\(.+\)\.B\.(.+)\)',
    'dst': lambda p: p.group(1) + '(' + 'UNION_READ('+p.group(3).lower()+'_t, '+p.group(3)+'_ADDR('+p.group(2)+'), U, B.'+p.group(4)+')' + ')'
})

# iterate over all lines in given input files
# no backup file is create since it is exepcted to run the script within git
lines_changed = 0
for line in fileinput.input(inplace=1):
    # iterate over all regular expressions (stop at the first one matching)
    for rgx in regex_dict:
        [line, n] = re.subn(rgx['src'], rgx['dst'], line.rstrip())
        if n > 0:
            lines_changed = lines_changed + 1
            break

    # write line to file (fileinput automatically redirects output to file)
    print(line)

# print quick report
if(lines_changed == 1):
    print('Changed ' + str(lines_changed) + ' line')
else:
    print('Changed ' + str(lines_changed) + ' lines')
