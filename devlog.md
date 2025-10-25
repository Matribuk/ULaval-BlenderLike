# DevLog - IFT3100A25_TP1_E02

**Equipe E02** | **Projet**: Editeur de scene 3D avec architecture ECS
**Periode**: 22 septembre 2025 -> 24 octobre 2025

---

## Semaine 1 (22-29 septembre 2025)

### Killian Cottrelle
- **Setup initial du projet** - Creation de la structure de base du projet avec openFrameworks
- **Architecture ECS Phase 1 & 2** - Implementation complete de l'Entity Component System:
  - EntityManager avec gestion d'IDs uniques et creation/destruction d'entites
  - ComponentRegistry avec systeme de templates pour registre/get/remove des composants
  - Components de base: Transform (position, rotation, scale), Renderable, Camera, Selectable

### Antonin Leprest
- **Configuration compilation** - Setup du .env pour le path openFrameworks et configuration Makefile
- **Fix compilation** - Correction des declarations de classes et erreurs de compilation

### Clement Barrier
- **Fix configuration projet** - Resolution des issues de compilation initiales
- **Preparation architecture** - Analyse et design des systemes a venir

### Leandre Cacarie
- **Tests unitaires** - Creation de tests pour valider EntityManager et ComponentRegistry

### Marion Kauffmann
- **Systeme d'evenements complet**:
  - EventTypes avec enum pour INPUT, SELECTION, CAMERA
  - EventManager avec map de callbacks, queue d'evenements, subscribe/emit
  - EventBridge pour integration avec openFrameworks (clavier, souris, fenetre)

**Metriques**: 15 commits | 3 PRs mergees

---

## Semaine 2 (30 septembre - 6 octobre 2025)

### Clement Barrier
- **CameraSystem complet**:
  - Implementation des mouvements: orbit, pan, zoom, focus
  - Gestion des inputs clavier/souris avec delta time
  - Integration avec CameraManager pour multi-cameras
- **CameraManager** - Gestion de plusieurs cameras avec switch entre cameras actives
- **Architecture viewport** - Refactoring pour lier cameras aux viewports via Transform

### Killian Cottrelle
- **ActionManager**:
  - Gestion correcte des inputs avec historique d'actions
- **FileManager**:
  - Import/export de meshes 3D (OBJ, PLY, STL)
  - Gestion des erreurs et validation des formats
- **UI Panels de base**:
  - Toolbar, Viewport, ColorPalette
  - Integration ImGui dans le projet

### Antonin Leprest
- **TransformSystem**:
  - Calcul des matrices de transformation
  - Gestion du dirty flag pour optimisation
  - Controle manuel des rotations pour entites selectionnees
- **PropertiesPanel**:
  - Panel UI pour editer les proprietes des entites
  - Affichage et modification des Transform
  - Interface popup et informations additionnelles

### Leandre Cacarie
- **Refactoring structure projet**:
  - Reorganisation: include files dans /include, chemins relatifs
  - Nettoyage des variables inutiles et defines
  - Amelioration de la structure de compilation
- **Optimisations** - amelioration de la gestion des cameras

### Marion Kauffmann
- **ResourceManager** - Implementation complete du systeme de gestion de ressources:
  - Cache de ressources (meshes, textures, shaders)
  - Methodes loadMesh(), loadTexture(), loadShader()
  - Reference counting pour gestion memoire
  - Nettoyage automatique des ressources inutilisees

**Metriques**: 57 commits | 12 PRs mergees

---

## Semaine 3 (7-13 octobre 2025)

### Clement Barrier
- **SelectionSystem avec raycast AABB**:
  - Implementation complete du raycast avec intersection ray-AABB
  - Calcul et transformation des bounding boxes
  - Affichage des bounding boxes sur entites selectionnees
  - Gestion du mode selection (activable/desactivable)
- **Focus automatique**:
  - Vue recentree automatique sur selection
  - Cadrage optimal pour voir tout le contenu
  - Modes perspective et orthographique
- **CursorManager** (commits semaine 3):
  - 5 types de curseurs dynamiques (arrow, ibeam, crosshair, hand, resize H)
  - Systeme de layers pour priorite des curseurs

