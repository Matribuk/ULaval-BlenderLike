# 🧩 TODO List – Fonctionnalités à implémenter

## 🎨 1. Import / Export & Couleur

### 1.1 Importation d’images
- [ ] En cours d’exécution, possibilité d’importer des fichiers images de manière interactive
- [ ] Affichage de ces images dans la scène sous une forme visuelle (sprite, texture, plan, etc.)

### 1.2 Exportation d’images
- [ ] En cours d’exécution, export interactif de séquences d’images correspondant au rendu de la scène
- [ ] Définir un intervalle de temps pour la séquence
- [ ] Sauvegarde automatique dans un dossier défini par l’utilisateur

### 1.4 Palette de couleur
- [ ] Création d’une palette personnalisée d’au moins 4 couleurs
- [ ] Possibilité de réutiliser ces couleurs pour colorer des éléments visuels
- [ ] Interface de sélection et modification des couleurs

---

## 🖱️ 2. Outils et Interaction

### 2.1 Curseur dynamique
- [ ] Minimum 5 représentations visuelles du curseur
- [ ] Changement du curseur selon l’état de l’application (dessin, sélection, transformation, etc.)

### 2.3 Primitives vectorielles
- [ ] Création interactive d’au moins **6 types de primitives** :
  - [ ] Point
  - [ ] Ligne
  - [ ] Carré
  - [ ] Rectangle
  - [ ] Cercle
  - [ ] Triangle

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
- [ ] Sélection de plusieurs éléments visuels simultanément
- [ ] Modification simultanée des attributs communs

### 3.3 Transformations interactives
- [x] Translation
- [x] Rotation
- [x] Mise à l’échelle (scale)
- [ ] Manipulation directe dans la scène (gizmo, handles)

---

## 🧱 4. 3D et Géométrie

### 4.1 Boîte de délimitation
- [ ] Affichage d’un bounding box autour de chaque modèle 3D
- [ ] Taille ajustée automatiquement à chaque modèle

### 4.2 Primitives géométriques
- [ ] Génération procédurale d’au moins 2 primitives 3D (ex: cube, sphère, cylindre, cône)
- [ ] Aucun fichier externe requis

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
