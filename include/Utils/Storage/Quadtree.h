//
// Created by bobi on 9. 9. 26.
//

#pragma once
#include "Utils/Math/Rectangle.h"
#include <algorithm>
#include <array>
#include <cmath>
#include <concepts>
#include <cstddef>
#include <functional>
#include <memory>
#include <optional>
#include <utility>
#include <vector>

namespace Utils
{

// Spatial index over objects that are themselves rectangles.
//
// The tree does not own anything: it holds `T*`, and the object's own bounds are the key.
// Each object is placed by its center, which lands in exactly one quadrant, so every object
// lives in a leaf and splits never have to deal with straddlers. Because an object can still
// reach outside the cell holding it, every node also caches `extent` -- the union of all
// rectangles in its subtree -- and queries prune on that rather than on the cell.
//
// The tree reads the object's bounds on insert and remove, so an object must be removed
// *before* it is moved or resized, then inserted again afterwards.
//
// Nodes split lazily: a leaf subdivides only once it exceeds `capacity`, and never deeper
// than `maxDepth`, which bounds recursion when many centers pile onto one spot.
template <typename T>
    requires std::derived_from<T, Maths::Rectangle>
class Quadtree
{
    struct Node
    {
        Maths::Rectangle boundary;
        Maths::Rectangle extent;
        std::vector<T *> entries;
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

        Node &childFor(Maths::Point p) const
        {
            for (const std::unique_ptr<Node> &child : children)
            {
                if (child->boundary.contains(p))
                    return *child;
            }
            // Unreachable for a point inside `boundary`, but float rounding in center() could
            // leave a sliver; the last quadrant is the one touching right/bottom.
            return *children[3];
        }

