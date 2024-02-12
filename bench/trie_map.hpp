#include <vector>
#include <array>
#include <memory>
#include <string_view>

class trie_map {
public:
    struct node {
        int32_t payload = -1;

        // children stored as indices in the parent map
        // to save space
        // index 0 (the root index) means that there is no child
        std::array<uint32_t, 0x100> child_index;
    };
    std::vector<std::unique_ptr<node>> m_nodes;

    node* get_root() {
        if (m_nodes.empty()) {
            m_nodes.push_back(std::make_unique<node>());
        }
        return m_nodes.front().get();
    }

    void insert(std::string_view key, int32_t value) {
        auto n = get_root();
        for (auto c : key) {
            auto& ci = n->child_index[uint8_t(c)];
            if (!ci) {
                ci = uint32_t(m_nodes.size());
                m_nodes.push_back(std::make_unique<node>());
                n = m_nodes.back().get();
            }
            else {
                n = m_nodes[ci].get();
            }
        }
        n->payload = value;
    }

    const int32_t* find(std::string_view key) const {
        if (m_nodes.empty()) return nullptr;
        auto n = m_nodes.front().get();
        for (auto c : key) {
            auto ci = n->child_index[uint8_t(c)];
            if (!ci) {
                return nullptr;
            }
            n = m_nodes[ci].get();
        }
        return &n->payload;
    }

    std::pair<uint32_t, int32_t> lower_bound(std::string_view val) const {
        std::pair<uint32_t, int32_t> ret = { 0, -1 };
        if (m_nodes.empty()) return ret;
        auto n = m_nodes.front().get();
        for (uint32_t i = 0; i < val.length(); ++i) {
            auto c = uint8_t(val[i]);
            auto ci = n->child_index[uint8_t(c)];
            if (!ci) {
                return ret;
            }
            n = m_nodes[ci].get();
            if (n->payload != -1) {
                ret = { i + 1, n->payload };
            }
        }
        return ret;
    }
};
