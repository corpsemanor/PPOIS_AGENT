#pragma once

#include <sc-memory/kpm/sc_agent.hpp>


#include "sc-agents-common/utils/GenerationUtils.hpp"
#include "sc-agents-common/utils/AgentUtils.hpp"
#include "sc-agents-common/utils/IteratorUtils.hpp"

#include "keynodes/keynodes.hpp"
#include "CycleSearchAgent.generated.hpp"

#include <list>
#include <map>
#include <climits>


#define DEBUG_MODE



namespace exampleModule {

	class CycleSearchAgent : public ScAgent {

		SC_CLASS(Agent, Event(Keynodes::question_find_cycle, ScEvent::Type::AddOutputEdge));
		SC_GENERATED_BODY();

	private:

		std::string get_list_string(std::list<size_t> list, std::string delimeter = " ") {

			std::string output_string = "";

			for (auto& elem : list)
				output_string += std::to_string(elem) + delimeter;

			return output_string;
		}

		std::string get_cycle_string(std::list<std::list<size_t>> cycle_list) {

			std::string output_string = "";

			for (auto& list : cycle_list) {

				for (auto& element : list)
					output_string += std::to_string(element) + ' ';

				output_string += '\n';
			}

			return output_string;
		}

		std::string get_container_string(std::map<size_t, std::list<size_t>>& graph_container) {

			std::string output_string = "";

			for (auto& a : graph_container) {

				output_string = output_string + "[" + std::to_string(a.first) + "]\t | ";

				for (auto& b : a.second)
					output_string += std::to_string(b) + ' ';

				output_string += '\n';
			}

			return output_string;
		}

	private:

		int is_present_in_cycle_list(std::list<std::list<size_t>>& cycle_list, std::list<size_t> main_cycle) {

			bool abort = false;

			for (auto& cycle : cycle_list)
				if (cycle.size() == main_cycle.size()) {

					std::list<size_t>::iterator cycle_iterator;

					for (std::list<size_t>::iterator it = cycle.begin(); it != cycle.end(); it++)
						if (*it == *main_cycle.begin()) {
							cycle_iterator = it;
							break;
						}
						else if (it == std::prev(cycle.end()))
							abort = true;

					if (abort == true) {
						abort = false;
						continue;
					}

					for (std::list<size_t>::iterator main_iterator = main_cycle.begin(); main_iterator != main_cycle.end(); main_iterator++) {

						if (*cycle_iterator != *main_iterator)
							break;

						if (cycle_iterator == std::prev(cycle.end()))
							cycle_iterator = cycle.begin();
						else
							cycle_iterator++;

						if (main_iterator == std::prev(main_cycle.end()))
							return 1;

					}


					for (std::list<size_t>::iterator it = cycle.begin(); it != cycle.end(); it++)
						if (*it == *main_cycle.begin()) {
							cycle_iterator = it;
							break;
						}

					std::list<size_t>::iterator main_iterator = main_cycle.begin();

					do {

						if (*cycle_iterator != *main_iterator) {

							abort = true;
							break;
						}

						if (main_iterator == main_cycle.begin())
							main_iterator = main_cycle.end();

						if (cycle_iterator == std::prev(cycle.end()))
							cycle_iterator = cycle.begin();
						else
							cycle_iterator++;

						main_iterator--;

					} while (main_iterator != main_cycle.begin());

					if (abort == true) {
						abort = false;
						continue;
					}

					return 1;
				}

			return 0;
		}


	private:

		void cycle_DFS(
			std::map<size_t, std::list<size_t>>& main_graph_container,
			std::list<std::list<size_t>>& cycle_list,
			std::list<size_t> visited_list,
			std::map<size_t, bool> visited_map,
			const size_t& begin,
			const size_t& current,
			size_t step = 0,
			size_t vertex_limit = INT_MAX
		) {

			visited_map[current] = true;
			visited_list.push_back(current);

			if (visited_list.size() > vertex_limit)
				return;

			for (auto& a : main_graph_container[current])
				if (visited_map[a] == false)
					cycle_DFS(main_graph_container, cycle_list, visited_list, visited_map, begin, a, step + 1, vertex_limit);
				else if (a == begin && step > 1)
					if (!is_present_in_cycle_list(cycle_list, visited_list) && visited_list.size() == vertex_limit)
						cycle_list.push_back(visited_list);

		}

	private:

