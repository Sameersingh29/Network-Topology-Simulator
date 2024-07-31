#include <GL/glut.h>
#include <vector>
#include <cmath>
#include <ctime>
#include <string>

bool animationRunning = false;
bool ringWaiting = false;
std::clock_t ringWaitStartTime;
const double ringWaitDuration = 2.0; // 2 seconds wait
bool showMainPage = true;
std::string projectTitle = "NETWORK TOPOLOGIES SIMULATION";

// Window dimensions
const int windowWidth = 800;
const int windowHeight = 600;

// Topology selection
enum Topology { BUS, MESH, RING, STAR, TREE };
Topology currentTopology = BUS;

// Animation parameters
float t = 0.0f; // Interpolation
bool dataDirection = true; // Data transfer direction
bool dataTransferComplete = false; // Flag of trip completion
float speed = 0.01f; // Speed of the data packet

// Bus topology parameters
float dataPos = -0.7f;
float clientPos = -0.7f;
float serverPos = 0.8f;
bool returnTrip = false;

// Mesh, Ring, Star topology parameters
const int numComputers = 6;
const float radius = 0.7f;
int currentComputer = 0;

// Star topology parameters
const float hubRadius = 0.1f;

// Tree topology parameters
const int numLevels = 3;
const int branchingFactor = 2;
const float nodeRadius = 0.05f;

bool waiting = false;
std::clock_t waitStartTime;
const int waitDuration = 3;

struct Node {
    float x, y;
    bool isRoot;
    bool isLeaf;
};

std::vector<Node> treeNodes;

struct Line {
    Node start;
    Node end;
};

std::vector<Line> lines;

// Function to draw one computer
void drawComputer(float x, float y, bool isClient, bool isServer) {
    if (isClient) {
        glColor3f(0.0f, 1.0f, 0.0f); // Green for client
    } else if (isServer) {
        glColor3f(0.0f, 0.0f, 1.0f); // Blue for server
    } else {
        glColor3f(1.0f, 1.0f, 1.0f); // White for regular computers
    }

    // Draw monitor
    glBegin(GL_QUADS);
    glVertex2f(x - 0.06f, y + 0.02f);
    glVertex2f(x + 0.06f, y + 0.02f);
    glVertex2f(x + 0.06f, y + 0.12f);
    glVertex2f(x - 0.06f, y + 0.12f);
    glEnd();

    // Draw monitor base
    glBegin(GL_LINE_LOOP);
    glVertex2f(x - 0.03f, y - 0.02f);
    glVertex2f(x + 0.03f, y - 0.02f);
    glVertex2f(x + 0.03f, y + 0.02f);
    glVertex2f(x - 0.03f, y + 0.02f);
    glEnd();

    // Draw computer case
    glBegin(GL_LINE_LOOP);
    glVertex2f(x - 0.05f, y - 0.08f);
    glVertex2f(x + 0.05f, y - 0.08f);
    glVertex2f(x + 0.05f, y - 0.02f);
    glVertex2f(x - 0.05f, y - 0.02f);
    glEnd();
}

// Function to draw data packet
void drawDataPacket(float x, float y) {
    glColor3f(1.0f, 0.0f, 0.0f); // Red for data packet
    glBegin(GL_QUADS);
    glVertex2f(x - 0.02f, y - 0.02f);
    glVertex2f(x + 0.02f, y - 0.02f);
    glVertex2f(x + 0.02f, y + 0.02f);
    glVertex2f(x - 0.02f, y + 0.02f);
    glEnd();
}

// Bus topology functions
void drawBus() {
    glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_LINES);
    glVertex2f(-0.9f, 0.0f);
    glVertex2f(0.9f, 0.0f);
    glEnd();
}

