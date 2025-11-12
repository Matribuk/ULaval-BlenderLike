# 🧩 TODO List – Fonctionnalités à implémenter

## 🎨 1. Import / Export & Couleur

### 1.1 Importation d’images
- [x] En cours d’exécution, possibilité d’importer des fichiers images de manière interactive
- [x] Affichage de ces images dans la scène sous une forme visuelle (sprite, texture, plan, etc.)

### 1.2 Exportation d’images
- [x] En cours d’exécution, export interactif de séquences d’images correspondant au rendu de la scène
- [x] Définir un intervalle de temps pour la séquence
- [x] Sauvegarde automatique dans un dossier défini par l’utilisateur

### 1.4 Palette de couleur
- [x] Création d’une palette personnalisée d’au moins 4 couleurs
- [x] Possibilité de réutiliser ces couleurs pour colorer des éléments visuels
- [x] Interface de sélection et modification des couleurs

---

## 🖱️ 2. Outils et Interaction

### 2.1 Curseur dynamique
- [x] Minimum 5 représentations visuelles du curseur
- [x] Changement du curseur selon l’état de l’application (dessin, sélection, transformation, etc.)

### 2.3 Primitives vectorielles
- [ ] Création interactive d’au moins **6 types de primitives** :
  - [x] Point
  - [x] Ligne
  - [x] Carré
  - [x] Rectangle
  - [x] Cercle
  - [x] Triangle

### 2.5 Interface
- [x] Un ou des éléments d’interface graphique offrent de la rétroaction informative visuelle
- [x] Des contrôles interactifs pour influencer les états de l’application.

---

## 🧠 3. Sélection et Transformation

### 3.1 Graphe de scène
- [x] Les éléments visuels présents organisés dans une scene permettent:
  - [x] L'ajout d’attributs
  - [x] La suppression d’attributs
  - [x] La sélection d’attributs
  - [x] L’édition d’attributs.

### 3.2 Sélection multiple
- [x] Sélection de plusieurs éléments visuels simultanément
- [x] Modification simultanée des attributs communs

### 3.3 Transformations interactives
- [x] Translation
- [x] Rotation
- [x] Mise à l’échelle (scale)
- [ ] Manipulation directe dans la scène (gizmo, handles) optionnel

---

## 🧱 4. 3D et Géométrie

### 4.1 Boîte de délimitation
- [x] Affichage d’un bounding box autour de chaque modèle 3D
- [x] Taille ajustée automatiquement à chaque modèle

### 4.2 Primitives géométriques
- [x] Génération procédurale d’au moins 2 primitives 3D
    - [x] cube
    - [x] sphère

### 4.3 Modèles 3D
- [x] Importation et affichage d’au moins 3 types de modèles 3D externes (OBJ, PLY, STL, etc.)

---

## 🎥 5. Caméras et Navigation

### 5.1 Caméra interactive
- [x] Mouvement intuitif (orbit, pan, zoom)
- [x] Contrôle fluide via clavier/souris

### 5.2 Caméras multiples
- [x] Support de plusieurs caméras distinctes
- [x] Chacune possède ses propres attributs
- [x] Modes perspective et orthographique

### 5.4 Focus automatique
- [x] Focus automatique sur la sélection
- [x] Vue recentrée avec cadrage optimal (cacher les autre element non focus)

---

## 🖼️ 6. Texture

### 6.1 Coordonnées de texture
- [x] L'application peut faire le rendu d'au moins 2 maillages géométriques 3D texturés
- [x] Les coordonnées de mapping sont adéquatement distribuées sur la surface (Box, Sphere, toutes les primitives)

### 6.4 Cubemap
- [x] Il existe au moins 2 textures de type cubemap
- [x] Ces cubemaps sont utilisées dans le rendu d'au moins 1 élément visuel d'une scène

### 6.5 Texture procédurale
- [x] Génération procédurale de textures
- [x] Au moins 2 types de textures procédurales différentes

---

## 💡 7. Illumination classique

