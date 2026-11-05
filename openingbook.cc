#include "openingbook.h"
#include <functional>
#include <sstream>

namespace {
// Main lines of popular openings (coordinate notation) with relative weights.
struct Line { int weight; const char *moves; };
const Line BOOK[] = {
    // 1.e4 e5
    {5, "e2e4 e7e5 g1f3 b8c6 f1b5 a7a6 b5a4 g8f6 e1g1 f8e7 f1e1 b7b5 a4b3 d7d6 c2c3 e8g8"}, // Ruy Lopez
    {3, "e2e4 e7e5 g1f3 b8c6 f1b5 g8f6 e1g1 f6e4 d2d4 e4d6 b5c6 d7c6 d4e5 d6f5 d1d8 e8d8"}, // Berlin
    {4, "e2e4 e7e5 g1f3 b8c6 f1c4 f8c5 c2c3 g8f6 d2d3 d7d6 e1g1 e8g8"},                     // Italian
    {2, "e2e4 e7e5 g1f3 b8c6 f1c4 g8f6 d2d3 f8e7 e1g1 e8g8 f1e1 d7d6"},                     // Two Knights
    {2, "e2e4 e7e5 g1f3 b8c6 d2d4 e5d4 f3d4 g8f6 d4c6 b7c6 e4e5 d8e7"},                     // Scotch
    {2, "e2e4 e7e5 g1f3 g8f6 f3e5 d7d6 e5f3 f6e4 d2d4 d6d5 f1d3 b8c6"},                     // Petroff
    // Sicilian
    {5, "e2e4 c7c5 g1f3 d7d6 d2d4 c5d4 f3d4 g8f6 b1c3 a7a6 c1e3 e7e5 d4b3 c8e6"},           // Najdorf
    {3, "e2e4 c7c5 g1f3 b8c6 d2d4 c5d4 f3d4 g8f6 b1c3 e7e5 d4b5 d7d6"},                     // Sveshnikov
    {2, "e2e4 c7c5 g1f3 e7e6 d2d4 c5d4 f3d4 b8c6 b1c3 d8c7"},                               // Taimanov
    {1, "e2e4 c7c5 b1c3 b8c6 g2g3 g7g6 f1g2 f8g7 d2d3 d7d6"},                               // Closed
    // Other 1.e4 defences
    {3, "e2e4 e7e6 d2d4 d7d5 b1c3 g8f6 c1g5 f8e7 e4e5 f6d7 g5e7 d8e7"},                     // French
    {3, "e2e4 c7c6 d2d4 d7d5 b1c3 d5e4 c3e4 c8f5 e4g3 f5g6 h2h4 h7h6 g1f3 b8d7"},           // Caro-Kann
    {1, "e2e4 d7d5 e4d5 d8d5 b1c3 d5a5 d2d4 g8f6 g1f3 c8f5"},                               // Scandinavian
    // 1.d4
    {4, "d2d4 d7d5 c2c4 e7e6 b1c3 g8f6 c1g5 f8e7 e2e3 e8g8 g1f3 b8d7"},                     // QGD
    {3, "d2d4 d7d5 c2c4 c7c6 g1f3 g8f6 b1c3 d5c4 a2a4 c8f5 e2e3 e7e6 f1c4 f8b4"},           // Slav
    {2, "d2d4 d7d5 c2c4 d5c4 g1f3 g8f6 e2e3 e7e6 f1c4 c7c5 e1g1 a7a6"},                     // QGA
    {3, "d2d4 g8f6 c2c4 e7e6 b1c3 f8b4 e2e3 e8g8 f1d3 d7d5 g1f3 c7c5"},                     // Nimzo-Indian
    {2, "d2d4 g8f6 c2c4 e7e6 g1f3 b7b6 g2g3 c8a6 b2b3 f8b4 c1d2 b4e7"},                     // Queen's Indian
    {3, "d2d4 g8f6 c2c4 g7g6 b1c3 f8g7 e2e4 d7d6 g1f3 e8g8 f1e2 e7e5 e1g1 b8c6"},           // King's Indian
    {1, "d2d4 g8f6 c2c4 c7c5 d4d5 e7e6 b1c3 e6d5 c4d5 d7d6 e2e4 g7g6"},                     // Benoni
    // Flank openings
    {2, "c2c4 e7e5 b1c3 g8f6 g1f3 b8c6 g2g3 d7d5 c4d5 f6d5 f1g2 d5b6"},                     // English
    {2, "g1f3 d7d5 g2g3 g8f6 f1g2 e7e6 e1g1 f8e7 d2d3 e8g8"},                               // Reti
};
} // namespace

OpeningBook::Tree *OpeningBook::Tree::child(const std::string &m) const {
  for (const auto &c : children)
    if (c->move == m) return c.get();
  return nullptr;
}

void OpeningBook::addLine(const std::string &line, int weight) {
  std::istringstream iss{line};
  std::string mv;
  Tree *node = &root;
  while (iss >> mv) {
    Tree *next = node->child(mv);
    if (!next) {
      node->children.push_back(std::make_unique<Tree>());
      next = node->children.back().get();
      next->move = mv;
    }
    next->weight += weight;
    node = next;
  }
}

OpeningBook::OpeningBook() {
  for (const auto &l : BOOK) addLine(l.moves, l.weight);
}

const OpeningBook &OpeningBook::instance() {
  static const OpeningBook book;
  return book;
}

std::string OpeningBook::lookup(const std::vector<std::string> &played,
                                std::mt19937 &rng) const {
  const Tree *node = &root;
  for (const auto &m : played) {
    node = node->child(m);
    if (!node) return "";
  }
  if (node->children.empty()) return "";
  int total = 0;
  for (const auto &c : node->children) total += c->weight;
  std::uniform_int_distribution<int> dist(1, total);
  int pick = dist(rng);
  for (const auto &c : node->children) {
    pick -= c->weight;
    if (pick <= 0) return c->move;
  }
  return node->children.back()->move;
}

std::vector<std::vector<std::string>> OpeningBook::lines() const {
  std::vector<std::vector<std::string>> out;
  std::vector<std::string> path;
  std::function<void(const Tree &)> walk = [&](const Tree &t) {
    if (t.children.empty()) {
      out.push_back(path);
      return;
    }
    for (const auto &c : t.children) {
      path.push_back(c->move);
      walk(*c);
      path.pop_back();
    }
  };
  walk(root);
  return out;
}