void displayBus() {
    drawBus();

    std::vector<float> computerPositions = {-0.7f, -0.4f, -0.1f, 0.2f, 0.5f, 0.8f};
    for (float pos : computerPositions) {
        drawComputer(pos, 0.1f, pos == computerPositions.front(), pos == computerPositions.back());
    }

    if (!dataTransferComplete) {
        float startX = computerPositions.front();
        float endX = computerPositions.back();

        if (!returnTrip) {
            dataPos = startX + t * (endX - startX);
        } else {
            dataPos = endX + t * (startX - endX);
        }

        drawDataPacket(dataPos, 0.0f);
    }
}

// Mesh topology functions
std::vector<Node> nodes;
std::vector<std::pair<int, int>> connections;

void initNodes() {
    nodes.clear();
    for (int i = 0; i < numComputers; ++i) {
        float theta = 2.0f * 3.1415926f * float(i) / float(numComputers);
        float x = radius * cosf(theta);
        float y = radius * sinf(theta);
        nodes.push_back({x, y, false, false});
    }
}

void initConnections() {
    connections.clear();
    for (int i = 0; i < numComputers; ++i) {
        for (int j = i + 1; j < numComputers; ++j) {
            connections.push_back({i, j});
        }
    }
}

void drawMesh() {
    glColor3f(1.0f, 1.0f, 1.0f);
    for (const auto& conn : connections) {
        glBegin(GL_LINES);
        glVertex2f(nodes[conn.first].x, nodes[conn.first].y);
        glVertex2f(nodes[conn.second].x, nodes[conn.second].y);
        glEnd();
    }
}

void displayMesh() {
    drawMesh();

    for (int i = 0; i < numComputers; i++) {
        drawComputer(nodes[i].x, nodes[i].y, i == 0, i == numComputers - 1);
    }

    if (!dataTransferComplete) {
        float x, y;
        if (dataDirection) {
            int totalConnections = connections.size();
            int currentConnectionIndex = static_cast<int>(t * totalConnections);
            if (currentConnectionIndex >= totalConnections) {
                currentConnectionIndex = totalConnections - 1;
            }

            auto conn = connections[currentConnectionIndex];
            float progress = (t * totalConnections) - currentConnectionIndex;

            x = nodes[conn.first].x + progress * (nodes[conn.second].x - nodes[conn.first].x);
            y = nodes[conn.first].y + progress * (nodes[conn.second].y - nodes[conn.first].y);
        } else {
            int totalConnections = connections.size();
            int currentConnectionIndex = static_cast<int>((1.0f - t) * totalConnections);
            if (currentConnectionIndex >= totalConnections) {
                currentConnectionIndex = totalConnections - 1;
            }

            auto conn = connections[currentConnectionIndex];
            float progress = ((1.0f - t) * totalConnections) - currentConnectionIndex;

            x = nodes[conn.second].x + progress * (nodes[conn.first].x - nodes[conn.second].x);
            y = nodes[conn.second].y + progress * (nodes[conn.first].y - nodes[conn.second].y);
        }
        drawDataPacket(x, y);
    }
}

// Ring topology functions
void drawRing() {
    glColor3f(1.0f, 1.0f, 1.0f);
    for (int i = 0; i < numComputers; i++) {
        float theta1 = 2.0f * 3.1415926f * float(i) / float(numComputers);
        float theta2 = 2.0f * 3.1415926f * float(i + 1) / float(numComputers);
        float x1 = radius * cosf(theta1);
        float y1 = radius * sinf(theta1);
        float x2 = radius * cosf(theta2);
        float y2 = radius * sinf(theta2);
        glBegin(GL_LINES);
        glVertex2f(x1, y1);
        glVertex2f(x2, y2);
        glEnd();
    }
}