### 7.1 Modèles d'illumination
- [x] Le rendu d'au moins 2 éléments visuels peut être fait avec les 4 modèles d'illumination classiques :
  - [x] Lambert
  - [ ] Gouraud
  - [x] Phong
  - [ ] Blinn-Phong

### 7.2 Matériaux
- [ ] Au moins 2 éléments visuels d'une scène ont une surface avec un matériau
- [ ] Sélection parmi un ensemble d'au moins 4 matériaux différents

### 7.3 Types de lumière
- [ ] L'application permet d'avoir dans une scène au moins une instance de 4 types de lumières différents :
  - [ ] Ambiante
  - [ ] Directionnelle
  - [ ] Ponctuelle
  - [ ] Projecteur (spot)

---

## 🔷 8. Topologie

### 8.1 Triangulation
- [ ] Algorithme de triangulation
- [ ] Application sur formes 2D complexes

### 8.2 Courbe paramétrique
- [ ] L'application permet d'afficher au moins 1 type de courbe paramétrique
- [ ] Avec au moins 5 points de contrôle
- [ ] Par exemple : spline de Bézier ou de Catmull-Rom

### 8.4 Effet de relief
- [ ] L'application permet de rendre au moins 3 modèles où une ou des textures sont utilisées pour simuler un effet de relief sur la surface
- [ ] Par exemple : displacement mapping ou normal mapping

---

## 🌟 9. Lancer de rayon

### 9.1 Intersection
- [ ] L'application est capable de calculer le point d'intersection entre un rayon et au moins 3 types de primitives géométriques
- [ ] Implémentation pour sphère, box, plan (minimum)

### 9.2 Réflexion
- [ ] Une technique de rendu inspirée des principes du lancer de rayon est utilisée pour rendre au moins 2 effets de réflexion
- [ ] Par exemple : une surface miroir

### 9.4 Ombrage
- [ ] Une technique de rendu inspirée des principes du lancer de rayon est utilisée pour calculer l'ombrage d'au moins 1 scène
- [ ] Calcul des ombres portées

---

## ✨ 10. Illumination moderne

### 10.1 PBR (Physically Based Rendering)
- [ ] Il existe au moins 3 matériaux inspirés des principes du rendu basé sur la physique (PBR)
- [ ] Permet d'illuminer une surface avec au moins 2 instances de lumière dynamique

### 10.2 Métallicité
- [ ] Il existe au moins 2 matériaux qui exposent un facteur de métallicité
- [ ] Permet d'influencer interactivement l'apparence métallique d'une surface

### 10.3 Microfacettes
- [ ] Il existe au moins 2 matériaux qui exposent un facteur de rugosité
- [ ] Permet d'influencer interactivement l'apparence d'une surface (rugosité des microfacettes)

---

### 🎯 À faire (par ordre de priorité suggéré)

**Phase 1 - Illumination de base (Section 7)**
1. 7.3 Types de lumière (ambiante, directionnelle, ponctuelle, spot)
2. 7.1 Modèles d'illumination (Lambert, Gouraud, Phong, Blinn-Phong)
3. 7.2 Matériaux (4+ types différents)

**Phase 2 - Textures avancées (Section 6)**
4. 6.4 Cubemap (2 textures minimum)
5. 6.5 Texture procédurale (2+ types)

**Phase 3 - Topologie (Section 8)**
6. 8.1 Triangulation
7. 8.2 Courbe paramétrique (Bézier/Catmull-Rom, 5+ points)
8. 8.4 Effet de relief (displacement/normal mapping)

**Phase 4 - Raytracing (Section 9)**
9. 9.1 Intersection rayon-primitive (3+ types)
10. 9.2 Réflexion (2+ effets)
11. 9.4 Ombrage par raytracing

**Phase 5 - PBR (Section 10)**
12. 10.1 PBR (3+ matériaux, 2+ lumières)
13. 10.2 Métallicité (2+ matériaux)
14. 10.3 Microfacettes/Rugosité (2+ matériaux)