### Killian Cottrelle
- **Multi-selection**:
  - Selection multiple avec Ctrl+clic
  - Toggle pour ajouter/retirer de la selection
- **Primitives geometriques 3D**:
  - Generation procedurale de cubes et spheres
  - Algorithmes sans donnees externes
- **Graphe de scene parent-enfant**:
  - Hierarchie d'entites avec parent/child
  - Transformations et suppressions en cascade
  - Fix des matrices globales

### Antonin Leprest
- **Optimisations**:
  - Amelioration de la queue d'evenements dans EventManager::processEvents()
  - Meilleure utilisation de _fileManager avant initialisation dans ofApp::setup()
- **ExportPanel**:
  - Export de sequences d'images
  - Configuration FPS et duree
  - Sauvegarde en memoire puis export final
- **Integration MaterialPanel**:
  - Integration du MaterialPanel de Marion dans l'architecture
  - Gestion des includes et dependencies
- **Fix TransformSystem**:
  - Panel R/S avec degres et pourcentages
  - Corrections du panel Transform
  - Amelioration de l'UI
- **Gestion texture**:
  - Application de textures par drag & drop

### Marion Kauffmann
- **ResourceManager** - Implementation complete du systeme de gestion de ressources:
  - Cache de ressources (meshes, textures, shaders)
  - Methodes loadMesh(), loadTexture(), loadShader()
  - Reference counting pour gestion memoire optimale
  - Nettoyage automatique des ressources inutilisees
- **MaterialPanel**:
  - Panel pour charger mesh, textures, shaders depuis fichiers
  - Boutons de chargement pour chaque type de ressource
  - Fonctions helper pour lisibilite du code
- **Fix texture loading** - Correction du chargement de textures sur primitives (generation de primitives supprimait la texture)

### Leandre Cacarie
- **Review & QA** - Reverts de PRs problematiques pour maintenir la stabilite
- **MVP Light & Sky**:
  - Systeme d'eclairage de base
  - Skybox
- **Assets Management**:
  - AssetsPanel avec auto-load du dossier ./data/
  - Bouton d'import pour creer des assets
  - Systeme de drag & drop d'assets sur la scene
  - Import/export d'images interactif

**Metriques**: 92 commits | 20 PRs mergees

---

## Semaine 4 (14-20 octobre 2025)

### Antonin Leprest
- **Color Palette saved colors** - Ajout de la fonctionnalite de sauvegarde de couleurs dans la palette (1 commit le 14 octobre)

**Tous les autres membres:**
*Pause dans le developpement - Aucune activite git*

**Metriques**: 1 commit | 0 PRs mergees

---

## Semaine 5 (21-24 octobre 2025)

### Antonin Leprest
- **EyedropperSystem (Pipette couleur)**:
  - Systeme de pipette pour echantillonner les couleurs des entites
  - Preview en temps reel lors du survol
  - Application de couleur par clic
  - Annulation par clic dans le vide

### Clement Barrier - **3 commits**
- **Primitives vectorielles 2D**:
  - Implementation de 5 primitives 2D: Rectangle, Point, Line, Triangle, Circle
  - Generation de mesh 2D avec vertices
  - Ajustement de la taille des points par defaut

### Killian Cottrelle
- **CameraManager improvements** (commits du 24 oct):
  - Ajout d'un bouton "Create Camera" dans l'UI
  - Systeme de creation de cameras infinies
  - Auto-creation d'une camera de remplacement si la derniere est supprimee
  - Synchronisation de la logique import/export

### Leandre Cacarie
- **Refactoring SelectionSystem** - **Elimination duplication de code**:
  - Creation d'un systeme de filtre generique pour le raycast
  - SelectionSystem utilise filtre "Selectable"
  - EyedropperSystem utilise filtre "Renderable"
- **Review & QA** - Review des PR du projet

### Marion Kauffmann
- **AABB Helpers** - Modification des methodes AABB en helpers pour reutilisation

**Metriques**: 20 commits | 2 PRs mergees

---

## Statistiques globales du projet

