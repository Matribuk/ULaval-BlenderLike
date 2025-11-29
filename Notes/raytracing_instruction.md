# Raytracing System - Documentation

## Architecture Overview

Le système de raytracing est un système **hybride** :
- Rasterization pour le rendu de base des meshes
- Raytracing GPU pour les ombres et reflections (optionnel)

## Comment ça marche

### 1. Collection des primitives (CPU - RenderSystem.cpp)

Dans `RenderSystem::_setPrimitiveUniforms()` :
- Parcourt toutes les entités
- Collecte les composants Sphere, Box, Plane
- Envoie au shader via uniform arrays :
  ```cpp
  uniform int primitiveTypes[MAX_PRIMITIVES];      // 0=sphere, 1=box, 2=plane
  uniform vec3 primitivePositions[MAX_PRIMITIVES];
  uniform vec3 primitiveParam1[MAX_PRIMITIVES];    // radius/dimensions/size
  uniform vec3 primitiveParam2[MAX_PRIMITIVES];    // normal pour plane
  uniform vec4 primitiveColors[MAX_PRIMITIVES];
  uniform float primitiveReflectivities[MAX_PRIMITIVES];
  ```

### 2. Intersection ray-primitive (GPU - shader)

Trois fonctions d'intersection :
- `intersectSphere(Ray, center, radius, out t)` - Équation quadratique
- `intersectBox(Ray, center, dimensions, out t)` - AABB intersection
- `intersectPlane(Ray, center, normal, size, out t)` - Test plan + bounds

### 3. Ray tracing (GPU - shader)

`traceRay(Ray, skipPrimitive)` :
- Boucle sur tous les primitives
- Teste intersection avec chaque primitive
- Retourne le hit le plus proche (plus petit t)
- Calcule la normale au point d'impact

### 4. Raytraced Shadows

`calculateRaytracedShadow(fragPos, lightDir, maxDist)` :
- Crée un shadow ray depuis le fragment
- Offset de 0.1 le long de la normale (évite self-intersection)
- Si intersection avant la lumière → ombre (return 0.0)
- Sinon → pas d'ombre (return 1.0)

### 5. Raytraced Reflections

`calculateRaytracedReflection(fragPos, viewDir, normal, bounces)` :
- Boucle itérative (max 3 bounces)
- À chaque bounce :
  - Lance un rayon réfléchi
  - Si hit → calcule couleur avec éclairage
  - Accumule la couleur avec poids décroissant
  - Continue si reflectivity > 0

### 6. Intégration dans le shader

Dans la boucle de lumières :
```glsl
if (raytracingEnabled == 1 && raytracedShadows == 1) {
    shadow = calculateRaytracedShadow(...);
} else {
    // Shadow maps classiques
    shadow = calculateShadow(...);
}
```

Après le calcul Phong :
```glsl
if (raytracingEnabled == 1 && raytracedReflections == 1) {
    vec3 reflectionColor = calculateRaytracedReflection(...);
    finalColor = mix(phongColor, reflectionColor, reflectivity);
}
```

## Contrôles UI (RaytracingPanel)

- **Enable Raytracing** : Active le système
- **Shadow Mode** : Shadow Maps vs Raytraced Shadows
- **Raytraced Reflections** : Active/désactive les reflections
- **Max Ray Bounces** : 1-3 bounces pour les reflections

## Uniforms requis dans TOUS les shaders qui supportent raytracing

```glsl
#define MAX_PRIMITIVES 32

// Raytracing control
uniform int raytracingEnabled;
uniform int raytracedShadows;
uniform int raytracedReflections;
uniform int maxRayBounces;

// Primitive data
uniform int numPrimitives;
uniform int primitiveTypes[MAX_PRIMITIVES];
uniform vec3 primitivePositions[MAX_PRIMITIVES];
uniform vec3 primitiveParam1[MAX_PRIMITIVES];
uniform vec3 primitiveParam2[MAX_PRIMITIVES];
uniform vec4 primitiveColors[MAX_PRIMITIVES];
uniform float primitiveReflectivities[MAX_PRIMITIVES];
```

## État actuel de l'implémentation

### Shaders avec support raytracing :
- ✅ raytraced_phong.frag - Support complet

### Shaders sans support raytracing :
- ❌ phong_shadow.frag - Manque raytracing
- ❌ reflective_phong.frag - Manque raytracing
- ❌ lambert.frag - Manque raytracing (si existe)

---

## Plan d'implémentation

### Étape 1 : Vérifier l'implémentation actuelle
- [ ] Tester raytraced_phong avec raytraced shadows
- [ ] Tester raytraced_phong avec raytraced reflections
- [ ] Vérifier que les 3 types de primitives fonctionnent (sphere, box, plane)

### Étape 2 : Ajouter raytracing à phong_shadow.frag
- [ ] Copier les structs Ray et HitInfo
- [ ] Copier les 3 fonctions d'intersection
- [ ] Copier traceRay()
- [ ] Copier calculateRaytracedShadow()
- [ ] Copier calculateRaytracedReflection()
- [ ] Ajouter les uniforms raytracing
- [ ] Modifier la boucle de lumières pour supporter raytraced shadows
- [ ] Tester

### Étape 3 : Ajouter raytracing à reflective_phong.frag
- [ ] Même processus que phong_shadow.frag
- [ ] Intégrer avec le système de cubemap reflection existant
- [ ] Tester

### Étape 4 : Optionnel - lambert.frag
- [ ] Ajouter support si le shader existe

---

## Notes techniques

### Self-intersection
Problème : Un rayon peut re-intersecter sa propre surface
Solution : Offset de 0.1 le long de la normale
```glsl
ray.origin = fragPos + normal * 0.1;
```

### Epsilon dans les tests d'intersection
Pour éviter les intersections à t=0 (point de départ) :
```glsl
t = t1 > 0.01 ? t1 : t2;
return t > 0.01;
```

### Limitation GLSL 120
Pas de récursion → utiliser boucle itérative pour les reflections multiples

### Performance
- MAX_PRIMITIVES = 32 (configurable dans RenderSystem.hpp)
- Les raytraced shadows testent TOUS les primitives
- Les raytraced reflections font jusqu'à 3 bounces max
