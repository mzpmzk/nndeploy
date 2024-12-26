#include <gtest/gtest.h>
#include <memory>
#include <vector>

#include "nndeploy/dag/edge.h"
#include "nndeploy/dag/node.h"
#include "nndeploy/dag/graph.h"
using namespace nndeploy::dag;

class GraphTest : public testing::Test {
    protected:
    std::unique_ptr<Graph> ConstructGraph(const std::string &name, 
                                          Edge *input, Edge *output) {
        return std::make_unique<Graph>(name, input, output);
    }

    std::unique_ptr<Graph> ConstructGraphWithVecArgs(const std::string &name, 
                                                     std::vector<Edge *> inputs,
                                                     std::vector<Edge *> outputs) {
        return std::make_unique<Graph>(name, inputs, outputs);
    }

    std::unique_ptr<Graph> ConstructGraphWithInitLsArgs(const std::string &name, 
                                                     std::initializer_list<Edge *> inputs,
                                                     std::initializer_list<Edge *> outputs) {
        return std::make_unique<Graph>(name, inputs, outputs);
    }
};

TEST_F(GraphTest, GraphWithOneInputOutputEdge) {
    auto edge_in =  std::make_unique<Edge>("edge_in");
    auto edge_out = std::make_unique<Edge>("edge_out");
    auto graph = ConstructGraph("@@!!##$$", edge_in.get(), edge_out.get());
    ASSERT_TRUE(graph->getConstructed());
    EXPECT_TRUE(graph->getName() == "@@!!##$$");
    EXPECT_TRUE(graph->getEdge("edge_in") == edge_in.get());
    EXPECT_TRUE(graph->getEdge("edge_out") == edge_out.get());
}


//This test fails, there may be chance of potential optimizations
TEST_F(GraphTest, GraphWithDuplicateOutputEdge) {
    auto edge_in =  std::make_unique<Edge>("edge_in");
    auto graph = ConstructGraph("3.141@@!!", edge_in.get(), edge_in.get());
    EXPECT_FALSE(graph->getConstructed()); //is false
    EXPECT_FALSE(graph->getAllOutput()[0] == edge_in.get()); // this should false
    EXPECT_TRUE(graph->getAllInput()[0] == edge_in.get()); //is true 
}

TEST_F(GraphTest, GraphWithVectorInputOutputEdge) {
    auto inputs = std::vector<Edge *>();
    auto outputs = std::vector<Edge *>();
    inputs.emplace_back(new Edge("in_1"));
    inputs.emplace_back(new Edge("in_2"));
    inputs.emplace_back(new Edge("in_3"));
    inputs.emplace_back(new Edge("in_4"));
    outputs.emplace_back(new Edge("out_1"));
    outputs.emplace_back(new Edge("out_2"));
    outputs.emplace_back(new Edge("out_3"));
    outputs.emplace_back(new Edge("out_4"));
    auto graph = ConstructGraphWithVecArgs("@@!!##$$", inputs, outputs);
    ASSERT_TRUE(graph->getConstructed());
    ASSERT_EQ(graph->getAllInput().size(), 4);
    ASSERT_EQ(graph->getAllOutput().size(), 4);
    
    //TODO: do this in ~GraphTest()
    for(auto in_edge: inputs) {delete in_edge;}
    for(auto out_edge: outputs) {delete out_edge;}
}
