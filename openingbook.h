#ifndef _OPENINGBOOK_H_
#define _OPENINGBOOK_H_
#include <memory>
#include <random>
#include <string>
#include <vector>

// Opening book stored as a weighted tree, as described in design.pdf:
// every node holds a move ("e2e4"), a weight and its children (the replies).
// The computer walks the tree along the moves played so far and picks a
// child with probability proportional to its weight.
class OpeningBook {
  struct Tree {
    std::string move;
    int weight = 0;
    std::vector<std::unique_ptr<Tree>> children;
    Tree *child(const std::string &m) const;
  };
  Tree root;
  void addLine(const std::string &line, int weight);

public:
  OpeningBook();
  // Returns a book move ("" if out of book) for the given move sequence.
  std::string lookup(const std::vector<std::string> &played, std::mt19937 &rng) const;
  // All lines, for self-testing.
  std::vector<std::vector<std::string>> lines() const;
  static const OpeningBook &instance();
};

#endif
