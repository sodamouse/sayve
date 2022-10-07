/* The MIT License (MIT)

Copyright (c) 2022 Sodamouse

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE. */

#include "environment.hpp"
#include "strings.hpp"
#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <iomanip>

const std::string DEST = "/mnt/archive/saves/";

struct Entry
{
    std::string name;
    std::string path;
    bool active;

    friend std::ostream& operator<<(std::ostream& stream, const Entry& e)
    {
        stream << (e.active ? "[A] " : "[D] ");
        stream << e.name << "\n\t" << e.path;
        return stream;
    }
};

std::vector<Entry> parse_entries_file(const std::string& fp)
{
    auto rawFileContent = soda::read_all(fp);
    auto rawEntries = soda::split_string(rawFileContent, '\n');

    std::vector<Entry> result;
    for (const auto& r : rawEntries)
    {
        if (r.length() == 0)
            continue;

        auto splits = soda::split_string(r, ':');
        bool active = splits[0][0] == '#' ? false : true;

        result.push_back({
            active ? splits[0] : splits[0].substr(1, splits[0].size()),
            splits[1],
            active
        });
    }

    return result;
}

void backup_entry(const Entry& e)
{
    if (!e.active)
        return;

    if (!std::filesystem::exists(e.path))
    {
        std::cout << "Skipping " << e.name << ": " << std::quoted(e.path) << " does not exist.\n";
        return;
    }

    auto destination = DEST + e.name;

    if (!std::filesystem::exists(destination))
    {
        if (!std::filesystem::create_directory(destination))
        {
            std::cerr << "Could not create directory: " << std::quoted(destination) << '\n';
            return;
        }
    }

    auto options = std::filesystem::copy_options::recursive | std::filesystem::copy_options::update_existing;
    std::error_code ec {};

    std::filesystem::copy(e.path, destination, options);

    if (ec)
        std::cerr << "Error backing up files: " << ec << '\n';

    else
        std::cout << "Backed up: " << std::quoted(e.name) << '\n';
}

void restore_entry(Entry& e)
{
    if (!std::filesystem::exists(e.path))
    {
        if (!std::filesystem::create_directories(e.path))
        {
            std::cerr << "Could not create: " << std::quoted(e.path) << '\n';
            return;
        }
    }

    auto src = DEST + e.name;
    auto options = std::filesystem::copy_options::recursive | std::filesystem::copy_options::update_existing;
    std::error_code ec {};

    std::filesystem::copy(src, e.path, options);

    if (ec)
        std::cerr << "Error restoring files: " << ec << '\n';

    else
    {
        e.active = true;
        std::cout << "Restored: " << std::quoted(e.name) << '\n';
    }

}

void dump_database(const std::string& fp, const std::vector<Entry>& entries)
{
    std::fstream file(fp, std::ios::out);

    if (!file)
    {
        std::cout << "Could not save database file: " << std::quoted(fp) << '\n';
        return;
    }

    for (const auto& e : entries)
    {
        if (!e.active) file << '#';
        file << e.name << ':' << e.path << '\n';
    }
}

void print_usage()
{
    std::cout
            << "Usage: sayve [options]\n"
            << "\t-h, --help\t\tDisplay this message\n"
            << "\t-v, --version\t\tDisplay program version information\n"
            << "\t-l, --list\t\tList database entries\n"
            << "\t-r, --restore <e>\tRestore saves from backup\n"
            << "\t-d, --disable <e>\tDisable entry\n"
            << "\t-e, --enable <e>\tEnable entry\n"
            << "\t-f, --freeze <e>\tBackup entry and delete the save files at source\n"
            << "\t-a, --add <name> <path>\tAdd entry\n"
            << "\t-x, --delete <e>\tDelete entry\n";
}

