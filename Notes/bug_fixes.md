# Bug Fixes - Session du 29 Nov

## Problème 1 : Les shaders se réinitialisent à lambert ✅ FIXÉ

### Cause
`PrimitiveSystem.cpp` ligne 47-49 écrasait TOUJOURS le shader avec lambert, même si l'entité en avait déjà un.

### Fix
```cpp
// AVANT (MAUVAIS)
if (render->material && this->_resourceManager) {
    render->material->illuminationShader = this->_resourceManager->getDefaultIlluminationShader();
}

// APRÈS (BON)
if (render->material && this->_resourceManager && !render->material->illuminationShader) {
    render->material->illuminationShader = this->_resourceManager->getDefaultIlluminationShader();
}
```

Maintenant lambert n'est appliqué QUE si aucun shader n'existe déjà.

**Fichier modifié :** `src/Systems/PrimitiveSystem.cpp:47`

---

## Problème 2 : Crash filesystem quand on clique sur shader ✅ FIXÉ

### Cause
`std::filesystem::directory_iterator` peut throw une exception si le working directory est invalide ou a changé.

### Fix
Ajout de try-catch autour de tous les `directory_iterator` dans MaterialPanel :

```cpp
try {
    if (std::filesystem::exists(shaderDir) && std::filesystem::is_directory(shaderDir)) {
        for (std::filesystem::directory_entry entry : std::filesystem::directory_iterator(shaderDir)) {
            // ...
        }
    }
} catch (const std::filesystem::filesystem_error& e) {
    ofLogError("MaterialPanel") << "Filesystem error: " << e.what();
}
```

**Fichiers modifiés :**
- `src/UI/MaterialPanel.cpp:118-133` (_loadIlluminationShader)
- `src/UI/MaterialPanel.cpp:166-180` (_loadEffectShader)
- `src/UI/MaterialPanel.cpp:532-544` (_renderNormalMapSelector)
- `src/UI/MaterialPanel.cpp:615-626` (_renderHeightMapSelector)

---

## Problème 3 : Crash avec 2 entités + raytracing ⚠️ À INVESTIGUER

### Symptômes
- Créer 2 entités
- Activer raytracing
- Application crash

### Hypothèses possibles

1. **Array out of bounds dans le shader**
   - numPrimitives dépasse MAX_PRIMITIVES (32)
   - Vérifier dans la console combien de primitives sont envoyées

2. **Problème avec les uniforms**
   - Les arrays de primitives ne sont pas bien initialisés
   - Vérifier que `_setPrimitiveUniforms()` ne dépasse pas 32

3. **Self-intersection ou infinite loop**
   - Le rayon se bloque dans une boucle infinie
   - Vérifier les epsilon values dans le shader

### Tests à faire

1. **Vérifier le nombre de primitives :**
   - Ajouter un log dans `RenderSystem::_setPrimitiveUniforms()` :
     ```cpp
     ofLogNotice("RenderSystem") << "Sending " << numPrimitives << " primitives to shader";
     ```

2. **Tester avec 1 seule primitive :**
   - Créer 1 sphere avec raytracing → OK ?
   - Créer 2e sphere → crash ?
   - Ou créer 1 sphere + 1 box → crash ?

3. **Tester sans raytracing :**
   - Créer 2 entités SANS raytracing → OK ?
   - Activer raytracing après → crash ?

4. **Vérifier les logs OpenGL :**
   - Chercher "UNSUPPORTED" ou "GLD_TEXTURE" dans la console
   - L'erreur "unit 0 GLD_TEXTURE_INDEX_2D is unloadable" peut indiquer un problème de texture

### Prochaines étapes

1. Lance l'app et teste la séquence exacte :
   - Quelle primitive crées-tu ? (sphere, box, plane ?)
   - Quel shader mets-tu ?
   - Quand crashes-tu exactement ?

2. Regarde la console pour les messages d'erreur

3. Si possible, partage un screenshot ou la stack trace du crash

---

## Problème 3 : Crash/freeze quand on drag & drop un modèle 3D ✅ FIXÉ

### Cause
Les meshes importés n'avaient pas de normales valides après le scaling des vertices. Sans normales, les shaders d'illumination (lambert, phong, etc.) crashent ou rendent n'importe quoi.

### Fix
Ajout d'une vérification et génération de normales basiques si nécessaires :

```cpp
// Après le scaling des vertices (ligne 58-63)
if (mesh.getNumNormals() == 0 || mesh.getNumNormals() != mesh.getNumVertices()) {
    mesh.clearNormals();
    for (size_t i = 0; i < mesh.getNumVertices(); i++) {
        mesh.addNormal(glm::vec3(0, 1, 0));
    }
}
```

**Note :** Les normales générées sont basiques (tous pointent vers le haut). Pour un meilleur rendu, il faudrait calculer les normales par face, mais ça suffit pour éviter le crash.

**Fichier modifié :** `src/Manager/FileManager.cpp:58-63`

---

## Résumé des changements

### Fichiers modifiés :
1. `src/Systems/PrimitiveSystem.cpp` - Fix shader reset à lambert
2. `src/UI/MaterialPanel.cpp` - Fix filesystem crashes (4 endroits)
3. `src/Manager/FileManager.cpp` - Fix crash drag & drop modèles 3D

### Tests réussis :
✅ Les shaders persistent quand on crée de nouvelles primitives
✅ Plus de crash filesystem quand on clique sur "Load Shader"
✅ Raytraced shadows fonctionnent
✅ Raytraced reflections fonctionnent
⏳ À tester : Drag & drop de modèles 3D

### Prochaines étapes :
1. Tester le drag & drop de modèles 3D
2. Si stable, ajouter raytracing aux autres shaders (phong_shadow, reflective_phong)
