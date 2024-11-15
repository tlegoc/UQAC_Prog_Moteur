#pragma once

#include <imgui.h>
#include <iostream>
#include <simplege/pch/precomp.h>

#include <simplege/components/collider.h>
#include <simplege/entity.h>
#include <simplege/systems/system.h>

namespace SimpleGE
{
    using ComponentVector = std::vector<gsl::not_null<ColliderComponent*>>;

    inline Area GetMinContainerArea(const ComponentVector& colliders)
    {
        Area area;

        if (colliders.empty())
        {
            return area;
        }

        area = colliders[0]->GetArea();
        for (auto c : colliders)
        {
            area = area.Union(c->GetArea());
        }

        return area;
    }

    struct CollisionPair
    {
        gsl::not_null<ColliderComponent*> first;
        gsl::not_null<ColliderComponent*> second;

        bool operator==(const CollisionPair& other) const
        {
            return first.get() == other.first.get() && second.get() == other.second.get() ||
                first.get() == other.second.get() && second.get() == other.first.get();
        }
    };

    class QuadTree
    {
    public:
        struct Node
        {
            int level;
            Area area;
            std::vector<gsl::not_null<ColliderComponent*>> colliders;
            bool hasChildrens;
            std::array<int, 4> children{-1, -1, -1, -1};
        };

        explicit QuadTree(const ComponentVector& colliders) : maxLevel(10), maxObjectsPerNode(3)
        {
            nodes.push_back({
                .level = 0,
                .area = GetMinContainerArea(colliders)
            });

            for (const auto& collider : colliders)
            {
                Insert(0, collider);
            }
        }

        ComponentVector Retrieve(const Area area)
        {
            return Retrieve(0, area);
        }

        ComponentVector Retrieve(const int node, const Area area)
        {
            std::vector<int> indexes = GetIndex(node, area);
            ComponentVector returnObjects = nodes[node].colliders;

            if (nodes[node].hasChildrens)
            {
                for (int index : indexes)
                {
                    ComponentVector objs = Retrieve(index, area);
                    returnObjects.insert(returnObjects.end(), objs.begin(), objs.end());
                }
            }

            // Filter duplicates
            if (nodes[node].level == 0)
            {
                std::unordered_set returnObjectsSet(returnObjects.begin(), returnObjects.end());

                returnObjects = std::vector(returnObjectsSet.begin(), returnObjectsSet.end());
            }

            return returnObjects;
        }

        std::vector<Node>& GetNodes()
        {
            return nodes;
        }

    private:
        void Split(int node)
        {
            if (nodes[node].hasChildrens) return;

            int nextLevel = nodes[node].level + 1;

            auto areas = nodes[node].area.Split();

            for (int i = 0; i < 4; i++)
            {
                nodes[node].children[i] = nodes.size();
                nodes.push_back({
                    .level = nextLevel,
                    .area = areas[i]
                });
            }

            nodes[node].hasChildrens = true;
        }

        std::vector<int> GetIndex(const int node, const Area area)
        {
            std::vector<int> indexes;

            if (!nodes[node].hasChildrens)
            {
                return indexes;
            }

            for (int i = 0; i < 4; i++)
            {
                int childIndex = nodes[node].children[i];
                if (nodes[childIndex].area.Intersects(area))
                {
                    indexes.push_back(childIndex);
                }
            }

            return indexes;
        }

        void Insert(int node, gsl::not_null<ColliderComponent*> component)
        {
            if (nodes[node].hasChildrens)
            {
                std::vector<int> indexes = GetIndex(node, component->GetArea());

                for (int index : indexes)
                {
                    Insert(index, component);
                }

                return;
            }

            nodes[node].colliders.push_back(component);

            if (nodes[node].colliders.size() > maxObjectsPerNode && nodes[node].level < maxLevel)
            {
                Split(node);

                for (int i = 0; i < nodes[node].colliders.size(); i++)
                {
                    std::vector<int> indexes = GetIndex(node, nodes[node].colliders[i]->GetArea());

                    for (int index : indexes)
                    {
                        Insert(index, nodes[node].colliders[i]);
                    }
                }

                nodes[node].colliders.clear();
            }
        }

        int maxLevel;
        int maxObjectsPerNode;
        std::vector<Node> nodes;
    };

    class PhysicSystem : public ISystem
    {
    public:
        static PhysicSystem& Instance()
        {
            static PhysicSystem instance;
            return instance;
        }