        void recomputeExtent()
        {
            extent = {};
            if (isLeaf())
            {
                for (const T *e : entries)
                    extent = extent.united(*e);
            }
            else
            {
                for (const std::unique_ptr<Node> &child : children)
                    extent = extent.united(child->extent);
            }
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

    // Fails for a null or degenerate object, or one whose center lies outside the tree's
    // boundary. The rectangle itself may overhang the boundary.
    bool insert(T *obj)
    {
        if (obj == nullptr || obj->empty() || !m_root->boundary.contains(obj->center()))
            return false;

        insertInto(*m_root, obj, 0U);
        m_size++;
        return true;
    }

    // Finds the object by its current center, so call this before changing its bounds.
    bool remove(T *obj)
    {
        if (obj == nullptr || !m_root->boundary.contains(obj->center()))
            return false;

        if (!removeFrom(*m_root, obj, obj->center()))
            return false;

        m_size--;
        return true;
    }

    // Visits every object overlapping `area`, without allocating.
    template <typename Fn> void query(const Maths::Rectangle &area, Fn &&fn) const
    {
        auto visitor = [&fn](T *obj) { fn(obj); };
        queryNode(*m_root, area, visitor);
    }

    std::vector<T *> query(const Maths::Rectangle &area) const
    {
        std::vector<T *> found;
        auto collect = [&found](T *obj) { found.push_back(obj); };
        queryNode(*m_root, area, collect);
        return found;
    }

    // Everything covering `p` -- the "what is under the cursor" query.
    std::vector<T *> query(Maths::Point p) const
    {
        std::vector<T *> found;
        auto collect = [&found](T *obj) { found.push_back(obj); };
        queryPointNode(*m_root, p, collect);
        return found;
    }

    template <typename Fn> void query(Maths::Point p, Fn &&fn) const
    {
        auto visitor = [&fn](T *obj) { fn(obj); };
        queryPointNode(*m_root, p, visitor);
    }

    template <typename Fn> void forEach(Fn &&fn) const
    {
        auto visitor = [&fn](T *obj) { fn(obj); };
        forEachNode(*m_root, visitor);
    }

    // Arrow-key navigation: the object the user most likely means by "the next one" in that
    // direction from `from`, or nothing when there is none. `from` itself need not be in the tree.
    std::optional<T *> left(const T *from) const { return neighbour(from, Direction::Left); }
    std::optional<T *> right(const T *from) const { return neighbour(from, Direction::Right); }
    std::optional<T *> up(const T *from) const { return neighbour(from, Direction::Up); }
    std::optional<T *> down(const T *from) const { return neighbour(from, Direction::Down); }

  private:
    enum class Direction
    {
        Left,
        Right,
        Up,
        Down
    };

    // A rectangle seen along a direction, flipped so that "further along" always means larger
    // `near`/`far`; `lo`/`hi`/`mid` describe it on the perpendicular axis.
    struct Span
    {
        float near, far;
        float lo, hi, mid;
    };

    static Span project(const Maths::Rectangle &r, Direction dir)
    {
        const Maths::Point c = r.center();
        switch (dir)
        {
        case Direction::Right:
            return {r.left(), r.right(), r.top(), r.bottom(), c.y};
        case Direction::Left:
            return {-r.right(), -r.left(), r.top(), r.bottom(), c.y};
        case Direction::Down:
            return {r.top(), r.bottom(), r.left(), r.right(), c.x};
        case Direction::Up:
            return {-r.bottom(), -r.top(), r.left(), r.right(), c.x};
        }
        return {};
    }

    // Modelled on Android's FocusFinder, which solves the same problem for D-pad focus.
    std::optional<T *> neighbour(const T *from, Direction dir) const
    {
        if (from == nullptr || empty())
            return std::nullopt;

        const Span src = project(*from, dir);

        // Only objects reaching past `from`'s far edge can qualify, so search just that side.
        const Maths::Rectangle &ext = m_root->extent;
        Maths::Rectangle area;
        switch (dir)
        {
        case Direction::Right:
            area = Maths::Rectangle::fromBounds({from->right(), ext.top()}, ext.botRight());
            break;
        case Direction::Left:
            area = Maths::Rectangle::fromBounds(ext.topLeft(), {from->left(), ext.bottom()});
            break;
        case Direction::Down:
            area = Maths::Rectangle::fromBounds({ext.left(), from->bottom()}, ext.botRight());
            break;
        case Direction::Up:
            area = Maths::Rectangle::fromBounds(ext.topLeft(), {ext.right(), from->top()});
            break;
        }
        if (area.empty())
            return std::nullopt;

        // It has to start further along than `from` and also end further along; that admits
        // overlapping rectangles while never offering one that doesn't go anywhere.
        auto isCandidate = [&](const T *obj, const Span &dst) {
            return obj != from && (src.near < dst.near || src.far <= dst.near) && src.far < dst.far;
        };

        // Vertically, an in-line object only gets priority while it starts before the nearest
        // candidate ends, so down goes to the next row rather than skipping it for something
        // directly below but further away. That needs the nearest far edge up front: deciding it
        // pairwise instead makes the winner depend on the order the tree is walked.
        const bool horizontal = dir == Direction::Left || dir == Direction::Right;
        float nearestFar = 0.0f;
        bool any = false;
        if (!horizontal)
        {
            auto measure = [&](T *obj) {
                const Span dst = project(*obj, dir);
                if (!isCandidate(obj, dst))
                    return;
                nearestFar = any ? std::min(nearestFar, dst.far) : dst.far;
                any = true;
            };
            queryNode(*m_root, area, measure);
            if (!any)
                return std::nullopt;
        }

        T *best = nullptr;
        bool bestPreferred = false;
        float bestScore = 0.0f;
        auto consider = [&](T *obj) {
            const Span dst = project(*obj, dir);
            if (!isCandidate(obj, dst))
                return;

            // Sideways, in line always wins, so left/right stay in the row.
            const bool inBeam = dst.lo < src.hi && dst.hi > src.lo;
            const bool preferred = inBeam && (horizontal || dst.near < nearestFar);

            // The gap along the arrow dominates: pressing "right" should not jump to something
            // barely to the right but far up the screen.
            const float gap = std::max(0.0f, dst.near - src.far);
            const float offset = std::abs(dst.mid - src.mid);
            const float score = 13.0f * gap * gap + offset * offset;

            // Ranked by (preferred, score), ties broken by address so the tree's shape never
            // changes the answer.
            bool wins;
            if (best == nullptr)
                wins = true;
            else if (preferred != bestPreferred)
                wins = preferred;
            else
                wins = score < bestScore || (score == bestScore && std::less<T *>{}(obj, best));

            if (!wins)
                return;
            best = obj;
            bestPreferred = preferred;
            bestScore = score;
        };
        queryNode(*m_root, area, consider);

        if (best == nullptr)
            return std::nullopt;
        return best;
    }

    void insertInto(Node &node, T *obj, size_t depth)
    {
        node.extent = node.extent.united(*obj);

        if (!node.isLeaf())
        {
            insertInto(node.childFor(obj->center()), obj, depth + 1U);
            return;
        }

        node.entries.push_back(obj);

        if (node.entries.size() <= m_capacity || depth >= m_maxDepth)
            return;

        node.subdivide();

        // Centers never straddle, so the leaf hands over everything it held.
        std::vector<T *> moving = std::move(node.entries);
        node.entries.clear();
        for (T *e : moving)
            insertInto(node.childFor(e->center()), e, depth + 1U);
    }

    bool removeFrom(Node &node, T *obj, Maths::Point center)
    {
        if (node.isLeaf())
        {
            auto it = std::find(node.entries.begin(), node.entries.end(), obj);
            if (it == node.entries.end())
                return false;

            *it = node.entries.back();
            node.entries.pop_back();
            node.recomputeExtent();
            return true;
        }

        if (!removeFrom(node.childFor(center), obj, center))
            return false;

        tryCollapse(node);
        node.recomputeExtent();
        return true;
    }

    // Folds four leaf children back into `node` once they could fit in it again, so a tree
    // that churns through remove + insert doesn't keep a fragmented structure.
    void tryCollapse(Node &node) const
    {
        size_t total = 0U;
        for (const std::unique_ptr<Node> &child : node.children)
        {
            if (!child->isLeaf())
                return;
            total += child->entries.size();
        }
        if (total > m_capacity)
            return;

        node.entries.reserve(total);
        for (std::unique_ptr<Node> &child : node.children)
        {
            node.entries.insert(node.entries.end(), child->entries.begin(), child->entries.end());
            child.reset();
        }
    }

    template <typename Fn> static void queryNode(const Node &node, const Maths::Rectangle &area, Fn &fn)
    {
        if (!node.extent.intersects(area))
            return;

        if (node.isLeaf())
        {
            for (T *e : node.entries)
            {
                if (e->intersects(area))
                    fn(e);
            }
            return;
        }

        for (const std::unique_ptr<Node> &child : node.children)
            queryNode(*child, area, fn);
    }

    // Extents overlap where objects overhang their cells, so a point can be under several
    // children at once -- all four are checked.
    template <typename Fn> static void queryPointNode(const Node &node, Maths::Point p, Fn &fn)
    {
        if (!node.extent.contains(p))
            return;

        if (node.isLeaf())
        {
            for (T *e : node.entries)
            {
                if (e->contains(p))
                    fn(e);
            }
            return;
        }

        for (const std::unique_ptr<Node> &child : node.children)
            queryPointNode(*child, p, fn);
    }

    template <typename Fn> static void forEachNode(const Node &node, Fn &fn)
    {
        for (T *e : node.entries)
            fn(e);

        if (node.isLeaf())
            return;

        for (const std::unique_ptr<Node> &child : node.children)
            forEachNode(*child, fn);
    }
};

} // namespace Utils