		std::map<size_t, std::list<size_t>> get_graph_container(ScMemoryContext* context, ScAddr main_context_node) {

			std::map<size_t, std::list<size_t>> output_graph_container;

			ScAddr main_question_node = main_context_node;
			ScAddr main_cluster_node = utils::IteratorUtils::getAnyByOutRelation(context, main_question_node, scAgentsCommon::CoreKeynodes::rrel_1);

			if (!main_cluster_node.IsValid())
				SC_LOG_INFO("[ERROR] NODE ADDRESS IS NULL");

			ScIterator3Ptr main_cluster_iterator = context->Iterator3(
				main_cluster_node,
				ScType::EdgeAccessConstPosPerm,
				ScType::NodeConst
			);
			//f_a_a f_a_f a_a_f
			while (main_cluster_iterator->Next()) {

				ScAddr current_node = main_cluster_iterator->Get(2);
				size_t current_node_index = 65536 * current_node.GetRealAddr().seg + current_node.GetRealAddr().offset;

				output_graph_container[current_node_index];

				ScIterator3Ptr current_node_iterator = context->Iterator3(
					current_node,
					ScType::ScType::EdgeUCommonConst,
					ScType::NodeConst);

				while (current_node_iterator->Next()) {

					ScAddr adjacent_node = current_node_iterator->Get(2);
					size_t adjacent_node_index = 65536 * adjacent_node.GetRealAddr().seg + adjacent_node.GetRealAddr().offset;

					output_graph_container[current_node_index].push_back(adjacent_node_index);
					output_graph_container[adjacent_node_index].push_back(current_node_index);
				}

#ifdef DEBUG_MODE
				SC_LOG_DEBUG("*************************");
				SC_LOG_DEBUG("[NODE] | [SEG] " + std::to_string(main_cluster_iterator->Get(2).GetRealAddr().seg) +
					" | [OFFSET] " + std::to_string(main_cluster_iterator->Get(2).GetRealAddr().offset));
#endif

			}

#ifdef DEBUG_MODE
			SC_LOG_DEBUG('\n' + get_container_string(output_graph_container));
#endif

			return output_graph_container;
		}

		std::list<std::list<size_t>> get_cycle_list(std::map<size_t, std::list<size_t>> main_graph_container, size_t max_cycle_length) {

			std::list<std::list<size_t>> output_cycle_list;
			std::map<size_t, bool> visited_map;
			std::list<size_t> visited_list;

			for (auto& a : main_graph_container)
				visited_map[a.first] = false;

			for (auto& a : main_graph_container)
				cycle_DFS(main_graph_container, output_cycle_list, visited_list, visited_map, a.first, a.first, 0, max_cycle_length);

			//remove_cycle_duplicates(output_cycle_list);

			return output_cycle_list;
		}

	private:

		void create_sc_answer_node(std::list<std::list<size_t>> cycle_list, ScMemoryContext* context, ScAddr main_context_node) {

			ScAddr answer_node = context->HelperResolveSystemIdtf("output_cycle_list", ScType::NodeConstStruct);
			ScAddr main_cluster_node = utils::IteratorUtils::getAnyByOutRelation(context, main_context_node, scAgentsCommon::CoreKeynodes::rrel_1);

			context->CreateEdge(ScType::EdgeAccessConstPosPerm, main_cluster_node, answer_node);

			for (auto& list : cycle_list) {

				ScAddr current_list_node = context->HelperResolveSystemIdtf("cycle_list_" + get_list_string(list, "_"), ScType::NodeConstStruct);
				context->CreateEdge(ScType::EdgeAccessConstPosPerm, answer_node, current_list_node);

				for (std::list<size_t>::iterator it = list.begin(); it != list.end(); it++) {

					ScAddr current_node = context->HelperResolveSystemIdtf(std::to_string(*it), ScType::NodeConst);
					ScAddr next_node = context->HelperResolveSystemIdtf(std::to_string(*std::next(it)), ScType::NodeConst);
					context->CreateEdge(ScType::EdgeAccessConstPosPerm, current_list_node, current_node);
					context->CreateEdge(ScType::EdgeUCommonConst, current_node, next_node);

				}

				ScAddr first_node = context->HelperResolveSystemIdtf(std::to_string(*list.begin()), ScType::NodeConst);
				ScAddr last_node = context->HelperResolveSystemIdtf(std::to_string(*std::prev(list.end())), ScType::NodeConst);
				context->CreateEdge(ScType::EdgeUCommonConst, last_node, first_node);

			}

			SC_LOG_INFO("[ANSWER] NODE 'concept_output_cycle_list' CREATED");

		}

	};

} // namespace exampleModule
