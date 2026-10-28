#include <iostream>
#include "graph.hpp"

static void drain(GraphIterator& it) {
    while (it.hasNext()) std::cout << it.next() << " ";
    std::cout << std::endl;
}

int main() {
    Graph g;
    g.addEdge(1, 2);
    g.addEdge(1, 3);
    g.addEdge(2, 4);

    std::cout << "BFS desde 1: ";
    std::unique_ptr<GraphIterator> it = std::make_unique<BFSIterator>(g, 1);
    while (it->hasNext()) {
        std::cout << it->next() << " ";
    }
    std::cout << std::endl;

    // Cambio dinamico de iterador: BFS -> DFS
    it = std::make_unique<DFSIterator>(g, 1);
    std::cout << "DFS desde 1: ";
    drain(*it);

    // Reinicio desde otro nodo
    it->reset(2);
    std::cout << "DFS desde 2: ";
    drain(*it);

    // Grafo con ciclos: no debe entrar en bucle infinito
    Graph c;
    c.addEdge(1, 2);
    c.addEdge(2, 3);
    c.addEdge(3, 1);
    c.addEdge(3, 3);
    std::cout << "\nGrafo ciclico, BFS desde 1: ";
    BFSIterator b(c, 1);
    drain(b);
    std::cout << "Grafo ciclico, DFS desde 1: ";
    DFSIterator d(c, 1);
    drain(d);
}
