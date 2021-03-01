#!/usr/bin/env python

# To work on both Python 2 and 3
from __future__ import print_function

"""
This script is to test the pnmdump 1.0

You should run it as:

    $ python test/runtests-1.0.py ./pnmdump.exe
"""

import sys
import os
import zipfile
import tempfile
import contextlib
import shutil
import subprocess

#
# main command line entry point. Runs all tests
#

def main(exename):
    '''Check that zip file at zippath meets the spec'''

    tests = [
        (test_running_noargs, 'Running program with no arguments'),
        (test_running_version, 'Running with --version'),
        (test_running_toomany_args, 'Running with too many arguments'),
        (test_hexdump_toomany_args, 'Running with too many arguments'),
        (test_running_hexdump_invalid, 'Running hexdump invalid filename'),
        (test_running_hexdump, 'Running hexdump with test files'),
        (test_running_hexdump_stdin, 'Running hexdump with test files on stdin'),
        (test_running_p2top5, 'Running --P2toP5'),
        (test_running_p2top5_withp5, 'Running --P2toP5 with P5 file as input'),
        (test_running_p2top5_with_corrupted, 'Running --P2toP5 with corrupted files as input'),
        (test_running_p5top2, 'Running --P5toP2'),
        (test_running_p5top2_withp2, 'Running --P5toP2 with P2 file as input'),
        (test_running_p5top2_with_corrupted, 'Running --P5toP2 with corrupted files as input'),
        (test_running_rotate_p2, 'Running rotate with P2'),
        (test_running_rotate_p5, 'Running rotate with P5'),
        (test_running_rotate90_p2, 'Running rotate90 with P2'),
        (test_running_rotate90_p5, 'Running rotate90 with P5'),
    ]

    # Loop through all tests...
    for testfunc, description in tests:
        print('Checking %s ...' % description)
        if not testfunc(exename):
            return 1
        else:
            print('Okay!\n')

    print(' ---------  All tests passed! ------------ \n')
    return 0

#
# The test functions.
#

USAGE = '''Usage:
./pnmdump.exe --version
./pnmdump.exe --usage
./pnmdump.exe --hexdump [FILE]
'''

BAD_USAGE = 'pnmdump: bad arguments\n' + USAGE

def test_running_noargs(exename):
    args = []
    out = ''
    err = BAD_USAGE
    ret = 1
    return test_with_args(exename, args, out, err, ret)

def test_running_version(exename):
    args = ['--version']
    out = '1.0\n'
    err = ''
    ret = 0
    return test_with_args(exename, args, out, err, ret)

def test_running_toomany_args(exename):
    out = ''
    err = BAD_USAGE
    ret = 1
    for n in 3, 4, 5:
        args = ['foo'] * n
        if not test_with_args(exename, args, out, err, ret):
            return False

    return True

def test_hexdump_toomany_args(exename):
    out = ''
    err = BAD_USAGE
    ret = 1
    args = ['--hexdump', 'bad', 'arguments']
    return test_with_args(exename, args, out, err, ret)

def test_running_hexdump_invalid(exename):
    filename = 'invalid_invalid.txt'
    args = ['--hexdump', filename]
    err = 'No such file: "%s"\n' % filename
    return test_with_args(exename, args, '', err, 1)

hd_filenames = 'hello.c', '127.dat', 'J.pbm'
# Reading 127.dat on stdin will fail on Windows
hd_filenames_stdin = [f for f in hd_filenames if f != '127.dat']

def test_running_hexdump(exename):
    for filename in hd_filenames:
        filepath = os.path.join('tests', filename)
        args = ['--hexdump', filepath]
        with open(filepath + '.txt') as expected_file:
            expected_output = expected_file.read()
        if not test_with_args(exename, args, expected_output, '', 0):
            return False
    return True

def test_running_hexdump_stdin(exename):
    for filename in hd_filenames_stdin:
        filepath = os.path.join('tests', filename)
        args = ['--hexdump']
        with open(filepath + '.txt') as expected_file:
            expected_output = expected_file.read()
        with open(filepath, "rb") as inputfile:
            bytes_input = inputfile.read()
        if not test_with_args(exename, args, expected_output, '', 0, bytes_input):
            return False
    return True

def test_running_p2top5_withp5(exename):
    tempdir = tempfile.mkdtemp()
    outfilepath = os.path.join(tempdir, 'output.pgm')
    args = ['--P2toP5', 'tests/feep_p5.pgm', outfilepath]
    return test_with_args(exename, args, '', 'Input is not in P2 format\n', 1)

def test_running_p2top5_with_corrupted(exename):
    for n in range(1, 4+1):
        filename = 'tests/feep_p2_corrupted%d.pgm' % n
        tempdir = tempfile.mkdtemp()
        outfilepath = os.path.join(tempdir, 'output.pgm')
        args = ['--P2toP5', filename, outfilepath]
        if not test_with_args(exename, args, '', 'Corrupted input file\n', 1):
            return False
    return True

def test_running_p5top2_with_corrupted(exename):
    for n in range(1, 2+1):
        filename = 'tests/feep_p5_corrupted%d.pgm' % n
        tempdir = tempfile.mkdtemp()
        outfilepath = os.path.join(tempdir, 'output.pgm')
        args = ['--P5toP2', filename, outfilepath]
        if not test_with_args(exename, args, '', 'Corrupted input file\n', 1):
            return False
    return True

