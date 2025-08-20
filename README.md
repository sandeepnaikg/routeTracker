# Smart Route Finder (C++ A* / Dijkstra with Traffic Simulation)

This project is a **console-based smart route finding system** built in **C++17**.  
It uses the **A* pathfinding algorithm** (with heuristic set to 0 → works like Dijkstra) to find the shortest route between two places.  
Additionally, it **simulates traffic congestion** by assigning random traffic levels (0.0–1.0) to every location.

---

##  Features
- **Graph-based city map** with locations and distances.  
- **Shortest path calculation** using A* search.  
- **Traffic simulation**: Each node gets a random traffic value between `0.0` (free) and `1.0` (jam).  
- **Traffic-aware routing**:  
  - Routes with **heavy traffic (≥ 0.8)** are avoided automatically.  
  - The algorithm guarantees the **best available path**.  
- **Case-insensitive input**: You can type locations in any format (`delhi`, `Delhi`, `DELHI`).  

---

##  Algorithms Used

### 1. Graph Representation
- The city is modeled as a **graph**:
  - **Nodes** = locations (Delhi, Noida, Gurgaon, etc.)  
  - **Edges** = distances between locations.  

### 2. A* Search Algorithm
- A popular **pathfinding algorithm** used in maps and games.  
- Uses:
  - **g(n)** = distance from start node to current node.  
  - **h(n)** = heuristic (estimated distance to destination).  
- In this project, `h(n) = 0`, so it behaves like **Dijkstra’s Algorithm**.

### 3. Dijkstra’s Algorithm (Special Case of A*)
- Guarantees the **shortest path** in a weighted graph.  
- Finds the least-cost path from the source to the destination.  

### 4. Traffic Simulation
- Each location is assigned a **random traffic factor** (0.0–1.0).  
- If a node has **traffic ≥ 0.8**, it is considered "jammed" and avoided in pathfinding.  

---

##  Build Instructions

### Requirements
- A **C++17 or later** compiler (g++, clang++, or MSVC).  
- Works on **macOS**, **Linux**, and **Windows**.

### Compilation

**macOS / Linux**
```bash
g++ -std=c++17 -O2 route.cpp -o route
./route
