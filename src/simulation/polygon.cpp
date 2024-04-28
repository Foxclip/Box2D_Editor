#include <cassert>
#include "simulation/polygon.h"

sf::Text vertex_text;

Logger& operator<<(Logger& lg, const b2Vec2& value) {
	return lg << "(" << value.x << " " << value.y << ")";
}

Logger& operator<<(Logger& lg, const sf::Vector2f& value) {
	return lg << "(" << value.x << " " << value.y << ")";
}

bool CutInfo::isGG() const {
	return green_zone && has_reciprocal && green_reciprocal;
}

bool CutInfo::isGY() const {
	bool gy = green_zone && has_reciprocal && !green_reciprocal;
	bool yg = !green_zone && has_reciprocal && green_reciprocal;
	return gy || yg;
}

bool CutInfo::isYY() const {
	return !green_zone && has_reciprocal && !green_reciprocal;
}

bool CutInfo::isG() const {
	return green_zone && !has_reciprocal;
}

bool CutInfo::isY() const {
	return !green_zone && !has_reciprocal;
}

CutInfo::CutType CutInfo::getType() const {
	if (isGG()) {
		return GG;
	} else if (isGY()) {
		return GY;
	} else if (isYY()) {
		return YY;
	} else if (isG()) {
		return G;
	} else if (isY()) {
		return Y;
	} else {
		return Unknown;
	}
}

std::string CutInfo::toStr() const {
	std::string result;
	std::string zone_str = green_zone ? "green" : "yellow";
	std::string recip_str = green_reciprocal ? ", green reciprocal" : ", yellow reciprocal";
	if (!has_reciprocal) {
		recip_str = "";
	}
	result += std::to_string(from) + "-" + std::to_string(to)
		+ " sqr_dist: " + std::to_string(sqr_dist)
		+ " angle_diff: " + std::to_string(angle_diff)
		+ " angle_diff_sum: " + std::to_string(angle_diff_sum)
		+ " " + zone_str + recip_str;
	return result;
}

SplittablePolygon::SplittablePolygon() { }

SplittablePolygon::SplittablePolygon(size_t count) {
	resetVarray(count);
}

SplittablePolygon::SplittablePolygon(const sf::VertexArray& varray) {
	assert(varray.getVertexCount() > 0);
	this->varray = varray;
}

size_t SplittablePolygon::getPointCount() const {
	return varray.getVertexCount() - 1;
}

sf::Vector2f SplittablePolygon::getPoint(size_t index) const {
	return varray[index].position;
}

sf::Vector2f SplittablePolygon::getLocalCenter() const {
	sf::Vector2f sum = sf::Vector2f(0.0f, 0.0f);
	for (size_t i = 0; i < getPointCount(); i++) {
		sum += getPoint(i);
	}
	return sum / (float)getPointCount();
}

sf::Vector2f SplittablePolygon::getGlobalCenter() const {
	sf::Vector2f local_center = getLocalCenter();
	return getTransform() * getLocalCenter();
}

sf::FloatRect SplittablePolygon::getLocalBounds() const {
	sf::FloatRect result = sf::FloatRect(getPoint(0), sf::Vector2f(0.0f, 0.0f));
	for (size_t i = 0; i < getPointCount(); i++) {
		sf::Vector2f point = getPoint(i);
		utils::extend_bounds(result, point);
	}
	return result;
}

sf::FloatRect SplittablePolygon::getGlobalBounds() const {
	sf::FloatRect result = sf::FloatRect(getTransform() * getPoint(0), sf::Vector2f(0.0f, 0.0f));
	for (size_t i = 0; i < getPointCount(); i++) {
		sf::Vector2f point = getTransform() * getPoint(i);
		utils::extend_bounds(result, point);
	}
	return result;
}

sf::Color SplittablePolygon::getFillColor() const {
	return fill_color;
}

std::vector<SplittablePolygon> SplittablePolygon::getConvexPolygons() const {
	return convex_polygons;
}

sf::Transform SplittablePolygon::getParentGlobalTransform() const {
	if (parent) {
		return parent->getGlobalTransform();
	}
	return sf::Transform::Identity;
}

sf::Transform SplittablePolygon::getGlobalTransform() const {
	return getParentGlobalTransform() * getTransform();
}