def test_running_p2top5(exename):
    return test_iofile(exename, ['--P2toP5', 'tests/feep_p2.pgm'], 'feep_p5.pgm', 'tests/feep_p5.pgm')

def test_running_p5top2_withp2(exename):
    tempdir = tempfile.mkdtemp()
    outfilepath = os.path.join(tempdir, 'output.pgm')
    args = ['--P5toP2', 'tests/feep_p2.pgm', outfilepath]
    return test_with_args(exename, args, '', 'Input is not in P5 format\n', 1)

def test_running_p5top2(exename):
    return test_iofile(exename, ['--P5toP2', 'tests/feep_p5.pgm'], 'feep_p2.pgm', 'tests/feep_p2.pgm')

def test_running_rotate_p2(exename):
    for name in 'feep', 'baboon':
        if not test_iofile(exename, ['--rotate', 'tests/%s_p2.pgm' % name],
                '%s_p2_r.pgm' % name, 'tests/%s_p2_r.pgm' % name):
            return False
    return True

def test_running_rotate_p5(exename):
    for name in 'feep', 'baboon':
        if not test_iofile(exename, ['--rotate', 'tests/%s_p5.pgm' % name],
                '%s_p5_r.pgm' % name, 'tests/%s_p5_r.pgm' % name):
            return False
    return True

def test_running_rotate90_p2(exename):
    for name in 'feep', 'baboon':
        if not test_iofile(exename, ['--rotate90', 'tests/%s_p2.pgm' % name],
                '%s_p2_r90.pgm' % name, 'tests/%s_p2_r90.pgm' % name):
            return False
    return True

def test_running_rotate90_p5(exename):
    for name in 'feep', 'baboon':
        if not test_iofile(exename, ['--rotate90', 'tests/%s_p5.pgm' % name],
                '%s_p5_r90.pgm' % name, 'tests/%s_p5_r90.pgm' % name):
            return False
    return True

def test_iofile(exename, args, outfilename, outfilematch):
    cmdline = [exename] + args + [outfilename]
    tempdir = tempfile.mkdtemp()
    outfilepath = os.path.join(tempdir, outfilename)
    args = args + [outfilepath]
    if not test_with_args(exename, args, '', '', 0):
        return False
    with open(outfilepath, "rb") as outfile:
        data = outfile.read()
    with open(outfilematch, "rb") as filematch:
        expected_data = filematch.read()
    if data != expected_data:
        print('I ran:', ' '.join(cmdline))
        print("And the output didn't match", outfilematch)
        return False
    return True

def test_with_args(exename, args,
        out_expected, err_expected, ret_expected, stdin=None):
    '''Test running the program with args and verify expected output'''
    out, err, ret = run_with_args(args, exename, stdin=stdin)
    if ( out_expected != out
      or err_expected != err
      or ret_expected != (ret & 1)):
        msg = 'stdout, stderr or return code incorrect.'
        expected = pretty_run_with_args(exename, args, out_expected,
                err_expected, ret_expected)
        found = pretty_run_with_args(exename, args, out, err, ret)
        failed_test_message(msg, expected, found)
        return False

    return True

#
# Need to give a good error message so that students
# can understand...
#

FAILED_MESSAGE = '''\

Error: %s

I expected to see this:
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
%s
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
But what I actually found is this:
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
%s
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
'''
def failed_test_message(msg, expected, found):
    msg = FAILED_MESSAGE % (msg, expected, found)
    print(msg, file=sys.stderr)

#
# Subprocess running functions
#

def pretty_out(out, fstring):
    lines = out.split('\n')
    lines = ['(%s): "%s"\n' % (fstring, line) for line in lines]
    return ''.join(lines)

def pretty_run_with_args(exename, args, out, err, ret):
    '''Format running program with args and seeing out, err and ret'''
    session_fmt = '''$ %s%s%s(return): %d'''
    cmdline = ' '.join([exename] + args) + '\n'
    if out:
        out = pretty_out(out, 'stdout')
    if err:
        err = pretty_out(err, 'stderr')
    return session_fmt % (cmdline, out, err, ret)


def run_subprocess(cmdline, stdin=None):
    '''Run command and return stdout, stderr and return code'''
    if stdin is None:
        process = subprocess.Popen(cmdline,
                stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        stdout, stderr = process.communicate()
    else:
        process = subprocess.Popen(cmdline, stdin=subprocess.PIPE,
                stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        stdout, stderr = process.communicate(input=stdin)
    # Should work on Py 2 and 3
    stdout = stdout.decode('ascii', errors='replace').replace('\r\n', '\n')
    stderr = stderr.decode('ascii', errors='replace').replace('\r\n', '\n')
    return stdout, stderr, process.returncode

def run_with_args(args, exename, stdin=None):
    '''Run the student's program with args. Return out, err, ret'''
    cmdline = [exename] + args
    return run_subprocess(cmdline, stdin=stdin)

if __name__ == "__main__":
    sys.exit(main(*sys.argv[1:]))
