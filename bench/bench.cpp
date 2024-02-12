// Copyright (c) Borislav Stanimirov
// SPDX-License-Identifier: MIT
//
#include "trie_map.hpp"

#include "picobench/picobench.hpp"

#include "repo_root.h"

#include <vector>
#include <cstdlib> // for rand
#include <fstream>

std::vector<std::string> strings = {
    "widow",
    "Watson",
    "Tom",
    "dead",
};

std::string read_file(const std::string& filename)
{
    std::ifstream file(filename);
    std::string content;
    file.seekg(0, std::ios::end);
    content.reserve(size_t(file.tellg()) + 1);
    file.seekg(0, std::ios::beg);
    content.assign(
        std::istreambuf_iterator<char>(file),
        std::istreambuf_iterator<char>()
    );
    return content;
}

std::vector<std::string> read_words(const std::string& filename)
{
    auto content = read_file(filename);
    std::vector<std::string> words;
    auto* cur_word = &words.emplace_back();
    for (char c : content)
    {
        if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '\'')
        {
            cur_word->push_back(c);
        }
        else if (!cur_word->empty())
        {
            cur_word = &words.emplace_back();
        }
    }
    return words;
}

const auto w = read_words(REPO_ROOT "/txt/hf_ch1.txt");

static void rand_vector(picobench::state& s)
{
    std::vector<int> v;
    for (auto _ : s)
    {
        v.push_back(rand());
    }
}
PICOBENCH(rand_vector); // Register the above function with picobench

// Another benchmarking function:
static void rand_vector_reserve(picobench::state& s)
{
    std::vector<int> v;
    v.reserve(s.iterations());
    for (auto _ : s)
    {
        v.push_back(rand());
    }
}
PICOBENCH(rand_vector_reserve);
