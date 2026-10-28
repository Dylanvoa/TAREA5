#pragma once
#include <algorithm>
#include <cstddef>
#include <memory>
#include <queue>
#include <stack>
#include <stdexcept>
#include <unordered_map>
#include <unordered_set>
#include <vector>

class BFSIterator;
class DFSIterator;

// Nodo del grafo: solo Graph puede modificar/consultar sus aristas.
class Node {
public:
    explicit Node(int id) : id_(id) {}
    int id() const { return id_; }
private:
    friend class Graph;
    void addNeighbor(int v) {
        if (std::find(out_.begin(), out_.end(), v) == out_.end()) out_.push_back(v);
    }
    const std::vector<int>& neighbors() const { return out_; }
    int id_;
    std::vector<int> out_;
};

// Grafo dirigido. La estructura interna es privada; solo los iteradores
// (friend) pueden consultar los vecinos.
class Graph {
public:
    void addEdge(int from, int to) {
        nodes_.try_emplace(from, from);
        nodes_.try_emplace(to, to);
        nodes_.at(from).addNeighbor(to);
    }
    bool hasNode(int id) const { return nodes_.count(id) > 0; }
    std::size_t size() const { return nodes_.size(); }

private:
    friend class BFSIterator;
    friend class DFSIterator;
    const std::vector<int>& neighborsOf(int id) const { return nodes_.at(id).neighbors(); }
    std::unordered_map<int, Node> nodes_;
};

// ---------- Interfaz del iterador ----------
class GraphIterator {
public:
    virtual ~GraphIterator() = default;
    virtual bool hasNext() = 0;
    virtual int next() = 0;
    virtual void reset(int start) = 0;  // reinicia desde otro nodo
};

// ---------- BFS ----------
class BFSIterator : public GraphIterator {
public:
    BFSIterator(const Graph& g, int start) : graph_(g) { reset(start); }

    bool hasNext() override { return !queue_.empty(); }

    int next() override {
        if (queue_.empty()) throw std::out_of_range("BFSIterator agotado");
        int cur = queue_.front();
        queue_.pop();
        for (int n : graph_.neighborsOf(cur)) {
            if (visited_.insert(n).second) queue_.push(n);  // marcar al encolar: evita ciclos
        }
        return cur;
    }

    void reset(int start) override {
        if (!graph_.hasNode(start)) throw std::invalid_argument("Nodo inicial inexistente");
        queue_ = {};
        visited_.clear();
        queue_.push(start);
        visited_.insert(start);
    }

private:
    const Graph& graph_;
    std::queue<int> queue_;
    std::unordered_set<int> visited_;
};

// ---------- DFS (iterativo, preorden) ----------
class DFSIterator : public GraphIterator {
public:
    DFSIterator(const Graph& g, int start) : graph_(g) { reset(start); }

    bool hasNext() override {
        while (!stack_.empty() && visited_.count(stack_.top())) stack_.pop();
        return !stack_.empty();
    }

    int next() override {
        if (!hasNext()) throw std::out_of_range("DFSIterator agotado");
        int cur = stack_.top();
        stack_.pop();
        visited_.insert(cur);
        const auto& ns = graph_.neighborsOf(cur);
        // Se apilan en orden inverso para visitar primero al primer vecino
        for (auto it = ns.rbegin(); it != ns.rend(); ++it)
            if (!visited_.count(*it)) stack_.push(*it);
        return cur;
    }

    void reset(int start) override {
        if (!graph_.hasNode(start)) throw std::invalid_argument("Nodo inicial inexistente");
        stack_ = {};
        visited_.clear();
        stack_.push(start);
    }

private:
    const Graph& graph_;
    std::stack<int> stack_;
    std::unordered_set<int> visited_;
};
