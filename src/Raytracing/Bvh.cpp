#include "Bvh.hpp"

Bvh::Bvh(const HittableList& list) :
    Bvh(
        const_cast<std::vector<std::shared_ptr<Hittable>>&>(list.objects),
        0, list.objects.size()
    )
{}

Bvh::Bvh(std::vector<std::shared_ptr<Hittable>>& objects, size_t start, size_t end)
{
    Aabb totalBox = this->_computeBoundingBox(objects, start, end);
    int axis = totalBox.longestAxis();

    auto comparator =
        (axis == 0) ? this->_boxXCompare :
        (axis == 1) ? this->_boxYCompare :
                      this->_boxZCompare;

    size_t objectSpan = end - start;

    if (objectSpan == 1) {
        this->_left = objects[start];
        this->_right = objects[start];
    } else if (objectSpan == 2) {
        if (comparator(objects[start], objects[start + 1])) {
            this->_left = objects[start];
            this->_right = objects[start + 1];
        } else {
            this->_left = objects[start + 1];
            this->_right = objects[start];
        }
    } else {
        std::sort(objects.begin() + start, objects.begin() + end, comparator);

        size_t mid = start + objectSpan / 2;

        this->_left  = std::make_shared<Bvh>(objects, start, mid);
        this->_right = std::make_shared<Bvh>(objects, mid, end);
    }

    Aabb leftBox  = this->_getBox(this->_left);
    Aabb rightBox = this->_getBox(this->_right);

    this->_bbox = Aabb::surroundingBox(leftBox, rightBox);
}

bool Bvh::hit(const Ray& r, Interval rayT, HitRecord& rec) const
{
    if (!this->_bbox.hit(r, rayT)) return false;

    bool hitLeft  = this->_left->hit(r, rayT, rec);
    bool hitRight = this->_right->hit(
        r,
        Interval(rayT.min, hitLeft ? rec.t : rayT.max),
        rec
    );

    return hitLeft || hitRight;
}

Aabb Bvh::boundingBox() const
{
    return this->_bbox;
}

Aabb Bvh::_computeBoundingBox(
    const std::vector<std::shared_ptr<Hittable>>& objects,
    size_t start,
    size_t end
) {
    Aabb result = _getBox(objects[start]);

    for (size_t i = start + 1; i < end; i++)
        result = Aabb::surroundingBox(result, _getBox(objects[i]));

    return result;
}

bool Bvh::_boxCompare(
    const std::shared_ptr<Hittable> a,
    const std::shared_ptr<Hittable> b,
    int axisIndex
) {
    Aabb boxA = _getBox(a);
    Aabb boxB = _getBox(b);

    return boxA.axisInterval(axisIndex).min < boxB.axisInterval(axisIndex).min;
}

bool Bvh::_boxXCompare(
    const std::shared_ptr<Hittable> a,
    const std::shared_ptr<Hittable> b
) {
    return _boxCompare(a, b, 0);
}

bool Bvh::_boxYCompare(
    const std::shared_ptr<Hittable> a,
    const std::shared_ptr<Hittable> b
) {
    return _boxCompare(a, b, 1);
}

bool Bvh::_boxZCompare(
    const std::shared_ptr<Hittable> a,
    const std::shared_ptr<Hittable> b
) {
    return _boxCompare(a, b, 2);
}

Aabb Bvh::_getBox(const std::shared_ptr<Hittable>& obj)
{
    return obj->boundingBox();
}
