# Prochaines étapes - Objectif 7.3 (Types de lumière)

## ✅ Ce qui a été fait

1. **Structure `Material`** : Ajout du booléen `isLightSource` dans [Renderable.hpp:34](include/Components/Renderable.hpp#L34)
2. **Interface UI** : Ajout de la checkbox "Projette de la lumière" dans [MaterialPanel.cpp:579-598](src/UI/MaterialPanel.cpp#L579-L598)

## 🚧 Ce qui reste à faire

Pour que les matériaux émissifs avec `isLightSource = true` éclairent réellement les autres objets, il faut :

### 1. Système de collecte des sources de lumière émissives

Créer une fonction dans `RenderSystem` qui :
- Parcourt toutes les entités avec composant `Renderable`
- Collecte celles dont `material->isLightSource == true` et `emissiveReflection > 0.0`
- Extrait leur position (centre du mesh) et leur couleur émissive

### 2. Multi-pass lighting dans les shaders

Actuellement, les shaders d'illumination (lambert, phong) supportent **une seule lumière** :
```glsl
uniform vec3 lightPosition;
uniform vec3 lightColor;
uniform float lightIntensity;
```

**Options :**

#### Option A : Multi-lights avec uniform arrays (simple)
Modifier les shaders pour accepter plusieurs lumières :
```glsl
#define MAX_LIGHTS 8
uniform int numLights;
uniform vec3 lightPositions[MAX_LIGHTS];
uniform vec3 lightColors[MAX_LIGHTS];
uniform float lightIntensities[MAX_LIGHTS];
```

Puis boucler sur les lumières :
```glsl
for (int i = 0; i < numLights; i++) {
    vec3 L = normalize(lightPositions[i] - vPosition);
    float diff = max(dot(N, L), 0.0);
    diffuse += lightColors[i] * diff * lightIntensities[i] * diffuseReflection * baseColor;
    // ... pareil pour specular
}
```

#### Option B : Additive blending (plus complexe)
Rendre la scène en plusieurs passes, une par lumière, avec blending additif.

### 3. Types de lumières (7.3)

L'objectif 7.3 demande **4 types de lumières** :
- **Ambiante** : Déjà implémentée (`ambientColor`)
- **Directionnelle** : Lumière parallèle (comme le soleil)
- **Ponctuelle** : Lumière qui émane d'un point (ce que feront les émissifs)
- **Spot** : Lumière conique avec direction et angle

**Structure suggérée** :
```cpp
enum class LightType {
    AMBIENT,
    DIRECTIONAL,
    POINT,
    SPOT
};

struct Light {
    LightType type;
    glm::vec3 position;      // Pour POINT et SPOT
    glm::vec3 direction;     // Pour DIRECTIONAL et SPOT
    glm::vec3 color;
    float intensity;
    float spotAngle;         // Pour SPOT uniquement
    float attenuation;       // Pour POINT et SPOT
};
```

### 4. Implémentation recommandée

**Phase 1** : Lumières statiques (plus simple)
- Créer un composant `LightSource` avec les propriétés ci-dessus
- Permettre à l'utilisateur de créer des lumières manuellement
- Les matériaux émissifs avec `isLightSource=true` créent automatiquement un `LightSource` de type POINT

**Phase 2** : Atténuation et ombres (optionnel)
- Ajouter l'atténuation de distance pour les lumières ponctuelles
- Implémenter le cone angle pour les spots

## 📝 Notes

- Les lumières émissives créées via les matériaux seront de type **POINT** (ponctuelle)
- Leur intensité sera calculée à partir de `emissiveReflection.x` (ou moyenne RGB)
- Leur couleur sera `emissiveReflection * baseColor`
- Leur position sera le centre du bounding box du mesh

## 🎯 Résultat attendu

Une fois implémenté, un cube avec matériau "Emissive" et checkbox "Projette de la lumière" cochée devrait :
1. Apparaître lumineux lui-même (déjà fonctionnel via emissive)
2. Éclairer les objets voisins avec sa couleur émissive (nouveau comportement)

Exemple : Un cube rouge émissif près d'un cube métallique devrait projeter une lueur rouge sur le métal.
