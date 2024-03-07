// Copyright (c) Borislav Stanimirov
// SPDX-License-Identifier: MIT
//
#include "trie_map.hpp"

#include "picobench/picobench.hpp"

#include "repo_root.h"

#include <fstream>
#include <random>
#include <unordered_map>


std::vector<std::string> strings = {
    "widow",
    "Watson",
    "Tom",
    "dead",
};

std::string read_file(const std::string& filename) {
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

std::vector<std::string> read_words(const std::string& filename) {
    auto content = read_file(filename);
    std::vector<std::string> words;
    auto* cur_word = &words.emplace_back();
    for (char c : content)     {
        if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '\'') {
            cur_word->push_back(c);
        }
        else if (!cur_word->empty()) {
            cur_word = &words.emplace_back();
        }
    }
    return words;
}

const auto w = read_words(REPO_ROOT "/txt/hf_ch1.txt");

PICOBENCH_SUITE("insert");

static void insert_unordered_map(picobench::state& s) {
    std::unordered_map<std::string_view, int> m;
    for (auto i : s) {
        m[w[i]] = i;
    }

    int sum = int(m.size());
    for (auto [k, v] : m) {
        sum += v;
    }
    s.set_result(sum);
}
PICOBENCH(insert_unordered_map);

static void insert_trie_map(picobench::state& s) {
    trie_map m;
    for (auto i : s) {
        m.insert(w[i], i);
    }

    int sum = 0;
    for (auto& n : m.m_nodes) {
        if (n->payload != -1) {
            ++sum;
            sum += n->payload;
        }
    }
    s.set_result(sum);
}
PICOBENCH(insert_trie_map);

PICOBENCH_SUITE("find");

static void find_unordered_map(picobench::state& s) {
    std::unordered_map<std::string_view, int> m;
    for (int i = 0; i < s.iterations(); ++i) {
        m[w[i]] = i;
    }
    std::minstd_rand rng(123);

    int sum = 0;
    for (auto _ : s) {
        auto i = rng() % s.iterations() + 10;
        std::string_view f = i >= s.iterations() ? "bagavag" : w[i].c_str();
        auto it = m.find(f);
        if (it != m.end()) {
            sum += it->second;
        }
    }
    s.set_result(sum);
}
PICOBENCH(find_unordered_map);

static void find_trie_map(picobench::state& s) {
    trie_map m;
    for (int i = 0; i < s.iterations(); ++i) {
        m.insert(w[i], i);
    }
    std::minstd_rand rng(123);

    int sum = 0;
    for (auto _ : s) {
        auto i = rng() % s.iterations() + 10;
        std::string_view f = i >= s.iterations() ? "bagavag" : w[i].c_str();
        auto it = m.find(f);
        if (it) {
            sum += *it;
        }
    }
    s.set_result(sum);
}
PICOBENCH(find_trie_map);
