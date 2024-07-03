#include <Siv3D.hpp>
#include <set>
#include <map>

struct Node {
    Point pos;
    std::set<Node *> paths;
};

struct UndirectedPath {
    Node *first, *second;

    UndirectedPath(Node *v1, Node *v2) {
        if (v1 < v2) {
            first = v1;
            second = v2;
        } else {
            first = v2;
            second = v1;
        }
    }

    auto operator<=>(const UndirectedPath &v) const {
        bool ff = first == v.first;
        if (ff && second == v.second)
            return std::strong_ordering::equivalent;
        if (ff)
            return second <=> v.second;
        return first <=> v.first;
    }
};

struct Graph {
    virtual void make_graph(int size) = 0;

    virtual void draw() = 0;
};


struct UndirectedGraph : public Graph {
    std::vector<Node> nodes;
    std::set<UndirectedPath> paths;

    virtual void make_graph(int size) {
        nodes = std::vector<Node>(size, Node());
        for (size_t i = 0; i < nodes.size(); i++) {
            nodes[i].pos = RandomPoint(Scene::Width(), Scene::Height());
        }
        for (unsigned int i = 0; i < nodes.size(); i++) {
            auto &node = nodes[i];
            std::map<double, size_t> distances;
            for (size_t j = 0; j < nodes.size(); j++) {
                if (i == j)
                    continue;
                distances[node.pos.distanceFrom(nodes[j].pos)] = j;
            }

            uint64 count = RandomInt64() % (1 + (nodes.size() / 2));
            if (count == 0)
                continue;
            auto itr = distances.begin();
            for (; 0 < count; count--) {
                auto &to_node = nodes[itr->second];
                node.paths.insert(&to_node);
                to_node.paths.insert(&node);
                paths.insert(UndirectedPath(&node, &to_node));
                itr++;
            }
        }
    }

    virtual void draw() {
        for (auto &node: nodes)
            Circle(node.pos, 5).draw(Palette::Blue);
        for (auto &path: paths)
            Line(path.first->pos, path.second->pos)
                    .draw(2, Palette::Black);
    }
};

struct ConnectedGraph : public Graph {
    std::vector<Node> nodes;
    std::set<UndirectedPath> paths;

    virtual void make_graph(int size) {
        nodes = std::vector<Node>(size, Node());
        for (size_t i = 0; i < nodes.size(); i++) {
            nodes[i].pos = RandomPoint(Scene::Width(), Scene::Height());
        }
        for (unsigned int i = 0; i < nodes.size(); i++) {
            auto &node = nodes[i];
            std::map<double, size_t> distances;
            for (size_t j = 0; j < nodes.size(); j++) {
                if (i == j)
                    continue;
                distances[node.pos.distanceFrom(nodes[j].pos)] = j;
            }

            uint64 count = 1 + RandomInt64() % (1 + (nodes.size() / 2));
            if (count == 0)
                continue;
            auto itr = distances.begin();
            for (; 0 < count; count--) {
                auto &to_node = nodes[itr->second];
                node.paths.insert(&to_node);
                to_node.paths.insert(&node);
                paths.insert(UndirectedPath(&node, &to_node));
                itr++;
            }
        }
    }

    virtual void draw() {
        for (auto &node: nodes)
            Circle(node.pos, 5).draw(Palette::Blue);
        for (auto &path: paths)
            Line(path.first->pos, path.second->pos)
                    .draw(2, Palette::Black);
    }
};

struct BipartiteGraph : public Graph {
    std::vector<Node> nodes1, nodes2;
    std::set<UndirectedPath> paths;

    virtual void make_graph(int size) {
        int n = 1 + Abs(RandomInt64()) % (size - 2);
        Logger << n;
        nodes1 = std::vector<Node>(n, Node());
        nodes2 = std::vector<Node>(size - n, Node());

        for (size_t i = 0; i < nodes1.size(); i++) {
            nodes1[i].pos = RandomPoint(Scene::Width(), Scene::Height());
        }
        for (size_t i = 0; i < nodes2.size(); i++) {
            nodes2[i].pos = RandomPoint(Scene::Width(), Scene::Height());
        }
        for (unsigned int i = 0; i < nodes1.size(); i++) {
            auto &node = nodes1[i];
            std::map<double, size_t> distances;
            for (size_t j = 0; j < nodes2.size(); j++) {
                distances[node.pos.distanceFrom(nodes2[j].pos)] = j;
            }

            uint64 count = 1 + RandomInt64() % (1 + (nodes2.size() / 2));
            auto itr = distances.begin();
            for (; 0 < count; count--) {
                Logger << U"(" << itr->second << U"/" << nodes2.size() << U")";
                auto &to_node = nodes2[itr->second];
                node.paths.insert(&to_node);
                to_node.paths.insert(&node);
                paths.insert(UndirectedPath(&node, &to_node));
                itr++;
            }
        }
    }

    virtual void draw() {
        for (auto &node: nodes1)
            Circle(node.pos, 5).draw(Palette::Red);
        for (auto &node: nodes2)
            Circle(node.pos, 5).draw(Palette::Blue);

        for (auto &path: paths)
            Line(path.first->pos, path.second->pos)
                    .draw(2, Palette::Black);
    }
};

struct CompleteGraph : public Graph {
    std::vector<Node> nodes;
    std::set<UndirectedPath> paths;

    virtual void make_graph(int size) {
        nodes = std::vector<Node>(size, Node());
        for (size_t i = 0; i < nodes.size(); i++) {
            nodes[i].pos = RandomPoint(Scene::Width(), Scene::Height());
        }
        for (unsigned int i = 0; i < nodes.size(); i++) {
            for (size_t j = 0; j < nodes.size(); j++) {
                if (i == j)
                    continue;
                auto &node = nodes[i], &to_node = nodes[j];
                node.paths.insert(&to_node);
                to_node.paths.insert(&node);
                paths.insert(UndirectedPath(&node, &to_node));
            }
        }
    }

    virtual void draw() {
        for (auto &node: nodes)
            Circle(node.pos, 5).draw(Palette::Blue);
        for (auto &path: paths)
            Line(path.first->pos, path.second->pos)
                    .draw(2, Palette::Black);
    }
};

void Main() {
    Scene::SetBackground(Palette::White);
    Font kinds(16);

    std::vector<String> modes = {
            U"無向グラフ",
            U"連結グラフ",
            U"二部グラフ",
            U"完全グラフ"
    };

    double size = 5;
    std::shared_ptr<Graph> graph = std::make_shared<UndirectedGraph>();
    graph->make_graph((size_t) size);
    while (System::Update()) {
        for (int i = 0; i < modes.size(); i++) {
            Rect box(10 + 100 * i, 10, 95, 30);
            if (box.leftClicked()) {
                switch (i) {
                    case 0:
                        graph = std::make_shared<UndirectedGraph>();
                        break;
                    case 1:
                        graph = std::make_shared<ConnectedGraph>();
                        break;
                    case 2:
                        graph = std::make_shared<BipartiteGraph>();
                        break;
                    case 3:
                        graph = std::make_shared<CompleteGraph>();
                        break;
                }
                graph->make_graph((size_t) size);
            }

            box.rounded(3).draw(Palette::Lightblue)
                    .drawFrame(1, 1, Palette::Blue);
            kinds(modes[i])
                    .draw(Arg::center(box.center()), Palette::Black);
            SimpleGUI::Slider(U"Size {}"_fmt((size_t) size),
                              size, 1.0, 10.0, Vec2{10, 50}, 100, 200);
        }
        graph->draw();
    }
}

