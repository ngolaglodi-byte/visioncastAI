# VisionCast‑AI — Documentation Officielle

> **Version** : 0.1.0  
> **Développé par** : Prestige Technologie Company  
> **Architecte & Directeur technique** : Glody Dimputu Ngola  
> **Classification** : Document professionnel — Destiné aux chaînes TV, studios de production, techniciens broadcast et intégrateurs

---

## Table des matières

1. [Présentation générale](#1-présentation-générale)
2. [Architecture globale du système](#2-architecture-globale-du-système)
3. [Installation et configuration](#3-installation-et-configuration)
4. [Workflow broadcast complet](#4-workflow-broadcast-complet)
5. [Module IA Python](#5-module-ia-python)
6. [Moteur vidéo C++ (Vision Engine)](#6-moteur-vidéo-c-vision-engine)
7. [Régie Qt (VisionCast Control Room)](#7-régie-qt-visioncast-control-room)
8. [SDK Broadcast](#8-sdk-broadcast)
9. [Sorties vidéo](#9-sorties-vidéo)
10. [Dépannage & FAQ](#10-dépannage--faq)
11. [Mentions officielles](#11-mentions-officielles)

---

## 1. Présentation générale

### 1.1 Qu'est‑ce que VisionCast‑AI ?

**VisionCast‑AI** est un système de production broadcast en direct alimenté par l'intelligence artificielle. Il combine la reconnaissance faciale en temps réel, la génération automatique d'habillages (lower thirds), le compositing GPU et la gestion multi-caméras au sein d'une plateforme unifiée et professionnelle.

> *VisionCast‑AI est une technologie développée par Prestige Technologie Company, architecturée et dirigée par Glody Dimputu Ngola.*

### 1.2 Objectifs du système

| Objectif | Description |
|----------|-------------|
| **Automatisation IA** | Détection et reconnaissance faciale en temps réel pour identifier les talents à l'antenne |
| **Habillage automatique** | Génération et affichage automatiques des lower thirds avec nom, rôle et organisation |
| **Qualité broadcast** | Pipeline vidéo GPU avec filtres cinéma, LUTs, HDR tonemapping et résolution 4K |
| **Intégration matérielle** | Support natif des cartes SDI/HDMI (Blackmagic DeckLink, AJA, Magewell) et du protocole NDI |
| **Diffusion multi-sorties** | Sorties simultanées SDI, NDI, SRT et RTMP |
| **Interface opérateur** | Régie Qt complète pour le contrôle en temps réel de la production |

### 1.3 Positionnement : IA + Broadcast + Automatisation

VisionCast‑AI se positionne à l'intersection de trois domaines :

- **Intelligence Artificielle** — Reconnaissance faciale basée sur `face_recognition` (dlib) avec suivi temporel, lissage de confiance et gestion multi-visages
- **Broadcast professionnel** — Compatible avec les standards SDI, NDI, SRT, RTMP et les résolutions 1080p/4K à 25, 29.97, 50 et 59.94 fps
- **Automatisation de production** — Réduction des interventions manuelles grâce à la détection automatique des talents et à l'application dynamique des habillages

---

## 2. Architecture globale du système

### 2.1 Pipeline complet

```
┌─────────────┐    ┌──────────────────┐    ┌──────────────────────────┐    ┌───────────────┐
│  Caméras    │───▶│  Table de régie  │───▶│     VisionCast‑AI        │───▶│ Sortie finale │
│ (SDI/HDMI/  │    │  (switcher)      │    │                          │    │               │
│  NDI)       │    │                  │    │  ┌──────┐ ┌───────────┐  │    │  • SDI        │
└─────────────┘    └──────────────────┘    │  │ IA   │ │ Moteur    │  │    │  • HDMI       │
                                           │  │Python│ │ vidéo C++ │  │    │  • NDI        │
                                           │  └──┬───┘ └─────┬─────┘  │    │  • SRT        │
                                           │     │  ZeroMQ   │        │    │  • RTMP       │
                                           │     └─────┬─────┘        │    │               │
                                           │     ┌─────┴─────┐        │    └───────────────┘
                                           │     │  Régie Qt  │        │
                                           │     └───────────┘        │
                                           └──────────────────────────┘
```

### 2.2 Schéma détaillé de l'architecture interne

```
╔══════════════════════════════════════════════════════════════════════════════╗
║                          VisionCast‑AI — Architecture                      ║
╠══════════════════════════════════════════════════════════════════════════════╣
║                                                                            ║
║  ┌─────────────────────────────────────────────────────────────────────┐   ║
║  │                      ENTRÉES VIDÉO                                  │   ║
║  │  DeckLink Input │ AJA Input │ Magewell Input │ NDI Input │ Webcam  │   ║
║  └────────────────────────────┬────────────────────────────────────────┘   ║
║                               │                                            ║
║                               ▼                                            ║
║  ┌────────────────────────────────────────────────────────────────────┐    ║
║  │                    CAPTURE MANAGER (C++)                           │    ║
║  │  Gère les sources vidéo multiples, décodage, format unifié BGRA8  │    ║
║  └────────────────────────────┬───────────────────────────────────────┘    ║
║                               │                                            ║
║               ┌───────────────┼───────────────┐                            ║
║               │               │               │                            ║
║               ▼               ▼               ▼                            ║
║  ┌────────────────┐  ┌──────────────┐  ┌──────────────────┐               ║
║  │ MODULE IA      │  │ FILTER CHAIN │  │ OVERLAY RENDERER │               ║
║  │ (Python)       │  │ (C++)        │  │ (C++)            │               ║
║  │                │  │              │  │                  │               ║
║  │ • Détection    │  │ • LUT cinéma │  │ • Lower thirds   │               ║
║  │ • Encodage     │  │ • Sharpen    │  │ • Logos           │               ║
║  │ • Reconnaissance│ │ • Noise red. │  │ • Animations      │               ║
║  │ • Suivi        │  │ • HDR tonemap│  │ • Templates JSON  │               ║
║  │ • talents.json │  │ • Contraste  │  │ • Timer affichage │               ║
║  └───────┬────────┘  └──────┬───────┘  └────────┬─────────┘               ║
║          │                  │                    │                          ║
║          │ ZeroMQ           │                    │                          ║
║          │ (JSON metadata)  │                    │                          ║
║          ▼                  ▼                    ▼                          ║
║  ┌────────────────────────────────────────────────────────────────────┐    ║
║  │                    PIPELINE MANAGER (C++)                          │    ║
║  │  Capture → Filtres → Overlays → Compositing GPU → Sortie          │    ║
║  └────────────────────────────┬───────────────────────────────────────┘    ║
║                               │                                            ║
║               ┌───────────────┼───────────────┐                            ║
║               │               │               │                            ║
║               ▼               ▼               ▼                            ║
║  ┌────────────────┐  ┌──────────────┐  ┌──────────────────┐               ║
║  │ GPU COMPOSITOR │  │ VISION       │  │ RÉGIE Qt         │               ║
║  │ (OpenGL 3.3)   │  │ ENGINE       │  │ (Control Room)   │               ║
║  │                │  │              │  │                  │               ║
║  │ • Textures     │  │ • Preview    │  │ • Sources vidéo  │               ║
║  │ • Alpha blend  │  │ • Render loop│  │ • Design/Overlays│               ║
║  │ • Animations   │  │ • GPU pipeline│ │ • Filtres        │               ║
║  │ • Transitions  │  │ • Capture    │  │ • Reconnaissance │               ║
║  └────────────────┘  └──────────────┘  │ • Monitoring     │               ║
║                                        └──────────────────┘               ║
║                               │                                            ║
║                               ▼                                            ║
║  ┌────────────────────────────────────────────────────────────────────┐    ║
║  │                      SORTIES VIDÉO                                 │    ║
║  │  DeckLink Output │ AJA Output │ NDI Output │ SRT Output │ RTMP    │    ║
║  └────────────────────────────────────────────────────────────────────┘    ║
║                                                                            ║
╚══════════════════════════════════════════════════════════════════════════════╝
```

### 2.3 Rôle de chaque module

#### 2.3.1 Module IA Python (`python/`)

Le module IA Python est responsable de la détection et de la reconnaissance faciale en temps réel.

| Composant | Fichier | Rôle |
|-----------|---------|------|
| Détection | `ai/detection.py` | Détection multi-visages (modèle HOG ou CNN), downscaling, skip-frame |
| Encodage | `ai/encoding.py` | Génération d'encodages faciaux 128 dimensions |
| Reconnaissance | `ai/recognition.py` | Matching contre la base de talents, suivi temporel, lissage EMA |
| Base de talents | `ai/talents_loader.py` | Chargement concurrent, cache disque SHA-256, hot-reload |
| Protocole IPC | `ipc/protocol.py` | Messages JSON : `TalentOverlayMessage`, `RecognitionResult`, `Heartbeat`, `LogMessage` |
| Transport | `ipc/zmq_sender.py` | Publication ZeroMQ PUB/SUB sur `tcp://127.0.0.1:5557` |
| Monitoring | `monitoring/system_monitor.py` | Surveillance des performances système |

#### 2.3.2 Moteur vidéo C++ (`engine/`)

Le moteur vidéo C++ gère le pipeline de traitement vidéo en temps réel avec accélération GPU.

| Composant | Fichier | Rôle |
|-----------|---------|------|
| Capture | `capture_manager.h/.cpp` | Gestion des sources vidéo multiples (caméras, SDI, NDI, fichiers) |
| Pipeline | `pipeline_manager.h/.cpp` | Orchestration : capture → filtres → overlays → sortie |
| Filtres | `filter_chain.h/.cpp`, `video_filters.h/.cpp` | Chaîne de filtres séquentielle : LUT, sharpen, noise, HDR, contraste |
| Overlays | `overlay_renderer.h/.cpp` | Composition des lower thirds, logos, métadonnées IA |
| GPU | `gpu_compositor.h/.cpp` | Compositing OpenGL 3.3 : textures, alpha blending, animations, transitions |
| Engine | `vision_engine.h/.cpp` | Moteur principal : capture, preview, pipeline GPU, boucle de rendu |
| ZMQ | `zmq_receiver.h/.cpp`, `metadata_receiver.h/.cpp` | Réception des métadonnées IA via ZeroMQ (thread-safe) |

#### 2.3.3 Régie Qt (`ui/`)

L'interface opérateur Qt fournit un contrôle complet de la production en direct.

| Panneau | Fichier | Rôle |
|---------|---------|------|
| Fenêtre principale | `main_window.h/.cpp` | Fenêtre dockable avec panneaux modulaires |
| Preview | `preview_panel.h/.cpp` | Aperçu vidéo et multiview 2×2 |
| Control Room | `control_room.h/.cpp` | Sélection des sources, démarrage/arrêt du moteur |
| Design | `design_panel.h/.cpp` | Configuration des lower thirds, templates, thèmes, couleurs, filtres |
| Overlays | `overlay_editor.h/.cpp` | Éditeur de templates d'overlay |
| Talents | `talent_manager.h/.cpp` | Gestion de la base de données de talents |
| Reconnaissance | `recognition_panel.h/.cpp` | Affichage des détections IA en temps réel |
| Monitoring | `monitoring_panel.h/.cpp` | Surveillance système et logs |
| Sorties | `output_config.h/.cpp` | Configuration des sorties et encodeurs |

#### 2.3.4 SDK Broadcast (`sdk/`)

La couche d'abstraction SDK unifie l'accès aux périphériques broadcast professionnels.

| Périphérique | Fichiers | Rôle |
|-------------|----------|------|
| Blackmagic DeckLink | `decklink_device.h`, `decklink_input.h`, `decklink_output.h` | Capture/sortie SDI et HDMI |
| AJA | `aja_device.h`, `aja_input.h`, `aja_output.h` | Capture/sortie AJA |
| Magewell | `magewell_device.h`, `magewell_input.h` | Capture Magewell |
| NDI | `ndi_device.h`, `ndi_input.h`, `ndi_output.h` | Entrée/sortie réseau NDI |
| SRT | `srt_output.h` | Sortie streaming SRT |
| RTMP | `rtmp_output.h` | Sortie streaming RTMP |

Tous les périphériques implémentent l'interface abstraite `IVideoDevice` avec le patron PIMPL.

### 2.4 Communication ZeroMQ

Le module IA Python et le moteur C++ communiquent via **ZeroMQ** en mode PUB/SUB :

| Canal | Endpoint | Direction | Contenu |
|-------|----------|-----------|---------|
| Publication | `tcp://127.0.0.1:5557` | Python → C++ | Métadonnées de reconnaissance, heartbeat |
| Configuration | `tcp://127.0.0.1:5556` | C++ → Python | Commandes de configuration |

**Topics ZeroMQ** :
- `face.result` — Résultats de reconnaissance faciale par frame
- `face.heartbeat` — Messages de santé du module IA
- `config.*` — Messages de configuration
- `monitor.*` — Messages de monitoring système

Les messages sont sérialisés en **JSON** pour l'interopérabilité.

### 2.5 Gestion des métadonnées

Les métadonnées de reconnaissance sont transmises via le message `RecognitionResult` :

```
RecognitionResult
├── timestamp_ms      (horodatage de la frame)
├── frame_id          (identifiant unique de la frame)
└── faces[]           (liste de visages reconnus)
    ├── location      (top, right, bottom, left)
    ├── talent        (id, name, role, overlay_template)
    └── confidence    (score de confiance 0.0–1.0)
```

Le moteur C++ reçoit ces métadonnées via `ZmqReceiver` / `MetadataReceiver` et les transmet à l'`OverlayRenderer` pour application sur la frame vidéo.

### 2.6 Gestion des overlays

Les overlays sont définis par des **templates JSON** situés dans `overlays/templates/` :

- **Style** : couleur de fond, opacité, couleur du texte, taille de police, épaisseur
- **Animation d'entrée** : `slide_left`, `slide_up`, `fade_in`, `scale_up`
- **Animation de sortie** : `fade_out`, `slide_down`
- **Transition** : type (`cut`, `fade`, `wipe`, `dissolve`), durée, easing
- **Layout** : position, hauteur de bande, marges

### 2.7 Gestion des filtres vidéo globaux

Les filtres vidéo sont configurés dans `config/video_filters.json` et appliqués séquentiellement par la `FilterChain` :

| Ordre | Filtre | Description |
|-------|--------|-------------|
| 1 | Noise Reduction | Filtre bilatéral préservant les contours |
| 2 | Cinema LUT | Color grading cinéma (highlights chauds, shadows teal) |
| 3 | HDR Tonemap | Mapping HDR→SDR avec gamma et saturation |
| 4 | Dynamic Contrast | Enhancement adaptatif CLAHE |
| 5 | Sharpen | Netteté unsharp-mask |

### 2.8 Gestion de la durée d'affichage des lower thirds

La durée d'affichage est configurable à trois niveaux :

1. **Par template** : champ `display_duration_ms` dans chaque template JSON (ex. 5000 ms par défaut)
2. **Par talent** : champ `animations.duration_ms` dans `talents.json`
3. **Via la régie Qt** : panneau Design avec contrôle de la durée

Le timer est géré par le `GpuCompositor` qui suit l'état d'animation de chaque couche (`CompositingLayer`) avec les phases : entrée → maintien → sortie.

---

## 3. Installation et configuration

### 3.1 Prérequis système

| Composant | Version minimale |
|-----------|-----------------|
| Python | 3.11+ |
| CMake | 3.20+ |
| Compilateur C++ | C++17 (GCC 10+, MSVC 2019+, Clang 12+) |
| OpenCV | 4.8+ |
| Qt | 5.15+ ou 6.x |
| dlib | 19.24+ |
| ZeroMQ | 4.3+ |

### 3.2 Installation Windows

```powershell
# 1. Cloner le dépôt
git clone https://github.com/prestige-technologie/visioncast-ai.git
cd visioncast-ai

# 2. Installer les dépendances Python
python -m pip install --upgrade pip
pip install -r python/requirements.txt

# 3. Compiler le moteur C++ et la régie Qt
mkdir build && cd build
cmake .. -G "Visual Studio 17 2022" -A x64
cmake --build . --config Release

# 4. Vérifier l'installation
.\build\bin\Release\visioncast_engine.exe --version
```

### 3.3 Installation macOS

```bash
# 1. Installer les dépendances système via Homebrew
brew install cmake opencv qt@6 zeromq dlib glfw glew freetype

# 2. Cloner le dépôt
git clone https://github.com/prestige-technologie/visioncast-ai.git
cd visioncast-ai

# 3. Installer les dépendances Python
python3 -m pip install -r python/requirements.txt

# 4. Compiler le moteur C++ et la régie Qt
mkdir build && cd build
cmake .. -DCMAKE_PREFIX_PATH=$(brew --prefix qt@6)
cmake --build . --config Release

# 5. Vérifier l'installation
./build/bin/visioncast_engine --version
```

### 3.4 Installation des drivers

#### 3.4.1 Blackmagic DeckLink

1. Télécharger **Desktop Video** depuis [blackmagicdesign.com/support/family/capture-and-playback](https://www.blackmagicdesign.com/support/family/capture-and-playback)
2. Installer le package correspondant à votre OS
3. Redémarrer la machine
4. Vérifier la détection via **Blackmagic Desktop Video Setup**
5. Placer les headers du SDK DeckLink dans `sdk/decklink/include/`

```
sdk/decklink/include/
├── DeckLinkAPI.h
├── DeckLinkAPIConfiguration.h
└── DeckLinkAPITypes.h
```

#### 3.4.2 AJA

1. Télécharger **AJA NTV2 SDK** depuis [aja.com/support](https://www.aja.com/support)
2. Installer les drivers AJA pour votre carte (Corvid, KONA, Io)
3. Placer les headers dans `sdk/aja/include/`
4. Vérifier via **AJA Control Panel**

#### 3.4.3 Magewell

1. Télécharger le SDK depuis [magewell.com/downloads](https://www.magewell.com/downloads)
2. Installer les drivers USB ou PCIe
3. Placer les headers dans `sdk/magewell/include/`
4. Vérifier via **Magewell Capture Express**

#### 3.4.4 NDI Tools

1. Télécharger **NDI Tools** depuis [ndi.video/tools](https://ndi.video/tools/)
2. Installer NDI Runtime et NDI Tools
3. Placer les headers dans `sdk/ndi/include/`
4. Vérifier avec **NDI Studio Monitor** que les sources NDI sont visibles sur le réseau

### 3.5 Configuration initiale

Le fichier de configuration principal est `config/system.json` :

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
    "primary": {
      "type": "decklink",
      "device_index": 0,
      "mode": "2160p25"
    },
    "ndi": {
      "enabled": true,
      "name": "VisionCast Program"
    },
    "srt": {
      "enabled": false,
      "url": "srt://localhost:9000"
    },
    "rtmp": {
      "enabled": false,
      "url": "rtmp://live.example.com/app",
      "key": "stream_key"
    }
  },
  "ui": {
    "preview_resolution": { "width": 1920, "height": 1080 },
    "multiview_layout": "2x2",
    "monitoring_refresh_ms": 1000
  }
}
```

### 3.6 Vérification du matériel

| Étape | Commande / Action | Résultat attendu |
|-------|-------------------|------------------|
| DeckLink détectée | Blackmagic Desktop Video Setup | Carte listée avec firmware à jour |
| NDI visible | NDI Studio Monitor | Source « VisionCast Program » visible |
| Webcam accessible | `python3 -c "import cv2; print(cv2.VideoCapture(0).isOpened())"` | `True` |
| GPU OpenGL | `glxinfo \| grep "OpenGL version"` (Linux/macOS) | Version 3.3+ |

### 3.7 Test du pipeline vidéo

```bash
# 1. Démarrer le module IA
cd python && python main.py &

# 2. Démarrer le moteur vidéo
./build/bin/visioncast_engine

# 3. Démarrer la régie Qt
./build/bin/visioncast_ui
```

Vérifier dans la régie :
- La source vidéo apparaît dans le panneau Preview
- Les détections IA s'affichent dans le panneau Reconnaissance
- Les lower thirds apparaissent automatiquement sur les talents reconnus

---

## 4. Workflow broadcast complet

### 4.1 Flux de production étape par étape

```
Étape 1          Étape 2              Étape 3             Étape 4
┌──────────┐     ┌──────────────┐     ┌───────────────┐   ┌──────────────┐
│ Caméras  │────▶│ Table de     │────▶│ VisionCast‑AI │──▶│ Sorties      │
│ filment  │     │ régie choisit│     │ traite le flux│   │ finales      │
└──────────┘     │ le plan      │     └───────┬───────┘   └──────────────┘
                 └──────────────┘             │
                                     ┌───────┴───────┐
                                     │ Sous-étapes   │
                                     │ 4a → 4b → 4c │
                                     └───────────────┘
```

### 4.2 Description détaillée

#### Étape 1 — Les caméras filment

Les caméras (SDI, HDMI ou NDI) capturent les flux vidéo en direct. Chaque source est connectée à la table de régie via les liaisons appropriées (BNC pour SDI, HDMI, ou réseau pour NDI).

#### Étape 2 — La table de régie choisit le plan

Le réalisateur sélectionne le plan actif via la table de régie (switcher). Le flux programme (Program Out) est envoyé vers VisionCast‑AI.

#### Étape 3 — VisionCast‑AI reçoit le flux final

Le `CaptureManager` du moteur C++ reçoit le flux programme via l'entrée configurée (DeckLink Input, AJA Input, Magewell Input ou NDI Input). La frame est décodée en format BGRA8 unifié.

#### Étape 4 — L'IA détecte et reconnaît les talents

Le module Python reçoit une copie de la frame et effectue :

1. **Détection** — Localisation de tous les visages dans la frame (modèle HOG pour CPU, CNN pour GPU)
2. **Encodage** — Génération d'un vecteur 128 dimensions pour chaque visage
3. **Reconnaissance** — Comparaison avec la base `talents.json` (tolérance configurable, défaut : 0.45)
4. **Suivi temporel** — Stabilisation des identifications via matching IoU et lissage EMA de la confiance
5. **Publication** — Envoi des métadonnées via ZeroMQ au moteur C++

#### Étape 5 — Le moteur vidéo applique les traitements

Le `PipelineManager` orchestre le traitement séquentiel de la frame :

| Sous-étape | Module | Action |
|------------|--------|--------|
| 5a | `FilterChain` | Application des **filtres vidéo globaux** (noise reduction, LUT cinéma, HDR tonemap, contraste, sharpen) |
| 5b | `OverlayRenderer` | Application des **overlays** : lower thirds avec nom/rôle du talent, logos |
| 5c | `GpuCompositor` | **Animations** d'entrée/sortie (slide, fade, scale) avec easing |
| 5d | `GpuCompositor` | Gestion du **timer d'affichage** : entrée → maintien → sortie |
| 5e | `GpuCompositor` | **Transitions** entre overlays (cut, fade, wipe, dissolve) |

#### Étape 6 — VisionCast‑AI renvoie le flux final

Le flux traité est envoyé simultanément vers toutes les sorties configurées :

| Sortie | Protocole | Usage |
|--------|-----------|-------|
| **SDI** | DeckLink Output / AJA Output | Régie broadcast, moniteurs de référence |
| **NDI** | NDI Output | Distribution réseau local, enregistreurs |
| **SRT** | SRT Output | Streaming longue distance, contribution |
| **RTMP** | RTMP Output | Diffusion live (YouTube, Facebook, plateformes) |

---

## 5. Module IA Python

### 5.1 Vue d'ensemble

Le module IA est situé dans `python/` et se compose de sous-modules spécialisés :

```
python/
├── main.py                  # Point d'entrée principal
├── requirements.txt         # Dépendances Python
├── ai/                      # Sous-module IA
│   ├── detection.py         # Détection multi-visages optimisée
│   ├── encoding.py          # Encodage facial 128 dimensions
│   ├── recognition.py       # Pipeline de reconnaissance complet
│   ├── talents_loader.py    # Chargeur de base de talents avec cache
│   ├── face_detector.py     # Détecteur (legacy)
│   ├── face_matcher.py      # Matcher (legacy)
│   └── talent_db.py         # Base de talents (legacy)
├── ipc/                     # Communication inter-processus
│   ├── protocol.py          # Schéma des messages JSON
│   ├── zmq_sender.py        # Émetteur ZeroMQ PUB
│   └── metadata_sender.py   # Émetteur de métadonnées
└── monitoring/
    └── system_monitor.py    # Surveillance système
```

### 5.2 Détection de visages

Le module `Detection` (`ai/detection.py`) effectue la détection multi-visages avec les optimisations suivantes :

| Paramètre | Description | Valeur par défaut |
|-----------|-------------|-------------------|
| `model` | Modèle de détection (`hog` = CPU, `cnn` = GPU) | `hog` |
| `scale_factor` | Facteur de réduction pour accélérer la détection | 0.5 |
| `skip_frames` | Nombre de frames à sauter entre deux détections | 2 |
| `upsample` | Nombre de suréchantillonnages pour les petits visages | 1 |

**Fonctionnement** :
1. La frame est réduite selon `scale_factor` pour accélérer le traitement
2. La détection est effectuée toutes les `skip_frames` frames ; les résultats sont mis en cache entre-temps
3. Les coordonnées des visages sont remises à l'échelle originale
4. Le résultat est une liste de tuples `(top, right, bottom, left)`

### 5.3 Reconnaissance

Le module `Recognition` (`ai/recognition.py`) assure le pipeline complet de reconnaissance :

| Fonctionnalité | Description |
|-----------------|-------------|
| **Matching** | Comparaison des encodages 128-d avec la base de talents (distance euclidienne, tolérance 0.45) |
| **Suivi temporel** | Matching IoU entre frames pour stabiliser les identifications |
| **Lissage EMA** | Moyenne mobile exponentielle de la confiance pour éviter les oscillations |
| **Confirmation** | Un talent doit être détecté pendant `confirm_frames` frames consécutives avant affichage |
| **Éviction** | Un track est supprimé après `max_miss` frames sans détection |

### 5.4 Structure complète de `talents.json`

```json
{
  "talents": [
    {
      "talent_id": "glody",
      "name": "Glody",
      "role": "Présentateur",
      "organization": "VisionCast Media",
      "photo": "talents/glody.jpeg",
      "overlay": "overlays/templates/default.json",
      "theme_color": "#1A73E8",
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
      "priority": 1
    },
    {
      "talent_id": "journaliste_01",
      "name": "Marie Kabongo",
      "role": "Journaliste",
      "organization": "VisionCast News",
      "photo": "talents/marie.jpeg",
      "overlay": "overlays/templates/modern_blue.json",
      "theme_color": "#E8731A",
      "filters": {
        "brightness": 1.05,
        "contrast": 1.1,
        "saturation": 1.0
      },
      "animations": {
        "entry": "slide_up",
        "exit": "slide_down",
        "duration_ms": 600
      },
      "priority": 2
    }
  ]
}
```

**Description des champs** :

| Champ | Type | Description |
|-------|------|-------------|
| `talent_id` | string | Identifiant unique du talent |
| `name` | string | Nom affiché sur le lower third |
| `role` | string | Rôle/fonction affiché |
| `organization` | string | Organisation du talent |
| `photo` | string | Chemin vers la photo de référence pour la reconnaissance |
| `overlay` | string | Chemin vers le template d'overlay JSON à utiliser |
| `theme_color` | string | Couleur hexadécimale thématique du talent |
| `filters` | object | Ajustements visuels spécifiques (brightness, contrast, saturation) |
| `animations.entry` | string | Animation d'entrée (`slide_left`, `slide_up`, `fade_in`, `scale_up`) |
| `animations.exit` | string | Animation de sortie (`fade_out`, `slide_down`) |
| `animations.duration_ms` | int | Durée de l'animation en millisecondes |
| `priority` | int | Priorité d'affichage (1 = plus haute) |

### 5.5 Gestion des priorités

Lorsque plusieurs talents sont détectés simultanément :

1. Les talents sont triés par **priorité** (champ `priority`, 1 = plus haute)
2. Le talent avec la plus haute priorité est affiché en premier
3. Les talents de même priorité sont triés par **confiance** décroissante
4. Le nombre maximum d'overlays simultanés est configurable via le `GpuCompositor`

### 5.6 Envoi des métadonnées vers C++

Le message `TalentOverlayMessage` est publié via ZeroMQ à chaque détection :

```json
{
  "talent_id": "glody",
  "name": "Glody",
  "role": "Présentateur",
  "organization": "VisionCast Media",
  "overlay": "overlays/templates/default.json",
  "theme_color": "#1A73E8",
  "filters": { "brightness": 1.0, "contrast": 1.0, "saturation": 1.0 },
  "animations": { "entry": "slide_left", "exit": "fade_out", "duration_ms": 500 },
  "confidence": 0.92,
  "display_duration_ms": 5000
}
```

---

## 6. Moteur vidéo C++ (Vision Engine)

### 6.1 Pipeline GPU

Le moteur vidéo utilise **OpenGL 3.3** (via GLFW + GLEW) pour l'accélération GPU, avec un fallback CPU via OpenCV lorsque OpenGL n'est pas disponible.

```
Frame d'entrée (BGRA8)
        │
        ▼
┌──────────────────────┐
│  Upload texture GPU   │ ← glTexImage2D
│  (ou buffer CPU)      │
└──────────┬───────────┘
           │
           ▼
┌──────────────────────┐
│  FilterChain          │ ← Filtres séquentiels sur GPU/CPU
│  (5 filtres)          │
└──────────┬───────────┘
           │
           ▼
┌──────────────────────┐
│  OverlayRenderer      │ ← Compositing des lower thirds et logos
│  + GpuCompositor      │ ← Animations et transitions GPU
└──────────┬───────────┘
           │
           ▼
┌──────────────────────┐
│  Readback / Output    │ ← glReadPixels ou transfert direct
└──────────────────────┘
```

### 6.2 Gestion des textures

Le `GpuCompositor` gère un système de couches (`CompositingLayer`) avec :

| Propriété | Description |
|-----------|-------------|
| `position` | Position (x, y) de la couche dans la frame |
| `size` | Dimensions de la couche |
| `opacity` | Opacité (0.0–1.0) |
| `pixels` | Données BGRA8 de la couche |
| `z_order` | Ordre de profondeur pour le compositing |
| `animation_state` | État courant de l'animation |

Les textures PNG avec canal alpha sont supportées nativement pour les logos et graphiques.

### 6.3 Compositing GPU

Le compositing est réalisé en plusieurs passes :

1. **Background** — Frame vidéo principale
2. **Filtres globaux** — Application séquentielle de la chaîne de filtres
3. **Lower thirds** — Bande semi-transparente avec texte du talent
4. **Logos** — Superposition d'images PNG avec alpha blending
5. **Animations** — Interpolation des propriétés (position, opacité) avec easing

### 6.4 Overlays professionnels

Les overlays sont définis par la structure `LowerThirdDesc` :

| Propriété | Description |
|-----------|-------------|
| `bg_color` | Couleur de fond RGBA |
| `text_color` | Couleur du texte principal |
| `subtitle_color` | Couleur du sous-titre |
| `font_scale` | Taille de police relative |
| `thickness` | Épaisseur du texte |
| `band_height_ratio` | Hauteur de la bande (proportion de la frame) |
| `opacity` | Opacité de la bande de fond |

Trois templates sont fournis :

| Template | Style | Animation | Durée |
|----------|-------|-----------|-------|
| `default.json` | Fond noir, opacité 0.6 | Slide left → Fade out | 5000 ms |
| `minimal.json` | Fond gris foncé, opacité 0.45 | Fade in → Fade out | 4000 ms |
| `modern_blue.json` | Fond accent brun, opacité 0.75 | Slide up → Slide down | 7000 ms |

### 6.5 Filtres vidéo globaux

#### 6.5.1 LUT cinéma (`LutFilter`)

Applique un color grading cinématographique via une Look-Up Table 3D :
- **Highlights** : teintes chaudes
- **Shadows** : teintes teal
- **Blacks** : relevés (lifted)
- **Intensité** : réglable de 0.0 à 1.0 (blend avec l'image originale)

#### 6.5.2 Sharpen (`SharpenFilter`)

Netteté par masque flou (unsharp mask) :
- **Kernel size** : 3 (configurable)
- **Strength** : 0.5 (configurable)
- Algorithme : soustraction du flou gaussien

#### 6.5.3 Noise Reduction (`NoiseReductionFilter`)

Réduction de bruit préservant les contours via filtre bilatéral :
- **Diameter** : 9
- **Sigma color** : 75.0
- **Sigma space** : 75.0

#### 6.5.4 HDR Tonemapping (`HdrTonemapFilter`)

Mappage HDR vers SDR pour les écrans broadcast standard :
- **Méthode** : Reinhard tonemapping
- **Gamma** : 1.0 (configurable)
- **Saturation** : 1.0 (configurable)

#### 6.5.5 Dynamic Contrast (`DynamicContrastFilter`)

Enhancement adaptatif du contraste via CLAHE (Contrast Limited Adaptive Histogram Equalization) :
- **Clip limit** : 2.0
- **Grid size** : 8×8

### 6.6 Timer d'affichage des lower thirds

Le timer est géré par le `GpuCompositor` avec trois phases :

```
├── Entry (animation d'entrée)    │ durée : animations.duration_ms
├── Hold (maintien à l'écran)     │ durée : display_duration_ms - entry - exit
├── Exit (animation de sortie)    │ durée : configurable par template
```

Le cycle complet :
1. **Trigger** — L'IA reconnaît un talent → métadonnées reçues par le moteur
2. **Entry** — L'animation d'entrée est jouée (slide, fade, scale)
3. **Hold** — Le lower third reste affiché pendant `display_duration_ms`
4. **Exit** — L'animation de sortie est jouée
5. **Cooldown** — Le talent ne sera pas ré-affiché pendant un délai configurable

### 6.7 Gestion des résolutions

| Résolution | Dimensions | Usage |
|------------|-----------|-------|
| **1080p** (Full HD) | 1920 × 1080 | Production standard, streaming |
| **4K** (Ultra HD) | 3840 × 2160 | Production haut de gamme, cinéma |

Formats de pixels supportés :

| Format | Description |
|--------|-------------|
| BGRA8 | 8 bits par canal, format interne principal |
| UYVY | YUV 4:2:2, format SDI standard |
| V210 | YUV 4:2:2 10 bits, format broadcast professionnel (requiert DeckLink/AJA) |
| NV12 | YUV 4:2:0, format compressé |

> **Note** : Le format V210 (10 bits) nécessite une carte de capture professionnelle (DeckLink, AJA). Les webcams et périphériques grand public utilisent généralement BGRA8 ou UYVY comme format de fallback.

Fréquences d'images supportées : **25**, **29.97**, **50**, **59.94** fps.

---

## 7. Régie Qt (VisionCast Control Room)

La régie Qt (`ui/`) fournit une interface graphique complète pour le contrôle de la production en direct. Elle est composée de panneaux dockables organisés dans une fenêtre principale (`MainWindow`).

### 7.1 Panneau 1 — Sources vidéo (`ControlRoom`)

**Rôle** : Sélection et gestion des sources vidéo d'entrée.

| Élément | Type | Description |
|---------|------|-------------|
| Liste des sources | `QListWidget` | Affiche les sources disponibles (nom, type, index) |
| Bouton « Démarrer » | `QPushButton` | Démarre le moteur vidéo sur la source sélectionnée |
| Bouton « Arrêter » | `QPushButton` | Arrête le moteur vidéo |
| Info source | `QLabel` | Affiche les détails de la source sélectionnée |

**Signaux émis** :
- `sourceSelected(VideoSourceInfo)` — Source sélectionnée par l'opérateur
- `engineStartRequested()` — Demande de démarrage du moteur
- `engineStopRequested()` — Demande d'arrêt du moteur

**Workflow** :
1. L'opérateur consulte la liste des sources détectées
2. Il sélectionne la source souhaitée (DeckLink, AJA, Magewell, NDI, webcam)
3. Il clique sur « Démarrer » pour lancer le pipeline

### 7.2 Panneau 2 — Talents & Détails (`TalentManager`)

**Rôle** : Gestion de la base de données de talents.

| Élément | Type | Description |
|---------|------|-------------|
| Liste des talents | `QTableWidget` | Nom, rôle, organisation, priorité |
| Photo | `QLabel` | Aperçu de la photo de référence |
| Champs d'édition | `QLineEdit` | Modification du nom, rôle, organisation |
| Bouton « Ajouter » | `QPushButton` | Ajoute un nouveau talent |
| Bouton « Supprimer » | `QPushButton` | Supprime le talent sélectionné |
| Bouton « Importer photo » | `QPushButton` | Importe une nouvelle photo de référence |

**Workflow** :
1. L'opérateur ajoute un talent avec sa photo, son nom et son rôle
2. Le système génère automatiquement l'encodage facial 128-d
3. Le talent est disponible immédiatement pour la reconnaissance IA

### 7.3 Panneau 3 — Design & Overlays (`DesignPanel`, `OverlayEditor`)

**Rôle** : Configuration visuelle des lower thirds et de l'habillage.

| Élément | Type | Description |
|---------|------|-------------|
| Titre lower third | `QLineEdit` | Texte principal du lower third |
| Sous-titre | `QLineEdit` | Texte secondaire (rôle, organisation) |
| Durée d'affichage | `QSpinBox` | Durée en millisecondes |
| Template | `QComboBox` | Sélection du template (default, modern_blue, minimal) |
| Thème | `QComboBox` | Sélection du thème (Dark, Light, Broadcast, Custom) |
| Couleur primaire | `QLineEdit` | Code hexadécimal de la couleur primaire |
| Couleur accent | `QLineEdit` | Code hexadécimal de la couleur d'accent |
| Logo | `QPushButton` + `QLineEdit` | Sélection du fichier logo |
| Type transition | `QComboBox` | Type de transition (Cut, Fade, Wipe, Dissolve) |
| Durée transition | `QSpinBox` | Durée de la transition en ms |
| Bouton « Appliquer » | `QPushButton` | Applique les modifications au lower third actif |

**Signaux émis** :
- `designChanged(QJsonObject)` — Configuration de design modifiée
- `lowerThirdApplied(QString title, QString subtitle, int duration)` — Lower third appliqué
- `themeChanged(QString)` — Thème modifié
- `colorsChanged(QString primary, QString accent)` — Couleurs modifiées
- `logoChanged(QString path)` — Logo modifié
- `transitionChanged(QString type, int duration)` — Transition modifiée

### 7.4 Panneau 4 — Filtres vidéo globaux (`DesignPanel`)

**Rôle** : Activation et configuration des filtres vidéo en temps réel.

| Élément | Type | Description |
|---------|------|-------------|
| Cinema LUT | `QCheckBox` | Active/désactive le color grading cinéma |
| Sharpen | `QCheckBox` | Active/désactive la netteté |
| Noise Reduction | `QCheckBox` | Active/désactive la réduction de bruit |
| HDR Tonemap | `QCheckBox` | Active/désactive le tonemapping HDR |
| Dynamic Contrast | `QCheckBox` | Active/désactive le contraste adaptatif |

**Signal émis** :
- `videoFiltersChanged(QJsonObject)` — État des filtres modifié

**Workflow** :
1. L'opérateur coche/décoche les filtres souhaités
2. Les changements sont appliqués en temps réel sur le flux vidéo
3. L'aperçu dans le panneau Preview reflète immédiatement les modifications

### 7.5 Panneau 5 — Durée d'affichage

**Rôle** : Contrôle de la durée d'affichage des lower thirds.

| Élément | Type | Description |
|---------|------|-------------|
| Durée (ms) | `QSpinBox` | Durée d'affichage du lower third (1000–30000 ms) |
| Durée transition entrée | `QSpinBox` | Durée de l'animation d'entrée |
| Durée transition sortie | `QSpinBox` | Durée de l'animation de sortie |

La durée totale d'affichage est : `entry_duration + hold_duration + exit_duration`.

### 7.6 Panneau 6 — IA & Reconnaissance (`RecognitionPanel`)

**Rôle** : Affichage en temps réel des résultats de reconnaissance IA.

| Élément | Type | Description |
|---------|------|-------------|
| Visage courant | `QLabel` | Nom du visage détecté |
| Talent | `QLabel` | Nom du talent reconnu |
| Rôle | `QLabel` | Rôle du talent |
| Confiance | `QProgressBar` | Barre de confiance (0–100 %) |
| Historique | `QListWidget` | Liste scrollable des 50 derniers événements |

**Signal émis** :
- `recognitionReceived(RecognitionEvent)` — Nouvel événement de reconnaissance

**Informations affichées** :
- Nom du talent, rôle, score de confiance en pourcentage
- Horodatage de chaque détection
- Historique des 50 dernières détections avec défilement

### 7.7 Panneau 7 — Logs & Monitoring (`MonitoringPanel`)

**Rôle** : Surveillance système et journalisation.

| Élément | Type | Description |
|---------|------|-------------|
| Logs système | `QTextEdit` | Affichage des messages de log en temps réel |
| FPS | `QLabel` | Fréquence d'images courante |
| CPU | `QProgressBar` | Utilisation CPU |
| GPU | `QProgressBar` | Utilisation GPU |
| Mémoire | `QProgressBar` | Utilisation mémoire |
| Statut IA | `QLabel` | État du module IA (connecté/déconnecté) |
| Statut moteur | `QLabel` | État du moteur vidéo |

**Sources de logs** :
- `ai` — Messages du module IA Python
- `engine` — Messages du moteur C++
- `zmq` — Messages du transport ZeroMQ

**Niveaux** : `DEBUG`, `INFO`, `WARNING`, `ERROR`

### 7.8 Panneau 8 — Sorties vidéo (`OutputConfig`)

**Rôle** : Configuration des sorties vidéo et des encodeurs.

| Élément | Type | Description |
|---------|------|-------------|
| Sortie primaire | `QComboBox` | Sélection du périphérique de sortie (DeckLink, AJA, NDI) |
| Mode sortie | `QComboBox` | Résolution et fréquence (1080p25, 2160p25, etc.) |
| NDI activé | `QCheckBox` | Active/désactive la sortie NDI |
| Nom NDI | `QLineEdit` | Nom de la source NDI sur le réseau |
| SRT URL | `QLineEdit` | URL de destination SRT |
| SRT activé | `QCheckBox` | Active/désactive la sortie SRT |
| RTMP URL | `QLineEdit` | URL du serveur RTMP |
| RTMP clé | `QLineEdit` | Clé de stream RTMP |
| RTMP activé | `QCheckBox` | Active/désactive la sortie RTMP |
| Bouton « Appliquer » | `QPushButton` | Applique la configuration de sortie |

---

## 8. SDK Broadcast

### 8.1 Interface commune `IVideoDevice`

Tous les périphériques broadcast implémentent l'interface abstraite `IVideoDevice` :

```cpp
class IVideoDevice {
public:
    virtual ~IVideoDevice() = default;
    virtual bool open(const DeviceConfig& config) = 0;
    virtual void close() = 0;
    virtual bool isOpen() const = 0;
    virtual bool captureFrame(Frame& frame) = 0;      // Pour les entrées
    virtual bool sendFrame(const Frame& frame) = 0;    // Pour les sorties
    virtual std::string deviceName() const = 0;
};
```

Le patron **PIMPL** (Pointer to IMPLementation) est utilisé pour isoler les détails d'implémentation spécifiques à chaque fabricant.

### 8.2 DeckLink (Blackmagic)

#### `DeckLinkInput`

| Méthode | Description |
|---------|-------------|
| `open(config)` | Ouvre la carte DeckLink en mode capture |
| `captureFrame(frame)` | Capture une frame depuis l'entrée SDI/HDMI |
| `setConnector(type)` | Sélectionne le connecteur (SDI, HDMI, Optical) |
| `setFormat(mode)` | Définit le mode vidéo (1080p25, 2160p25, etc.) |

#### `DeckLinkOutput`

| Méthode | Description |
|---------|-------------|
| `open(config)` | Ouvre la carte DeckLink en mode sortie |
| `sendFrame(frame)` | Envoie une frame vers la sortie SDI/HDMI |
| `setGenlockSource(source)` | Configure la source de genlock (reference, input) |
| `setFormat(mode)` | Définit le mode vidéo de sortie |

### 8.3 AJA

#### `AJAInput`

| Méthode | Description |
|---------|-------------|
| `open(config)` | Ouvre la carte AJA en mode capture |
| `captureFrame(frame)` | Capture une frame depuis l'entrée AJA |

#### `AJAOutput`

| Méthode | Description |
|---------|-------------|
| `open(config)` | Ouvre la carte AJA en mode sortie |
| `sendFrame(frame)` | Envoie une frame vers la sortie AJA |

### 8.4 Magewell

#### `MagewellInput`

| Méthode | Description |
|---------|-------------|
| `open(config)` | Ouvre le périphérique Magewell en mode capture |
| `captureFrame(frame)` | Capture une frame (USB ou PCIe) |

### 8.5 NDI

#### `NDIInput`

| Méthode | Description |
|---------|-------------|
| `open(config)` | Ouvre un récepteur NDI |
| `captureFrame(frame)` | Reçoit une frame depuis une source NDI sur le réseau |
| `discoverSources()` | Découvre les sources NDI disponibles |

#### `NDIOutput`

| Méthode | Description |
|---------|-------------|
| `open(config)` | Crée un émetteur NDI avec le nom configuré |
| `sendFrame(frame)` | Envoie une frame vers le réseau NDI |

---

## 9. Sorties vidéo

### 9.1 SDI (Serial Digital Interface)

| Caractéristique | Détail |
|-----------------|--------|
| Cartes supportées | Blackmagic DeckLink, AJA KONA/Corvid/Io |
| Connecteur | BNC (75 Ω) |
| Standards | SD-SDI, HD-SDI, 3G-SDI, 6G-SDI, 12G-SDI |
| Résolutions | 1080i, 1080p, 2160p (4K) |
| Avantages | Faible latence, standard broadcast, longue distance (jusqu'à 100 m en 3G-SDI) |

### 9.2 HDMI

| Caractéristique | Détail |
|-----------------|--------|
| Cartes supportées | DeckLink (sortie HDMI), Magewell (entrée) |
| Standards | HDMI 1.4, 2.0 |
| Résolutions | Jusqu'à 4K 60 fps |
| Usage | Moniteurs de référence, écrans de plateau |

### 9.3 NDI (Network Device Interface)

| Caractéristique | Détail |
|-----------------|--------|
| Protocole | NDI v5+ (Newtek/Vizrt) |
| Transport | Réseau IP local (TCP/UDP) |
| Latence | Très faible (< 1 frame sur réseau Gigabit) |
| Découverte | Automatique via mDNS |
| Nom source | Configurable (défaut : « VisionCast Program ») |
| Usage | Distribution réseau, enregistrement, monitoring distant |

### 9.4 SRT (Secure Reliable Transport)

| Caractéristique | Détail |
|-----------------|--------|
| Classe | `SrtOutput` (`sdk/include/visioncast_sdk/srt_output.h`) |
| Protocole | SRT (UDP avec correction d'erreurs ARQ) |
| Configuration | URL de destination, latence configurable |
| Chiffrement | AES-128/256 supporté |
| Usage | Contribution longue distance, liaison inter-sites |

### 9.5 RTMP (Real-Time Messaging Protocol)

| Caractéristique | Détail |
|-----------------|--------|
| Classe | `RtmpOutput` (`sdk/include/visioncast_sdk/rtmp_output.h`) |
| Protocole | RTMP sur TCP |
| Configuration | URL du serveur + clé de stream |
| Plateformes | YouTube Live, Facebook Live, Twitch, serveurs RTMP personnalisés |
| Usage | Diffusion live vers les plateformes de streaming |

---

## 10. Dépannage & FAQ

### 10.1 L'IA ne reconnaît pas les talents

| Cause possible | Solution |
|---------------|----------|
| Photo de référence de mauvaise qualité | Utiliser une photo nette, bien éclairée, visage de face |
| Tolérance trop stricte | Augmenter `tolerance` dans `config/system.json` (défaut : 0.45, essayer 0.5) |
| Visage trop petit dans la frame | Augmenter `upsample` dans la configuration de détection |
| Éclairage insuffisant | Améliorer l'éclairage du plateau |
| Talent absent de `talents.json` | Vérifier que le talent est ajouté avec photo et encodage |
| Module IA non démarré | Vérifier que `python main.py` est en cours d'exécution |
| Connexion ZeroMQ perdue | Vérifier les endpoints dans `config/system.json` |

### 10.2 L'overlay ne s'affiche pas

| Cause possible | Solution |
|---------------|----------|
| Template JSON invalide | Vérifier la syntaxe du template dans `overlays/templates/` |
| Chemin overlay incorrect dans `talents.json` | Vérifier le champ `overlay` du talent |
| Durée d'affichage à 0 | Vérifier `display_duration_ms` dans le template |
| Opacité à 0 | Vérifier `background_opacity` dans le template |
| Moteur vidéo non démarré | Démarrer le moteur via la régie Qt |

### 10.3 Les filtres vidéo ne sont pas appliqués

| Cause possible | Solution |
|---------------|----------|
| Filtre désactivé | Vérifier les checkboxes dans le panneau Design de la régie |
| Configuration invalide | Vérifier `config/video_filters.json` |
| Filtre non dans la chaîne | Vérifier `filter_order` dans `video_filters.json` |
| Performances insuffisantes | Désactiver les filtres les plus coûteux (HDR, Noise Reduction) |

### 10.4 Problèmes DeckLink

| Cause possible | Solution |
|---------------|----------|
| Carte non détectée | Vérifier l'installation du driver Desktop Video |
| Mode vidéo non supporté | Vérifier les modes supportés dans Desktop Video Setup |
| Signal absent | Vérifier le câble SDI/HDMI et la source |
| Firmware obsolète | Mettre à jour via Blackmagic Desktop Video Setup |
| Conflit de périphériques | Fermer les autres applications utilisant la carte |

### 10.5 Problèmes NDI

| Cause possible | Solution |
|---------------|----------|
| Source non visible | Vérifier que NDI Runtime est installé |
| Pare-feu bloquant | Autoriser les ports NDI dans le pare-feu (TCP 5960-5969, UDP 5960-5969) |
| Réseau saturé | Utiliser un réseau Gigabit dédié |
| Nom de source en doublon | Changer le nom dans `config/system.json` |

### 10.6 Latence excessive

| Cause possible | Solution |
|---------------|----------|
| Trop de filtres actifs | Désactiver les filtres non essentiels |
| Résolution trop élevée | Passer de 4K à 1080p |
| GPU insuffisant | Vérifier les performances GPU, activer l'accélération matérielle |
| `process_every_n_frames` trop faible | Augmenter la valeur (défaut : 3) |
| Réseau lent (NDI/SRT) | Vérifier la bande passante réseau |

### 10.7 Résolution incorrecte

| Cause possible | Solution |
|---------------|----------|
| Configuration moteur incorrecte | Vérifier `engine.resolution` dans `config/system.json` |
| Mode sortie incorrect | Vérifier le mode dans `output.primary.mode` |
| Source d'entrée en résolution différente | Vérifier la résolution de la source dans Desktop Video Setup ou AJA Control Panel |
| Scaling non configuré | Vérifier que le moteur gère la conversion de résolution |

---

## 11. Mentions officielles

> **VisionCast‑AI** est une solution professionnelle développée par **Prestige Technologie Company**, sous la direction technique et architecturale de **Glody Dimputu Ngola**.

---

**Document généré pour VisionCast‑AI v0.1.0**  
**© Prestige Technologie Company — Tous droits réservés**
