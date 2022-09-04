#pragma once

#include "ranges.h"

#include <cstdlib>
#include <vector>

//class TransportSerialization;

namespace graph {

using VertexId = size_t;
using EdgeId = size_t;

enum class RouteInfo{
    Wait = 0,
    Ride = 1
};

struct RideInfo{
    std::string_view bus;
    int span_count;
    //double time;
};

struct WaitInfo{
    std::string_view stop;
};

template <typename Weight>
struct Edge {
    VertexId from;
    VertexId to;
    Weight weight;
    RouteInfo edge_status;
    RideInfo ride_info;
    WaitInfo wait_info;

};



template <typename Weight>
class DirectedWeightedGraph {
private:
    using IncidenceList = std::vector<EdgeId>;
    using IncidentEdgesRange = ranges::Range<typename IncidenceList::const_iterator>;

public:
    //friend class TransportSerialization;
    DirectedWeightedGraph() = default;
    explicit DirectedWeightedGraph(size_t vertex_count);
    EdgeId AddEdge(const Edge<Weight>& edge);

    size_t GetVertexCount() const;
    size_t GetEdgeCount() const;
    Edge<Weight>& GetEdge(EdgeId edge_id);
    IncidentEdgesRange GetIncidentEdges(VertexId vertex) const;
    const std::vector<Edge<Weight>>& GetAllEdges() const;
    const std::vector<std::vector<EdgeId>>& GetIncidenceLists() const;

private:
    std::vector<Edge<Weight>> edges_;
    std::vector<IncidenceList> incidence_lists_;
};

template <typename Weight>
DirectedWeightedGraph<Weight>::DirectedWeightedGraph(size_t vertex_count)
    : incidence_lists_(vertex_count) {
}

template <typename Weight>
EdgeId DirectedWeightedGraph<Weight>::AddEdge(const Edge<Weight>& edge) {
    edges_.push_back(edge);
    const EdgeId id = edges_.size() - 1;
    incidence_lists_.at(edge.from).push_back(id);
    return id;
}

template <typename Weight>
size_t DirectedWeightedGraph<Weight>::GetVertexCount() const {
    return incidence_lists_.size();
}

template <typename Weight>
size_t DirectedWeightedGraph<Weight>::GetEdgeCount() const {
    return edges_.size();
}

template <typename Weight>
 Edge<Weight>& DirectedWeightedGraph<Weight>::GetEdge(EdgeId edge_id)  {
    return edges_.at(edge_id);
}

template <typename Weight>
typename DirectedWeightedGraph<Weight>::IncidentEdgesRange
DirectedWeightedGraph<Weight>::GetIncidentEdges(VertexId vertex) const {
    return ranges::AsRange(incidence_lists_.at(vertex));
}

    template<typename Weight>
    const std::vector<std::vector<EdgeId>> &DirectedWeightedGraph<Weight>::GetIncidenceLists() const {
        return incidence_lists_;
    }

    template<typename Weight>
    const std::vector<Edge<Weight>> &DirectedWeightedGraph<Weight>::GetAllEdges() const {
        return edges_;
    }
}  // namespace graph