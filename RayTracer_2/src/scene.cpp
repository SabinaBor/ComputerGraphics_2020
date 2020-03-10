#include "scene.h"

#include "hit.h"
#include "image.h"
#include "material.h"
#include "ray.h"
#include "sphere.h"

#include <algorithm>
#include <cmath>
#include <limits>

using namespace std;

pair<ObjectPtr, Hit> Scene::castRay(Ray const &ray) const {
    // Find hit object and distance
    Hit min_hit(numeric_limits<double>::infinity(), Vector());
    ObjectPtr obj = nullptr;
    for (unsigned idx = 0; idx != objects.size(); ++idx) {
        Hit hit(objects[idx]->intersect(ray));
        if (hit.t < min_hit.t) {
            min_hit = hit;
            obj = objects[idx];
        }
    }

    return pair<ObjectPtr, Hit>(obj, min_hit);
}

Color Scene::trace(Ray const &ray, unsigned depth) {
    pair<ObjectPtr, Hit> mainhit = castRay(ray);
    ObjectPtr obj = mainhit.first;
    Hit min_hit = mainhit.second;

    // No hit? Return background color.
    if (!obj)
        return Color(0.0, 0.0, 0.0);


    Material const &material = obj->material;
    Point hit = ray.at(min_hit.t);
    Vector V = -ray.D;

    if(material.hasTexture){
        Vector uandv = toUV(hit);
        material.texture.colorAt(uandv[0], 1.0-uandv[1]);
    }
    // Pre-condition: For closed objects, N points outwards.
    Vector N = min_hit.N;

    // The shading normal always points in the direction of the view,
    // as required by the Phong illumination model.
    Vector shadingN;
    if (N.dot(V) >= 0.0)
        shadingN = N;
    else
        shadingN = -N;

    Color matColor = material.color;

    // Add ambient once, regardless of the number of lights.
    Color color = material.ka * matColor;
    Point hit1 = hit + epsilon * shadingN, hit2 = hit - epsilon * shadingN;

    // Add diffuse and specular components.
    for (auto const &light : lights) {
        Vector L = (light->position - hit).normalized();

        if (renderShadows) {
            Vector shadowD = light->position - hit1;
            Ray shadow(hit1, shadowD.normalized());
            if (castRay(shadow).second.t < shadowD.length()) {
                continue;
            }
        }

        // Add diffuse.
        double diffuse = std::max(shadingN.dot(L), 0.0);
        color += diffuse * material.kd * light->color * matColor;

        // Add specular.
        Vector reflectDir = reflect(-L, shadingN);
        double specAngle = std::max(reflectDir.dot(V), 0.0);
        double specular = std::pow(specAngle, material.n);

        color += specular * material.ks * light->color;
    }

    if (depth > 0 and material.isTransparent) {
        // The object is transparent, and thus refracts and reflects light.
        // Use Schlick's approximation to determine the ratio between the two.
        double cosi = (N.normalized()).dot(ray.D.normalized()), kr;
        double scalar = 1, nt = material.nt, ni = scalar / nt;
        Vector shadeN = shadingN.normalized();
        Vector reflectionColor, refractionColor;
        Vector T, R = reflect(ray.D.normalized(), shadingN.normalized());
        if (cosi < 0) { cosi = -cosi; }
        else {
            swap(scalar, nt);
            shadeN = -shadingN.normalized();
        }
        double k = 1 - pow(ni, 2) * (1 - pow(cosi, 2));
        if (k < 0) {
            T = Triple(0, 0, 0);
        } else {
            T = ni * ray.D + (ni * cosi - sqrt(k)) * shadeN;
        }
        double sint = scalar / nt * sqrt(max(0.0, 1 - pow(cosi, 2)));
        if (sint >= 1) {
            kr = 1;
        } else {
            cosi = fabs(cosi);
            double Rs = ((nt * cosi) - (scalar * sqrt(max(0.0, 1 - pow(sint, 2))))) /
                        ((nt * cosi) + (scalar * sqrt(max(0.0, 1 - pow(sint, 2)))));
            double Rp = ((scalar * cosi) - (nt * sqrt(max(0.0, 1 - pow(sint, 2))))) /
                        ((scalar * cosi) + (nt * sqrt(max(0.0, 1 - pow(sint, 2)))));
            kr = (pow(Rs, 2) + pow(Rp, 2)) / 2;
        }
        double kt = 1 - kr;
        if ((kr < 1)&&(kt > 0)) {
            Ray refractShadow(shadingN.normalized().dot(ray.D.normalized()) < 0 ? hit - (shadingN*epsilon).normalized() : hit + (shadingN*epsilon).normalized(), T.normalized());
            refractionColor = trace(refractShadow, depth - 1);
        }
        Ray reflectShadow(shadingN.normalized().dot(ray.D.normalized()) < 0 ? hit + (shadingN*epsilon).normalized() : hit - (shadingN*epsilon).normalized(), R.normalized());
        reflectionColor = trace(reflectShadow, depth - 1);
        color += reflectionColor * kr + refractionColor * kt;
    } else if (depth > 0 and material.ks > 0.0) {
        // The object is not transparent, but opaque.
        Vector R = reflect(ray.D, shadingN);
        Ray reflectShadow(hit1, R);
        Color tmpColor = material.ks * trace(reflectShadow, depth - 1);
        color += tmpColor;
    }

    return color;
}

void Scene::render(Image &img) {
    unsigned w = img.width();
    unsigned h = img.height();

    for (unsigned y = 0; y < h; ++y)
        for (unsigned x = 0; x < w; ++x) {
            Point pixel(x + 0.5, h - 1 - y + 0.5, 0);
            Ray ray(eye, (pixel - eye).normalized());
            Color col = trace(ray, recursionDepth);
            col.clamp();
            img(x, y) = col;
        }
}

//void fresnel()

// --- Misc functions ----------------------------------------------------------

// Defaults
Scene::Scene()
        :
        objects(),
        lights(),
        eye(),
        renderShadows(false),
        recursionDepth(0),
        supersamplingFactor(1) {}

void Scene::addObject(ObjectPtr obj) {
    objects.push_back(obj);
}

void Scene::addLight(Light const &light) {
    lights.push_back(LightPtr(new Light(light)));
}

void Scene::setEye(Triple const &position) {
    eye = position;
}

unsigned Scene::getNumObject() {
    return objects.size();
}

unsigned Scene::getNumLights() {
    return lights.size();
}

void Scene::setRenderShadows(bool shadows) {
    renderShadows = shadows;
}

void Scene::setRecursionDepth(unsigned depth) {
    recursionDepth = depth;
}

void Scene::setSuperSample(unsigned factor) {
    supersamplingFactor = factor;
}
