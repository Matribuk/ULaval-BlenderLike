#include "Components/Renderable.hpp"

Renderable::Renderable(
    const ofMesh& m,
    const ofColor& c,
    bool v,
    ofShader* s,
    ofTexture* t,
    bool primitive
) :
    mesh(m),
    color(c),
    visible(v),
    isPrimitive(primitive)
{
    material = new Material();
    material->shader = s;
    if (s) material->effects.push_back(s);
    material->texture = t;
}

Renderable::Renderable(const Renderable& other)
:
    mesh(other.mesh),
    color(other.color),
    visible(other.visible),
    showOutline(other.showOutline),
    isPrimitive(other.isPrimitive)
{
    if (other.material) material = new Material(*other.material);
    else material = nullptr;
}

Renderable& Renderable::operator=(const Renderable& other)
{
    if (this != &other) {
        mesh = other.mesh;
        color = other.color;
        visible = other.visible;
        showOutline = other.showOutline;
        isPrimitive = other.isPrimitive;

        if (material) delete material;

        material = other.material ? new Material(*other.material) : nullptr;
    }
    return *this;
}

Renderable::~Renderable()
{
    delete material;
}