bool SplittablePolygon::isConvex() const {
	for (size_t i = 0; i < getPointCount(); i++) {
		if (!isConvexVertex(i)) {
			return false;
		}
	}
	return true;
}

void SplittablePolygon::setPoint(size_t index, const sf::Vector2f& point) {
	assert(index < getPointCount());
	setCutsValid(false);
	varray[index].position = point;
	if (index == 0) {
		varray[varray.getVertexCount() - 1].position = point;
	} else if (index == varray.getVertexCount() - 1) {
		varray[0].position = point;
	}
}

void SplittablePolygon::setLineColor(const sf::Color& color) {
	for (int i = 0; i < varray.getVertexCount(); i++) {
		varray[i].color = color;
	}
	line_color = color;
}

void SplittablePolygon::setFillColor(const sf::Color& color) {
	fill_color = color;
	if (is_convex) {
		for (size_t i = 0; i < triangle_fan.getVertexCount(); i++) {
			triangle_fan[i].color = color;
		}
	} else {
		for (size_t i = 0; i < convex_polygons.size(); i++) {
			convex_polygons[i].setFillColor(color);
		}
	}
}

void SplittablePolygon::calcPotentialCuts(bool consider_convex_vertices) {
	logger << __FUNCTION__"\n";
	LoggerIndent polygon_object_indent;
	std::vector<std::vector<CutInfo>> result(getPointCount());
	std::vector<bool> is_convex_vertex(getPointCount());
	for (size_t vert_i = 0; vert_i < getPointCount(); vert_i++) {
		is_convex_vertex[vert_i] = isConvexVertex(vert_i);
	}
	for (size_t vert_i = 0; vert_i < getPointCount(); vert_i++) {
		sf::Vector2f vertex = getPoint(vert_i);
		if (is_convex_vertex[vert_i]) {
			logger << "Vertex: " << vert_i << ", pos: " << vertex << ", convex" << "\n";
			if (!consider_convex_vertices) {
				continue;
			}
		} else {
			logger << "Vertex: " << vert_i << ", pos: " << vertex << ", concave" << "\n";
		}
		LoggerIndent vertex_indent;
		sf::Vector2f prev_vertex = getPoint(indexLoop(vert_i - 1));
		sf::Vector2f next_vertex = getPoint(indexLoop(vert_i + 1));
		sf::Vector2f side1_dir = utils::normalize(vertex - prev_vertex);
		sf::Vector2f side2_dir = utils::normalize(vertex - next_vertex);
		sf::Vector2f side1_dir_rot = utils::rot90CCW(side1_dir);
		sf::Vector2f side2_dir_rot = utils::rot90CW(side2_dir);
		sf::Vector2f vertex_normal = utils::normalize(side1_dir_rot + side2_dir_rot);
		for (size_t cut_i = 0; cut_i < getPointCount(); cut_i++) {
			bool prev = cut_i == indexLoop(vert_i - 1);
			bool curr = cut_i == indexLoop(vert_i);
			bool next = cut_i == indexLoop(vert_i + 1);
			if (prev || curr || next) {
				logger << "Vertex cut " << vert_i << "-" << cut_i << " is adjacent edge" << "\n";
				continue;
			}
			sf::Vector2f vertex_cut = getPoint(cut_i);
			bool right_side_1 = utils::right_side(vertex_cut, prev_vertex, vertex);
			bool right_side_2 = utils::right_side(vertex_cut, vertex, next_vertex);
			bool red_zone = right_side_1 && right_side_2;
			if (red_zone) {
				logger << "Vertex cut " << vert_i << "-" << cut_i << " is in red zone" << "\n";
				continue;
			}
			size_t edge_index;
			if (intersectsEdge(vertex, vertex_cut, edge_index)) {
				logger << "Vertex cut " << vert_i << "-" << cut_i << " intersects edge " << edge_index << "\n";
				continue;
			}
			bool green_zone = !right_side_1 && !right_side_2;
			bool to_concave = !is_convex_vertex[cut_i];
			{
				std::string green_zone_str = green_zone ? ", green zone" : ", yellow zone";
				std::string to_concave_str = to_concave ? ", to concave" : "";
				logger << "Vertex cut " << vert_i << "-" << cut_i << " OK" << green_zone_str << to_concave_str << "\n";
			}
			sf::Vector2f cut_vector = vertex_cut - vertex;
			CutInfo pv;
			pv.from = vert_i;
			pv.to = cut_i;
			pv.pos_from = vertex;
			pv.pos_to = vertex_cut;
			pv.to_concave = to_concave;
			pv.green_zone = green_zone;
			pv.has_reciprocal = false; // is calculated later
			pv.green_reciprocal = false; // is calculated later
			pv.sqr_dist = utils::sqr_distance(vertex, vertex_cut);
			pv.angle_diff = utils::vector_angle(vertex_normal, cut_vector);
			pv.angle_diff_sum = pv.angle_diff; // full sum is calculated later
			pv.score = pv.angle_diff * 100.0f + sqrt(pv.sqr_dist);
			result[vert_i].push_back(pv);
		}
		logger << "Potential cuts: ";
		for (size_t pv_i = 0; pv_i < result[vert_i].size(); pv_i++) {
			logger << result[vert_i][pv_i].to << " ";
		}
		logger << "\n";
	}

	std::vector<CutInfo> cuts;
	for (size_t vert_i = 0; vert_i < result.size(); vert_i++) {
		for (size_t cut_i = 0; cut_i < result[vert_i].size(); cut_i++) {
			cuts.push_back(result[vert_i][cut_i]);
		}
	}
	logger << "Finding reciprocal cuts\n";
	LoggerIndent reciprocal_cuts_indent;
	for (size_t i = 0; i < cuts.size(); i++) {
		CutInfo& cut1 = cuts[i];
		if (cut1.has_reciprocal) {
			continue;
		}
		for (size_t j = 0; j < cuts.size(); j++) {
			CutInfo& cut2 = cuts[j];
			if (cut1.to == cut2.from && cut2.to == cut1.from) {
				std::string cut1_str = cut1.green_zone ? "green" : "yellow";
				std::string cut2_str = cut2.green_zone ? "green" : "yellow";
				logger << cut1.from << "-" << cut1.to << ", " << cut1_str << "-" << cut2_str << "\n";
				cut1.has_reciprocal = true;
				cut2.has_reciprocal = true;
				cut1.green_reciprocal = cut2.green_zone;
				cut2.green_reciprocal = cut1.green_zone;
				cut1.angle_diff_sum += cut2.angle_diff;
				cut2.angle_diff_sum += cut1.angle_diff;
			}
		}
	}
	potential_cuts = cuts;
	setCutsValid(true);
	createCutsVarray();
}

