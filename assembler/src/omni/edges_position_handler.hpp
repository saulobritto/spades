/*
 * edges_position_handler.hpp
 *
 *  Created on: 22.07.2011
 *
 */

#ifndef EDGES_POSITION_HANDLER_HPP_
#define EDGES_POSITION_HANDLER_HPP_

//#include "utils.hpp"
#include "graph_labeler.hpp"
#include "simple_tools.hpp"
using namespace omnigraph;

namespace omnigraph {

class EdgePosition {
public:
	int start_;
	int end_;
	EdgePosition (int start, int end): start_(start), end_(end) {};
};

bool PosCompare(const EdgePosition &a, const EdgePosition &b){
	return (a.end_<b.end_);
}

vector<EdgePosition> GluePositionsLists(vector<EdgePosition> v1, vector<EdgePosition> v2){
	vector<EdgePosition> res;
	if (v1.size() == 0 || v2.size() == 0) return res;
	for( size_t i = 0; i< v1.size(); i++){
		for( size_t j = 0; j< v2.size(); j++){
			if (v1[i].end_ + 1 == v2[j].start_) {
				res.push_back(EdgePosition(v1[i].start_, v2[j].end_));
	 		}
	 	}
	}
	return res;
}


template<class Graph>
class EdgesPositionHandler: public GraphActionHandler<Graph> {
	typedef typename Graph::VertexId VertexId;
	typedef typename Graph::EdgeId EdgeId;
	typedef int realIdType;

public:
	map<EdgeId, vector<EdgePosition> > EdgesPositions;
	void AddEdgePosition (EdgeId NewEdgeId, int start, int end) {
		if (EdgesPositions.find(NewEdgeId) == EdgesPositions.end()) {
			vector<EdgePosition> NewVec;
			EdgesPositions[NewEdgeId] = NewVec;
		}
		EdgePosition NewPos(start, end);
		(EdgesPositions[NewEdgeId]).push_back(NewPos);
		DEBUG("Add pos "<<NewPos.start_<<" "<<NewPos.end_<<" for edge "<<NewEdgeId<<" total positions: "<< EdgesPositions[NewEdgeId].size());

		if (EdgesPositions[NewEdgeId].size()>1){
			std::sort(EdgesPositions[NewEdgeId].begin(), EdgesPositions[NewEdgeId].end(), PosCompare);
		}

	}
	std::string str(EdgeId edgeId){
		std::string s = "";
		if (EdgesPositions.find(edgeId) != EdgesPositions.end()) {
			TRACE("Number of labels "<<EdgesPositions[edgeId].size());
			for (size_t i = 0; i < EdgesPositions[edgeId].size(); i++){
				s+="("+ToString((EdgesPositions[edgeId])[i].start_)+"-"+ToString((EdgesPositions[edgeId])[i].end_)+")\\n";
			}
		}
		return s;
	}

	EdgesPositionHandler(Graph &g) :
		GraphActionHandler<Graph> (g, "EdgePositionHandler") {
	}
	virtual ~EdgesPositionHandler() {
		TRACE("~EdgePositionHandler ok");
	}

	 virtual void HandleGlue(EdgeId new_edge, EdgeId edge1, EdgeId edge2) {
		 DEBUG("Handle glue ");

		 for( size_t i = 0; i< EdgesPositions[edge1].size(); i++){
			 AddEdgePosition(new_edge, (EdgesPositions[edge1])[i].start_,(EdgesPositions[edge1])[i].end_);
		 }
		 for( size_t j = 0; j< EdgesPositions[edge2].size(); j++){
			 AddEdgePosition(new_edge, (EdgesPositions[edge2])[j].start_,(EdgesPositions[edge2])[j].end_);
		 }

/*		 for( size_t i = 0; i< EdgesPositions[edge1].size(); i++){
			 for( size_t j = 0; j< EdgesPositions[edge2].size(); j++){
//				 DEBUG(" "<<EdgesPositions[edge1])[i].start_<<" "<<EdgesPositions[edge1])[i].end_);
//				 DEBUG(" "<<EdgesPositions[edge2])[j].start_<<" "<<EdgesPositions[edge2])[j].end_);
				 if ((EdgesPositions[edge1])[i].end_ + 1 == (EdgesPositions[edge2])[j].start_) {
					 AddEdgePosition(new_edge, (EdgesPositions[edge1])[i].start_, (EdgesPositions[edge2])[j].end_);
				 }
			 }
		 }
		 */
	 }


	 virtual void HandleSplit(EdgeId oldEdge, EdgeId newEdge1, EdgeId newEdge2) {
		 DEBUG("EdgesPositionHandler not handled Split yet");
	 }

 	 virtual void HandleMerge(vector<EdgeId> oldEdges, EdgeId newEdge) {
		 DEBUG("HandleMerge by position handler");
 		 // we assume that all edge have good ordered position labels.
 		 size_t n = oldEdges.size();
 		 vector<EdgePosition> res = (EdgesPositions[oldEdges[0]]);
 		 for (size_t i = 1; i < n; i++) {
 			 res = GluePositionsLists(res, EdgesPositions[oldEdges[i]]);
 		 }

 		 for(size_t i =0; i < res.size(); i++){
 			AddEdgePosition(newEdge, res[i].start_, res[i].end_);
 		 }
	 }
/*
	virtual void HandleAdd(VertexId v) {
		AddVertexIntId(v);
	}
	virtual void HandleDelete(VertexId v) {
		ClearVertexId(v);
	}
*/
 	virtual void HandleAdd(EdgeId e) {
 		DEBUG("Add edge "<<e);
		if (EdgesPositions.find(e) == EdgesPositions.end()) {
 			vector<EdgePosition> NewVec;
 			EdgesPositions[e] = NewVec;
		}
 	}
	virtual void HandleDelete(EdgeId e) {
		if (EdgesPositions[e].size() > 0) {
			DEBUG("Delete edge "<<e<<" handled. Not empty info: "<<EdgesPositions[e].size());
			for (size_t i = 0; i < EdgesPositions[e].size(); i++){
				DEBUG("Position info: "<<EdgesPositions[e][i].start_<<" --- "<<EdgesPositions[e][i].end_);
			}
		}
		else {
			DEBUG("Delete edge "<<e<<" handled.");
		}
		EdgesPositions.erase(e);
	}
 	void HandleVertexSplit(VertexId newVertex, vector<pair<EdgeId, EdgeId> > newEdges, VertexId oldVertex) {
 	}


};



template<class Graph>
class EdgesPosGraphLabeler: public GraphLabeler<Graph> {

protected:
	typedef GraphLabeler<Graph> super;
	typedef typename super::EdgeId EdgeId;
	typedef typename super::VertexId VertexId;
	Graph& g_;
public:
	EdgesPositionHandler<Graph>& EdgesPos;

	EdgesPosGraphLabeler(Graph& g, EdgesPositionHandler<Graph>& EdgesPosit) :
		g_(g), EdgesPos(EdgesPosit) {
	}

	virtual std::string label(VertexId vertexId) const {
		return g_.str(vertexId);
	}

	virtual std::string label(EdgeId edgeId) const {
		return EdgesPos.str(edgeId) + ": " + g_.str(edgeId);
	}
	virtual ~EdgesPosGraphLabeler() {
		TRACE("~EdgesPosGraphLabeler");
	}

};

}

#endif /* EDGES_POSITION_HANDLER_HPP_ */
