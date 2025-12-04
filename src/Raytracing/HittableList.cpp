#include "HittableList.hpp"

HittableList::HittableList(std::shared_ptr<Hittable> object)
{
    this->add(object);
}

void HittableList::clear()
{
    this->objects.clear();
}

void HittableList::add(std::shared_ptr<Hittable> object)
{
    this->objects.push_back(object);
}

bool HittableList::hit(const Ray& r, Interval rayT, HitRecord& rec) const
{
    HitRecord tempRecord;
    bool hitAnything = false;
    auto closestSoFar = rayT.max;

    for (const auto& object : this->objects) {
        if (object->hit(r, Interval(rayT.min, closestSoFar), tempRecord)) {
            hitAnything = true;
            closestSoFar = tempRecord.t;
            rec = tempRecord;
        }
    }

    return hitAnything;
}

Aabb HittableList::boundingBox() const
{
    if (this->objects.empty()) return Aabb(point3(0, 0, 0), point3(0, 0, 0));

    Aabb result = this->objects[0]->boundingBox();

    for (size_t i = 1; i < this->objects.size(); i++)
        result = Aabb::surroundingBox(result, this->objects[i]->boundingBox());

    return result;
}
