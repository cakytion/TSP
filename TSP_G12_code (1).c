#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_PROVINCES 20
#define MAX_CODE_LENGTH 4

// โครงสร้างสำหรับเก็บข้อมูลของโหนดใน adjacency list
struct node {
  int vertex; // ดัชนีของ vertex ที่โหนดนี้เชื่อมต่อไป
  struct node *next; // โหนดถัดไปใน adjacency list
};

// โครงสร้างสำหรับเก็บข้อมูลของกราฟ
struct Graph {
  int numVertices; // จำนวน vertices ในกราฟ
  char provinceCodes[MAX_PROVINCES][MAX_CODE_LENGTH]; // รหัสจังหวัด
  long long adjMatrix[MAX_PROVINCES]
                     [MAX_PROVINCES]; // adjacency matrix สำหรับเก็บค่า cost
  struct node **adjLists; // adjacency list สำหรับเก็บโหนดที่เชื่อมต่อกัน
};

// สร้างโหนดใหม่ใน adjacency list
struct node *createNode(int v) {
  struct node *newNode = malloc(sizeof(struct node));
  newNode->vertex = v;
  newNode->next = NULL;
  return newNode;
}

// เพิ่มขอบ (edge) เข้าไปในกราฟ (ในรูปแบบ adjacency list)
void addEdge(struct Graph *graph, int s, int d, long long cost) {
  struct node *newNode = createNode(d);
  newNode->next = graph->adjLists[s];
  graph->adjLists[s] = newNode;
}

// อ่านข้อมูลกราฟจากไฟล์
int readProvinceData(struct Graph *graph, const char *filename) {
  FILE *file = fopen(filename, "r");
  if (file == NULL) {
    perror("Error opening file");
    return 0;
  }

  // อ่านจำนวน vertices
  fscanf(file, "%d", &graph->numVertices);

  // สร้าง adjacency list สำหรับแต่ละ vertex
  graph->adjLists = malloc(graph->numVertices * sizeof(struct node *));
  for (int i = 0; i < graph->numVertices; i++) {
    graph->adjLists[i] = NULL;
  }

  // อ่านรหัสจังหวัด
  for (int i = 0; i < graph->numVertices; i++) {
    fscanf(file, "%s", graph->provinceCodes[i]);
  }

  // อ่าน adjacency matrix และสร้าง adjacency list
  for (int i = 0; i < graph->numVertices; i++) {
    for (int j = 0; j < graph->numVertices; j++) {
      fscanf(file, "%lld", &graph->adjMatrix[i][j]);
      if (graph->adjMatrix[i][j] != 0) {
        addEdge(graph, i, j, graph->adjMatrix[i][j]);
      }
    }
  }

  fclose(file);
  return 1;
}

// หาดัชนีของจังหวัดจากรหัสจังหวัด
int findProvinceIndex(struct Graph *graph, char *provinceCode) {
  for (int i = 0; i < graph->numVertices; i++) {
    if (strcmp(graph->provinceCodes[i], provinceCode) == 0) {
      return i;
    }
  }
  return -1;
}

// หาจังหวัดที่ใกล้ที่สุดที่ยังไม่ถูกเยี่ยมชม
int findNearestNeighbor(struct Graph *graph, int currentVertex, int *visited,
                        int *included) {
  long long minDistance = LLONG_MAX;
  int nearestNeighbor = -1;

  for (int i = 0; i < graph->numVertices; i++) {
    if (graph->adjMatrix[currentVertex][i] != 0 && !visited[i] && included[i] &&
        graph->adjMatrix[currentVertex][i] < minDistance) {
      minDistance = graph->adjMatrix[currentVertex][i];
      nearestNeighbor = i;
    }
  }

  return nearestNeighbor;
}

// สลับค่าระหว่างตัวแปรสองตัว
void swap(int *a, int *b) {
  int temp = *a;
  *a = *b;
  *b = temp;
}

// คำนวณค่ารวมของเส้นทางที่เลือก
long long calculateCost(struct Graph *graph, int *permutation,
                        int numProvincesToVisit) {
  long long cost = 0;
  for (int i = 0; i < numProvincesToVisit; i++) {
    cost += graph->adjMatrix[permutation[i]][permutation[i + 1]];
  }
  cost +=
      graph->adjMatrix[permutation[numProvincesToVisit - 1]][permutation[0]];
  return cost;
}

// สร้างการเรียงลำดับใหม่ของอาร์เรย์แบบ recursive
void permute(int *arr, int start, int end, struct Graph *graph, int startVertex,
             long long *minCost, int *bestPath) {
  if (start == end) {
    long long currentCost = calculateCost(graph, arr, end);

    if (currentCost < *minCost && arr[0] == startVertex) {
      *minCost = currentCost;
      for (int i = 0; i < end + 1; i++) {
        bestPath[i] = arr[i];
      }
    }
  } else {
    for (int i = start; i <= end; i++) {
      swap(&arr[start], &arr[i]);
      permute(arr, start + 1, end, graph, startVertex, minCost, bestPath);
      swap(&arr[start], &arr[i]);
    }
  }
}