int main(int argc, char* argv[])
{
    std::string pathsFilePath = "/home/soda/.config/sayve/paths.conf";
    auto entries = parse_entries_file(pathsFilePath);
    auto args = soda::map_args(argc, argv);

    for (const auto& kv : args)
    {
        if (kv.second == "-h" || kv.second == "--help")
        {
            print_usage();
            return 0;
        }

        if (kv.second == "-v" || kv.second == "--version")
        {
            std::cout << "Sayve (C++) v1.0\n";
            return 0;
        }

        else if (kv.second == "-l" || kv.second == "--list")
        {
            for (const auto& e : entries)
                std::cout << e << "\n\n";

            return 0;
        }

        else if (kv.second == "-r" || kv.second == "--restore")
        {
            std::size_t index = kv.first + 1;
            if (args.size() < index)
            {
                std::cout << "Please specify entry to restore.\n";
                return 1;
            }

            for (auto& e : entries)
            {
                if (!e.name.compare(args[index]))
                {
                    restore_entry(e);
                    dump_database(pathsFilePath, entries);
                }
            }

            return 0;
        }

        else if (kv.second == "-d" || kv.second == "--disable")
        {
            std::size_t index = kv.first + 1;
            if (args.size() < index)
            {
                std::cout << "Please specify entry to disable.\n";
                return 1;
            }

            for (auto& e : entries)
            {
                if (!e.name.compare(args[index]))
                {
                    e.active = false;
                    dump_database(pathsFilePath, entries);
                    std::cout << "Disabled: " << std::quoted(args[index]) << '\n';
                }
            }

            return 0;
        }

        else if (kv.second == "-e" || kv.second == "--enable")
        {
            std::size_t index = kv.first + 1;
            if (args.size() < index)
            {
                std::cout << "Please specify entry to enable.\n";
                return 1;
            }

            for (auto& e : entries)
            {
                if (!e.name.compare(args[index]))
                {
                    e.active = true;
                    dump_database(pathsFilePath, entries);
                    std::cout << "Enabled: " << std::quoted(args[index]) << '\n';
                }
            }

            return 0;
        }

        else if (kv.second == "-f" || kv.second == "--freeze")
        {
            std::size_t index = kv.first + 1;
            if (args.size() < index)
            {
                std::cout << "Please specify entry to freeze.\n";
                return 1;
            }

            for (auto& e : entries)
            {
                if (!e.name.compare(args[index]))
                {
                    backup_entry(e);

                    if (std::filesystem::exists(e.path))
                        std::filesystem::remove_all(e.path);

                    e.active = false;

                    dump_database(pathsFilePath, entries);
                    std::cout << "Froze: " << std::quoted(args[index]) << '\n';
                }
            }

            return 0;
        }

        else if (kv.second == "-a" || kv.second == "--add")
        {
            std::size_t nameIndex = kv.first + 1;
            std::size_t pathIndex = kv.first + 2;
            if (args.size() < nameIndex)
            {
                std::cout << "Please specify the name and path to the entry.\n";
                return 1;
            }

            if (args.size() < pathIndex)
            {
                std::cout << "Please specify the path to the entry.\n";
                return 1;
            }

            entries.push_back({
                args[nameIndex],
                args[pathIndex],
                true
            });

            dump_database(pathsFilePath, entries);
            std::cout << "Added: " << std::quoted(args[nameIndex]) << '\n';

            return 0;
        }

        else if (kv.second == "-x" || kv.second == "--delete")
        {
            std::size_t index = kv.first + 1;
            if (args.size() < index)
            {
                std::cout << "Please specify entry to delete.\n";
                return 1;
            }

            for (std::size_t i = 0; i < entries.size(); ++i)
            {
                if (!entries[i].name.compare(args[index]))
                {
                    entries.erase(entries.begin() + i);
                    dump_database(pathsFilePath, entries);
                    std::cout << "Deleted: " << std::quoted(args[index]) << '\n';
                }
            }

            return 0;
        }

        else
        {
            std::cerr << "Unrecognized arugment: " << std::quoted(kv.second) << '\n';
            return 1;
        }
    }

    for (const auto& e : entries)
        backup_entry(e);

    return 0;
}
