#include "event.hpp"

namespace TitiCplusplus {

Event::Event(float rank, std::size_t currentIndex) : m_rank{rank}, m_currentIndex{currentIndex} {}

Event::~Event() {}

const float Event::getRank() const {
	return m_rank;
}

const std::size_t Event::getCurrentIndex() const {
	return m_currentIndex;
}

}; // TitiCplusplus
