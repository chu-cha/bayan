# File Duplicates Finder (Bayan)

## Description

A utility for finding duplicate files with flexible scan configuration options. Supports various hashing algorithms and file filtering.

## Command Line Parameters

### Basic Parameters

| Parameter         | Description |
|-------------------|-------------|
| `-h`, `--help`    | Show help message and exit |
| `-d`, `--dir` <paths> | Directories to scan (multiple paths can be specified separated by spaces) |
| `-x`, `--exdir` <paths> | Directories to exclude from scanning (multiple paths can be specified separated by spaces) |
| `-l`, `--level`   | Deep scan mode (with recursive subdirectory traversal) |
| `-s`, `--min_size` <bytes> | Minimum file size to process (default: 1 byte) |
| `-b`, `--block_size` <bytes> | File read block size (default: 4096 bytes) |
| `-a`, `--hash_algo` <algorithm> | Hashing algorithm (available: `crc32`, `md5`, `sha1`) |

## Usage Examples

1. Basic scan:
   ```bash
   ./bayan -d /path/to/directory
   ```

2. Scan with directory exclusions:
```bash
./bayan -d /path1 /path2 -x /path1/temp /path2/cache
```

3. Deep scan with size filtering:
```bash
./bayan -d /data -l -s 1024
```
4. Scan with custom settings:
```bash
./bayan -d /storage -x /storage/tmp -l -b 8192 -a md5
```

5. Output Format
The program outputs duplicate file groups in the following format:
```
Duplicate group:
  /path/to/file1
  /path/to/file2
  ...
Size: X bytes
Hash: XXXXXXXX
```

## Notes

All paths must be absolute or relative to the current working directory

1. Shows help message when no parameters are specified

2. Requires read permissions for scanned directories

3. Execution time depends on:

* Number and size of files

* Selected hashing algorithm

* Scan depth
