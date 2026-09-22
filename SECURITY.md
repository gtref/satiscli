# Security Policy

## Supported Versions

Only the latest commit on `main` receives security fixes.

| Branch / Tag | Supported |
|---|---|
| `main` (latest) | ✅ |
| Older tags | ❌ |

## Reporting a Vulnerability

**Please do not open a public GitHub issue for security vulnerabilities.**

Report vulnerabilities privately via [GitHub Security Advisories](https://github.com/G-type/satiscli/security/advisories/new).

Include as much of the following as possible:

- A description of the vulnerability and its potential impact
- Steps to reproduce (commands, `.satisconf` entries, OS version)
- Any suggested fix or patch

You can expect an initial response within **72 hours** and a status update within **7 days**.

## Scope

The following are considered in-scope:

- Arbitrary command execution via malformed `.satisconf` entries
- Path traversal or directory escape in built-in commands
- Environment variable injection leading to privilege escalation
- Memory safety issues (buffer overflows, use-after-free) in `src/`

Out of scope: issues in third-party tools invoked by satiscli (e.g. the programs it spawns via `CreateProcess`).

## Disclosure Policy

We follow **coordinated disclosure**. Once a fix is merged to `main`, the advisory will be published publicly with full credit to the reporter.
