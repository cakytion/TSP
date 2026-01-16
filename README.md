# 🗺️ Traveling Salesman Problem (TSP) Solver

![Language](https://img.shields.io/badge/Language-C-blue)
![Algorithm](https://img.shields.io/badge/Algorithm-Brute%20Force%20%7C%20Nearest%20Neighbor-orange)
![Subject](https://img.shields.io/badge/Course-CPE112%20Data%20Structures-green)

> **"Finding the optimal route across 14 provinces in Thailand."**
> A C-based application that compares algorithms to solve the classic Traveling Salesman Problem for travel planning.

---

## 📄 Full Project Report
For the complete analysis, algorithm comparison, and complexity study ($O(n!)$ vs $O(n^2)$), please view the full report below:

https://drive.google.com/file/d/18BW39C0qi9NBWIP6-WRAt6NAc1WzfvCA/view?usp=sharing
*(Note: Please click the link above to view the detailed documentation)*

---

## 🔍 Project Overview

### 🎯 Objective
To apply Data Structure concepts to solve real-world logistics problems. This project implements and compares two major algorithms to find the shortest path for visiting multiple provinces in Thailand (e.g., Bangkok, Chiang Mai, Phuket).

### ⚙️ Algorithms Implemented
1.  **Brute Force Algorithm:**
    * **Method:** Calculates *every possible permutation* of routes.
    * **Pros:** Guarantees the **absolute shortest path** (100% Accuracy).
    * **Cons:** Very slow for many cities ($O(n!)$ complexity).
2.  **Nearest Neighbor Algorithm:**
    * **Method:** Always chooses the closest unvisited city.
    * **Pros:** Very fast calculation.
    * **Cons:** May not find the optimal path (Greedy approach).

---

## 💻 How It Works (Demo)

The program allows users to select provinces, choose an algorithm, and visualize the route.

![Program Result](result_screen.png)
*(Example: Calculating optimal route using Brute Force Algorithm)*

### 🛠️ Key Features (Code Structure)
The system uses **Adjacency Matrix** and **Adjacency List** to represent the map and distances between provinces.

```c
// Sample from: TSP_G12_code (1).c
struct Graph {
  int numVertices; // จำนวน vertices ในกราฟ
  char provinceCodes[MAX_PROVINCES][MAX_CODE_LENGTH]; // รหัสจังหวัด
  long long adjMatrix[MAX_PROVINCES][MAX_PROVINCES]; // Adjacency Matrix
  struct node **adjLists; // Adjacency List
};
