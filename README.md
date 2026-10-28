# Tarea #5: Patrones de Diseño — Programación III (UTEC 2026-2)

| Ejercicio | Patrón | Carpeta |
|---|---|---|
| 1 | Singleton (TransactionManager concurrente) | `ejercicio1_singleton/` |
| 2 | Abstract Factory (motor OpenGL / Vulkan) | `ejercicio2_abstract_factory/` |
| 3 | Composite (expresiones simbólicas y derivadas) | `ejercicio3_composite/` |
| 4 | Iterator (BFS / DFS sobre grafo dirigido) | `ejercicio4_iterator/` |

## Compilar y ejecutar
```bash
mkdir build && cd build
cmake .. && make
./ejercicio1 && ./ejercicio2 && ./ejercicio3 && ./ejercicio4
```
Requiere un compilador con C++17.

## Decisiones de diseño
- **Ej. 1**: Singleton de Meyers (thread-safe en C++11). Cada `Transaction::execute()` modifica el saldo bajo el mutex de la cuenta; el historial tiene su propio mutex.
- **Ej. 2**: el cliente solo usa `RenderFactory`; cambiar de plataforma es reasignar el `unique_ptr`, sin `if`/`switch`.
- **Ej. 3**: sin `dynamic_cast` (se usa `isConstant()` virtual). Derivadas con regla del producto, cociente, potencia y cadena, con simplificación algebraica básica.
- **Ej. 4**: `Graph` y `Node` ocultan su estructura; solo los iteradores (friend) consultan vecinos. Se marcan nodos visitados para evitar ciclos infinitos.