size_t SplittablePolygon::getPotentialCutsCount() const {
	return potential_cuts.size();
}

void SplittablePolygon::drawPotentialCuts(sf::RenderTarget& target) {
	target.draw(cuts_varray, getTransform());
}

CutInfo SplittablePolygon::getBestCut(BestCutCriterion criterion) const {
	logger << __FUNCTION__"\n";
	LoggerIndent get_best_cut_indent;
	std::vector<CutInfo> cuts(potential_cuts);
	auto cmp = [&](const CutInfo& left, const CutInfo& right) {
		LoggerIndent cmp_indent;
		CutInfo::CutType left_type = left.getType();
		CutInfo::CutType right_type = right.getType();
		if (left_type != right_type) {
			return left_type < right_type;
		} else {
			if (criterion == BestCutCriterion::MIN_DIST) {
				return left.sqr_dist < right.sqr_dist;
			} else if (criterion == BestCutCriterion::MAX_DIST) {
				return left.sqr_dist > right.sqr_dist;
			} else if (criterion == BestCutCriterion::MIN_ANGLE) {
				return left.angle_diff_sum < right.angle_diff_sum;
			} else {
				mAssert(false, "Unknown BestCutCriterion: " + std::to_string(criterion));
				return false;
			}
		}
	};
	std::sort(cuts.begin(), cuts.end(), cmp);
	logger << "Sorted cuts:\n";
	LoggerIndent sorted_cuts_indent;
	for (size_t i = 0; i < cuts.size(); i++) {
		logger << cuts[i].toStr() << "\n";
	}
	return cuts[0];
}

