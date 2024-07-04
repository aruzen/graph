#include <Siv3D.hpp>
#include <set>
#include <map>

struct Node {
    Point pos;
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
            return std::partial_ordering::equivalent;
        if (ff)
            return (std::partial_ordering) (second <=> v.second);
        return (std::partial_ordering) (first <=> v.first);
    }
};

struct DirectedPath {
    Node *first, *second;

    DirectedPath(Node *v1, Node *v2) {
        first = v1;
        second = v2;
    }

    auto operator<=>(const DirectedPath &v) const = default;
};

std::vector<std::vector<Node>> make_nodes(size_t size, size_t part, bool align);

std::set<DirectedPath>
make_directed_path(std::vector<std::vector<Node>> &nodes, size_t order, bool complete, bool near);

std::set<UndirectedPath>
make_undirected_path(std::vector<std::vector<Node>> &nodes, size_t order, bool complete, bool near);

void Main() {
    Scene::SetBackground(Palette::White);
    Font kinds(16);

    std::vector<String> buttons = {
            U"再生成",
            U"無向グラフ",
            U"連結グラフ",
            U"二部グラフ",
            U"完全グラフ"
    };

    double size = 5, order = 3, part = 1;
    bool directed = false, complete = false, align = false, near = true;
    std::set < DirectedPath > direction_paths;
    std::set < UndirectedPath > undirection_paths;
    std::vector<std::vector<Node>> nodes = make_nodes((size_t) size, (size_t) part, align);
    if (directed)
        direction_paths = make_directed_path(nodes, order, complete, near);
    else
        undirection_paths = make_undirected_path(nodes, order, complete, near);
    while (System::Update()) {
        ClearPrint();
        for (int i = 0; i < buttons.size(); i++) {
            Rect box(10 + 100 * i, 10, 95, 30);
            if (box.leftClicked()) {
                switch (i) {
                    case 0:
                        // 再生成
                        break;
                    case 1:
                        // 無向グラフ
                        directed = false;
                        break;
                    case 2:
                        // 連結グラフ
                        break;
                    case 3:
                        // 二部グラフ
                        break;
                    case 4:
                        // 完全グラフ
                        break;
                }
                nodes = make_nodes(size, part, align);
                if (directed)
                    direction_paths = make_directed_path(nodes, order, complete, near);
                else
                    undirection_paths = make_undirected_path(nodes, order, complete, near);
            }

            box.rounded(3).draw(Palette::Lightblue)
                    .drawFrame(1, 1, Palette::Blue);
            kinds(buttons[i])
                    .draw(Arg::center(box.center()), Palette::Black);

            size_t casted_size = (size_t) size;
            SimpleGUI::Slider(U"サイズ:{}"_fmt(casted_size),
                              size, 1.0, 10.0, Vec2{10, 50}, 200, 200);
            SimpleGUI::Slider(U"位数　:{}"_fmt((size_t) order),
                              order, 1.0, (casted_size - 1) * casted_size / 2, Vec2{10, 90}, 200, 200, !complete);
            SimpleGUI::Slider(U"部　　:{}"_fmt((size_t) part),
                              part, 1.0, 10.0, Vec2{10, 130}, 200, 200);
            SimpleGUI::CheckBox(directed, U"有向", Vec2{10, 170});
            SimpleGUI::CheckBox(complete, U"完全", Vec2{110, 170});
            SimpleGUI::CheckBox(align, U"整列", Vec2{210, 170});
            SimpleGUI::CheckBox(near, U"整頓", Vec2{310, 170});
        }

        for (int i = 0; i < nodes.size(); i++)
            for (auto &node: nodes[i]) {
                Circle(node.pos, 5).draw(HSV((int) (240 + i * 360 / nodes.size()) % 360, 100, 50));
            }
        if (directed)
            for (auto &path: direction_paths)
                Line(path.first->pos, path.second->pos)
                        .draw(2, Palette::Black);
        else
            for (auto &path: undirection_paths)
                Line(path.first->pos, path.second->pos)
                        .draw(2, Palette::Black);
        // graph->draw();
    }
}

