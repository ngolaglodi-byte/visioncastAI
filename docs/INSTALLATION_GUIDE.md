# VisionCast-AI — Guide d'installation / Installation Guide

**VisionCast-AI** — Licence officielle Prestige Technologie Company,
développée par Glody Dimputu Ngola.

---

## Installation

1. Run the installer (Windows: `.exe` / macOS: `.dmg`)
2. Launch VisionCast-AI
3. Enter your license key when prompted
4. Click **Activate**
5. Done! VisionCast-AI is ready to use.

---

## First Launch — License Activation

When you launch VisionCast-AI for the first time, the activation screen
appears automatically:

```
┌──────────────────────────────────────────┐
│   Bienvenue dans VisionCast-AI           │
│                                          │
│   Machine ID: a7f3c9...                  │
│   Clé de licence: [__________________]   │
│   Status: Non activé                     │
│                                          │
│   [Activer]  [Vérifier]    [Fermer]     │
└──────────────────────────────────────────┘
```

- Enter the license key provided with your purchase.
- Click **Activer** (Activate).
- Once activated, VisionCast-AI will open automatically.

**No environment variables, no JSON editing, no command line required.**

---

## Managing Your License

After activation, you can manage your license at any time via:

**Help → Manage License…**

From this dialog you can:
- View your current activation status
- Re-activate or transfer your license to another machine
- Deactivate your license to free a seat

---

## Advanced: Build-Time Configuration (Developers)

API credentials are embedded at compile time. To override the defaults:

```bash
cmake -S . -B build \
  -DVISIONCAST_LICENSE_API_URL="https://your-project.supabase.co/functions/v1/license-api" \
  -DVISIONCAST_LICENSE_API_KEY="your-supabase-anon-key"
```

You can also override at runtime using environment variables:

```bash
export LICENSE_API_URL="https://your-project.supabase.co/functions/v1/license-api"
export LICENSE_API_KEY="your-supabase-anon-key"
```

See `.env.example` for the full list of supported environment variables.

---

## Troubleshooting

| Problem | Solution |
|---------|----------|
| Activation dialog doesn't appear | Ensure VisionCast-AI is properly installed and run from the install directory |
| "Network Error" during activation | Check your internet connection; the license server requires HTTPS access |
| License shows as expired | Contact Prestige Technologie Company for license renewal |
| Cannot deactivate | Contact support to remotely deactivate your license |

---

*© Prestige Technologie Company. All rights reserved.*
