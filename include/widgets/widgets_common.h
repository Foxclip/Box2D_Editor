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

	template <typename TPFunc, typename TNode>
	concept NodeVectorFunc = requires(TPFunc f, TNode n) {
		{ f(n) } -> std::convertible_to<const std::vector<TNode>&>;
	};

	template<typename TNode, typename TPFunc>
	requires NodeVectorFunc<TPFunc, TNode>
	std::vector<std::vector<TNode>> toposort(
		const std::vector<TNode>& nodes,
		const TPFunc& get_parents_func
	) {
		std::vector<std::vector<TNode>> result;
		std::map<TNode, size_t> proceessed_nodes;
		std::set<TNode> node_stack;
		std::function<size_t(const TNode&)> process_node = [&](const TNode& node) {
			node_stack.insert(node);
			const std::vector<TNode>& parents = get_parents_func(node);
			ptrdiff_t max_layer = -1;
			for (const TNode& parent : parents) {
				if (node_stack.contains(parent)) {
					throw std::runtime_error("Loop detected");
				}
				size_t parent_layer = 0;
				auto it = proceessed_nodes.find(parent);
				if (it == proceessed_nodes.end()) {
					parent_layer = process_node(parent);
				} else {
					parent_layer = it->second;
				}
				max_layer = std::max(max_layer, (ptrdiff_t)parent_layer);
			}
			max_layer++;
			if (max_layer >= (ptrdiff_t)result.size()) {
				result.push_back(std::vector<TNode>());
			}
			result[max_layer].push_back(node);
			proceessed_nodes[node] = max_layer;
			node_stack.erase(node);
			return max_layer;
		};
		for (const TNode& node : nodes) {
			if (!proceessed_nodes.contains(node)) {
				process_node(node);
			}
		}
		return result;
	}


}