std::vector<SplittablePolygon> SplittablePolygon::getCutPolygons(const CutInfo& cut) const {
	logger << __FUNCTION__"\n";
	LoggerIndent polygon_cut_indent;
	logger << "From: " << cut.from << " to: " << cut.to << "\n";
	assert(cut.from != cut.to);
	std::vector<SplittablePolygon> result(2);
	size_t lower_index = std::min(cut.from, cut.to);
	size_t upper_index = std::max(cut.from, cut.to);
	size_t polygon2_count = upper_index - lower_index + 1;
	size_t polygon1_count = getPointCount() - polygon2_count + 2;
	logger << "Polygon 1 count: " << polygon1_count << "\n";
	logger << "Polygon 2 count: " << polygon2_count << "\n";
	SplittablePolygon polygon1(polygon1_count);
	SplittablePolygon polygon2(polygon2_count);
	size_t polygon1_cur = 0;
	size_t polygon2_cur = 0;
	for (size_t i = 0; i < getPointCount(); i++) {
		logger << "Vertex " << i << "\n";
		LoggerIndent setting_point_indent;
		if (i <= lower_index || i >= upper_index) {
			logger << "Setting point in polygon 1: " << polygon1_cur << "\n";
			polygon1.setPoint(polygon1_cur, getPoint(i));
			polygon1_cur++;
			logger << "polygon1_cur: " << polygon1_cur << "\n";
		}
		if (i >= lower_index && i <= upper_index) {
			logger << "Setting point in polygon 2: " << polygon2_cur << "\n";
			polygon2.setPoint(polygon2_cur, getPoint(i));
			polygon2_cur++;
			logger << "polygon2_cur: " << polygon2_cur << "\n";
		}
	}
	result[0] = polygon1;
	result[1] = polygon2;
	return result;
}

std::vector<SplittablePolygon> SplittablePolygon::cutWithBestCut(bool cut_convex) {
	logger << __FUNCTION__"\n";
	LoggerIndent cut_indent;
	std::vector<SplittablePolygon> result;
	if (!cuts_valid) {
		calcPotentialCuts(cut_convex);
	}
	if (getPotentialCutsCount() == 0) {
		return result;
	}
	//BestCutCriterion criterion = cut_convex ? BestCutCriterion::MIN_ANGLE : BestCutCriterion::MIN_DIST;
	BestCutCriterion criterion = BestCutCriterion::MIN_ANGLE;
	CutInfo cut = getBestCut(criterion);
	std::vector<SplittablePolygon> new_polygons = getCutPolygons(cut);
	result.push_back(new_polygons[0]);
	result.push_back(new_polygons[1]);
	return result;
}

std::vector<SplittablePolygon> SplittablePolygon::cutIntoConvex(size_t max_vertices) {
	logger << __FUNCTION__"\n";
	LoggerIndent cut_indent;
	std::vector<SplittablePolygon> result;
	bool is_convex = isConvex();
	bool vertices_ok = max_vertices == 0 || getPointCount() <= max_vertices;
	if (is_convex && vertices_ok) {
		result.push_back(*this);
		return result;
	}
	std::vector<SplittablePolygon> child_polygons = cutWithBestCut(is_convex);
	for (size_t i = 0; i < child_polygons.size(); i++) {
		std::vector<SplittablePolygon> child_children = child_polygons[i].cutIntoConvex(max_vertices);
		result.insert(result.end(), child_children.begin(), child_children.end());
	}
	return result;
}

void SplittablePolygon::resetVarray(size_t vertex_count) {
	assert(vertex_count > 0);
	varray = sf::VertexArray(sf::LinesStrip, vertex_count + 1);
}

void SplittablePolygon::recenter() {
	sf::Vector2f local_center = getLocalCenter();
	for (size_t i = 0; i < getPointCount(); i++) {
		sf::Vector2f new_pos = getPoint(i) - local_center;
		setPoint(i, new_pos);
	}
	setPosition(local_center);
	setRotation(0.0f);
}

