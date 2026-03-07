# Security Policy

## Supported Versions

Only the latest release of VisionCast-AI receives security fixes.
Older versions are not actively maintained.

| Version | Supported |
|---------|-----------|
| latest  | ✅ Yes     |
| < latest | ❌ No     |

---

## Reporting a Vulnerability

VisionCast-AI is proprietary software used in professional broadcast
environments. We take security issues seriously.

**Do NOT report security vulnerabilities through public GitHub issues.**

### How to report

1. Open a **private** security advisory via GitHub:
   [New security advisory](https://github.com/ngolaglodi-byte/visioncast-AI/security/advisories/new)

2. Provide the following information:
   - A clear description of the vulnerability.
   - The affected component(s) and version(s).
   - Steps to reproduce (proof-of-concept if possible).
   - The potential impact and attack scenario.
   - Any suggested mitigation or patch.

### Response timeline

| Stage | Target |
|-------|--------|
| Initial acknowledgement | Within 72 hours |
| Triage and severity assessment | Within 7 days |
| Patch or workaround available | Within 30 days (critical), 90 days (others) |
| Public disclosure | Coordinated with reporter |

---

## Security Considerations

VisionCast-AI processes live video feeds and metadata in professional
broadcast environments. Key security areas to be aware of:

- **IPC (ZeroMQ)**: The Python ↔ C++ IPC channel binds to
  `tcp://127.0.0.1:5555` (loopback only). Do not expose this port
  externally.
- **Talent database**: The `talents/talents.json` file contains personal
  data (names, photos). Ensure appropriate file-system access controls.
- **Dependencies**: Keep `face_recognition`, `opencv-python`, `pyzmq`,
  and all C++ dependencies up to date to benefit from upstream security
  fixes.

---

## Contact

For security enquiries, use the GitHub private advisory channel above.