void displayRing() {
    // Draw the ring connections
    glColor3f(1.0f, 1.0f, 1.0f);
    for (int i = 0; i < numComputers; i++) {
        float theta1 = 2.0f * 3.1415926f * float(i) / float(numComputers);
        float theta2 = 2.0f * 3.1415926f * float((i + 1) % numComputers) / float(numComputers);
        float x1 = radius * cosf(theta1);
        float y1 = radius * sinf(theta1);
        float x2 = radius * cosf(theta2);
        float y2 = radius * sinf(theta2);
        glBegin(GL_LINES);
        glVertex2f(x1, y1);
        glVertex2f(x2, y2);
        glEnd();
    }

    // Draw computers
    for (int i = 0; i < numComputers; i++) {
        float theta = 2.0f * 3.1415926f * float(i) / float(numComputers);
        float x = radius * cosf(theta);
        float y = radius * sinf(theta);
        drawComputer(x, y, i == 0, i == numComputers - 1);
    }

    // Draw data packet
    if (!dataTransferComplete && !ringWaiting) {
        int startNode = currentComputer;
        int endNode = (currentComputer + (dataDirection ? 1 : -1) + numComputers) % numComputers;

        float startTheta = 2.0f * 3.1415926f * float(startNode) / float(numComputers);
        float endTheta = 2.0f * 3.1415926f * float(endNode) / float(numComputers);

        float x1 = radius * cosf(startTheta);
        float y1 = radius * sinf(startTheta);
        float x2 = radius * cosf(endTheta);
        float y2 = radius * sinf(endTheta);

        float x = x1 + t * (x2 - x1);
        float y = y1 + t * (y2 - y1);

        drawDataPacket(x, y);
    }
}

// Star topology functions
void drawStar() {
    glColor3f(1.0f, 1.0f, 1.0f);
    for (int i = 0; i < numComputers; i++) {
        float theta = 2.0f * 3.1415926f * float(i) / float(numComputers);
        float x = radius * cosf(theta);
        float y = radius * sinf(theta);
        glBegin(GL_LINES);
        glVertex2f(0.0f, 0.0f);
        glVertex2f(x, y);
        glEnd();
    }
}

void drawHub() {
    glColor3f(1.0f, 1.0f, 0.0f); // Yellow for Middle hub
    glBegin(GL_POLYGON);
    for (int i = 0; i < 20; i++) {
        float theta = 2.0f * 3.1415926f * float(i) / 20.0f;
        float x = hubRadius * cosf(theta);
        float y = hubRadius * sinf(theta);
        glVertex2f(x, y);
    }
    glEnd();
}

void displayStar() {
    drawStar();
    drawHub();

    for (int i = 0; i < numComputers; i++) {
        float theta = 2.0f * 3.1415926f * float(i) / float(numComputers);
        float x = radius * cosf(theta);
        float y = radius * sinf(theta);
        drawComputer(x, y, i == 0, i == numComputers - 1);
    }

    if (!dataTransferComplete) {
        float x, y;
        if (dataDirection) {
            if (t < 0.5f) {
                float theta = 2.0f * 3.1415926f * float(0) / float(numComputers);
                x = radius * cosf(theta) * (1.0f - 2.0f * t);
                y = radius * sinf(theta) * (1.0f - 2.0f * t);
            } else {
                float theta = 2.0f * 3.1415926f * float(numComputers - 1) / float(numComputers);
                x = radius * cosf(theta) * (2.0f * t - 1.0f);
                y = radius * sinf(theta) * (2.0f * t - 1.0f);
            }
        } else {
            if (t < 0.5f) {
                float theta = 2.0f * 3.1415926f * float(numComputers - 1) / float(numComputers);
                x = radius * cosf(theta) * (1.0f - 2.0f * t);
                y = radius * sinf(theta) * (1.0f - 2.0f * t);
            } else {
                float theta = 2.0f * 3.1415926f * float(0) / float(numComputers);
                x = radius * cosf(theta) * (2.0f * t - 1.0f);
                y = radius * sinf(theta) * (2.0f * t - 1.0f);
            }
        }
        drawDataPacket(x, y);
    }
}