// อัลกอริทึม brute force สำหรับการแก้ปัญหา TSP
void TSP_BruteForce(struct Graph *graph, int startVertex, int *included) {
  int numIncluded = 0;
  for (int i = 0; i < graph->numVertices; i++) {
    if (included[i]) {
      numIncluded++;
    }
  }

  int S[numIncluded];
  int j = 0;
  for (int i = 0; i < graph->numVertices; i++) {
    if (included[i]) {
      S[j++] = i;
    }
  }

  long long minCost = LLONG_MAX;
  int bestPath[numIncluded];

  permute(S, 0, numIncluded - 1, graph, startVertex, &minCost, bestPath);

  printf("TSP Path using Brute Force: ");
  for (int i = 0; i < numIncluded; i++) {
    printf("%s -> ", graph->provinceCodes[bestPath[i]]);
  }
  printf("%s\n", graph->provinceCodes[startVertex]);
  printf("Total Cost: %lld\n", minCost);
}

// อัลกอริทึม nearest neighbor สำหรับการแก้ปัญหา TSP (พร้อม backtracking)
void TSP_NearestNeighbor(struct Graph *graph, int startVertex, int *included) {
  int visited[MAX_PROVINCES] = {0};
  int path[MAX_PROVINCES];
  int currentVertex = startVertex;
  long long totalCost = 0;
  int provincesToVisit = 0;

  for (int i = 0; i < graph->numVertices; i++) {
    if (included[i]) {
      provincesToVisit++;
    }
  }

  visited[startVertex] = 1;
  path[0] = startVertex;
  int count = 1;

  while (count < provincesToVisit) {
    int nearest = findNearestNeighbor(graph, currentVertex, visited, included);

    if (nearest != -1) {
      visited[nearest] = 1;
      path[count] = nearest;
      totalCost += graph->adjMatrix[currentVertex][nearest];
      currentVertex = nearest;
      count++;
    } else {
      // Backtracking
      if (count > 1) {
        count--;
        visited[currentVertex] = 0;
        currentVertex = path[count - 1];
      } else {
        printf(
            "Error: Could not find a valid path for the selected provinces.\n");
        printf("This might be due to limitations of the Nearest Neighbor "
               "algorithm.\n");
        return;
      }
    }
  }

  totalCost += graph->adjMatrix[currentVertex][startVertex];
  path[provincesToVisit] = startVertex;

  printf("TSP Path using Nearest Neighbor: ");
  for (int i = 0; i <= provincesToVisit; i++) {
    printf("%s", graph->provinceCodes[path[i]]);
    if (i < provincesToVisit) {
      printf(" -> ");
    }
  }
  printf("\nTotal Distance: %lld\n", totalCost);
}

// เพิ่มจังหวัดใหม่เข้าไปในกราฟและอัปเดตไฟล์ข้อมูล
int addNewProvince(const char *filename, const char *provinceCode,
                   long long *distances, int numExistingProvinces,
                   struct Graph *graph) {
  for (int i = 0; i < numExistingProvinces; i++) {
    if (strcmp(graph->provinceCodes[i], provinceCode) == 0) {
      printf("Province code %s already exists.\n", provinceCode);
      return 0;
    }
  }

  FILE *file = fopen(filename, "r+");
  if (file == NULL) {
    perror("Error opening file");
    return 0;
  }

  // อัปเดตจำนวน provinces
  fprintf(file, "%d\n", numExistingProvinces + 1);

  // อัปเดตรหัสจังหวัด
  for (int i = 0; i < numExistingProvinces; i++) {
    fprintf(file, "%s ", graph->provinceCodes[i]);
  }
  fprintf(file, "%s\n", provinceCode);

  // อัปเดต adjacency matrix
  for (int i = 0; i < numExistingProvinces; i++) {
    for (int j = 0; j < numExistingProvinces; j++) {
      fprintf(file, "%lld ", graph->adjMatrix[i][j]);
    }
    fprintf(file, "%lld ", distances[i]);
    fprintf(file, "\n");
  }

  for (int i = 0; i < numExistingProvinces; i++) {
    fprintf(file, "%lld ", distances[i]);
  }
  fprintf(file, "0\n");

  fclose(file);
  return 1;
}

// ล้างบัฟเฟอร์อินพุต (กำจัดตัวอักษรที่ค้างอยู่)
void clear_input_buffer() {
  int c;
  while ((c = getchar()) != '\n' && c != EOF)
    ;
}

