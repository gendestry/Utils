//
// Created by bobi on 9. 9. 26.
//

#pragma once
#include "Utils/Math/Rectangle.h"
#include <array>
#include <cstddef>
#include <memory>
#include <utility>
#include <vector>

namespace Utils
{

// Spatial index over axis-aligned rectangles.
//
// Each entry is stored exactly once, at the deepest node whose boundary fully contains it.
// An entry straddling a split line therefore stays at the parent rather than being copied
// into every child it overlaps -- so queries never return duplicates and never need a
// dedup pass.
//
// Nodes split lazily: a leaf subdivides only once it exceeds `capacity`, and never deeper
// than `maxDepth`, which bounds recursion when many rectangles pile onto one spot.
template <typename T> class Quadtree
{
  public:
    struct Entry
    {
        Maths::Rectangle bounds;
        T value;
    };

  private:
    struct Node
    {
        Maths::Rectangle boundary;
        std::vector<Entry> entries;
        std::array<std::unique_ptr<Node>, 4> children;

        explicit Node(const Maths::Rectangle &boundary) : boundary(boundary) {}

        bool isLeaf() const { return children[0] == nullptr; }

        // Children tile the parent exactly: Rectangle's bounds are half-open, so every
        // point falls in precisely one quadrant, with no overlap and no gap.
        void subdivide()
        {
            const Maths::Point c = boundary.center();
            children[0] = std::make_unique<Node>(Maths::Rectangle::fromBounds(boundary.topLeft(), c));
            children[1] = std::make_unique<Node>(
                Maths::Rectangle::fromBounds({c.x, boundary.top()}, {boundary.right(), c.y}));
            children[2] = std::make_unique<Node>(
                Maths::Rectangle::fromBounds({boundary.left(), c.y}, {c.x, boundary.bottom()}));
            children[3] = std::make_unique<Node>(Maths::Rectangle::fromBounds(c, boundary.botRight()));
        }
    };

    std::unique_ptr<Node> m_root;
    size_t m_capacity = 8U;
    size_t m_maxDepth = 8U;
    size_t m_size = 0U;

  public:
    explicit Quadtree(const Maths::Rectangle &boundary, size_t capacity = 8U, size_t maxDepth = 8U)
        : m_root(std::make_unique<Node>(boundary)), m_capacity(capacity == 0U ? 1U : capacity),
          m_maxDepth(maxDepth)
    {}

    const Maths::Rectangle &boundary() const { return m_root->boundary; }
    size_t size() const { return m_size; }
    bool empty() const { return m_size == 0U; }

    void clear()
    {
        m_root = std::make_unique<Node>(m_root->boundary);
        m_size = 0U;
    }

    // Fails when `bounds` is degenerate or reaches outside the tree's boundary.
    bool insert(const Maths::Rectangle &bounds, const T &value)
    {
        if (bounds.empty() || !m_root->boundary.contains(bounds))
            return false;

        insertInto(*m_root, Entry{bounds, value}, 0U);
        m_size++;
        return true;
    }

    // Removes every entry matching both the rectangle and the value. Returns how many went.
    size_t remove(const Maths::Rectangle &bounds, const T &value)
    {
        const size_t removed = removeFrom(*m_root, bounds, value);
        m_size -= removed;
        return removed;
    }

    // Visits the value of every entry overlapping `area`, without allocating.
    template <typename Fn> void query(const Maths::Rectangle &area, Fn &&fn) const
    {
        auto visitor = [&fn](const T &value) { fn(value); };
        queryNode(*m_root, area, visitor);
    }

    std::vector<T> query(const Maths::Rectangle &area) const
    {
        std::vector<T> found;
        auto collect = [&found](const T &value) { found.push_back(value); };
        queryNode(*m_root, area, collect);
        return found;
    }

    // Everything covering `p` -- the "what is under the cursor" query.
    std::vector<T> query(Maths::Point p) const
    {
        std::vector<T> found;
        auto collect = [&found](const T &value) { found.push_back(value); };
        queryPointNode(*m_root, p, collect);
        return found;
    }

    template <typename Fn> void query(Maths::Point p, Fn &&fn) const
    {
        auto visitor = [&fn](const T &value) { fn(value); };
        queryPointNode(*m_root, p, visitor);
    }

    template <typename Fn> void forEach(Fn &&fn) const
    {
        auto visitor = [&fn](const Maths::Rectangle &bounds, const T &value) { fn(bounds, value); };
        forEachNode(*m_root, visitor);
    }

  private:
    void insertInto(Node &node, Entry entry, size_t depth)
    {
        // Descend only while a single child can hold the whole rectangle; a straddler
        // stops here, which is what keeps each entry stored exactly once.
        if (!node.isLeaf())
        {
            if (Node *child = childContaining(node, entry.bounds))
            {
                insertInto(*child, std::move(entry), depth + 1U);
                return;
            }
            node.entries.push_back(std::move(entry));
            return;
        }

        node.entries.push_back(std::move(entry));

        if (node.entries.size() <= m_capacity || depth >= m_maxDepth)
            return;

        node.subdivide();

        // Push down whatever the new children can fully contain; the rest stays put.
        std::vector<Entry> retained;
        retained.reserve(node.entries.size());
        for (Entry &e : node.entries)
        {
            if (Node *child = childContaining(node, e.bounds))
                insertInto(*child, std::move(e), depth + 1U);
            else
                retained.push_back(std::move(e));
        }
        node.entries = std::move(retained);
    }

    static Node *childContaining(Node &node, const Maths::Rectangle &bounds)
    {
        for (const std::unique_ptr<Node> &child : node.children)
        {
            if (child->boundary.contains(bounds))
                return child.get();
        }
        return nullptr;
    }

    static size_t removeFrom(Node &node, const Maths::Rectangle &bounds, const T &value)
    {
        size_t removed = 0U;
        for (auto it = node.entries.begin(); it != node.entries.end();)
        {
            if (it->bounds == bounds && it->value == value)
            {
                it = node.entries.erase(it);
                removed++;
            }
            else
            {
                ++it;
            }
        }

        if (!node.isLeaf())
        {
            for (const std::unique_ptr<Node> &child : node.children)
            {
                // Only a subtree whose boundary could hold the rectangle can store it.
                if (child->boundary.intersects(bounds))
                    removed += removeFrom(*child, bounds, value);
            }
        }
        return removed;
    }

    template <typename Fn> static void queryNode(const Node &node, const Maths::Rectangle &area, Fn &fn)
    {
        if (!node.boundary.intersects(area))
            return;

        for (const Entry &e : node.entries)
        {
            if (e.bounds.intersects(area))
                fn(e.value);
        }

        if (node.isLeaf())
            return;

        for (const std::unique_ptr<Node> &child : node.children)
            queryNode(*child, area, fn);
    }

    template <typename Fn> static void queryPointNode(const Node &node, Maths::Point p, Fn &fn)
    {
        if (!node.boundary.contains(p))
            return;

        for (const Entry &e : node.entries)
        {
            if (e.bounds.contains(p))
                fn(e.value);
        }

        if (node.isLeaf())
            return;

        // A point lands in exactly one child, but recursing into all four costs a
        // contains() check each and keeps this in step with the region query.
        for (const std::unique_ptr<Node> &child : node.children)
            queryPointNode(*child, p, fn);
    }

    template <typename Fn> static void forEachNode(const Node &node, Fn &fn)
    {
        for (const Entry &e : node.entries)
            fn(e.bounds, e.value);

        if (node.isLeaf())
            return;

        for (const std::unique_ptr<Node> &child : node.children)
            forEachNode(*child, fn);
    }
};

} // namespace Utils
