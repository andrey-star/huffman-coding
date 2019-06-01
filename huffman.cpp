#include "huffman.h"

#include <queue>

struct huffman::node {
    unsigned char value;
    ui freq;
    node *left, *right;

    node(unsigned char value, ui freq) {
        left = nullptr;
        right = nullptr;
        this->value = value;
        this->freq = freq;
    }

    bool isLeaf() {
        return !(this->left || this->right);
    }
};

void huffman::deleteNode(huffman::node *node) {
    if (!node) {
        return;
    }
    deleteNode(node->left);
    deleteNode(node->right);
    delete node;
}

struct huffman::node_wrapper {
    node *root;

    explicit node_wrapper(huffman::node *root) : root(root) {}

    ~node_wrapper() {
        deleteNode(root);
    }
};

struct huffman::code {
    ui size_;
    ui value_;

    code() : size_(0), value_(0) {}

    code(ui size_, ui value) : size_(size_), value_(value) {}

    void add_one() {
        value_ <<= 1u;
        value_ += 1;
        size_++;
    }

    void add_zero() {
        value_ <<= 1u;
        size_++;
    }

    ui size() {
        return size_;
    }

    ui value() {
        return value_;
    }

    bool get(ui j) {
        return static_cast<bool>((value_ >> j) & 1u);
    }

    void add(code &code) {
        value_ <<= code.size();
        size_ += code.size();
        value_ += code.value_;
    }

};

struct huffman::compare {
    bool operator()(node *l, node *r) {
        return (l->freq > r->freq);
    }
};

void huffman::get_codes(node *root, const code &cur_code, std::vector<code> &codes) {
    if (root->isLeaf()) {
        codes[root->value] = cur_code;
        return;
    }
    code left = cur_code;
    code right = cur_code;
    left.add_zero();
    right.add_one();
    get_codes(root->left, left, codes);
    get_codes(root->right, right, codes);
}

void huffman::build_huffman_tree(std::priority_queue<node *, std::vector<huffman::node *>,
        compare> &build, ui *freq) {
    for (unsigned char i = 0;; i++) {
        build.push(new huffman::node(i, freq[i]));
        if (i == 255) {
            break;
        }
    }
    while (build.size() > 1) {
        node *left = build.top();
        build.pop();
        node *right = build.top();
        build.pop();
        node *parent = new huffman::node('\0', left->freq + right->freq);
        parent->left = left;
        parent->right = right;
        build.push(parent);
    }
}

void huffman::gen_codes(ui *freq, std::vector<code> &codes) {
    std::priority_queue<huffman::node *, std::vector<huffman::node *>, compare> build;
    build_huffman_tree(build, freq);
    node_wrapper root = node_wrapper(build.top());
    get_codes(root.root, code(), codes);
}

void get_freq(buffered_reader &in, ui *freq) {
    unsigned char i;
    while (in.read_char(i)) {
        freq[i]++;
    }
}

huffman::code huffman::print_full_chars_from_code(code c, buffered_writer &out) {
    ui value = c.value();
    ui size = c.size();
    for (ui i = 0; i < size; i++) {
        if (size < BITS_IN_CHAR * (i + 1)) {
            ui new_size = (size - BITS_IN_CHAR * i);
            ui new_val = value & ((1u << new_size) - 1);
            return {new_size, new_val};
        } else {
            ui byte = (value >> (size - BITS_IN_CHAR * (i + 1)));
            auto ch = static_cast<unsigned char>(byte & 0xffu);
            out.write_char(ch);
        }
    }
    return {};
}

void print_number(ui n, buffered_writer &out) {
    for (ui i = 4; i-- > 0;) {
        out.write_char(static_cast<const unsigned char>((n >> (BITS_IN_CHAR * i)) & 0xffu));
    }
}

void huffman::encode(std::istream &input, std::ostream &output) {
    ui freq[ALPHABET_SIZE];
    for (ui &i : freq) {
        i = 0;
    }
    std::vector<code> codes(ALPHABET_SIZE);
    buffered_reader in(input);
    get_freq(in, freq);
    gen_codes(freq, codes);
    in.reset();
    buffered_writer out(output);
    for (ui i : freq) {
        print_number(i, out);
    }
    ui last_bits = 0;
    unsigned char c;
    for (size_t i = 0; i < ALPHABET_SIZE; i++) {
        last_bits += (1ull * codes[i].size() * freq[i]) & 0b111u;
        last_bits &= 0b111u;
    }
    out.write_char(static_cast<const unsigned char>(last_bits));
    code cur_code, rest;
    while (in.read_char(c)) {
        cur_code.add(codes[c]);
        rest = print_full_chars_from_code(cur_code, out);
        cur_code = rest;
    }
    if (cur_code.size() > 0) {
        out.write_char(static_cast<const unsigned char>(cur_code.value()));
    }
    in.reset();
}


ui read_number(buffered_reader &in) {
    ui res = 0;
    unsigned char c;
    for (ui i = 0; i < 4; i++) {
        if (!in.read_char(c)) {
            throw std::invalid_argument("Encoded file corrupted");
        }
        res <<= BITS_IN_CHAR;
        res += c;
    }
    return res;
}

void huffman::process_code(node *&cur_node, node *&root, code &code, buffered_writer &out, ui *freq) {
    for (ui i = code.size(); i-- > 0;) {
        if (!code.get(i)) {
            if (!cur_node->left) {
                throw std::invalid_argument("Encoded file corrupted");
            }
            cur_node = cur_node->left;
        } else {
            if (!cur_node->right) {
                throw std::invalid_argument("Encoded file corrupted");
            }
            cur_node = cur_node->right;
        }
        if (cur_node->isLeaf()) {
            freq[cur_node->value]++;
            out.write_char(cur_node->value);
            cur_node = root;
        }
    }
}

void huffman::decode(std::istream &input, std::ostream &output) {
    ui freq[ALPHABET_SIZE];
    for (ui &i : freq) {
        i = 0;
    }
    buffered_reader in(input);
    unsigned char c;
    for (ui &i : freq) {
        i = read_number(in);
    }
    if (!in.read_char(c)) {
        throw std::invalid_argument("Encoded file corrupted");
    }
    ui last_bits = c;
    if (last_bits > 7) {
        throw std::invalid_argument("Encoded file corrupted");
    }
    std::priority_queue<node *, std::vector<node *>, compare> build;
    build_huffman_tree(build, freq);
    node_wrapper root_wrapper = node_wrapper(build.top());
    node *root = root_wrapper.root;

    buffered_writer out(output);
    ui res_freq[ALPHABET_SIZE];
    for (ui &i : res_freq) {
        i = 0;
    }
    node *cur_node = root;
    code last_code;
    bool started = false;
    while (in.read_char(c)) {
        code cod = code(BITS_IN_CHAR, c);
        if (started) {
            process_code(cur_node, root, last_code, out, res_freq);
        }
        last_code = cod;
        started = true;
    }
    if (last_bits == 0) {
        process_code(cur_node, root, last_code, out, res_freq);
    } else {
        last_code = code(last_bits, c);
        process_code(cur_node, root, last_code, out, res_freq);
    }
    for (size_t i = 0; i < ALPHABET_SIZE; i++) {
        if (freq[i] != res_freq[i]) {
            throw std::invalid_argument("Encoded file corrupted");
        }
    }
}