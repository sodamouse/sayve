# Sayve
Sayve is a cross-platform, command-line utility written in C++ that automates backing up save data.

## Building
``` shell
git clone https://github.com/sodamouse/sayve.git && cd sayve
make
```

## Usage
Sayve stores its database as a plain-text file in $HOME/.config/sayve/paths.conf. Populate this file with entries following this exact format:
``` shell
name of the game:path to its save file
```

Alternatively, use the --add <name> <path> argument to add entries through the program itself.

When run, and unless a command-line argument is supplied, Sayve will backup each of the entries in a directory heirarchy rooted at /mnt/archive/saves/. This path is currently hard-coded, but will later be exposed to a config file.

The program currently supports the following cli-aruments:
``` shell
Usage: sayve [options]
    -h, --help                Display this message
    -v, --version             Display program version information
    -l, --list                List database entries
    -r, --restore <e>         Restore saves from backup
    -d, --disable <e>         Disable entry
    -e, --enable <e>          Enable entry
    -f, --freeze <e>          Backup entry and delete the save files at source
    -a, --add <name> <path>   Add entry
    -x, --delete <e>          Delete entry
```
