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

### IPC Security (ZeroMQ)

The Python ↔ C++ IPC channel uses ZeroMQ PUB/SUB for low-latency
communication. Security measures:

- **Loopback-only binding**: All ZeroMQ endpoints bind to `127.0.0.1`
  (localhost) by default:
  - `tcp://127.0.0.1:5557` — AI metadata pub/sub
  - `tcp://127.0.0.1:5556` — Configuration channel

- **Endpoint validation**: The system validates that configured endpoints
  use the loopback interface. Non-loopback endpoints trigger a warning
  log and are blocked to prevent accidental network exposure.

- **Recommended firewall rules**: Even with loopback binding, configure
  your host firewall to block external access to ports 5555–5560:
  ```bash
  # Linux (iptables)
  iptables -A INPUT -p tcp --dport 5555:5560 -j DROP
  
  # Windows Firewall
  netsh advfirewall firewall add rule name="Block ZMQ" dir=in action=block protocol=tcp localport=5555-5560
  ```

- **Future enhancements**: ZeroMQ CURVE authentication is planned for
  deployments requiring additional IPC security (see roadmap).

### Sensitive Data Protection

The `talents/talents.json` file contains personal data (names, roles,
organizations, and photo paths). Implement these access controls:

- **File permissions**: Restrict read access to the application user:
  ```bash
  chmod 600 talents/talents.json
  chown visioncast:visioncast talents/talents.json
  ```

- **Photo storage**: Store talent photos in a non-web-accessible
  directory. The default `talents/` folder is appropriately located
  outside public directories.

- **Data minimization**: Only store necessary personal data. Remove
  unused talent entries periodically.

- **Backup encryption**: Encrypt backups containing talent data using
  AES-256 or equivalent.

### Dependencies

Keep all dependencies up to date to benefit from upstream security fixes:

- Python: `face_recognition`, `opencv-python`, `pyzmq`, `numpy`
- C++: OpenCV, ZeroMQ (libzmq), FFmpeg, FreeType
- Use `pip list --outdated` and `vcpkg upgrade` regularly

### Network Outputs

For SRT/RTMP streaming outputs:

- Store stream keys in environment variables, not in `config/system.json`
- Use SRT encryption (passphrase) for remote destinations
- Rotate stream keys periodically

---

## Contact

For security enquiries, use the GitHub private advisory channel above.
