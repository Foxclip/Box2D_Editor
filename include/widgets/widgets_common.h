#pragma once

#include <SFML/Graphics.hpp>
#include <cassert>
#include <string>
#include <iostream>
#include "logger.h"

namespace fw {

#ifndef NDEBUG

	void _print_msg(bool value);
	void _print_msg(bool value, const std::string& message);

#define wAssert(value, ...) \
	_print_msg(value, __VA_ARGS__); \
	assert(value);

#else

#define wAssert(value, ...)

#endif // !NDEBUG

	Logger& operator<<(Logger& lg, const sf::Vector2f& value);
	sf::Vector2i to2i(const sf::Vector2f& vec);
	sf::Vector2f to2f(const sf::Vector2i& vec);
	sf::Vector2f to2f(const sf::Vector2u& vec);
	void extend_bounds(sf::FloatRect& rect1, const sf::FloatRect& rect2);
	bool parseLL(const std::string& str, long long& result);
	bool parseFloat(const std::string& str, float& result);
	bool contains_point(const sf::FloatRect& rect, const sf::Vector2f& point);
	bool contains_point(const sf::RectangleShape& shape, const sf::Vector2f& point);
	void quantize_position(sf::Transform& transform);
	enum QuantizeMode {
		QUANTIZE_MODE_FLOOR,
		QUANTIZE_MODE_FLOOR_SUBTRACT,
		QUANTIZE_MODE_CEIL_SUBTRACT,
	};
	sf::FloatRect quantize_rect(const sf::FloatRect& rect, QuantizeMode quantize_mode);

	template <typename TNode>
	concept NodeLess = requires(const TNode& left, const TNode& right) {
		left < right;
	};

	template <typename TPFunc, typename TNode>
	concept NodeVectorFunc = requires(TPFunc f, TNode n) {
		{ f(n) } -> std::convertible_to<const std::vector<TNode>&>;
	};

	template<typename TNode, typename TPFunc>
	requires NodeLess<TNode> && std::equality_comparable<TNode> && NodeVectorFunc<TPFunc, TNode>
	std::vector<std::vector<TNode>> toposort(
		const std::vector<TNode>& nodes,
		const TPFunc& get_parents_func,
		std::function<void(const std::vector<TNode>&)>* OnLoopDetected = nullptr
	) {
		std::vector<std::vector<TNode>> result;
		std::map<TNode, size_t> processed_nodes;
		std::vector<TNode> node_stack;
		std::set<TNode> node_stack_set;
		bool loop_detected = false;
		std::set<std::set<TNode>> loops;
		std::function<ptrdiff_t(const TNode&)> process_node = [&](const TNode& node) {
			node_stack.push_back(node);
			node_stack_set.insert(node);
			const std::vector<TNode>& parents = get_parents_func(node);
			ptrdiff_t max_layer = -1;
			bool looping = false;
			for (const TNode& parent : parents) {
				if (node_stack_set.contains(parent)) {
					looping = true;
					loop_detected = true;
					max_layer = -1;
					if (OnLoopDetected) {
						auto it = std::find(node_stack.begin(), node_stack.end(), parent);
						std::vector<TNode> loop(it, node_stack.end());
						std::set<TNode> loop_set = std::set<TNode>(loop.begin(), loop.end());
						if (!loops.contains(loop_set)) {
							loops.insert(loop_set);
							(*OnLoopDetected)(loop);
						}
					} else {
						throw std::runtime_error("toposort: loop detected");
					}
				} else {
					ptrdiff_t parent_layer = -1;
					auto it = processed_nodes.find(parent);
					if (loop_detected || it == processed_nodes.end()) {
						parent_layer = process_node(parent);
					} else {
						parent_layer = it->second;
					}
					if (parent_layer == -1) {
						looping = true;
					}
					if (!looping) {
						max_layer = std::max(max_layer, parent_layer);
					}
				}
			}
			if (!looping) {
				max_layer++;
				if (max_layer >= (ptrdiff_t)result.size()) {
					result.push_back(std::vector<TNode>());
				}
				result[max_layer].push_back(node);
			}
			processed_nodes[node] = max_layer;
			node_stack.pop_back();
			node_stack_set.erase(node);
			return max_layer;
		};
		for (const TNode& node : nodes) {
			if (loop_detected || !processed_nodes.contains(node)) {
				process_node(node);
			}
		}
		return result;
	}

}
