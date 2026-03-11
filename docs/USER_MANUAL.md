# Manuel Utilisateur — VisionCast‑AI

**Version 1.0 — Mars 2026**

> Système de production broadcast professionnel 4K, piloté par intelligence artificielle, avec reconnaissance faciale en temps réel et habillage graphique automatique.

---

## Table des matières

1. [Introduction](#1-introduction)
   - 1.1 [Présentation de VisionCast‑AI](#11-présentation-de-visioncast-ai)
   - 1.2 [Fonctionnement général](#12-fonctionnement-général)
   - 1.3 [Architecture globale](#13-architecture-globale)
2. [Installation](#2-installation)
   - 2.1 [Prérequis matériels et logiciels](#21-prérequis-matériels-et-logiciels)
   - 2.2 [Installation sous Windows](#22-installation-sous-windows)
   - 2.3 [Installation sous macOS](#23-installation-sous-macos)
   - 2.4 [Drivers DeckLink / AJA / Magewell](#24-drivers-decklink--aja--magewell)
   - 2.5 [Installation NDI Tools](#25-installation-ndi-tools)
   - 2.6 [Configuration initiale](#26-configuration-initiale)
3. [Workflow broadcast](#3-workflow-broadcast)
   - 3.1 [Flux de production : Caméras → Régie → VisionCast‑AI → Sortie finale](#31-flux-de-production--caméras--régie--visioncast-ai--sortie-finale)
   - 3.2 [Réception du flux vidéo](#32-réception-du-flux-vidéo)
   - 3.3 [Reconnaissance des talents par l'IA](#33-reconnaissance-des-talents-par-lia)
   - 3.4 [Affichage automatique des overlays](#34-affichage-automatique-des-overlays)
   - 3.5 [Gestion des filtres vidéo globaux](#35-gestion-des-filtres-vidéo-globaux)
   - 3.6 [Durée d'affichage des lower thirds](#36-durée-daffichage-des-lower-thirds)
4. [Utilisation de la régie VisionCast Control Room](#4-utilisation-de-la-régie-visioncast-control-room)
   - 4.1 [Panneau Sources vidéo (Control Room)](#41-panneau-sources-vidéo-control-room)
   - 4.2 [Panneau Talents & Détails (Talent Manager)](#42-panneau-talents--détails-talent-manager)
   - 4.3 [Panneau Design & Overlays (Design Panel)](#43-panneau-design--overlays-design-panel)
   - 4.4 [Panneau Filtres vidéo](#44-panneau-filtres-vidéo)
   - 4.5 [Panneau Durée d'affichage](#45-panneau-durée-daffichage)
   - 4.6 [Panneau IA & Reconnaissance (Recognition Panel)](#46-panneau-ia--reconnaissance-recognition-panel)
   - 4.7 [Panneau Logs & Monitoring (Monitoring Panel)](#47-panneau-logs--monitoring-monitoring-panel)
5. [Gestion des talents](#5-gestion-des-talents)
   - 5.1 [Ajouter un talent](#51-ajouter-un-talent)
   - 5.2 [Modifier un talent](#52-modifier-un-talent)
   - 5.3 [Associer un overlay](#53-associer-un-overlay)
   - 5.4 [Associer un thème](#54-associer-un-thème)
   - 5.5 [Associer des filtres globaux](#55-associer-des-filtres-globaux)
6. [Gestion des overlays](#6-gestion-des-overlays)
   - 6.1 [Templates](#61-templates)
   - 6.2 [Thèmes](#62-thèmes)
   - 6.3 [Animations](#63-animations)
   - 6.4 [Logos](#64-logos)
   - 6.5 [Couleurs](#65-couleurs)
7. [Sorties vidéo](#7-sorties-vidéo)
   - 7.1 [SDI (DeckLink / AJA)](#71-sdi-decklink--aja)
   - 7.2 [NDI](#72-ndi)
   - 7.3 [SRT](#73-srt)
   - 7.4 [RTMP](#74-rtmp)
8. [Dépannage](#8-dépannage)
   - 8.1 [L'IA ne reconnaît pas les talents](#81-lia-ne-reconnaît-pas-les-talents)
   - 8.2 [L'overlay ne s'affiche pas](#82-loverlay-ne-saffiche-pas)
   - 8.3 [Les filtres vidéo ne sont pas appliqués](#83-les-filtres-vidéo-ne-sont-pas-appliqués)
   - 8.4 [Problèmes DeckLink / NDI](#84-problèmes-decklink--ndi)

---

## 1. Introduction

### 1.1 Présentation de VisionCast‑AI

**VisionCast‑AI** est un système de production broadcast professionnel conçu pour les chaînes de télévision et les opérateurs de régie. Il combine un moteur vidéo 4K haute performance, un module d'intelligence artificielle de reconnaissance faciale en temps réel, et un habillage graphique automatique (lower thirds, overlays animés).

**Fonctionnalités principales :**

| Fonctionnalité | Description |
|---|---|
| **Reconnaissance faciale IA** | Identification automatique des talents à l'antenne en temps réel |
| **Overlays automatiques** | Affichage de lower thirds animés avec nom, fonction et habillage |
| **Moteur vidéo 4K** | Traitement GPU/CPU jusqu'en 3840 × 2160 à 25/29.97/50/59.94 fps |
| **SDK broadcast** | Support natif DeckLink, AJA, Magewell, NDI, SRT et RTMP |
| **Régie Qt** | Interface de contrôle professionnelle à panneaux ancrables |
| **Filtres vidéo** | LUT cinéma, netteté, réduction de bruit, HDR tonemap, contraste dynamique |

### 1.2 Fonctionnement général

VisionCast‑AI fonctionne en trois couches interconnectées :

1. **Module IA Python** — Capture chaque image, détecte les visages, compare les encodages faciaux (vecteurs 128 dimensions) avec la base de talents, et transmet les métadonnées au moteur vidéo via ZeroMQ.
2. **Moteur vidéo C++** — Reçoit le flux vidéo et les métadonnées IA, applique la chaîne de filtres, compose les overlays GPU, et envoie les images traitées vers les sorties broadcast.
3. **Régie Qt (Control Room)** — Interface opérateur pour piloter l'ensemble : sources vidéo, gestion des talents, design des overlays, monitoring temps réel et configuration des sorties.

### 1.3 Architecture globale

```
┌─────────────────────────────────────────────────────────────────┐
│                     VisionCast‑AI — Architecture                │
│                                                                 │
│  ┌──────────────┐    ZeroMQ     ┌─────────────────────────┐    │
│  │  Module IA   │──────────────▶│   Moteur vidéo C++      │    │
│  │  (Python)    │  tcp://5557   │                         │    │
│  │              │               │  ┌───────────────────┐  │    │
│  │ • Détection  │               │  │ Chaîne de filtres │  │    │
│  │ • Encodage   │               │  │ (LUT, Sharpen,    │  │    │
│  │ • Matching   │               │  │  HDR, Contraste)  │  │    │
│  │ • Talents DB │               │  └────────┬──────────┘  │    │
│  └──────────────┘               │           ▼             │    │
│                                 │  ┌───────────────────┐  │    │
│  ┌──────────────┐               │  │ GPU Compositor    │  │    │
│  │  Régie Qt    │               │  │ (Lower thirds,    │  │    │
│  │ Control Room │◀─────────────▶│  │  Animations,      │  │    │
│  │              │  Signaux/Slots │  │  Logos)           │  │    │
│  │ • Sources    │               │  └────────┬──────────┘  │    │
│  │ • Talents    │               │           ▼             │    │
│  │ • Design     │               │  ┌───────────────────┐  │    │
│  │ • Filtres    │               │  │ Output Manager    │  │    │
│  │ • Monitoring │               │  │ (SDI, NDI, SRT,   │  │    │
│  └──────────────┘               │  │  RTMP, Fichier)   │  │    │
│                                 │  └───────────────────┘  │    │
│                                 └─────────────────────────┘    │
│                                                                 │
│  ┌─────────────────────────────────────────────────────────┐   │
│  │              SDK Broadcast (Abstraction matérielle)      │   │
│  │  DeckLink  │  AJA  │  Magewell  │  NDI  │  SRT  │ RTMP │   │
│  └─────────────────────────────────────────────────────────┘   │
└─────────────────────────────────────────────────────────────────┘
```

**Communication inter-modules :**

| Canal | Transport | Direction | Usage |
|---|---|---|---|
| `tcp://127.0.0.1:5557` | ZeroMQ PUB/SUB | Python → C++ | Résultats de reconnaissance faciale et métadonnées overlay des talents |
| Signaux Qt | In-process | UI ↔ Engine | Contrôle de la régie |

---

## 2. Installation

### 2.1 Prérequis matériels et logiciels

**Matériel recommandé :**

| Composant | Minimum | Recommandé |
|---|---|---|
| Processeur | 4 cœurs, 3 GHz | 8+ cœurs, 3.5+ GHz |
| Mémoire vive | 8 Go | 16+ Go |
| GPU | Intégré (CPU fallback) | NVIDIA/AMD dédié (CUDA/OpenCL) |
| Stockage | SSD 256 Go | SSD NVMe 512+ Go |
| Carte d'acquisition | Webcam (test) | DeckLink / AJA / Magewell |
| Réseau | Ethernet 1 Gbps | Ethernet 10 Gbps (NDI 4K) |

**Logiciels requis :**

| Logiciel | Version minimum |
|---|---|
| Python | 3.11+ |
| CMake | 3.20+ |
| OpenCV | 4.8+ |
| Qt | 5.15+ ou 6.x |
| FFmpeg | 6.0+ |
| ZeroMQ | 4.3+ |
| dlib / face_recognition | 1.3.0+ |
| Compilateur C++ | GCC 10+ / Clang 12+ / MSVC 2019+ |

### 2.2 Installation sous Windows

1. **Installer Python 3.11+** depuis [python.org](https://www.python.org/downloads/). Cocher « Add Python to PATH ».

2. **Installer Visual Studio 2019+** avec le module « Développement Desktop en C++ ».

3. **Installer CMake 3.20+** depuis [cmake.org](https://cmake.org/download/).

4. **Installer Qt 5.15+ ou 6.x** via le Qt Online Installer.

5. **Cloner le dépôt et installer les dépendances Python :**

   ```powershell
   git clone https://github.com/votre-org/visioncast-AI.git
   cd visioncast-AI
   python -m venv .venv
   .venv\Scripts\activate
   pip install -r python\requirements.txt
   ```

6. **Compiler le moteur C++, le SDK et l'interface Qt :**

   ```powershell
   mkdir build
   cd build
   cmake .. -G "Visual Studio 17 2022" -DCMAKE_BUILD_TYPE=Release ^
     -DCMAKE_PREFIX_PATH=C:\Qt\6.x\msvc2019_64 ^
     -DWITH_DECKLINK=ON -DWITH_AJA=ON -DWITH_NDI=ON
   cmake --build . --config Release --parallel
   ```

7. **Vérifier l'installation** en lançant `bin\visioncast_ui.exe`.

### 2.3 Installation sous macOS

1. **Installer Homebrew** puis les dépendances :

   ```bash
   brew install python@3.11 cmake opencv qt ffmpeg zeromq
   ```

2. **Cloner le dépôt et installer les dépendances Python :**

   ```bash
   git clone https://github.com/votre-org/visioncast-AI.git
   cd visioncast-AI
   python3 -m venv .venv
   source .venv/bin/activate
   pip install -r python/requirements.txt
   ```

3. **Compiler le moteur C++ :**

   ```bash
   mkdir build && cd build
   cmake .. -DCMAKE_BUILD_TYPE=Release \
     -DCMAKE_PREFIX_PATH=$(brew --prefix qt) \
     -DWITH_DECKLINK=ON -DWITH_NDI=ON
   cmake --build . --parallel
   ```

4. **Lancer le système** (voir section [2.6 Configuration initiale](#26-configuration-initiale)).

### 2.4 Drivers DeckLink / AJA / Magewell

#### Blackmagic DeckLink

1. Télécharger **Desktop Video** depuis [blackmagicdesign.com/support](https://www.blackmagicdesign.com/support).
2. Installer le package (Windows : `.exe` / macOS : `.dmg`).
3. Redémarrer la machine.
4. Ouvrir **Blackmagic Desktop Video Setup** pour vérifier la détection de la carte.
5. Configurer le mode vidéo (ex. : 2160p25, 1080p50) et le connecteur (SDI, HDMI, AUTO).

#### AJA

1. Télécharger **AJA Desktop Software** depuis [aja.com/support](https://www.aja.com/support).
2. Installer le driver et redémarrer.
3. Ouvrir **AJA Control Panel** pour vérifier la détection du périphérique.
4. Configurer le mode vidéo et la référence genlock si nécessaire.

#### Magewell

1. Télécharger le driver depuis [magewell.com/downloads](https://www.magewell.com/downloads).
2. Pour les modèles USB : brancher le périphérique, le driver s'installe automatiquement.
3. Pour les modèles PCI : installer le driver, redémarrer, vérifier dans le gestionnaire de périphériques.
4. Magewell est utilisé en **capture uniquement** (entrée HDMI, DVI, composante, composite).

### 2.5 Installation NDI Tools

1. Télécharger **NDI Tools** depuis [ndi.video/tools](https://ndi.video/tools/).
2. Installer le package complet (NDI Studio Monitor, NDI Screen Capture, etc.).
3. Vérifier que le **NDI Runtime** est bien présent sur le système.
4. Sur le réseau local, les sources NDI sont découvertes automatiquement.
5. Dans VisionCast‑AI, la source NDI apparaîtra dans le panneau **Sources vidéo** de la régie.

### 2.6 Configuration initiale

Après installation, configurez les trois fichiers principaux dans le répertoire `config/` :

#### `config/system.json` — Configuration système

```json
{
  "engine": {
    "resolution": { "width": 3840, "height": 2160 },
    "frame_rate": 25.0,
    "pixel_format": "V210"
  },
  "ai": {
    "model": "hog",
    "tolerance": 0.45,
    "process_every_n_frames": 3,
    "zmq_pub_endpoint": "tcp://127.0.0.1:5557",
    "zmq_config_endpoint": "tcp://127.0.0.1:5556"
  },
  "output": {
    "primary": { "type": "decklink", "device_index": 0, "mode": "2160p25" },
    "ndi": { "enabled": true, "source_name": "VisionCast Program" },
    "srt": { "enabled": false, "destination": "srt://localhost:9000", "latency_ms": 120 },
    "rtmp": { "enabled": false, "server_url": "rtmp://live.example.com/app", "stream_key": "" },
    "recording": { "enabled": false, "path": "/recordings/", "codec": "h265", "bitrate_mbps": 50 }
  },
  "ui": {
    "preview_resolution": { "width": 1920, "height": 1080 },
    "multiview_layout": { "rows": 2, "cols": 2 },
    "monitoring_refresh_ms": 1000
  }
}
```

| Paramètre | Description | Valeurs possibles |
|---|---|---|
| `engine.resolution` | Résolution du moteur vidéo | `1920×1080`, `3840×2160` |
| `engine.frame_rate` | Cadence image | `23.98`, `25`, `29.97`, `50`, `59.94` |
| `engine.pixel_format` | Format pixel | `BGRA8`, `UYVY`, `V210`, `NV12` |
| `ai.model` | Modèle de détection faciale | `hog` (CPU), `cnn` (GPU) |
| `ai.tolerance` | Seuil de correspondance faciale | `0.0` à `1.0` (défaut : `0.45`) |
| `ai.process_every_n_frames` | Traiter 1 image sur N | `1` à `10` (défaut : `3`) |
| `output.primary.type` | Type de sortie principale | `decklink`, `aja`, `ndi`, `webcam` |

#### `config/design.json` — Configuration design

Ce fichier contrôle les paramètres visuels par défaut des overlays et de l'interface :

```json
{
  "lower_thirds": {
    "default_title": "",
    "default_subtitle": "",
    "style": "default",
    "display_duration_s": 5.0
  },
  "templates": {
    "available": ["default", "modern_blue", "minimal"],
    "selected": "default"
  },
  "themes": {
    "available": ["Dark", "Light", "Broadcast", "Custom"],
    "selected": "Dark"
  },
  "colors": { "primary": "#000000", "accent": "#FFFFFF" },
  "logo": { "path": "", "position": "bottom_right" },
  "transitions": { "type": "fade", "duration_ms": 500 },
  "video_filters": {
    "cinema_lut": false,
    "sharpen": false,
    "noise_reduction": false,
    "hdr_tonemap": false,
    "dynamic_contrast": false
  }
}
```

#### Démarrage du système

Lancez les trois composants dans des terminaux séparés :

```bash
# Terminal 1 — Moteur vidéo C++
./build/bin/visioncast_engine

# Terminal 2 — Module IA Python
source .venv/bin/activate
python python/main.py

# Terminal 3 — Interface de régie Qt
./build/bin/visioncast_ui
```

---

## 3. Workflow broadcast

### 3.1 Flux de production : Caméras → Régie → VisionCast‑AI → Sortie finale

Le flux de production VisionCast‑AI s'intègre dans une chaîne broadcast standard :

```
Caméras (SDI/HDMI/NDI)
       │
       ▼
Table de régie matérielle (mélangeur)
       │
       ▼ Signal Program (SDI / NDI)
┌──────────────────────────────────────────┐
│            VisionCast‑AI                 │
│                                          │
│  1. Capture (SDK DeckLink/AJA/NDI)       │
│  2. Analyse IA (détection + matching)    │
│  3. Filtres vidéo (LUT, HDR, etc.)       │
│  4. Composition GPU (overlays animés)    │
│  5. Encodage et distribution             │
│                                          │
└──────────────────┬───────────────────────┘
                   │
                   ▼
          Sorties multiples
    ┌──────┬──────┬──────┬──────┐
    │ SDI  │ NDI  │ SRT  │ RTMP │
    │      │      │      │      │
    └──────┴──────┴──────┴──────┘
```

**Étapes du flux :**

1. Les caméras de studio alimentent la table de régie matérielle.
2. Le signal **Program** (sortie régie) entre dans VisionCast‑AI via une carte d'acquisition (DeckLink, AJA, Magewell) ou via le réseau (NDI).
3. Le module IA Python analyse chaque Nᵉ image pour détecter et identifier les visages.
4. Le moteur C++ applique les filtres vidéo puis compose les overlays (lower thirds, logos) sur le flux.
5. Le signal final habillé est distribué vers les sorties configurées (SDI, NDI, SRT, RTMP, enregistrement).

### 3.2 Réception du flux vidéo

VisionCast‑AI reçoit le flux vidéo à travers le **SDK broadcast** qui fournit une abstraction unifiée pour tous les périphériques supportés :

| Périphérique | Type d'entrée | Interface | Résolutions |
|---|---|---|---|
| **Blackmagic DeckLink** | SDI, HDMI | PCIe | 1080p, 4K |
| **AJA** | SDI | PCIe | 1080p, 4K |
| **Magewell** | HDMI, DVI, Composite | USB / PCIe | 1080p, 4K |
| **NDI** | Réseau IP | Ethernet | 1080p, 4K |
| **Webcam** | USB | USB | Variable (test) |

Le **Capture Manager** du moteur vidéo gère le routage des images depuis les sources multiples. Les images capturées sont placées dans un **ring buffer** de 5 à 10 trames pour découpler l'acquisition du traitement.

### 3.3 Reconnaissance des talents par l'IA

Le module IA Python effectue la reconnaissance faciale selon le pipeline suivant :

```
Image capturée
      │
      ▼
┌─────────────────────┐
│  Détection faciale   │  Modèle HOG (CPU) ou CNN (GPU)
│  face_locations()    │  Localise les visages dans l'image
└──────────┬──────────┘
           ▼
┌─────────────────────┐
│  Encodage facial     │  Calcul du vecteur 128 dimensions
│  face_encodings()    │  Empreinte biométrique unique
└──────────┬──────────┘
           ▼
┌─────────────────────┐
│  Matching            │  Comparaison avec la base de talents
│  face_distance()     │  Seuil de tolérance : 0.45
└──────────┬──────────┘
           ▼
┌─────────────────────┐
│  Envoi métadonnées   │  ZeroMQ PUB vers le moteur C++
│  RecognitionResult   │  Nom, rôle, confiance, position
└─────────────────────┘
```

**Paramètres clés :**

| Paramètre | Fichier | Description |
|---|---|---|
| `ai.model` | `config/system.json` | `hog` pour CPU, `cnn` pour GPU (plus précis) |
| `ai.tolerance` | `config/system.json` | Seuil de distance (0.0 = exact, 1.0 = permissif). Défaut : `0.45` |
| `ai.process_every_n_frames` | `config/system.json` | Traite 1 image sur N pour optimiser les performances. Défaut : `3` |

**Performance typique :** ~12.5 fps d'analyse IA (25 fps ÷ traitement chaque 2ᵉ à 3ᵉ image).

### 3.4 Affichage automatique des overlays

Lorsqu'un talent est reconnu, le moteur vidéo affiche automatiquement un **lower third** :

1. Le module IA envoie un message `TalentOverlayMessage` via ZeroMQ avec les métadonnées du talent (nom, rôle, template, couleur, animations).
2. Le **GPU Compositor** charge le template d'overlay associé au talent (fichier JSON dans `overlays/templates/`).
3. L'overlay est rendu avec l'animation d'entrée configurée (slide, fade, scale).
4. Le lower third reste affiché pendant la **durée d'affichage** définie (par défaut 5 secondes).
5. L'animation de sortie est jouée (fade out, slide down, etc.).

**Exemple de message envoyé au moteur :**

```json
{
  "type": "talent_overlay",
  "talent_id": "glody",
  "name": "Glody",
  "role": "Présentateur",
  "overlay": "overlays/templates/default.json",
  "theme_color": "#1A73E8",
  "confidence": 0.92,
  "display_duration_ms": 5000.0
}
```

### 3.5 Gestion des filtres vidéo globaux

Les filtres vidéo sont appliqués à **l'ensemble du flux** avant la composition des overlays. Ils sont configurés dans `config/video_filters.json` et activés/désactivés depuis la régie ou le fichier `config/design.json`.

| Filtre | Description | Paramètres |
|---|---|---|
| **Cinema LUT** | Étalonnage colorimétrique via table LUT 3D | `lut_path`, `intensity` (0–1) |
| **Sharpen** | Renforcement de la netteté (unsharp mask) | `strength` (0–5), `kernel_size` |
| **Noise Reduction** | Réduction de bruit bilatérale préservant les contours | `strength` (1–50), `sigma_color`, `sigma_space` |
| **HDR Tonemap** | Conversion HDR vers SDR (Reinhard) | `gamma` (0.1–5), `saturation` (0–3) |
| **Dynamic Contrast** | Amélioration adaptative du contraste (CLAHE) | `clip_limit` (1–40), `tile_grid_size` (2–32) |

**Ordre de traitement :** Réduction de bruit → LUT cinéma → HDR Tonemap → Contraste dynamique → Netteté.

### 3.6 Durée d'affichage des lower thirds

La durée d'affichage des lower thirds est contrôlable à plusieurs niveaux :

| Niveau | Fichier / Interface | Paramètre | Défaut |
|---|---|---|---|
| **Global** | `config/design.json` | `lower_thirds.display_duration_s` | 5.0 s |
| **Par template** | `overlays/templates/*.json` | `style.display_duration_ms` | Variable |
| **Par talent** | `talents/talents.json` | Via le template associé | Variable |
| **Régie** | Panneau Design & Overlays | Champ « Durée (s) » | Modifiable en direct |

**Priorité :** La durée définie dans la régie (en direct) > la durée du template du talent > la durée globale du design.

---

## 4. Utilisation de la régie VisionCast Control Room

La régie VisionCast Control Room est l'interface Qt professionnelle de pilotage du système. Elle se compose de **panneaux ancrables** (dock widgets) que l'opérateur peut réorganiser selon ses préférences.

### 4.1 Panneau Sources vidéo (Control Room)

Ce panneau gère les sources vidéo et le contrôle du moteur de traitement.

**Fonctionnalités :**

- **Liste des sources** : Affiche toutes les sources vidéo disponibles (DeckLink, AJA, Magewell, NDI, Webcam) avec leur type et index.
- **Sélection de source** : Cliquer sur une source pour la sélectionner comme entrée active.
- **Démarrage / Arrêt du moteur** : Bouton Start/Stop pour lancer ou arrêter le pipeline vidéo.
- **Indicateur d'état** : Label indiquant si le moteur est en cours d'exécution ou arrêté.

**Utilisation :**

1. Vérifiez que vos périphériques sont détectés dans la liste des sources.
2. Sélectionnez la source vidéo souhaitée.
3. Cliquez sur **Start Engine** pour lancer le traitement.
4. Le label d'état passe à « En cours d'exécution ».

### 4.2 Panneau Talents & Détails (Talent Manager)

Ce panneau offre une interface CRUD complète pour gérer la base de données des talents.

**Interface :**

- **Tableau des talents** : Vue tabulaire listant tous les talents avec leurs propriétés.
- **Panneau de détails** : Formulaire d'édition pour le talent sélectionné :
  - Nom
  - Rôle / Fonction
  - Organisation
  - Photo (chemin vers l'image)
  - Template d'overlay (liste déroulante)
  - Couleur du thème (sélecteur hexadécimal avec aperçu)
  - Animation d'entrée (slide_left, slide_up, fade_in, scale_in)
  - Animation de sortie (fade_out, slide_down, slide_right, scale_out)
  - Durée d'animation (ms)
- **Boutons d'action** : Ajouter, Supprimer, Appliquer les modifications.

**Base de données** : `talents/talents.json`

### 4.3 Panneau Design & Overlays (Design Panel)

Ce panneau regroupe tous les paramètres visuels de l'habillage sous forme d'onglets :

| Onglet | Contenu |
|---|---|
| **Lower Thirds** | Titre, sous-titre, durée d'affichage, bouton Appliquer |
| **Templates** | Sélection du template d'overlay (default, modern_blue, minimal) |
| **Thèmes** | Choix du thème UI (Dark, Light, Broadcast, Custom) |
| **Couleurs** | Couleur primaire et couleur d'accent (sélecteur hexadécimal) |
| **Logos** | Chemin du logo, bouton parcourir |
| **Transitions** | Type de transition (fade, dissolve, wipe, cut), durée (ms) |
| **Filtres vidéo** | Cases à cocher pour activer/désactiver chaque filtre |

### 4.4 Panneau Filtres vidéo

Accessible depuis l'onglet **Filtres vidéo** du panneau Design & Overlays, ce sous-panneau permet d'activer ou désactiver individuellement chaque filtre du pipeline :

| Case à cocher | Filtre | Effet |
|---|---|---|
| ☐ Cinema LUT | `cinema_lut` | Étalonnage couleur type cinéma |
| ☐ Sharpen | `sharpen` | Renforcement de la netteté |
| ☐ Noise Reduction | `noise_reduction` | Réduction de bruit |
| ☐ HDR Tonemap | `hdr_tonemap` | Conversion HDR → SDR |
| ☐ Dynamic Contrast | `dynamic_contrast` | Contraste adaptatif |

Les modifications sont appliquées en temps réel au flux vidéo. Les paramètres détaillés de chaque filtre (intensité, sigma, gamma, etc.) se configurent dans le fichier `config/video_filters.json`.

### 4.5 Panneau Durée d'affichage

La durée d'affichage des lower thirds se configure dans l'onglet **Lower Thirds** du panneau Design :

- **Champ « Durée (s) »** : Valeur en secondes (type flottant, ex. : `5.0`).
- **Bouton Appliquer** : Applique immédiatement la nouvelle durée au flux en cours.
- Cette valeur est sauvegardée dans `config/design.json` sous `lower_thirds.display_duration_s`.

> **Note** : Chaque template d'overlay peut définir sa propre durée via `style.display_duration_ms`. La valeur définie dans la régie a priorité.

### 4.6 Panneau IA & Reconnaissance (Recognition Panel)

Ce panneau affiche en temps réel les résultats de la reconnaissance faciale :

- **Visage détecté** : Nom du visage reconnu.
- **Talent associé** : Nom complet et rôle du talent identifié.
- **Score de confiance** : Barre de progression indiquant le niveau de certitude (0–100 %).
- **Historique** : Liste déroulante des 50 derniers événements de reconnaissance avec horodatage.

Chaque nouvelle détection déclenche le signal `recognitionReceived`, qui peut être utilisé pour des actions automatiques (ex. : overlay, log, alerte).

### 4.7 Panneau Logs & Monitoring (Monitoring Panel)

Ce panneau fournit une vue complète de l'état du système :

**Métriques système (onglet Métriques) :**

| Métrique | Description |
|---|---|
| **CPU** | Utilisation processeur (pourcentage + barre) |
| **GPU** | Utilisation GPU (pourcentage + barre) |
| **Latence** | Latence de traitement en millisecondes |
| **FPS** | Images par seconde traitées |
| **Images perdues** | Nombre d'images non traitées à temps |

**Onglets de logs :**

| Onglet | Source | Contenu |
|---|---|---|
| **IA** | Module Python | Messages de détection, matching, erreurs IA |
| **Engine** | Moteur C++ | Messages du pipeline vidéo, filtres, overlays |
| **ZeroMQ** | IPC | Messages de communication inter-modules |

**Configuration** : Le rafraîchissement et le nombre maximum de lignes se configurent dans `config/system.json` :

```json
{
  "ui": {
    "monitoring_refresh_ms": 1000,
    "monitoring": {
      "max_log_lines": 1000,
      "log_level": "INFO",
      "log_sources": ["ai", "engine", "zmq"]
    }
  }
}
```

---

## 5. Gestion des talents

### 5.1 Ajouter un talent

Pour ajouter un nouveau talent au système :

1. **Préparer la photo** :
   - Prendre une photo de face, bien éclairée, fond neutre de préférence.
   - Résolution recommandée : 512 × 512 pixels minimum.
   - Formats acceptés : JPEG, PNG.
   - Placer la photo dans le répertoire `talents/` (ex. : `talents/marie_dupont.jpeg`).

2. **Via la régie (Talent Manager)** :
   - Ouvrir le panneau **Talents & Détails**.
   - Cliquer sur **Ajouter**.
   - Remplir les champs : Nom, Rôle, Organisation, chemin de la Photo.
   - Sélectionner un template d'overlay et une couleur de thème.
   - Cliquer sur **Appliquer**.

3. **Via le fichier JSON** (édition manuelle) :
   Ajouter une entrée dans `talents/talents.json` :

   ```json
   {
     "talent_id": "marie_dupont",
     "name": "Marie Dupont",
     "role": "Journaliste",
     "organization": "VisionCast Media",
     "photo": "talents/marie_dupont.jpeg",
     "overlay": "overlays/templates/default.json",
     "theme_color": "#E81A73",
     "filters": {
       "brightness": 1.0,
       "contrast": 1.0,
       "saturation": 1.0
     },
     "animations": {
       "entry": "slide_left",
       "exit": "fade_out",
       "duration_ms": 500
     },
     "priority": 2
   }
   ```

4. **Encodage automatique** : Au démarrage, le module IA Python charge automatiquement les photos, calcule les encodages faciaux (vecteurs 128 dimensions) et les stocke en mémoire pour le matching en temps réel.

### 5.2 Modifier un talent

1. **Via la régie** : Sélectionner le talent dans le tableau, modifier les champs souhaités dans le panneau de détails, puis cliquer sur **Appliquer**.

2. **Via le fichier JSON** : Éditer directement l'entrée correspondante dans `talents/talents.json`.

> **Important** : Si vous modifiez la photo d'un talent, l'encodage facial sera recalculé au prochain démarrage du module IA.

### 5.3 Associer un overlay

Chaque talent peut être associé à un template d'overlay spécifique via le champ `overlay` :

```json
"overlay": "overlays/templates/modern_blue.json"
```

**Dans la régie** : Utiliser la liste déroulante **Template** dans le panneau Talents & Détails pour sélectionner parmi les templates disponibles (default, modern_blue, minimal).

Le template détermine :
- Le style visuel du lower third (couleurs, opacité, dimensions).
- Les animations d'entrée et de sortie.
- La durée d'affichage par défaut.
- La position du logo.

### 5.4 Associer un thème

Le champ `theme_color` définit la couleur d'accent personnalisée pour chaque talent :

```json
"theme_color": "#1A73E8"
```

**Dans la régie** : Saisir le code hexadécimal dans le champ **Couleur du thème** du panneau Talents & Détails. Un aperçu de la couleur est affiché à côté du champ.

Cette couleur peut être utilisée par les templates d'overlay pour personnaliser l'apparence des lower thirds par talent (bande de couleur, accent, bordure).

### 5.5 Associer des filtres globaux

Chaque talent possède un bloc `filters` qui définit des ajustements visuels spécifiques :

```json
"filters": {
  "brightness": 1.0,
  "contrast": 1.0,
  "saturation": 1.0
}
```

| Paramètre | Plage | Description |
|---|---|---|
| `brightness` | 0.0 – 2.0 | Luminosité (1.0 = neutre) |
| `contrast` | 0.0 – 2.0 | Contraste (1.0 = neutre) |
| `saturation` | 0.0 – 2.0 | Saturation (1.0 = neutre) |

Ces filtres sont appliqués en complément des filtres vidéo globaux lorsque le talent est reconnu à l'antenne.

---

## 6. Gestion des overlays

### 6.1 Templates

Les templates d'overlay sont des fichiers JSON situés dans `overlays/templates/`. Chaque template définit le style complet d'un lower third.

**Templates intégrés :**

| Template | Fichier | Style | Entrée | Sortie | Durée |
|---|---|---|---|---|---|
| **Default** | `default.json` | Bande noire classique, texte blanc | Slide gauche (500 ms) | Fade out (400 ms) | 5 000 ms |
| **Modern Blue** | `modern_blue.json` | Accent brun/orange, police plus grande | Slide haut (600 ms) | Slide bas (400 ms) | 7 000 ms |
| **Minimal** | `minimal.json` | Subtil, faible opacité, compact | Fade in (800 ms) | Fade out (600 ms) | 4 000 ms |

**Structure d'un template :**

```json
{
  "name": "nom_du_template",
  "description": "Description du template",
  "style": {
    "background_opacity": 0.6,
    "background_color": [0, 0, 0],
    "text_color": [255, 255, 255],
    "subtitle_color": [200, 200, 200],
    "font_scale": 1.0,
    "thickness": 2,
    "band_height_ratio": 0.18,
    "animation": "slide_left",
    "animation_duration_ms": 500,
    "exit_animation": "fade_out",
    "exit_animation_duration_ms": 400,
    "easing": "ease_in_out",
    "display_duration_ms": 5000
  },
  "layout": {
    "title_offset_x": 40,
    "title_offset_y": 50,
    "subtitle_offset_x": 40,
    "subtitle_offset_y": 90,
    "logo_position": "bottom_right",
    "logo_path": ""
  },
  "transition": {
    "type": "fade",
    "duration_ms": 500,
    "easing": "ease_in_out"
  }
}
```

**Créer un template personnalisé :**

1. Dupliquer un template existant dans `overlays/templates/`.
2. Modifier les paramètres de style, layout et transition selon vos besoins.
3. Nommer le fichier (ex. : `custom_newscast.json`).
4. Ajouter le nom dans `config/design.json` → `templates.available`.
5. Le template apparaîtra dans les listes déroulantes de la régie.

### 6.2 Thèmes

Les thèmes contrôlent l'apparence globale de l'interface et des overlays :

| Thème | Description |
|---|---|
| **Dark** | Fond sombre, texte clair — adapté aux régies en faible luminosité |
| **Light** | Fond clair, texte sombre — pour une meilleure lisibilité en studio éclairé |
| **Broadcast** | Thème professionnel broadcast standard |
| **Custom** | Thème personnalisé via les couleurs primaire et accent |

**Changer de thème :** Panneau Design & Overlays → Onglet **Thèmes** → Sélectionner dans la liste déroulante.

### 6.3 Animations

Le GPU Compositor supporte les animations suivantes pour les overlays :

**Animations d'entrée :**

| Animation | Effet |
|---|---|
| `fade_in` | Apparition progressive (opacité 0 → 1) |
| `slide_left` | Glissement depuis la gauche |
| `slide_right` | Glissement depuis la droite |
| `slide_up` | Glissement depuis le bas |
| `slide_down` | Glissement depuis le haut |
| `scale_in` | Zoom progressif (petit → taille normale) |

**Animations de sortie :**

| Animation | Effet |
|---|---|
| `fade_out` | Disparition progressive (opacité 1 → 0) |
| `slide_left` | Glissement vers la gauche |
| `slide_right` | Glissement vers la droite |
| `slide_up` | Glissement vers le haut |
| `slide_down` | Glissement vers le bas |
| `scale_out` | Réduction progressive (taille normale → petit) |

**Courbes d'accélération (easing) :**

| Easing | Description |
|---|---|
| `linear` | Vitesse constante |
| `ease_in` | Démarrage lent, accélération |
| `ease_out` | Démarrage rapide, décélération |
| `ease_in_out` | Démarrage lent, accélération, décélération |

**Types de transition entre overlays :**

| Transition | Description |
|---|---|
| `cut` | Changement instantané |
| `fade` | Fondu enchaîné |
| `dissolve` | Dissolution progressive |
| `wipe` | Balayage (gauche, droite, haut, bas) |

### 6.4 Logos

Le logo est un élément graphique (PNG avec transparence alpha recommandé) positionné sur l'overlay.

**Configuration :**

- **Chemin du logo** : `config/design.json` → `logo.path` ou champ `layout.logo_path` du template.
- **Position** : `bottom_right`, `bottom_left`, `top_right`, `top_left`.

**Depuis la régie** : Panneau Design & Overlays → Onglet **Logos** → Saisir le chemin ou cliquer sur **Parcourir**.

Le GPU Compositor supporte la composition PNG avec canal alpha pour une intégration transparente du logo sur le flux vidéo.

### 6.5 Couleurs

Les couleurs des overlays se configurent à deux niveaux :

**Couleurs globales** (Panneau Design → Onglet Couleurs) :

| Paramètre | Description | Format |
|---|---|---|
| **Couleur primaire** | Couleur de fond principale | Hexadécimal (#RRGGBB) |
| **Couleur d'accent** | Couleur d'accentuation | Hexadécimal (#RRGGBB) |

**Couleurs par template** (fichiers JSON) :

| Paramètre | Description | Format |
|---|---|---|
| `background_color` | Couleur de fond du lower third | Tableau BGR [B, G, R] |
| `text_color` | Couleur du texte principal | Tableau BGR [B, G, R] |
| `subtitle_color` | Couleur du sous-titre | Tableau BGR [B, G, R] |
| `background_opacity` | Opacité du fond | Flottant (0.0 – 1.0) |

> **Attention** : Les couleurs dans les templates JSON sont au format **BGR** (Blue, Green, Red), conformément à la convention OpenCV utilisée par le moteur vidéo. Les couleurs de thème dans `talents.json` sont au format **hexadécimal RGB** standard (#RRGGBB).

---

## 7. Sorties vidéo

VisionCast‑AI supporte plusieurs types de sortie simultanés, configurés dans `config/system.json` sous la section `output`.

### 7.1 SDI (DeckLink / AJA)

La sortie SDI est la méthode standard pour la diffusion broadcast professionnelle.

**Configuration :**

```json
"primary": {
  "type": "decklink",
  "device_index": 0,
  "mode": "2160p25"
}
```

| Paramètre | Description | Valeurs |
|---|---|---|
| `type` | Type de périphérique | `decklink`, `aja` |
| `device_index` | Index du périphérique (si plusieurs cartes) | `0`, `1`, `2`… |
| `mode` | Mode vidéo de sortie | `1080p25`, `1080p50`, `2160p25`, `2160p50`… |

**Caractéristiques SDI :**

- Format pixel : V210 (10-bit 4:2:2) — standard broadcast.
- Connecteurs : SDI (BNC), HDMI.
- Support genlock / référence externe.
- Latence minimale (< 1 trame).

**Depuis la régie** : Panneau **Output Config** → Sélectionner le périphérique, la résolution, la cadence et le format.

### 7.2 NDI

NDI (Network Device Interface) permet la distribution vidéo sur le réseau local avec une latence minimale.

**Configuration :**

```json
"ndi": {
  "enabled": true,
  "source_name": "VisionCast Program"
}
```

| Paramètre | Description |
|---|---|
| `enabled` | Active/désactive la sortie NDI |
| `source_name` | Nom de la source tel qu'il apparaîtra sur le réseau |

**Caractéristiques NDI :**

- Transport réseau IP (TCP/UDP).
- Découverte automatique des sources sur le LAN.
- Supporte les métadonnées intégrées.
- Latence : < 1 trame sur réseau Gigabit.
- Compatible avec tous les logiciels NDI (OBS, vMix, Wirecast, etc.).

**Vérification** : Utiliser **NDI Studio Monitor** (inclus dans NDI Tools) pour visualiser la sortie VisionCast sur le réseau.

### 7.3 SRT

SRT (Secure Reliable Transport) est un protocole de streaming chiffré à faible latence, adapté à la contribution à distance.

**Configuration :**

```json
"srt": {
  "enabled": false,
  "destination": "srt://remote-server:9000",
  "latency_ms": 120
}
```

| Paramètre | Description | Défaut |
|---|---|---|
| `enabled` | Active/désactive la sortie SRT | `false` |
| `destination` | URL du serveur SRT de destination | — |
| `latency_ms` | Latence du buffer SRT en millisecondes | `120` |

**Caractéristiques SRT :**

- Chiffrement AES 128/192/256.
- Correction d'erreurs (ARQ) pour les réseaux instables.
- Latence configurable selon la qualité du réseau.
- Idéal pour la contribution à distance et les liaisons internet.

### 7.4 RTMP

RTMP (Real-Time Messaging Protocol) est utilisé pour le streaming vers les plateformes en ligne (YouTube Live, Facebook Live, etc.).

**Configuration :**

```json
"rtmp": {
  "enabled": false,
  "server_url": "rtmp://live.youtube.com/app",
  "stream_key": "votre-clé-de-stream"
}
```

| Paramètre | Description |
|---|---|
| `enabled` | Active/désactive la sortie RTMP |
| `server_url` | URL du serveur RTMP de la plateforme |
| `stream_key` | Clé de stream fournie par la plateforme |

**Caractéristiques RTMP :**

- Compatible YouTube Live, Facebook Live, Twitch, et toute plateforme RTMP.
- Encodage H.264/H.265 configurable.
- Adapté au streaming public en direct.

**Sortie simultanée** : VisionCast‑AI peut émettre vers plusieurs sorties en parallèle (ex. : SDI pour la régie + NDI pour le monitoring + RTMP pour le web).

---

### 7.5 Multi-Streaming (plusieurs plateformes simultanément)

VisionCast-AI permet de diffuser **simultanément** sur YouTube Live, Facebook Live, Twitch, et toute destination RTMP personnalisée, **sans interruption du signal TV** (SDI/NDI/SRT).

#### Accès au panneau

Le panneau **MULTI-STREAMING** est visible dans la barre du bas de l'interface, entre le panneau de monitoring et la configuration des sorties.

#### Démarrage rapide

**1. Configurer une plateforme :**

- Cliquez sur le bouton de la plateforme souhaitée (**YouTube**, **Facebook**, **Twitch**) dans la barre "Add:" en bas du panneau.
- La plateforme apparaît dans la liste avec l'URL pré-remplie.
- Cliquez sur **▼** pour déplier la ligne et vérifier l'URL.

**2. Saisir la clé de stream :**

- Ouvrez le dialogue d'édition en cliquant sur **+ Custom** ou utilisez `bridge.updateRtmpStream()`.
- Collez la clé de stream fournie par la plateforme (YouTube Studio, Facebook Live, Twitch Dashboard).

**3. Lancer le stream :**

- Cliquez sur **Start** pour démarrer la diffusion vers cette plateforme.
- Le statut passe de `IDLE` → `CONNECTING` → `LIVE` (indicateur vert).

**4. Surveiller :**

- Le compteur de streams actifs s'affiche dans l'en-tête.
- Dépliez la ligne (bouton **▼**) pour voir les derniers logs temps réel.
- En cas d'erreur, l'indicateur passe au rouge avec le message d'erreur.

**5. Arrêter :**

- Cliquez sur **Stop** pour arrêter un stream individuel.
- Cliquez sur **Stop All** pour arrêter tous les streams d'un coup.

#### Ajouter une destination personnalisée

Cliquez sur **+ Custom** pour ouvrir le dialogue :

| Champ | Description | Exemple |
|-------|-------------|---------|
| Name | Nom d'affichage | `Ma Chaîne` |
| RTMP URL | Adresse du serveur | `rtmp://live.example.com/app` |
| Stream Key | Clé de publication (masquée) | `xxxx-xxxx-xxxx` |

#### URLs RTMP des plateformes principales

| Plateforme | URL RTMP | Où trouver la clé |
|------------|----------|------------------|
| YouTube Live | `rtmp://a.rtmp.youtube.com/live2` | YouTube Studio → Live |
| Facebook Live | `rtmp://live-api-s.facebook.com:80/rtmp` | Meta Business Suite → Live |
| Twitch | `rtmp://live.twitch.tv/app` | Twitch → Creator Dashboard → Stream Key |
| Restream | `rtmp://live.restream.io/live` | Restream Dashboard |
| Custom | (votre serveur) | (votre config) |

#### Indicateurs de statut

| Couleur | Statut | Signification |
|---------|--------|---------------|
| 🟢 Vert | `LIVE` | Stream actif, connexion établie |
| 🟡 Orange (clignotant) | `CONNECTING` | Connexion en cours |
| 🔴 Rouge | `ERROR` | Échec (URL invalide, clé incorrecte, réseau) |
| ⚫ Gris | `IDLE` | Stream arrêté, prêt à démarrer |

#### Gestion des erreurs

En cas d'erreur sur un stream :
- Les autres streams ne sont pas affectés (chaque stream est indépendant).
- Le message d'erreur est visible dans la zone de log dépliée.
- Corrigez l'URL/clé et cliquez à nouveau sur **Start**.

#### Sauvegarder la configuration

Cliquez sur **Save** dans l'en-tête du panneau pour persister la liste des streams.  
La configuration est aussi accessible dans `config/system.json` → `output.rtmp_streams`.

---

## 8. Dépannage

### 8.1 L'IA ne reconnaît pas les talents

| Symptôme | Cause probable | Solution |
|---|---|---|
| Aucun visage détecté | Éclairage insuffisant | Améliorer l'éclairage du plateau. Vérifier l'exposition caméra. |
| Aucun visage détecté | Résolution trop basse | Augmenter la résolution de capture ou réduire `process_every_n_frames`. |
| Visage détecté mais non reconnu | Tolérance trop stricte | Augmenter `ai.tolerance` dans `config/system.json` (ex. : de `0.45` à `0.55`). |
| Visage détecté mais mauvais talent | Tolérance trop permissive | Diminuer `ai.tolerance` (ex. : de `0.45` à `0.35`). |
| Reconnaissance lente | Modèle trop lourd | Passer de `cnn` à `hog` dans `ai.model`, ou augmenter `process_every_n_frames`. |
| Talent non reconnu | Photo inadaptée | Utiliser une photo de face, bien éclairée, 512×512 px minimum. Éviter les photos de profil. |
| Module IA ne démarre pas | Dépendances manquantes | Vérifier `pip install -r python/requirements.txt`. Vérifier que `face_recognition` et `dlib` sont installés. |

**Vérification dans les logs** : Consulter l'onglet **IA** dans le panneau Logs & Monitoring pour les messages d'erreur détaillés.

### 8.2 L'overlay ne s'affiche pas

| Symptôme | Cause probable | Solution |
|---|---|---|
| Aucun overlay visible | Template non trouvé | Vérifier le chemin `overlay` dans `talents/talents.json`. Le fichier JSON du template doit exister dans `overlays/templates/`. |
| Aucun overlay visible | GPU Compositor non initialisé | Vérifier les logs Engine. S'assurer que le moteur vidéo est démarré (bouton Start Engine). |
| Overlay clignotant | Reconnaissance instable | Augmenter `process_every_n_frames` pour stabiliser la détection. Ajuster la tolérance. |
| Overlay mal positionné | Paramètres layout incorrects | Vérifier `title_offset_x`, `title_offset_y`, `band_height_ratio` dans le template JSON. |
| Animation absente | Type d'animation invalide | Vérifier que les valeurs `animation` et `exit_animation` sont parmi : `fade_in`, `fade_out`, `slide_left`, `slide_right`, `slide_up`, `slide_down`, `scale_in`, `scale_out`. |
| Logo absent | Chemin invalide | Vérifier `logo_path` dans le template ou `logo.path` dans `config/design.json`. Utiliser un fichier PNG avec canal alpha. |

### 8.3 Les filtres vidéo ne sont pas appliqués

| Symptôme | Cause probable | Solution |
|---|---|---|
| Aucun filtre actif | Filtres désactivés | Vérifier que les filtres souhaités sont cochés dans le panneau Filtres vidéo de la régie, ou que `enabled: true` est défini dans `config/video_filters.json`. |
| LUT cinéma non appliquée | Fichier LUT manquant | Vérifier `lut_path` dans `config/video_filters.json`. Le fichier LUT 3D doit exister à l'emplacement indiqué. |
| Filtre sans effet visible | Paramètres trop faibles | Augmenter les valeurs : `strength` pour sharpen/noise, `clip_limit` pour le contraste, `intensity` pour le LUT. |
| Filtres appliqués dans le mauvais ordre | Ordre de la chaîne | Vérifier `filter_order` dans `config/video_filters.json`. L'ordre recommandé est : noise_reduction → cinema_lut → hdr_tonemap → dynamic_contrast → sharpen. |
| Performance dégradée avec filtres | Trop de filtres actifs | Désactiver les filtres non essentiels. Réduire la résolution du moteur. Utiliser le GPU si disponible. |

### 8.4 Problèmes DeckLink / NDI

#### Problèmes DeckLink

| Symptôme | Cause probable | Solution |
|---|---|---|
| Carte non détectée | Driver non installé | Installer **Blackmagic Desktop Video** et redémarrer. |
| Carte non détectée | Conflit matériel | Vérifier le slot PCIe. Essayer un autre slot. Vérifier la compatibilité de la carte mère. |
| Pas de signal en sortie | Mauvais mode vidéo | Vérifier que le mode (`2160p25`, `1080p50`…) correspond aux capacités de la carte et du moniteur de destination. |
| Pas de signal en sortie | Mauvais connecteur | Vérifier la connexion SDI/HDMI. Tester avec un autre câble. |
| Signal instable | Pas de genlock | Configurer la référence genlock dans Blackmagic Desktop Video Setup si le système nécessite une synchronisation externe. |
| `device_index` incorrect | Plusieurs cartes installées | Ouvrir Desktop Video Setup pour identifier l'index correct de chaque carte. |

#### Problèmes NDI

| Symptôme | Cause probable | Solution |
|---|---|---|
| Source NDI non visible | NDI Runtime absent | Installer **NDI Tools** (inclut le NDI Runtime). |
| Source NDI non visible | Pare-feu bloquant | Ouvrir les ports NDI dans le pare-feu (TCP/UDP, plage dynamique). Autoriser `visioncast_engine` dans les règles du pare-feu. |
| Source NDI non visible | Réseau différent | Vérifier que les machines source et destination sont sur le même sous-réseau / VLAN. |
| Latence élevée | Réseau saturé | Passer à un réseau Gigabit dédié. Réduire la résolution NDI si nécessaire. |
| Image dégradée | Bande passante insuffisante | Utiliser un réseau 1 Gbps minimum. Vérifier qu'aucun autre trafic ne sature le réseau. |

**Outil de diagnostic NDI** : Utiliser **NDI Studio Monitor** pour vérifier la présence et la qualité des flux NDI sur le réseau.

---

## Annexe — Référence rapide des fichiers de configuration

| Fichier | Emplacement | Rôle |
|---|---|---|
| `config/system.json` | Configuration système | Moteur, IA, sorties, UI |
| `config/design.json` | Configuration design | Templates, thèmes, couleurs, filtres |
| `config/video_filters.json` | Filtres vidéo | Paramètres détaillés de chaque filtre |
| `talents/talents.json` | Base de talents | Identités, photos, overlays, animations |
| `overlays/templates/*.json` | Templates d'overlay | Style, layout, animations, transitions |

---

*VisionCast‑AI — Système de production broadcast intelligent*
*© VisionCast Media — Tous droits réservés*