        PhysicSystem(const PhysicSystem&) = delete;
        PhysicSystem(PhysicSystem&&) = delete;
        PhysicSystem& operator=(const PhysicSystem&) = delete;
        PhysicSystem& operator=(PhysicSystem&&) = delete;

        ~PhysicSystem() override { Expects(colliders.empty()); }

        void Register(gsl::not_null<ColliderComponent*> comp)
        {
            colliders.insert(comp);
        }

        void Unregister(gsl::not_null<ColliderComponent*> comp)
        {
            colliders.erase(comp);
            // quadTree.Remove(comp);
        }

        void Iterate(const Timing& timing) override
        {
            // debug overlay to display colliders and quadtree
            ImGuiWindowFlags window_flags = 0;
            window_flags |= ImGuiWindowFlags_NoBackground;
            window_flags |= ImGuiWindowFlags_NoTitleBar;
            window_flags |= ImGuiWindowFlags_NoResize;
            window_flags |= ImGuiWindowFlags_NoMove;
            window_flags |= ImGuiWindowFlags_NoScrollbar;
            window_flags |= ImGuiWindowFlags_NoCollapse;
            auto io = ImGui::GetIO();
            // ImGui::SetNextWindowBgAlpha(0.35f);
            ImGui::SetNextWindowSize(ImVec2(io.DisplaySize.x, io.DisplaySize.y));
            ImGui::SetNextWindowPos(ImVec2(0, 0));

            static bool open = true;
            ImGui::Begin("Overlay", &open, window_flags);
            ImGui::SetWindowPos(ImVec2(0, 0));
            ImDrawList* draw_list = ImGui::GetWindowDrawList();
            int collidesCount = 0;

            ComponentVector collidersVec;
            for (auto c : colliders)
            {
                if (c->Enabled() && c->Owner().Active())
                {
                    collidersVec.push_back(c);
                }
            }

            std::vector<CollisionPair> collisions;

            // function that will be executed on each pair of colliders that MIGHT collide
            auto func = [&](gsl::not_null<ColliderComponent*> c1, gsl::not_null<ColliderComponent*> c2)
            {
                // check if the colliders are from different owners and if they have the correct flags
                if (&c1->Owner() != &c2->Owner() && c1->GetMask() & c2->GetFlag() || c2->GetMask() & c1->GetFlag())
                {
                    collidesCount++;
                    draw_list->AddLine(ImVec2(c1->GetArea().x(), c1->GetArea().y()),
                                       ImVec2(c2->GetArea().x(), c2->GetArea().y()),
                                       IM_COL32(255, 255, 0, 255));
                    if (c1->Collides(*c2))
                    {
                        CollisionPair collision = {c1, c2};
                        // ensure we dont have any duplicate collisions
                        if (std::find(collisions.begin(), collisions.end(), collision) == collisions.end())
                        {
                            collisions.push_back(collision);
                        }
                    }
                }
            };

            QuadTree tree(collidersVec);

            // new method
            for (int i = 0; i < collidersVec.size(); i++)
            {
                auto c1 = collidersVec[i];

                ComponentVector candidates = tree.Retrieve(c1->GetArea());

                for (auto& candidate : candidates)
                {
                    func(c1, candidate);
                }
            }

            // old method
            // for (int i = 0; i < std::size(collidersVec); i++)
            // {
            //     auto c1 = collidersVec[i];
            //
            //     for (int j = i + 1; j < std::size(collidersVec); j++)
            //     {
            //         auto c2 = collidersVec[j];
            //
            //         func(c1, c2);
            //     }
            // }

            for (auto col : collisions)
            {
                col.first->OnCollision(*col.second);
                col.second->OnCollision(*col.first);
            }

            // draw each collider
            for (auto c : colliders)
            {
                if (c->Enabled() && c->Owner().Active())
                {
                    auto area = c->GetArea();
                    draw_list->AddRectFilled(ImVec2(area.xMin(), area.yMin()), ImVec2(area.xMax(), area.yMax()),
                                             IM_COL32(255, 0, 0, 255));
                }
            }

            auto& nodes = tree.GetNodes();

            for (auto& node : nodes)
            {
                auto area = node.area;
                draw_list->AddRect(ImVec2(area.xMin(), area.yMin()), ImVec2(area.xMax(), area.yMax()),
                                   IM_COL32(0, 255, 0, 255));
            }

            ImGui::Text("Number of collision checks: %d", collidesCount);
            ImGui::Text("Colliders: %d", colliders.size());
            ImGui::End();
        }

    private:
        PhysicSystem() = default;

        std::unordered_set<gsl::not_null<ColliderComponent*>> colliders;
    };
} // namespace SimpleGE
