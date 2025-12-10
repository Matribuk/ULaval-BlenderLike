#include "CameraWithLights.hpp"

void CameraWithLights::render(const Hittable& world, std::vector<unsigned char>& pixels)
{
    this->_initialize();

    pixels.resize(imageWidth * this->_imageHeight * 3);

    unsigned int num_threads = std::thread::hardware_concurrency();
    if (num_threads == 0) num_threads = 4;

    std::vector<std::thread> threads;
    int rows_per_thread = this->_imageHeight / num_threads;

    for (unsigned int t = 0; t < num_threads; ++t) {
        int start_row = t * rows_per_thread;
        int end_row = (t == num_threads - 1) ? this->_imageHeight : (t + 1) * rows_per_thread;

        threads.emplace_back([this, &world, &pixels, start_row, end_row]() {
            for (int j = start_row; j < end_row; j++) {
                for (int i = 0; i < imageWidth; i++) {
                    Color pixel_color(0, 0, 0);

                    for (int sample = 0; sample < samplesPerPixel; sample++) {
                        Ray r = this->_getRay(i, j);
                        pixel_color += this->_rayColor(r, maxDepth, world);
                    }

                    auto scale = 1.0 / samplesPerPixel;
                    auto r = pixel_color.x() * scale;
                    auto g = pixel_color.y() * scale;
                    auto b = pixel_color.z() * scale;

                    r = this->_linearToGamma(r);
                    g = this->_linearToGamma(g);
                    b = this->_linearToGamma(b);

                    int rbyte = int(256 * std::clamp(r, 0.0, 0.999));
                    int gbyte = int(256 * std::clamp(g, 0.0, 0.999));
                    int bbyte = int(256 * std::clamp(b, 0.0, 0.999));

                    int pixel_index = ((this->_imageHeight - 1 - j) * imageWidth + i) * 3;
                    pixels[pixel_index + 0] = rbyte;
                    pixels[pixel_index + 1] = gbyte;
                    pixels[pixel_index + 2] = bbyte;
                }
            }
        });
    }

    for (auto& thread : threads) {
        thread.join();
    }
}

void CameraWithLights::_initialize()
{
    this->_imageHeight = int(imageWidth / aspectRatio);
    this->_imageHeight = (this->_imageHeight < 1) ? 1 : this->_imageHeight;

    this->_pixelSamplesScale = 1.0 / samplesPerPixel;

    this->_center = lookFrom;

    auto theta = this->_degreesToRadians(vfov);
    auto h = std::tan(theta / 2);
    auto viewport_height = 2 * h * this->_focusDist;
    auto viewport_width = viewport_height * (double(imageWidth) / this->_imageHeight);

    this->_w = unitVector(lookFrom - lookAt);
    this->_u = unitVector(cross(vup, this->_w));
    this->_v = cross(this->_w, this->_u);

    Vec3 viewportU = viewport_width * this->_u;
    Vec3 viewportV = viewport_height * -this->_v;

    this->_pixelDeltaU = viewportU / imageWidth;
    this->_pixelDeltaV = viewportV / this->_imageHeight;

    auto viewportUpperLeft = this->_center - (this->_focusDist * this->_w) - viewportU / 2 - viewportV / 2;

    this->_pixel00Loc = viewportUpperLeft + 0.5 * (this->_pixelDeltaU + this->_pixelDeltaV);
}

Ray CameraWithLights::_getRay(int i, int j) const
{
    auto offset = this->_sampleSquare();
    auto pixel_sample = this->_pixel00Loc + ((i + offset.x()) * this->_pixelDeltaU) + ((j + offset.y()) * this->_pixelDeltaV);
    auto ray_origin = this->_center;
    auto ray_direction = pixel_sample - ray_origin;

    return Ray(ray_origin, ray_direction);
}

Vec3 CameraWithLights::_sampleSquare() const
{
    return Vec3(_randomDouble() - 0.5, this->_randomDouble() - 0.5, 0);
}

Color CameraWithLights::_rayColor(const Ray& r, int depth, const Hittable& world) const
{
    if (depth <= 0) return Color(0, 0, 0);

    HitRecord rec;

    if (world.hit(r, Interval(0.001, INFINITY), rec)) {
        Ray scattered;
        Color attenuation;
        Color emitted = rec.mat->emitted();

        if (!rec.mat->scatter(r, rec, attenuation, scattered)) {
            return emitted;
        }

        Color indirectLight = attenuation * this->_rayColor(scattered, depth - 1, world);

        if (lights && !lights->lights.empty()) {
            Vec3 view_dir = unitVector(-r.direction());
            Color albedo = attenuation;

            Color directLight =
                lights->computeTotalLighting(
                    rec.p,
                    rec.normal,
                    view_dir,
                    albedo,
                    32.0,
                    world
                );

            return emitted + directLight + indirectLight * 0.3;
        }
        else {
            return emitted + indirectLight;
        }
    }

    if (this->skybox && this->skybox->isLoaded()) {
        return this->skybox->sample(r.direction());
    }

    Vec3 unit_direction = unitVector(r.direction());
    auto a = 0.5 * (unit_direction.y() + 1.0);

    return (1.0 - a) * Color(1.0, 1.0, 1.0)
         + a * Color(0.5, 0.7, 1.0);
}

double CameraWithLights::_randomDouble()
{
    return std::rand() / (RAND_MAX + 1.0);
}

double CameraWithLights::_degreesToRadians(double degrees)
{
    return degrees * M_PI / 180.0;
}

double CameraWithLights::_linearToGamma(double linear_component)
{
    if (linear_component > 0)
        return std::sqrt(linear_component);
    return 0;
}