int main() {
  struct Graph graph;
  char filename[] = "province_data.txt";
  char userInput[MAX_CODE_LENGTH];
  int numProvincesToVisit;

  // อ่านข้อมูลจังหวัดจากไฟล์
  if (!readProvinceData(&graph, filename)) {
    return 1;
  }

  while (1) {
    system("cls");
    printf("\n=========================\n");
    printf("     Province Travel     \n");
    printf("=========================\n\n");

    // แสดงรายชื่อจังหวัดที่มีอยู่
    printf("Available Provinces: ");
    for (int i = 0; i < graph.numVertices; i++) {
      printf("%s ", graph.provinceCodes[i]);
    }
    printf("\n\n");

    // เมนูหลัก
    printf("Choose an option:\n");
    printf("1. Calculate TSP\n");
    printf("2. Add new province\n");
    printf("3. Exit\n");
    printf("Enter your choice: ");

    int choice;
    while (scanf("%d", &choice) != 1) {
      printf("Invalid input. Please enter a number: ");
      clear_input_buffer();
    }

    if (choice == 1) {
      // เมนูเลือกอัลกอริทึม TSP
      printf("\nChoose a TSP algorithm:\n");
      printf("1. Nearest Neighbor\n");
      printf("   - Advantage: Fast and simple to implement. Good for quick "
             "estimations.\n");
      printf("   - Use case: When a near-optimal solution is sufficient and "
             "speed is a priority.\n");
      printf("   - Note: May not find a solution in some cases due to "
             "algorithm limitations.\n"); // เพิ่มหมายเหตุ
      printf("2. Brute Force\n");
      printf("   - Advantage: Guaranteed to find the most optimal solution.\n");
      printf("   - Use case: When finding the absolute best solution is "
             "critical, but may be slow for larger problems.\n");

      int algoChoice;
      while (scanf("%d", &algoChoice) != 1 ||
             (algoChoice != 1 && algoChoice != 2)) {
        printf("Invalid input. Please enter 1 or 2: ");
        clear_input_buffer();
      }

      // จำนวนจังหวัดที่ต้องการเยี่ยมชม
      printf("\nHow many provinces do you want to visit? ");
      while (scanf("%d", &numProvincesToVisit) != 1 ||
             numProvincesToVisit <= 0) {
        printf("Invalid input. Please enter a positive number: ");
        clear_input_buffer();
      }

      int included[MAX_PROVINCES] = {0};
      int valid_province_count = 0;

      // รับรหัสจังหวัดที่ต้องการเยี่ยมชม
      printf("\nEnter the province codes you want to visit (separated by "
             "spaces):\n");
      while (valid_province_count < numProvincesToVisit) {
        scanf("%s", userInput);
        int index = findProvinceIndex(&graph, userInput);
        if (index != -1) {
          included[index] = 1;
          valid_province_count++;
        } else {
          printf("Invalid province code: %s. Please try again.\n", userInput);
        }
      }

      // รับรหัสจังหวัดเริ่มต้น
      printf("\nEnter your starting province code: ");
      scanf("%s", userInput);
      int startVertex = findProvinceIndex(&graph, userInput);

      while (startVertex == -1) {
        printf("Invalid starting province code. Please try again.\n");
        scanf("%s", userInput);
        startVertex = findProvinceIndex(&graph, userInput);
      }

      if (startVertex != -1) {
        // เลือกใช้อัลกอริทึม
        if (algoChoice == 1) {
          TSP_NearestNeighbor(&graph, startVertex, included);
        } else if (algoChoice == 2) {
          TSP_BruteForce(&graph, startVertex, included);
        } else {
          printf("Invalid algorithm choice!\n");
        }
      } else {
        printf("Invalid starting province code.\n");
      }

      clear_input_buffer();

    } else if (choice == 2) {
      // เพิ่มจังหวัดใหม่
      char newProvinceCode[MAX_CODE_LENGTH];
      long long distances[MAX_PROVINCES];

      printf("\nEnter the new province code: ");
      scanf("%s", newProvinceCode);

      // ถ้ารหัสจังหวัดซ้ำ
      if (addNewProvince(filename, newProvinceCode, distances,
                         graph.numVertices, &graph)) {
        printf("\nEnter the distances from this province to all other "
               "provinces:\n");
        for (int i = 0; i < graph.numVertices; i++) {
          printf("Distance to %s: ", graph.provinceCodes[i]);
          while (scanf("%lld", &distances[i]) != 1) {
            printf("Invalid input. Please enter a number: ");
            clear_input_buffer();
          }
        }

        // อัปเดตข้อมูลในกราฟ
        graph.numVertices++;
        strcpy(graph.provinceCodes[graph.numVertices - 1], newProvinceCode);

        for (int i = 0; i < graph.numVertices; i++) {
          graph.adjMatrix[i][graph.numVertices - 1] = distances[i];
          graph.adjMatrix[graph.numVertices - 1][i] = distances[i];
        }

        graph.adjLists =
            realloc(graph.adjLists, graph.numVertices * sizeof(struct node *));
        graph.adjLists[graph.numVertices - 1] = NULL;

        for (int i = 0; i < graph.numVertices; i++) {
          if (graph.adjMatrix[graph.numVertices - 1][i] != 0) {
            addEdge(&graph, graph.numVertices - 1, i, distances[i]);
            addEdge(&graph, i, graph.numVertices - 1, distances[i]);
          }
        }

        printf("\nNew province added successfully!\n");
        clear_input_buffer();
      } else {
        printf(
            "\nError adding new province or province code already exists.\n");
        clear_input_buffer();
      }

    } else if (choice == 3) {
      // ออกจากโปรแกรม
      printf("Exiting program.\n");
      break;
    } else {
      printf("Invalid choice.\n");
      clear_input_buffer();
    }

    clear_input_buffer();
  }

  return 0;
}
