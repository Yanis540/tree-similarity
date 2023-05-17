#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include "unit_cost_model.h"
#include "string_label.h"
#include "node.h"
#include "bracket_notation_parser.h"
#include "cted_tree_index.h"

int main() {

  // Type aliases.
  using Label = label::StringLabel;
  using CostModel = cost_model::UnitCostModelLD<Label>;
  using LabelDictionary = label::LabelDictionary<Label>;

  // Declare parser.
  parser::BracketNotationParser<Label> bnp;

  // Trees.
  node::Node<Label> t1;
  node::Node<Label> t2;
  
  // Initialise label dictionary - separate dictionary for each test tree
  // becuse it is easier to keep track of label ids.
  LabelDictionary ld;
  
  // Initialise cost model.
  CostModel ucm(ld);

  // Parse test cases from file.
  std::ifstream test_cases_file("cted_ub_ted_test_data.txt");
  if (!test_cases_file.is_open()) {
    std::cerr << "Error while opening file." << std::endl;
    return -1;
  }

  // Initialise String Edit Distance algorithm.
  ted_ub::CTEDTreeIndex<CostModel, node::TreeIndexCTED> cted_algorithm(ucm);
  
  // Initialise two tree indexes.
  // Use TreeIndexAll that is a superset of all algorithms' indexes.
  node::TreeIndexCTED ti1;
  node::TreeIndexCTED ti2;

  // Read test cases from a file line by line.
  for (std::string line; std::getline( test_cases_file, line);) {
    if (line[0] == '#') {
      std::string comment = line;
      // Read the single test case.
      std::getline(test_cases_file, line);
      std::string input_tree_1_string = line;
      std::getline(test_cases_file, line);
      std::string input_tree_2_string = line;
      std::getline(test_cases_file, line);
      double correct_result = std::stod(line);

      // Parse test tree.
      try {
        t1 = bnp.parse_single(input_tree_1_string);
        t2 = bnp.parse_single(input_tree_2_string);
      }
      catch(const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return -1;
      }

      // Index input trees.
      node::index_tree(ti1, t1, ld, ucm);
      node::index_tree(ti2, t2, ld, ucm);

      // Execute the algorithm.
      double computed_results = cted_algorithm.ted(ti1, ti2);

      if (correct_result != computed_results) {
        std::cerr << "Incorrect CTED result: " << computed_results << " instead of " << correct_result << std::endl;
        std::cerr << "Test case: " << comment << std::endl;
        std::cerr << input_tree_1_string << std::endl;
        std::cerr << input_tree_2_string << std::endl;
        return -1;
      }
    }
  }

  return 0;
}
