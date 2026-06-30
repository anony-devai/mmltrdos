# MML Transposer DOS (MMLTRDOS)

MMLTRDOS is the 16-bit real-mode MS-DOS edition of the MML Transposer tool.
It transposes plain-text MML source files commonly used in NSF music
creation. This program does not process NSF data directly.

This edition uses the 16-bit optimized engine `mmleng16.c`, which maintains
the same functional specifications as the Win32 version while minimizing
memory usage for real-mode DOS environments.

## Supported Environment

- 16-bit Real Mode MS-DOS
- Windows 95 MS-DOS mode (verified)

## Usage

```bash
mmltrdos [options] <input.mml> [shift] [output.mml]
```

## Options

- `-i <file>`  
  Specify input file.

- `-o <file>`  
  Specify output file.

- `-s <shift>`  
  Transpose amount (-12 to +12).  
  `0` means no transpose.  
  Leading `+` is optional.

- `-p`, `--pure`  
  Pure mode (no formatting).

- `-f`, `--fmt`  
  FMT mode (formatted output).

- `-r`, `--relative`  
  Relative octave mode.  
  First note uses `oX`, subsequent notes use `<` and `>`.

- `-a`, `--absolute`  
  Absolute octave mode (all notes use `oX`).

- `-d`, `--dch`  
  Transpose D-channel (noise channel included).  
  The D-channel octave is always fixed to `o0`.

- `-h`, `--help`  
  Show English help (`/h`, `/?` also supported).

- `-hjp`, `/hjp`  
  Show Japanese help.

## Examples

```bash
mmltrdos input.mml
mmltrdos input.mml output.mml
mmltrdos input.mml -s 0
mmltrdos input.mml 5
mmltrdos input.mml -2 output.mml -p
mmltrdos input.mml +3 output.mml -p -a
mmltrdos input.mml +7 output.mml -f
mmltrdos input.mml -5 output.mml -f -r -d
```

## Mode Description

### Pure / FMT Mode
Notes and octaves are automatically reassigned while preserving the
original intent of the MML.

- `-p` / `--pure` — no formatting  
- `-f` / `--fmt` — formatted output

### Octave Mode
- `-r` / `--relative` — relative octave (`<>`)  
- `-a` / `--absolute` — absolute octave (`oX`)  

These can be combined with Pure / FMT modes.

### D-Channel Transpose
- `-d` / `--dch` transposes the D-channel (noise channel included).  
- The D-channel octave is always fixed to `o0`.

## Notes

- This program transposes MML source code used for NSF music creation.
- For detailed help, use:
  - `mmltrdos -h | more` — English help  
  - `mmltrdos -hjp | more` — Japanese help

All source code in this project was created with the assistance of Copilot.
Unexpected issues may occur.
