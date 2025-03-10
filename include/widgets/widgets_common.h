#pragma once

#include <SFML/Graphics.hpp>
#include <cassert>
#include <string>
#include <iostream>
#include <numbers>
#include "common/logger.h"
#include "font.h"

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
	float to_degrees(float angle);
	float to_radians(float angle);
	sf::Vector2i to2i(const sf::Vector2f& vec);
	sf::Vector2i to2i(const sf::Vector2u& vec);
	sf::Vector2f to2f(const sf::Vector2i& vec);
	sf::Vector2f to2f(const sf::Vector2u& vec);
	void extend_bounds(sf::FloatRect& rect1, const sf::FloatRect& rect2);
	bool parseLL(const std::string& str, long long& result);
	bool parseFloat(const std::string& str, float& result);
	bool contains_point(const sf::FloatRect& rect, const sf::Vector2f& point, bool include_upper_bound = false);
	bool contains_point(const sf::RectangleShape& shape, const sf::Vector2f& point, bool include_upper_bound = false);
	void quantize_position(sf::Transform& transform);
	enum QuantizeMode {
		QUANTIZE_MODE_FLOOR,
		QUANTIZE_MODE_FLOOR_SUBTRACT,
		QUANTIZE_MODE_CEIL_SUBTRACT,
	};
	sf::FloatRect quantize_rect(const sf::FloatRect& rect, QuantizeMode quantize_mode);
	void str_to_file(std::string& str, const std::filesystem::path& path);
	std::string file_to_str(const std::filesystem::path& path);
	std::vector<std::string> read_file_lines(const std::filesystem::path& path);
	std::string trim(const std::string &s);

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
			if (!loop_detected) {
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

	template <typename TVec2>
	TVec2 get_circle_vertex(ptrdiff_t index, size_t point_count, float radius, float angle_offset = 0.0f) {
		float angle = (float)((float)index / point_count * 2 * std::numbers::pi + angle_offset);
		float x = std::cos(angle) * radius;
		float y = std::sin(angle) * radius;
		return TVec2(x, y);
	}

	template <typename TVec2>
	std::vector<TVec2> get_regular_polygon(size_t point_count, float radius, float angle_offset = 0.0f) {
		std::vector<TVec2> vertices;
		for (size_t i = 0; i < point_count; i++) {
			TVec2 vertex = get_circle_vertex<TVec2>(i, point_count, radius, angle_offset);
			vertices.push_back(vertex);
		}
		return vertices;
	}

	template <typename T>
	float length(const T& vec) {
		return sqrt(vec.x * vec.x + vec.y * vec.y);
	}

}