void SplittablePolygon::recut() {
	LoggerTag tag_recut("recut");
	logger << __FUNCTION__"\n";
	LoggerIndent recut_indent;
	convex_polygons = cutIntoConvex(b2_maxPolygonVertices);
	for (size_t polygon_i = 0; polygon_i < convex_polygons.size(); polygon_i++) {
		SplittablePolygon& polygon = convex_polygons[polygon_i];
		polygon.recenter();
		polygon.triangle_fan = sf::VertexArray(sf::TriangleFan, polygon.getPointCount() + 2);
		auto set_vertex = [&](size_t index, sf::Vector2f pos) {
			sf::Vertex vertex;
			vertex.position = pos;
			vertex.color = fill_color;
			polygon.triangle_fan[index] = vertex;
		};
		set_vertex(0, sf::Vector2f());
		for (size_t vertex_i = 0; vertex_i < polygon.getPointCount() + 1; vertex_i++) {
			set_vertex(vertex_i + 1, polygon.getPoint(polygon.indexLoop(vertex_i)));
		}
		polygon.is_convex = true;
		polygon.parent = this;
		polygon.draw_varray = false;
	}
}

SplittablePolygon SplittablePolygon::createRect(sf::Vector2f size) {
	SplittablePolygon rect(4);
	rect.setPoint(0, sf::Vector2f(size.x / 2.0f, size.y / 2.0f));
	rect.setPoint(1, sf::Vector2f(-size.x / 2.0f, size.y / 2.0f));
	rect.setPoint(2, sf::Vector2f(-size.x / 2.0f, -size.y / 2.0f));
	rect.setPoint(3, sf::Vector2f(size.x / 2.0f, -size.y / 2.0f));
	return rect;
}

void SplittablePolygon::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	states.transform *= getTransform();
	if (is_convex) {
		target.draw(triangle_fan, states);
	} else {
		for (size_t i = 0; i < convex_polygons.size(); i++) {
			target.draw(convex_polygons[i], states);
		}
	}
	if (draw_varray || (parent && parent->draw_varray)) {
		target.draw(varray, states);
	}
}

bool SplittablePolygon::isConvexVertex(size_t index) const {
	assert(getPointCount() >= 3);
	sf::Vector2f v1 = getPoint(indexLoop(index - 1));
	sf::Vector2f v2 = getPoint(indexLoop(index));
	sf::Vector2f v3 = getPoint(indexLoop(index + 1));
	return !utils::left_side(v2, v1, v3);
}

bool SplittablePolygon::intersectsEdge(const sf::Vector2f& v1, const sf::Vector2f& v2, size_t& intersect) const {
	for (size_t i = 0; i < getPointCount(); i++) {
		sf::Vector2f e1 = getPoint(i);
		sf::Vector2f e2 = getPoint(indexLoop(i + 1));
		sf::Vector2f intersection;
		if (utils::line_intersect(v1, v2, e1, e2, 0.000001f, intersection)) {
			intersect = i;
			return true;
		}
	}
	return false;
}

size_t SplittablePolygon::indexLoop(ptrdiff_t index) const {
	return utils::neg_mod(index, (ptrdiff_t)getPointCount());
}

void SplittablePolygon::createCutsVarray() {
	if (!cuts_valid) {
		return;
	}
	cuts_varray = sf::VertexArray(sf::Lines, potential_cuts.size() * 2);
	for (size_t i = 0; i < potential_cuts.size(); i++) {
		CutInfo& cut = potential_cuts[i];
		sf::Color color;
		switch (cut.getType()) {
			case CutInfo::GG: color = sf::Color(0, 255, 0); break;
			case CutInfo::GY: color = sf::Color(210, 210, 0); break;
			case CutInfo::YY: color = sf::Color(255, 128, 0); break;
			case CutInfo::G: color = sf::Color(0, 128, 0); break;
			case CutInfo::Y: color = sf::Color(128, 64, 0); break;
			default: color = sf::Color(255, 0, 255); break;
		}
		cuts_varray[i * 2].position = cut.pos_from;
		cuts_varray[i * 2].color = color;
		cuts_varray[i * 2 + 1].position = cut.pos_to;
		cuts_varray[i * 2 + 1].color = color;
	}
}

void SplittablePolygon::setCutsValid(bool value) {
	if (value) {
		cuts_valid = true;
	} else {
		cuts_valid = false;
		cuts_varray = sf::VertexArray();
		convex_polygons = std::vector<SplittablePolygon>();
	}
}