// Tree topology functions
std::vector<Node> generateTreeNodes() {
    std::vector<Node> nodes;
    float levelSpacing = 2.0f / (numLevels + 1);
    float initialXSpacing = 2.0f / (std::pow(branchingFactor, numLevels - 1) * 2);

    for (int level = 0; level < numLevels; ++level) {
        int nodesInLevel = std::pow(branchingFactor, level);
        float xSpacing = initialXSpacing * std::pow(2, numLevels - level - 1);
        for (int i = 0; i < nodesInLevel; ++i) {
            float x = -1.0f + (i * 2 + 1) * xSpacing;
            float y = 1.0f - (level + 1) * levelSpacing;
            bool isRoot = (level == 0);
            bool isLeaf = (level == numLevels - 1);
            nodes.push_back({x, y, isRoot, isLeaf});
        }
    }

    return nodes;
}

void createLines() {
    lines.clear();
    for (int level = 0; level < numLevels - 1; ++level) {
        int nodesInLevel = std::pow(branchingFactor, level);
        for (int i = 0; i < nodesInLevel; ++i) {
            int parentIndex = i + (std::pow(branchingFactor, level) - 1) / (branchingFactor - 1);
            for (int j = 0; j < branchingFactor; ++j) {
                int childIndex = parentIndex * branchingFactor + j + 1;
                if (childIndex < treeNodes.size()) {
                    lines.push_back({treeNodes[parentIndex], treeNodes[childIndex]});
                }
            }
        }
    }
}

void drawTree() {
    glColor3f(1.0f, 1.0f, 1.0f);
    for (const auto& line : lines) {
        glBegin(GL_LINES);
        glVertex2f(line.start.x, line.start.y);
        glVertex2f(line.end.x, line.end.y);
        glEnd();
    }
}


void displayTree() {
    drawTree();

    for (const auto& node : treeNodes) {
        drawComputer(node.x, node.y, node.isRoot, node.isLeaf);
    }

    if (!dataTransferComplete) {
        float x, y;
        if (dataDirection) {
            float progress = t * lines.size();
            int lineIndex = static_cast<int>(progress);
            float lineT = progress - lineIndex;

            if (lineIndex >= lines.size()) {
                lineIndex = lines.size() - 1;
                lineT = 1.0f;
            }

            const auto& line = lines[lineIndex];
            x = line.start.x + lineT * (line.end.x - line.start.x);
            y = line.start.y + lineT * (line.end.y - line.start.y);
        } else {
            float progress = (1.0f - t) * lines.size();
            int lineIndex = static_cast<int>(progress);
            float lineT = progress - lineIndex;

            if (lineIndex >= lines.size()) {
                lineIndex = lines.size() - 1;
                lineT = 1.0f;
            }

            const auto& line = lines[lineIndex];
            x = line.end.x + lineT * (line.start.x - line.end.x);
            y = line.end.y + lineT * (line.start.y - line.end.y);
        }
        drawDataPacket(x, y);
    }
}

void drawText(float x, float y, const std::string& text, void* font = GLUT_BITMAP_HELVETICA_18) {
    glRasterPos2f(x, y);
    for (char c : text) {
        glutBitmapCharacter(font, c);
    }
}

// Function to display the main page
void displayMainPage() {
    glClear(GL_COLOR_BUFFER_BIT);

    // Set text color to white
    glColor3f(1.0f, 1.0f, 1.0f);

    // Draw institute name
    drawText(-0.5f, 0.8f, "CMR INSTITUTE OF TECHNOLOGY");

    // Draw department name
    drawText(-0.7f, 0.6f, "DEPARTMENT OF COMPUTER SCIENCE AND ENGINEERING");

    // Draw course code and name
    drawText(-0.7f, 0.4f, "21CSL67 - COMPUTER GRAPHICS WITH MINI PROJECT LABORATORY");

    // Draw project title
    drawText(-0.3f, 0.1f, "NETWORK TOPOLOGY");
    drawText(-0.3f, 0.0f, projectTitle);

    // Draw "Created By" section
    drawText(-0.7f, -0.3f, "Created By");
    drawText(-0.7f, -0.4f, "SAMEER SINGH (1CR21CS162)");
    drawText(-0.7f, -0.5f, "SAKSHAM SINGH (1CR21CS161)");

    // Draw "Guided By" section
    drawText(0.2f, -0.3f, "Guided By");
    drawText(0.2f, -0.4f, "Dr. Preethi Sheba Hepsiba, Associate Professor");
    drawText(0.2f, -0.5f, "Sreedevi N, Associate Professor");

    // Draw instructions
    drawText(-0.5f, -0.8f, "Press ENTER to start the simulation", GLUT_BITMAP_HELVETICA_12);

    glutSwapBuffers();
}

