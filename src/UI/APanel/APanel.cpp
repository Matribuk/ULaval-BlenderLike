#include "APanel.hpp"

void APanel::setPosition(float x, float y)
{
    this->_pos = { x, y };
}

void APanel::setSize(float w, float h)
{
    this->_size = { w, h };
}

void APanel::update() {}