std::vector<std::vector<Node>> make_nodes(size_t size, size_t part, bool align) {
    if (part == 1) {
        if (align) {
            int r = Min(Scene::Height(), Scene::Width()) / 2 - 100;
            auto nodes = std::vector<Node>(size, Node());
            for (int i = 0; i < size; i++)
                nodes[i].pos = Scene::Center() + Point((int) (r * Sin(i * 2_pi / size)),
                                                       (int) (r * Cos(i * 2_pi / size)));
            return {nodes};
        } else {
            auto nodes = std::vector<Node>(size, Node());
            for (size_t i = 0; i < nodes.size(); i++) {
                nodes[i].pos = RandomPoint(Scene::Width(), Scene::Height());
            }
            return {nodes};
        }
    } else {
        if (align) {
            int r = Min(Scene::Height(), Scene::Width()) / 2 - 100;
            auto result = std::vector<std::vector<Node>>(part, std::vector<Node>());
            for (int i = 0; i < size; i++)
                result[Abs(RandomInt64()) % part].push_back(Node());
            auto p1 = Scene::Center() + Point((int) (r * Sin(0)),
                                              (int) (r * Cos(0)));
            for (int i = 0; i < part; i++) {
                auto p2 = Scene::Center() + Point((int) (r * Sin((i + 1) * 2_pi / part)),
                                                  (int) (r * Cos((i + 1) * 2_pi / part)));
                Vec2 vec(p2 - p1);
                for (int j = 0; j < result[i].size(); j++) {
                    result[i][j].pos = (p1 + vec * (((double) j + 0.5) / (double) (result[i].size() + 1))).asPoint();
                    Logger << (((double) j + 0.5) / (double) (result[i].size() + 1));
                }
                p1 = p2;
            }
            return result;
        } else {
            auto result = std::vector<std::vector<Node>>(part, std::vector<Node>());
            for (int i = 0; i < size; i++)
                result[Abs(RandomInt64()) % part].push_back(Node{RandomPoint(Scene::Width(), Scene::Height())});
            return result;
        }
    }
}

std::set<DirectedPath>
make_directed_path(std::vector<std::vector<Node>> &nodes, size_t &order, bool complete, bool near) {
    if (nodes.size() == 1) {
        // 有向グラフ
        std::set < DirectedPath > all_path;
        for (int i = 0; i < nodes[0].size(); i++)
            for (int j = i + 1; j < nodes[0].size(); j++) {
                if (i == j)
                    continue;
                all_path.insert(DirectedPath(&nodes[0][i], &nodes[0][j]));
            }
        if (complete)
            return all_path;

        std::set < DirectedPath > result;
        for (size_t i = 0; i < order; i++) {
            auto itr = std::next(all_path.begin(), Abs(RandomInt64()) % all_path.size());
            result.insert(*itr);
            all_path.erase(itr);
        }
        return result;
    } else {
        // 有向n部グラフ
        std::set < DirectedPath > all_path;
        for (int target_part = 0; target_part < nodes.size(); target_part++)
            for (int to_part = 0; to_part < nodes.size(); to_part++) {
                if (target_part == to_part)
                    continue;
                for (int i = 0; i < nodes[target_part].size(); i++)
                    for (int j = 0; j < nodes[to_part].size(); j++) {
                        all_path.insert(DirectedPath(&nodes[to_part][i], &nodes[to_part][j]));
                    }
            }

        if (complete)
            return all_path;

        std::set < DirectedPath > result;
        for (size_t i = 0; i < order; i++) {
            auto itr = std::next(all_path.begin(), Abs(RandomInt64()) % all_path.size());
            result.insert(*itr);
            all_path.erase(itr);
        }
        return result;
    }
}

std::set<UndirectedPath>
make_undirected_path(std::vector<std::vector<Node>> &nodes, size_t order, bool complete, bool near) {
    if (nodes.size() == 1) {
        // 無向グラフ
        if (near) {
            std::set < UndirectedPath > result;
            for (unsigned int i = 0; i < nodes[0].size(); i++) {
                auto &node = nodes[0][i];
                std::map<double, size_t> distances;
                for (size_t j = 0; j < nodes[0].size(); j++) {
                    if (i == j)
                        continue;
                    distances[node.pos.distanceFrom(nodes[0][j].pos)] = j;
                }

                uint64 count = 1 + RandomInt64() % (1 + (nodes.size() / 2));
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
        std::set < UndirectedPath > all_path;
        for (int i = 0; i < nodes[0].size(); i++)
            for (int j = i + 1; j < nodes[0].size(); j++) {
                all_path.insert(UndirectedPath(&nodes[0][i], &nodes[0][j]));
            }

        if (complete)
            return all_path;

        std::set < UndirectedPath > result;
        if (near)
            for (unsigned int i = 0; i < nodes.size(); i++) {
                int i = 0;
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
        else
            for (size_t i = 0; i < order; i++) {
                auto itr = std::next(all_path.begin(), Abs(RandomInt64()) % all_path.size());
                result.insert(*itr);
                all_path.erase(itr);
            }
        return result;
    } else {
        // 無向n部グラフ
        std::set < UndirectedPath > all_path;
        for (int target_part = 0; target_part < nodes.size(); target_part++)
            for (int to_part = target_part + 1; to_part < nodes.size(); to_part++) {
                if (target_part == to_part)
                    continue;
                for (int i = 0; i < nodes[target_part].size(); i++)
                    for (int j = 0; j < nodes[to_part].size(); j++) {
                        all_path.insert(UndirectedPath(&nodes[to_part][i], &nodes[to_part][j]));
                    }
            }

        if (complete)
            return all_path;

        std::set < UndirectedPath > result;
        if (near)
            for (unsigned int i = 0; i < nodes.size(); i++) {
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
        else
            for (size_t i = 0; i < order; i++) {
                auto itr = std::next(all_path.begin(), Abs(RandomInt64()) % all_path.size());
                result.insert(*itr);
                all_path.erase(itr);
            }
        return result;
    }
}