// Main display function
void display() {
    if (showMainPage) {
        displayMainPage();
    } else {
        // Your existing display code
        glClear(GL_COLOR_BUFFER_BIT);

        switch (currentTopology) {
            case BUS:
                displayBus();
                break;
            case MESH:
                displayMesh();
                break;
            case RING:
                displayRing();
                break;
            case STAR:
                displayStar();
                break;
            case TREE:
                displayTree();
                break;
        }

        glutSwapBuffers();
    }
}

// A keyboard function to handle the ENTER key
void keyboard(unsigned char key, int x, int y) {
    if (key == 13 && showMainPage) {  // 13 is the ASCII code for ENTER
        showMainPage = false;
        glutPostRedisplay();
    }
}

// Timer function
void timer(int) {
    if (animationRunning) {
        if (!waiting && !ringWaiting) {
            t += speed;
        }

        if (t >= 1.0f) {
            t = 0.0f;
            if (currentTopology == BUS) {
                if (!returnTrip) {
                    returnTrip = true;
                } else {
                    dataTransferComplete = true;
                    animationRunning = false;  // Stop animation when complete
                }
            } else if (currentTopology == RING) {
                currentComputer = (currentComputer + (dataDirection ? 1 : -1) + numComputers) % numComputers;
                if (currentComputer == numComputers - 1 && dataDirection) {
                    ringWaiting = true;
                    ringWaitStartTime = std::clock();
                } else if (currentComputer == 0 && !dataDirection) {
                    dataTransferComplete = true;
                    animationRunning = false;
                }
            } else {
                if (dataDirection) {
                    dataDirection = false;
                    waiting = true;
                    waitStartTime = std::clock();
                } else {
                    dataTransferComplete = true;
                    animationRunning = false;  // Stop animation when complete
                }
            }
        }

        if (waiting) {
            double elapsedTime = double(std::clock() - waitStartTime) / CLOCKS_PER_SEC;
            if (elapsedTime >= waitDuration) {
                waiting = false;
            }
        }
        if (ringWaiting) {
            double elapsedTime = double(std::clock() - ringWaitStartTime) / CLOCKS_PER_SEC;
            if (elapsedTime >= ringWaitDuration) {
                ringWaiting = false;
                dataDirection = false;
            }
        }

        glutPostRedisplay();
    }

    glutTimerFunc(16, timer, 0);
}

// Menu function
void menu(int option) {
    currentTopology = static_cast<Topology>(option);
    t = 0.0f;
    dataDirection = true;
    dataTransferComplete = false;
    waiting = false;
    animationRunning = true;  // Start the animation when a menu option is selected

    switch (currentTopology) {
        case MESH:
            initNodes();
            initConnections();
            break;
        case TREE:
            treeNodes = generateTreeNodes();
            createLines();
            break;
    }

    glutPostRedisplay();
}

// Main function
int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(windowWidth, windowHeight);
    glutCreateWindow("Network Topologies");

    glutDisplayFunc(display);
    glutTimerFunc(16, timer, 0);
    glutKeyboardFunc(keyboard);

    // Create menu
    glutCreateMenu(menu);
    glutAddMenuEntry("Bus Topology", BUS);
    glutAddMenuEntry("Mesh Topology", MESH);
    glutAddMenuEntry("Ring Topology", RING);
    glutAddMenuEntry("Star Topology", STAR);
    glutAddMenuEntry("Tree Topology", TREE);
    glutAttachMenu(GLUT_RIGHT_BUTTON);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-1.0, 1.0, -1.0, 1.0);

    glutMainLoop();

    return 0;
}
