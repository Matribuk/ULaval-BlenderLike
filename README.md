# TODO List - Projet ECS openFrameworks

## 📁 Phase 1: Structure de base et Types fondamentaux

### Core/Entity.h

- [x] Créer type `EntityID` (typedef uint32_t)
- [x] Créer classe `Entity` avec ID unique
- [x] Méthode `getId()` const
- [x] Opérateurs de comparaison (==, <)
- [x] Constructeur/destructeur

### Core/ComponentRegistry.h

- [x] Map pour stocker les composants par type
- [x] Template `registerComponent<T>(EntityID)`
- [x] Template `getComponent<T>(EntityID)`
- [x] Template `removeComponent<T>(EntityID)`
- [x] Template `hasComponent<T>(EntityID)`
- [x] Méthode `removeAllComponents(EntityID)`
- [x] Destructeur avec cleanup

### Core/EntityManager.h

- [x] Générateur d'IDs uniques
- [x] Vector des entités actives
- [x] Méthode `createEntity()` → EntityID
- [x] Méthode `destroyEntity(EntityID)`
- [x] Méthode `isEntityValid(EntityID)`
- [x] Méthode `getAllEntities()`

## 📁 Phase 2: Composants de base

### Components/Transform.h

- [x] `glm::vec3 position`
- [x] `glm::vec3 rotation`
- [x] `glm::vec3 scale`
- [x] Constructeur par défaut
- [x] TransformSystem calcul les matrices

### Components/Renderable.h

- [x] `ofMesh mesh`
- [x] `ofColor color`
- [x] `bool visible`
- [x] Constructeur par défaut
- [x] RenderSystem manipule le mesh

### Components/Camera.h

- [x] `glm::vec3 target`
- [x] `glm::vec3 position`
- [x] `float fov`
- [x] `float nearClip, farClip`
- [x] CameraSystem calcule les matrices

### Components/Selectable.h

- [x] `bool isSelected`
- [x] `ofColor selectedColor`
- [x] `ofColor normalColor`
- [x] Constructeur par défaut

### Components/Primitive/**.h

- [x] Créer un composant Box avec glm::vec3 dimensions
- [x] Créer un composant Sphere avec float radius
- [x] Créer un composant Plane avec glm::vec2 size
- [x] PrimitiveSystem génère les meshes à partir des composants correspondants

## 📁 Phase 3: Système d'événements

### Events/EventTypes.h

- [x] Enum Class `EventType` (INPUT, SELECTION, CAMERA, etc.)
- [x] Struct de base `Event` avec type et timestamp
- [X] Structs spécialisés:
  - [x] `MouseEvent` (x, y, button, type)
  - [x] `KeyEvent` (key, type)
  - [x] `SelectionEvent` (entityID, selected)
  - [X] `CameraEvent` (position, target, type)

### Events/EventManager.h

- [x] Map de callbacks par type d'événement
- [x] Queue d'événements à traiter
- [x] Template `subscribe<T>(callback)`
- [x] Template `unsubscribe<T>(callback)`
- [x] Template `emit<T>(event)`
- [x] Méthode `processEvents()` (vidage de la queue)

### Events/EventBridge.h

- [x] Référence vers EventManager
- [x] Constructeur avec EventManager*
- [x] `onKeyPressed(int key)`
- [x] `onKeyReleased(int key)`
- [x] `onMousePressed(int x, int y, int button)`
- [x] `onMouseReleased(int x, int y, int button)`
- [x] `onMouseMoved(int x, int y)`
- [x] `onMouseDragged(int x, int y, int button)`
- [x] `onMouseScrolled(int x, int y, float sx, float sy)`
- [x] `onWindowResized(int w, int h)`
- [x] `onDragEvent(ofDragInfo dragInfo)`

## 📁 Phase 4: Systèmes logiques

### Systems/RenderSystem.h

- [x] Référence vers ComponentRegistry
- [x] Référence vers caméra active
- [x] Méthode `render()`
- [x] Méthode `setActiveCamera(EntityID)`
- [x] Rendu des entités avec Transform + Renderable
- [x] Gestion des materials et shaders
- [x] Culling et optimisations

### Systems/SelectionSystem.h

- [ ] Référence vers ComponentRegistry et EventManager
- [ ] Subscribe aux MouseEvents
- [ ] Méthode `handleMouseClick(MouseEvent)`
- [ ] Ray casting pour sélection 3D
- [ ] Mise à jour composants Selectable
- [ ] Émission SelectionEvents

### Systems/TransformSystem.h

- [ ] Mise à jour matrices de transformation
- [ ] Gestion hiérarchies parent/enfant
- [ ] Méthode `updateTransforms()`
- [ ] Calcul matrices globales
- [ ] Optimisations (dirty flags)

### Systems/CameraSystem.h

- [x] Subscribe aux événements caméra
- [x] Gestion multiple caméras
- [x] Méthodes de déplacement (orbit, pan, zoom)
- [ ] Méthode `updateCamera(float deltaTime)`
- [x] Contraintes de mouvement

## 📁 Phase 5: Managers globaux

### Manager/InputManager.h