| Membre                          | Commits | PRs mergees          | Role principal                      |
|---------------------------------|---------|----------------------|-------------------------------------|
| **Clement Barrier**             | 78      | 11                   | Architecture & Core Systems         |
| **Killian Cottrelle**           | 66      | 13                   | Features & Primitives               |
| **Antonin Leprest**             | 31      | 10                   | UI/UX, Integration & Bugfixes       |
| **Marion Kauffmann**            | 18      | 7                    | Material System                     |
| **Leandre Cacarie**             | 20      | 6                    | Refactoring & Review Lead           |
| **TOTAL**                       | **213** | **47**               | -                                   |

---

## Resume des accomplissements par membre

### Clement Barrier
- **SelectionSystem** avec raycast AABB complet
- **CameraSystem** avec orbit/pan/zoom/focus
- **Focus automatique** sur selection
- **5 primitives** (2D: Rectangle, Point, Line, Triangle, Circle)
- **Bounding boxes** visibles
- **Architecture** UIManager clean
- **CursorManager** (5 curseurs dynamiques)

### Killian Cottrelle
- **Architecture ECS** complete (EntityManager, ComponentRegistry, Components)
- **FileManager** (import/export meshes)
- **Multi-selection** avec Ctrl
- **Primitives geometriques** 3D procedurales
- **Graphe de scene** avec hierarchie parent-enfant
- **Multi-cameras** avec creation dynamique
- **3 primitives** (3D: Box, Sphere, Plane)

### Antonin Leprest
- **4 UI Panels**: Properties, Assets, Export, Color
- **TransformSystem** avec dirty flags et controle manuel rotation
- **Export de sequences d'images** avec configuration FPS/duree
- **Color Palette** avec sauvegarde de couleurs
- **EyedropperSystem** (pipette couleur) avec preview temps reel
- **AABB Helpers** publics statiques reutilisables
- **Integration**: MaterialPanel de Marion, coordination entre systemes

### Marion Kauffmann
- **ResourceManager complet**:
  - Cache de ressources (meshes, textures, shaders)
  - loadMesh(), loadTexture(), loadShader() avec reference counting
  - Nettoyage automatique des ressources inutilisees
- **MaterialPanel complet**:
  - Panel pour charger mesh, textures, shaders depuis fichiers
- **Systeme d'evenements** (EventManager, EventBridge)

### Leandre Cacarie
- **Tests unitaires** - Creation de tests pour valider EntityManager et ComponentRegistry
- **Refactoring** structure projet
- **Review** de 19 PRs (leader en review)
- **Maintien** de la stabilite du projet
- **Optimisations** - Nettoyage du code et amelioration de la gestion des cameras
- **Light & Sky** system
- **Assets Management** avec drag & drop, auto-load, import/export images

---

## Criteres TP1 implementes

### Image (3/5)
- 1.1 Importation d'images
- 1.2 Exportation d'images
- 1.4 Palette de couleur

### Dessin vectoriel (3/5)
- 2.1 Curseur dynamique
- 2.3 Primitives vectorielles
- 2.5 Interface

### Transformation (3/5)
- 3.1 Graphe de scene
- 3.2 Selection multiple
- 3.3 Transformations interactives

### Geometrie (3/5)
- 4.1 Boites de delimitation
- 4.2 Primitives geometriques
- 4.3 Modeles 3D

### Camera (3/5)
- 5.1 Camera interactive
- 5.2 Cameras multiples
- 5.3 Points de vue multiples

**Total: 15 criteres sur 25**

---

## Notes importantes

### Qualite du code
- **Architecture professionnel** - ECS bien structure, pas de couplage fort
- **Hotfixes reactifs** - Memory leaks et bugs critiques corriges rapidement
- **Review actif** - Leandre a assure 19 merges avec QA

### Conformite TP1
- **Criteres fonctionnels** - 15/25 (objectif 15)
- **Document de design** - En cours de finalisation
- **Video** - A produire
- **DevLog** - Complete (ce document)

---

*DevLog maintenu conformement aux exigences de l'enonce TP1*
*Derniere mise a jour: 24 octobre 2025*
