# DevLog - IFT3100A25_TP1_E02

**Equipe E02** | **Projet**: Editeur de scene 3D avec architecture ECS
**Periode**: 22 septembre 2025 -> 24 octobre 2025

---

## Semaine 1 (22-29 septembre 2025)

### Killian Cottrelle
- **Setup initial du projet** - Creation de la structure de base du projet avec openFrameworks
- **Architecture ECS Phase 1 & 2** (PR #1, #2) - Implementation complete de l'Entity Component System:
  - EntityManager avec gestion d'IDs uniques et creation/destruction d'entites
  - ComponentRegistry avec systeme de templates pour registre/get/remove des composants
  - Components de base: Transform (position, rotation, scale), Renderable, Camera, Selectable
- **Systeme d'evenements complet** (PR #13, #17, #19):
  - EventTypes avec enum pour INPUT, SELECTION, CAMERA
  - EventManager avec map de callbacks, queue d'evenements, subscribe/emit
  - EventBridge pour integration avec openFrameworks (clavier, souris, fenetre)
- **Tests unitaires** - Creation de tests pour valider EntityManager et ComponentRegistry

### Antonin Leprest (Matribuk)
- **Configuration compilation** (PR #15) - Setup du .env pour le path openFrameworks et configuration Makefile
- **Fix compilation** (PR #20) - Correction des declarations de classes et erreurs de compilation

### Clement Barrier (Maskalito)
- **Fix configuration projet** - Resolution des issues de compilation initiales
- **Preparation architecture** - Analyse et design des systemes a venir

### Leandre Cacarie (Richon)
*Pas encore actif cette semaine*

### Marion Kauffmann
*Pas encore active cette semaine*

**Metriques**: 15 commits | 3 PRs mergees

---

## Semaine 2 (30 septembre - 6 octobre 2025)

### Clement Barrier (Maskalito) - **Lead architecture**
- **CameraSystem complet** (PR #38, #40):
  - Implementation des mouvements: orbit, pan, zoom, focus
  - Gestion des inputs clavier/souris avec delta time
  - Integration avec CameraManager pour multi-cameras
- **CameraManager** - Gestion de plusieurs cameras avec switch entre cameras actives
- **Architecture viewport** - Refactoring pour lier cameras aux viewports via Transform
- **Optimisations** - Nettoyage du code et amelioration de la gestion des cameras

### Killian Cottrelle
- **ActionManager & HistoryManager** (PR #39, #43):
  - Implementation du Command Pattern pour undo/redo
  - Gestion correcte des inputs avec historique d'actions
- **FileManager** (PR #36):
  - Import/export de meshes 3D (OBJ, PLY, STL)
  - Gestion des erreurs et validation des formats
- **UI Panels de base** (PR #28, #29, #31):
  - Toolbar, Viewport, ColorPalette
  - Integration ImGui dans le projet

### Antonin Leprest (Matribuk)
- **TransformSystem** (PR #42):
  - Calcul des matrices de transformation
  - Gestion du dirty flag pour optimisation
  - Controle manuel des rotations pour entites selectionnees
- **PropertiesPanel** (PR #46):
  - Panel UI pour editer les proprietes des entites
  - Affichage et modification des Transform
  - Interface popup et informations additionnelles

### Leandre Cacarie (Richon)
- **Refactoring structure projet** (PR #37):
  - Reorganisation: include files dans /include, chemins relatifs
  - Nettoyage des variables inutiles et defines
  - Amelioration de la structure de compilation

### Marion Kauffmann
- **ResourceManager** - Implementation complete du systeme de gestion de ressources:
  - Cache de ressources (meshes, textures, shaders)
  - Methodes loadMesh(), loadTexture(), loadShader()
  - Reference counting pour gestion memoire
  - Nettoyage automatique des ressources inutilisees

**Metriques**: 57 commits | 12 PRs mergees

---

## Semaine 3 (7-13 octobre 2025) - **SPRINT MVP**

### Clement Barrier (Maskalito) - **42 commits**
- **SelectionSystem avec raycast AABB** (PR #67, #79, #92):
  - Implementation complete du raycast avec intersection ray-AABB
  - Calcul et transformation des bounding boxes
  - Affichage des bounding boxes sur entites selectionnees
  - Gestion du mode selection (activable/desactivable)
- **Focus automatique** (PR #61, #62):
  - Vue recentree automatique sur selection
  - Cadrage optimal pour voir tout le contenu
  - Modes perspective et orthographique
- **Cleanup architecture** (PR #91):
  - Refactoring UIManager avec methode setup
  - Resolution imports circulaires
  - Clean de ofApp setup

### Killian Cottrelle - **27 commits**
- **Multi-selection** (PR #81):
  - Selection multiple avec Ctrl+clic
  - Toggle pour ajouter/retirer de la selection
- **CursorManager** (commits semaine 3):
  - 6 types de curseurs dynamiques (arrow, ibeam, crosshair, hand, resize H/V)
  - Systeme de layers pour priorite des curseurs
- **Primitives geometriques 3D** (PR #84):
  - Generation procedurale de cubes et spheres
  - Algorithmes sans donnees externes
- **Graphe de scene parent-enfant** (PR #74, #87):
  - Hierarchie d'entites avec parent/child
  - Transformations et suppressions en cascade
  - Fix des matrices globales
- **MVP Light & Sky** (PR #70):
  - Systeme d'eclairage de base
  - Skybox

### Antonin Leprest (Matribuk) - **15 commits**
- **Hotfix memory leaks** (PR #72) - **BUGS CRITIQUES CORRIGES**:
  - Fix invalidation de la queue d'evenements dans EventManager::processEvents()
  - Fix utilisation de _fileManager avant initialisation dans ofApp::setup()
  - Correction des segfaults et undefined behaviors
  - 3 reviews approuves (Richon, Marion, Killian)
- **Assets Management** (PR #77, #78, #93):
  - AssetsPanel avec auto-load du dossier ./data/
  - Bouton d'import pour creer des assets
  - Systeme de drag & drop d'assets sur la scene
  - Import/export d'images interactif
- **ExportPanel** (PR #77):
  - Export de sequences d'images
  - Configuration FPS et duree
  - Sauvegarde en memoire puis export final
- **Integration MaterialPanel** (PR #77):
  - Integration du MaterialPanel de Marion dans l'architecture
  - Gestion des includes et dependencies
- **Fix TransformSystem** (PR #60):
  - Panel R/S avec degres et pourcentages
  - Corrections du panel Transform
  - Amelioration de l'UI
- **Code quality**:
  - Fix auto types (best practices)
  - Fix function naming conventions (private/public)
  - Fix include files (external libs)
  - Application de textures par drag & drop

### Marion Kauffmann (THORINKAUFFMANN) - **12 commits**
- **ResourceManager** - Implementation complete du systeme de gestion de ressources:
  - Cache de ressources (meshes, textures, shaders)
  - Methodes loadMesh(), loadTexture(), loadShader()
  - Reference counting pour gestion memoire optimale
  - Nettoyage automatique des ressources inutilisees
- **MaterialPanel** (PR #82, #89):
  - Panel pour charger mesh, textures, shaders depuis fichiers
  - Factory pattern pour load from file
  - Boutons de chargement pour chaque type de ressource
  - Hotfix: correction des boutons du MaterialPanel
  - Fonctions helper pour lisibilite du code
- **Fix texture loading** - Correction du chargement de textures sur primitives (generation de primitives supprimait la texture)

### Leandre Cacarie (Richon) - **1 commit**
- **Review & QA** - Reverts de PRs problematiques pour maintenir la stabilite

**Metriques**: 92 commits | 20 PRs mergees | **Sprint le plus productif**

---

## Semaine 4 (14-20 octobre 2025)

### Antonin Leprest (Matribuk)
- **Color Palette saved colors** - Ajout de la fonctionnalite de sauvegarde de couleurs dans la palette (1 commit le 14 octobre)

**Tous les autres membres:**
*Pause dans le developpement - Aucune activite git*

**Metriques**: 1 commit | 0 PRs mergees

---

## Semaine 5 (21-24 octobre 2025) - **FINALISATION TP1**

### Antonin Leprest (Matribuk) - **6 commits**
- **EyedropperSystem (Pipette couleur)** (PR #93):
  - Systeme de pipette pour echantillonner les couleurs des entites
  - Preview en temps reel lors du survol
  - Application de couleur par clic
  - Annulation par clic dans le vide
- **Refactoring SelectionSystem** - **Elimination duplication de code**:
  - Creation d'un systeme de filtre generique pour le raycast
  - SelectionSystem utilise filtre "Selectable"
  - EyedropperSystem utilise filtre "Renderable"
  - Suppression de ~110 lignes de code duplique
- **AABB Helpers publics** - Transformation des methodes privees en helpers publics statiques pour reutilisation
- **Fix type auto** - Remplacement de `auto` par `EntityFilter` pour meilleure lisibilite

### Clement Barrier (Maskalito) - **3 commits**
- **Primitives vectorielles 2D** (PR #94):
  - Implementation de 5 primitives 2D: Rectangle, Point, Line, Triangle, Circle
  - Generation de mesh 2D avec vertices
  - Ajustement de la taille des points par defaut
- **Fix import** - Correction des issues d'importation

### Killian Cottrelle
- **CameraManager improvements** (commits du 24 oct):
  - Ajout d'un bouton "Create Camera" dans l'UI
  - Systeme de creation de cameras infinies
  - Auto-creation d'une camera de remplacement si la derniere est supprimee
  - Suppression du bouton "Create Entity" obsolete
  - Synchronisation de la logique import/export
- **Hotfix Import/Export** (Issue #95) - Corrections de la logique d'import/export

### Leandre Cacarie (Richon)
*Pas actif cette semaine*

### Marion Kauffmann
*Pas active cette semaine*

**Metriques**: 15+ commits | 2 PRs mergees | **Projet pret pour livraison**

---

## Statistiques globales du projet

| Membre | Commits | PRs mergees (auteur) | PRs reviewed (merger) | Role principal |
|--------|---------|---------------------|----------------------|----------------|
| **Clement Barrier** (Maskalito) | 84 | 11 | - | Architecture & Core Systems |
| **Killian Cottrelle** | 78 | 13 | - | Features & Primitives |
| **Antonin Leprest** (Matribuk) | 31 | 10 | - | UI/UX, Integration & Bugfixes |
| **Marion Kauffmann** | 12 | 2 | - | Material System |
| **Leandre Cacarie** (Richon) | 8 | 1 | 19 | Refactoring & Review Lead |
| **TOTAL** | **213** | **47** | - | - |

---

## Resume des accomplissements par membre

### Clement Barrier (Maskalito) - **Architecture & Rendering**
- SelectionSystem avec raycast AABB complet
- CameraSystem avec orbit/pan/zoom/focus
- Focus automatique sur selection
- 8 primitives (3D: Box, Sphere, Plane + 2D: Rectangle, Point, Line, Triangle, Circle)
- Bounding boxes visibles
- Architecture UIManager clean

### Killian Cottrelle - **Features & Assets**
- Architecture ECS complete (EntityManager, ComponentRegistry, Components)
- Systeme d'evenements (EventManager, EventBridge)
- ActionManager avec undo/redo (Command Pattern)
- FileManager (import/export meshes)
- Multi-selection avec Ctrl (PR #81)
- CursorManager (6 types de curseurs dynamiques)
- Primitives geometriques 3D procedurales
- Graphe de scene avec hierarchie parent-enfant
- Light & Sky system
- Multi-cameras avec creation dynamique

### Antonin Leprest (Matribuk) - **UI/UX, Integration & Bugfixes**
- **10 PRs authored** (15, 20, 23, 42, 46, 60, 72, 77, 78, 93)
- **Bugfixes critiques** (PR #72): memory leaks, segfaults, EventManager queue invalidation, _fileManager initialization order
- **4 UI Panels complets**: Properties, Assets, Export, Color
- **TransformSystem** (PR #42) avec dirty flags et controle manuel rotation
- **Assets Management** (PR #77, #78, #93) avec drag & drop, auto-load, import/export images
- **Export de sequences d'images** avec configuration FPS/duree
- **Color Palette** avec sauvegarde de couleurs
- **EyedropperSystem** (pipette couleur) avec preview temps reel
- **Refactoring SelectionSystem** (elimination ~110 lignes duplication code)
- **AABB Helpers** publics statiques reutilisables
- **Code quality**: fix auto types, naming conventions, includes, compilation errors
- **Integration**: MaterialPanel de Marion, coordination entre systemes

### Marion Kauffmann - **Resource & Material System**
- **ResourceManager complet**:
  - Cache de ressources (meshes, textures, shaders)
  - loadMesh(), loadTexture(), loadShader() avec reference counting
  - Nettoyage automatique des ressources inutilisees
- **MaterialPanel complet**:
  - Panel pour charger mesh, textures, shaders depuis fichiers
  - Factory pattern pour load from file
  - Hotfixes MaterialPanel
  - Fix texture loading sur primitives

### Leandre Cacarie (Richon) - **Refactoring & QA**
- Refactoring structure projet (include paths)
- Nettoyage code (variables inutiles, defines)
- Review de 19 PRs (leader en review)
- QA avec reverts de PRs problematiques
- Maintien de la stabilite du projet

---

## Criteres TP1 implementes

### Image (3/5)
- 1.1 Importation d'images (Killian, Antonin)
- 1.2 Exportation d'images (Antonin)
- 1.4 Palette de couleur (Antonin - Eyedropper)

### Dessin vectoriel (3/5)
- 2.1 Curseur dynamique (Clement - 6 types!)
- 2.3 Primitives vectorielles (Clement - 8 types!)
- 2.5 Interface (Antonin - 10 panels)

### Transformation (3/5)
- 3.1 Graphe de scene (Killian - ECS)
- 3.2 Selection multiple (Clement)
- 3.3 Transformations interactives (Antonin)

### Geometrie (3/5)
- 4.1 Boites de delimitation (Clement - AABB)
- 4.2 Primitives geometriques (Clement, Killian)
- 4.3 Modeles 3D (Killian, Marion)

### Camera (3/5)
- 5.1 Camera interactive (Clement)
- 5.2 Cameras multiples (Clement, Killian)
- 5.3 Points de vue multiples (Clement)

**Total: 18-19 criteres sur 25**

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