- [x] État actuel clavier/souris
- [x] Historique des inputs
- [x] Méthodes `isKeyPressed(int key)`
- [x] Méthodes `getMousePosition()`
- [x] Méthodes `getMouseDelta()`
- [x] Gestion raccourcis clavier
- [x] Subscribe aux EventBridge events

### Manager/CameraManager.h

- [ ] Liste des caméras disponibles
- [ ] ID de la caméra active
- [ ] Méthodes `createCamera(EntityID)`
- [ ] Méthodes `setActiveCamera(EntityID)`
- [ ] Méthodes `getActiveCamera()`
- [ ] Mise à jour automatique aspect ratio

### Manager/HistoryManager.h

- [ ] Stack des commandes (Command Pattern)
- [ ] Méthodes `executeCommand(Command*)`
- [ ] Méthodes `undo()`
- [ ] Méthodes `redo()`
- [ ] Limite historique configurable
- [ ] Sérialisation/désérialisation états

### Manager/FileManager.h

- [ ] Méthodes `saveScene(string filename)` - optionel
- [ ] Méthodes `loadScene(string filename)` - optionel
- [x] Méthodes `exportMesh(EntityID, string filename)`
- [x] Méthodes `importMesh(string filename)` → EntityID
- [x] Support formats (OBJ, PLY, STL)
- [x] Gestion erreurs et validations

### Manager/ResourceManager.h

- [ ] Cache des ressources (meshes, textures, shaders)
- [ ] Méthodes `loadMesh(string path)`
- [ ] Méthodes `loadTexture(string path)`
- [ ] Méthodes `loadShader(string path)`
- [ ] Reference counting
- [ ] Nettoyage automatique ressources inutilisées

## 📁 Phase 6: Interface utilisateur

### UI/ToolBar.h

- [x] Liste des outils disponibles
- [x] Outil actuellement sélectionné
- [x] Méthodes `addTool(Tool)`
- [x] Méthodes `selectTool(ToolType)`
- [x] Méthodes `render()`
- [ ] Gestion événements clic outils

### UI/ColorPalette.h

- [x] Couleur actuellement sélectionnée
- [x] Palette de couleurs prédéfinies
- [x] Méthodes `setSelectedColor(ofColor)`
- [x] Méthodes `getSelectedColor()`
- [x] Méthodes `render()`
- [ ] Interface picker couleur

### UI/Properties.h

- [ ] Affichage propriétés entité sélectionnée
- [ ] Champs éditables pour Transform
- [ ] Champs éditables pour Material
- [ ] Méthodes `setSelectedEntity(EntityID)`
- [ ] Méthodes `render()`
- [ ] Validation et application changements

### UI/Viewport.h

- [x] Zone de rendu 3D principal
- [x] Gestion resize
- [ ] Overlays (gizmos)
- [x] Overlays (grid, axes)
- [x] Méthodes `render()`
- [x] Conversion coordonnées écran ↔ 3D
- [x] Gestion multi-viewport

## 📁 Phase 7: Core Systems

### Core/SystemManager.h

- [ ] Liste de tous les systèmes
- [ ] Ordre d'exécution des systèmes
- [ ] Méthodes `registerSystem<T>()`
- [ ] Méthodes `updateSystems(float deltaTime)`
- [ ] Méthodes `renderSystems()`
- [ ] Gestion activation/désactivation systèmes

### Core/SceneManager.h (World/Scene Manager)

- [ ] Référence vers tous les managers
- [ ] Méthodes `initialize()`
- [ ] Méthodes `update(float deltaTime)`
- [ ] Méthodes `render()`
- [ ] Méthodes `cleanup()`
- [ ] Méthodes `loadScene(string name)`
- [ ] Méthodes `saveCurrentScene()`
- [ ] Gestion états de l'application

## 📁 Phase 8: Intégration et finitions

### ofApp.h/cpp (déjà fait)

- [x] Intégration des managers principaux
- [x] Relais événements vers EventBridge
- [x] Cycle update/render

### Tests et optimisations

- [ ] Tests unitaires composants de base
- [ ] Tests systèmes de rendu
- [ ] Tests sélection/interaction
- [ ] Profiling et optimisations performance
- [ ] Gestion mémoire et fuites
- [ ] Tests sur différentes plateformes

### Documentation

- [ ] Documentation API des composants
- [ ] Guide d'utilisation
- [ ] Exemples d'extension du système
- [ ] Diagrammes d'architecture mis à jour

### Fonctionnalités avancées (optionnel)

- [ ] Système de plugins
- [ ] Scripting (Lua/Python)
- [ ] Networking pour collaboration
- [ ] Animation et timeline
- [ ] Système de particules
- [ ] Post-processing effects

---

## 📋 Ordre de développement recommandé

1. **Phase 1** → Base solide ECS
2. **Phase 2** → Composants essentiels
3. **Phase 3** → Communication événements
4. **Phase 4** → Rendu et interactions de base
5. **Phase 7** → SceneManager (intégration)
6. **Phase 5** → Managers spécialisés
7. **Phase 6** → Interface utilisateur
8. **Phase 8** → Polish et optimisations

**Chaque phase devrait être testée avant de passer à la suivante !**
